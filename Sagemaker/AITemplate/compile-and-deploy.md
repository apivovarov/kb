# Compile Stable Diffusion Model with AITemplate and Deploy it as AWS Sagemaker Endpoint

## Select AWS instance
Select AWS instance where you want to host the model.
Sagemaker supports the following instances:

- ml.p3  - V100
- ml.g4dn - T4
- ml.g5  - A10G
- ml.p4d - A100 (limited availability)

You need to start similar ec2 instance to compile the model. Ubuntu 20.04+ OS should work.

## Install required software

We need to install the following packages on the ec2 instance
- [nvidia-driver](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda-drivers-fabricmanager](https://docs.nvidia.com/datacenter/tesla/pdf/fabric-manager-user-guide.pdf)
- [docker](https://docs.docker.com/engine/install/ubuntu/)
- [nvidia-container-toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)

We can also use Deep Learning AMI which has the software above pre-installed. For example, Deep Learning AMI GPU PyTorch 2.0.1 (Ubuntu 20.04) 20230613 (`ami-0f9cdc510d79ae56b`)


## Sagemaker Container Images

Sagemaker Endpoint runs models inside Sagemaker Deep Learning containers. In order to be compatible with Sagemaker container we are going to compile our models in sagemaker container.

The list of Available Sagemaker Container Images is here - [aws/deep-learning-containers](https://github.com/aws/deep-learning-containers/blob/master/available_images.md)

Let's pull the following Sagemaker Pytorch 2.0.1 inference image
```bash
# run aws configure if needed
aws configure

$(aws ecr get-login --no-include-email --registry-ids 763104351884 --region us-west-2)
docker pull 763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.1-gpu-py310-cu118-ubuntu20.04-sagemaker
```

Let's run Sagemaker container in an interactive mode. It will use GPUs and couple shared folders on the host
```bash
docker run -ti \
--name sm_pt201 \
--runtime=nvidia --gpus 1 \
-v ~/workspace:/root/workspace \
-v ~/.cache/huggingface:/root/.cache/huggingface \
763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.1-gpu-py310-cu118-ubuntu20.04-sagemaker /bin/bash
```

Test that the container sees GPU and CUDA is available
```bash
nvidia-smi
```
Test PyTorch
```python
import torch
torch.cuda.is_available()
```

## Install AITemplate
[AITemplate](https://github.com/facebookincubator/AITemplate) (AIT) is a Python framework that transforms deep neural networks into CUDA (NVIDIA GPU) / HIP (AMD GPU) C++ code for lightning-fast inference serving.

To install AIT run the following inside the container:
```bash
cd ~/workspace
git clone --recursive https://github.com/facebookincubator/AITemplate

cd AITemplate/python
python3 setup.py bdist_wheel
pip3 install dist/*.whl --force-reinstall
```

Install additional python packages
```bash
pip3 install diffusers transformers accelerate click cuda-python pyclean
```

## Compile Stable Diffusion Model
AIT provides [example/05_stable_diffusion](https://github.com/facebookincubator/AITemplate/tree/main/examples/05_stable_diffusion). It includes scripts to download, compile and run the model.

### Download Stable Diffusion model

```bash
cd ~/workspace/AITemplate/examples/05_stable_diffusion

# Stable Diffusion 2.1 has two editions on Huggingface.
# - base edition - "stabilityai/stable-diffusion-2-1-base" 512x512
# - regular edition - "stabilityai/stable-diffusion-2-1" 768x768

# Download SD 2.1 base model files to workdir tmp
python3 scripts/download_pipeline.py \
--model-name stabilityai/stable-diffusion-2-1-base
```

### Compile Stable Diffusion model

```bash
# Set correct model resolution
H=512 # for stabilityai/stable-diffusion-2-1-base model
H=768 # for stabilityai/stable-diffusion-2-1 model

python3 scripts/compile.py --height $H --width $H --batch-size 1

# Compilation should take 5-10 min. It will generate the following .so files
# - tmp/UNet2DConditionModel/test.so
# - tmp/AutoencoderKL/test.so
# - tmp/CLIPTextModel/test.so
```

*Variable batch size support (e.g. `--batch-size 1 8`) is implemented in the following repo/branch - https://github.com/apivovarov/AITemplate/tree/add_batch_to_sd

### Run model locally

To run the model locally use demo.py script. Example:
```bash
python3 scripts/demo.py \
--height $H --width $H --batch 1 \
--prompt "a photo of an astronaut riding a horse on mars"
```
It will generate `example_ait_[0..n].png` files.


## Prepare Sagemaker Model

### Prepare sm_model folder
```bash
mkdir -p sm_model/compiled

mkdir sm_model/compiled/AutoencoderKL
mkdir sm_model/compiled/CLIPTextModel
mkdir sm_model/compiled/UNet2DConditionModel

# Copy compiled model files to sm_model/compiled/
mv tmp/AutoencoderKL/test.so sm_model/compiled/AutoencoderKL/
mv tmp/CLIPTextModel/test.so sm_model/compiled/CLIPTextModel/
mv tmp/UNet2DConditionModel/test.so sm_model/compiled/UNet2DConditionModel/

# Copy original SD model files to sm_model
mv tmp/diffusers-pipeline/stabilityai/stable-diffusion-v2/* sm_model/
```
### Prepare code sub-folder
```bash
mkdir -p sm_model/code
cd sm_model/code

# Copy aitemplate wheel file to code folder (fix whl name if needed)
cp ~/workspace/AITemplate/python/dist/aitemplate-0.3.dev0-py3-none-any.whl .

# Create file requirements.txt and add the following lines into it (fix whl name if needed)

/opt/ml/model/code/aitemplate-0.3.dev0-py3-none-any.whl
accelerate
diffusers
transformers

# download inference.py and pipeline_stable_diffusion_ait.py to code folder
wget https://raw.githubusercontent.com/apivovarov/kb/main/Sagemaker/AITemplate/code/inference.py
wget https://raw.githubusercontent.com/apivovarov/kb/main/Sagemaker/AITemplate/code/pipeline_stable_diffusion_ait.py

# Edit inference.py file and set correct height and width in process_data() function - 512x512 (for base model) or 768x768 (for regular model)
```

Finally, sm_model folder should contain the following folders/files:
```bash
tree sm_model

sm_model/
├── code
│   ├── aitemplate-0.3.dev0-py3-none-any.whl
│   ├── inference.py
│   ├── pipeline_stable_diffusion_ait.py
│   └── requirements.txt
├── compiled
│   ├── AutoencoderKL
│   │   └── test.so
│   ├── CLIPTextModel
│   │   └── test.so
│   └── UNet2DConditionModel
│       └── test.so
├── model_index.json
├── scheduler
│   └── scheduler_config.json
├── text_encoder
│   ├── config.json
│   └── pytorch_model.bin
├── tokenizer
│   ├── merges.txt
│   ├── special_tokens_map.json
│   ├── tokenizer_config.json
│   └── vocab.json
├── unet
│   ├── config.json
│   └── diffusion_pytorch_model.bin
└── vae
    ├── config.json
    └── diffusion_pytorch_model.bin

```
### Test model in SM container
Exit from the SM container where we compiled the model

Start SM container with the prepared model in serving mode
```bash
docker run --rm \
--runtime=nvidia --gpus 1 \
-p 8080:8080 \
-v ~/workspace/AITemplate/examples/05_stable_diffusion/sm_model:/opt/ml/model \
763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.1-gpu-py310-cu118-ubuntu20.04-sagemaker serve
```
Check if the system out shows any exceptions / stack traces

Prepare test_request.json file
```json
{
    "prompt": ["a photo of an astronaut riding a horse on mars"],
}
```
You can use prompt array with multiple prompts if the model was compiled with batch size greater than one (or variable batch size).

Send test request to the endpoint.

Open another terminal and run the following
```bash
curl -s -d "@test_request.json" -H 'Content-Type: application/json' \
-X POST http://localhost:8080/invocations -o output.json
```
`output.json` should contain json where images are encoded in base64, example:
```json
{"images": ["oW5K...AAAA","je2A...BBBB"]}
```
Stop the container

## Deploy the model to Sagemaker
Create model tar.gz and upload it to s3
```bash
cd sm_model
# remove python temp files
sudo pyclean .
# create model archive
tar vzcf ../sm_model_g5.tar.gz *
cd ..
```
Copy model archive to s3
```bash
aws s3 cp sm_model_g5.tar.gz s3://sagemaker-us-west-2-345967381662/stable-diffusion/text-to-image/
```
### Create Model
```python
import boto3
# Prepare boto3 Sagemaker client
region = "us-west-2"
sm_client = boto3.client("sagemaker", region_name=region)

# Role to give SageMaker service permission to access your account resources (s3, etc.). Change role ARN to correct one.
sagemaker_role = "arn:aws:iam::345967381662:role/service-role/AmazonSageMaker-ExecutionRole-20180829T140091"
```
```python
### Create Sagemaker endpoint and deploy the model
# https://docs.aws.amazon.com/sagemaker/latest/dg/realtime-endpoints-deployment.html

import yaml
#Get model from S3
model_url = f"s3://sagemaker-us-west-2-345967381662/stable-diffusion/text-to-image/sm_model_g5.tar.gz"

#Get SM container image (prebuilt example)
container = "763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.1-gpu-py310-cu118-ubuntu20.04-sagemaker"

# ==== Create model ====
model_name = "stable-diffusion-2-1-base-g5"

create_model_response = sm_client.create_model(
    ModelName = model_name,
    ExecutionRoleArn = sagemaker_role,
    Containers = [{
        "Image": container,
        "Mode": "SingleModel",
        "ModelDataUrl": model_url,
    }]
)
print(yaml.dump(create_model_response))
```
#### Create Endpoint Config
```python
##### === Create Endpoint Config ====

endpoint_config_name = "stable-diffusion-2-1-base-g5"
instance_type = "ml.g5.2xlarge"

endpoint_config_response = sm_client.create_endpoint_config(
    EndpointConfigName=endpoint_config_name, # You will specify this name in a CreateEndpoint request.
    # List of ProductionVariant objects, one for each model that you want to host at this endpoint.
    ProductionVariants=[
        {
            "VariantName": "variant1", # The name of the production variant.
            "ModelName": model_name,
            "InstanceType": instance_type, # Specify the compute instance type.
            "InitialInstanceCount": 1 # Number of instances to launch initially.
        }
    ]
)
print(yaml.dump(endpoint_config_response))
```
#### Create Endpoint
```python
# ==== Create Endpoint ====

endpoint_name = 'stable-diffusion-2-1-base-g5'

create_endpoint_response = sm_client.create_endpoint(
        EndpointName=endpoint_name,
        EndpointConfigName=endpoint_config_name
)
print(yaml.dump(create_endpoint_response))
```
#### Check Endpoint Status
```python
# ==== Check Endpoint Status ====
desc_endpoint_response = sm_client.describe_endpoint(
    EndpointName=endpoint_name
)
print(f"EndpointStatus: {desc_endpoint_response.get('EndpointStatus', None)}")
print("==========================================================")
print(yaml.dump(desc_endpoint_response))
```
#### Invoke the Endpoint via Boto3 SageMaker Client
```python
content_type = "application/json"
request_body = {
    "prompt": ["a photo of an astronaut riding a horse on mars"],
}
import yaml
import json
import boto3
from botocore.config import Config
# Serialize data for endpoint
payload = json.dumps(request_body)

config = Config(region_name = 'us-west-2')
sm_runtime_client = boto3.client("sagemaker-runtime", config=config)
response = sm_runtime_client.invoke_endpoint(
    # change to your endpoint name returned in the previous step
    EndpointName=endpoint_name,
    ContentType="application/json",
    Body=payload,
)
print(yaml.dump(response['ResponseMetadata']))
res = response["Body"].read()
```
In case of 500 error look at the endpoint log files
```
# CloudWatch URL to check endpoint logs
https://us-west-2.console.aws.amazon.com/cloudwatch/home?region=us-west-2#logEventViewer:group=/aws/sagemaker/Endpoints/stable-diffusion-2-1-base-g5
```

#### Visualize the Generated Image
```python
import matplotlib.pyplot as plt
import base64
import numpy as np

for img_encoded in eval(res)["images"]:
    pred_decoded_byte = base64.decodebytes(
        bytes(img_encoded, encoding="utf-8")
    )
    # update H for used model edition (base - 512, regular - 768)
    H = 512
    pred_decoded = np.reshape(np.frombuffer(pred_decoded_byte, dtype=np.uint8), (H, H, 3))
    plt.imshow(pred_decoded)
    plt.axis("off")
    plt.show()
```

![astronaut_on_mars_512.png](astronaut_on_mars_512.png)

![astronaut_on_mars_768.png](astronaut_on_mars_768.png)

## Compiled model performance
The table below shows Models Inference time in seconds to process one prompt and generate one image
```
-----------------------------------------------------------
            base 512x512            regular 768x768
 HW     Uncompiled   Compiled     Uncompiled   Compiled
-----------------------------------------------------------
g4dn       6.61     4.70 (1.5x)    16.82    14.11 (1.2x)
g5         2.74     1.86 (1.5x)     6.12     4.86 (1.3x)
p4d        1.56     0.91 (1.7x)     2.76     2.00 (1.4x)
-----------------------------------------------------------

* Number in parentheses represents compiled model speedup in comparison to uncompiled one
* Uncompiled model params - revision="fp16", torch_dtype=torch.float16
* Compiled model params - use-fp16-acc=True
```
