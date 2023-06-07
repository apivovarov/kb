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
# - change model name to "stabilityai/stable-diffusion-2-1-base"
# - comment out "use_auth_token=token ..." line

# Download SD pipeline files to workdir tmp
python3 scripts/download_pipeline.py
```

### Compile Stable Diffusion model

```
python3 scripts/compile.py

# Compilation should take 5-10 min. It will generate the following .so files
# - tmp/UNet2DConditionModel/test.so
# - tmp/AutoencoderKL/test.so
# - tmp/CLIPTextModel/test.so
```

### Run model locally

To run the model locally use demo.py script. Example.
```
python3 scripts/demo.py --prompt "a photo of an astronaut riding a horse on mars"
```
It will generate example_ait.png file.


## Prepare Sagemaker Model

```

mkdir -p sm_model/tmp/AutoencoderKL
mkdir -p sm_model/tmp/CLIPTextModel
mkdir -p sm_model/tmp/UNet2DConditionModel

mv tmp/AutoencoderKL/test.so sm_model/tmp/AutoencoderKL/
mv tmp/CLIPTextModel/test.so sm_model/tmp/CLIPTextModel/
mv tmp/UNet2DConditionModel/test.so sm_model/tmp/UNet2DConditionModel/

mv tmp/diffusers-pipeline/stabilityai/stable-diffusion-v2/* sm_model/
```
