# Compile Stable Diffusion Model with AITemplate and Deploy it as AWS Sagemaker Endpoint

## Install requires software

We need to install the following packages
- [cuda-drivers](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda-drivers-fabricmanager](https://docs.nvidia.com/datacenter/tesla/pdf/fabric-manager-user-guide.pdf)
- [docker](https://docs.docker.com/engine/install/ubuntu/)
- [nvidia-container-toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)

## Sagemaker Container Images

Sagemaker Endroint runs models inside Sagemaker Deep Learning containers. In order to be compatible with Sagemaker container libraries ee need to compile our models in sagemaker container.

The list of Available Sagemaker Container Images is here - [aws/deep-learning-containers](https://github.com/aws/deep-learning-containers/blob/master/available_images.md)

Lets pull the following Sagemaker Pytorch 2.0.0 inference image
```
$(aws ecr get-login --no-include-email --registry-ids 763104351884 --region us-west-2)
docker pull 763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.0-gpu-py310-cu118-ubuntu20.04-sagemaker
```

Lets run the compilation container. It will use GPUs and couple shared folders on the host
```
docker run -ti \
--name sm_pt200 \
--runtime=nvidia --gpus all \
-v ~/workspace:/root/workspace \
-v ~/.cache/huggingface:/root/.cache/huggingface \
763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.0-gpu-py310-cu118-ubuntu20.04-sagemaker /bin/bash
```

Test that the container runs with Cuda
```
nvidia-smi
```
Test PyTorch
```python
import torch
torch.cuda.is_available()
```

## Install AITemplate
[AITemplate](https://github.com/facebookincubator/AITemplate) (AIT) is a Python framework that transforms deep neural networks into CUDA (NVIDIA GPU) / HIP (AMD GPU) C++ code for lightning-fast inference serving.

To install AIT run the following inside the contailer:
```
cd ~/workspace
git clone --recursive https://github.com/facebookincubator/AITemplate

cd AITemplate
cd python
python3 setup.py bdist_wheel
pip3 install dist/*.whl --force-reinstall
```

Install additional python packages
```
pip3 install diffusers transformers accelerate click cuda-python
```

## Compiler Stable Diffusion Model
AIT provides [exaple/05_stable_diffusion](https://github.com/facebookincubator/AITemplate/tree/main/examples/05_stable_diffusion). It includes scripts to download, compile and run the model.

### Download Stable Diffusion model

```
cd ~/workspace/AITemplate/examples/05_stable_diffusion

# Edit scripts/download_pipeline.py and
# - change model name to "stabilityai/stable-diffusion-2-1-base" for 512x512
# - or to "stabilityai/stable-diffusion-2-1" for 768x768 output image
# - comment out "use_auth_token=token ..." line

# Download SD pipeline files to workdir tmp
python3 scripts/download_pipeline.py
```

### Compile Stable Diffusion model

```
# Set correct model resolution
H=512 # for stabilityai/stable-diffusion-2-1-base model
H=768 # for stabilityai/stable-diffusion-2-1 model


python3 scripts/compile.py --height $H --width $H

# Compilation should take 5-10 min. It will generate the following .so files
# - tmp/UNet2DConditionModel/test.so
# - tmp/AutoencoderKL/test.so
# - tmp/CLIPTextModel/test.so
```

### Run model locally

To run the model locally use demo.py script. Example.
```
python3 scripts/demo.py \
--height $H --width $H \
--prompt "a photo of an astronaut riding a horse on mars"
```
It will generate example_ait.png file.


## Prepare Sagemaker Model

### Prepare sm_model folder
```
mkdir -p sm_model/compiled

mkdir sm_model/compiled/AutoencoderKL
mkdir sm_model/compiled/CLIPTextModel
mkdir sm_model/compiled/UNet2DConditionModel

# Copy compiled model files to sm_model/compiled/
mv tmp/AutoencoderKL/test.so sm_model/compiled/AutoencoderKL/
mv tmp/CLIPTextModel/test.so sm_model/compiled/CLIPTextModel/
mv tmp/UNet2DConditionModel/test.so sm_model/compiled/UNet2DConditionModel/

# Copy originad SD model files to sm_model
mv tmp/diffusers-pipeline/stabilityai/stable-diffusion-v2/* sm_model/
```
### Prepare code sub-folder
```
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

# Edit inference.py and set correct height and width in process_data function - 512x512 (for base model) or 768x768 (for regular model)
```

Finally, sm_model folder should contain the followong folders/files:
```
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
```
docker run --rm \
--runtime=nvidia --gpus all \
-p 8080:8080 \
-v ~/workspace/AITemplate/examples/05_stable_diffusion/sm_model:/opt/ml/model \
763104351884.dkr.ecr.us-west-2.amazonaws.com/pytorch-inference:2.0.0-gpu-py310-cu118-ubuntu20.04-sagemaker serve
```
Check if the system out shows any exceptions / stacktraces
