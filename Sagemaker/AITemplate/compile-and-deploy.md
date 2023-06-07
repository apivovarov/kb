# Compile Stable Diffusion Model with AITemplate and Deploy it as AWS Sagemaker Endpoint

## Install requires software

We need to install the following packages
- [cuda-drivers](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda](https://developer.nvidia.com/cuda-toolkit-archive)
- [cuda-drivers-fabricmanager](https://docs.nvidia.com/datacenter/tesla/pdf/fabric-manager-user-guide.pdf)
- [docker](https://docs.docker.com/engine/install/ubuntu/)
- [nvidia-container-toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)

## Sagemaker Container

Sagemaker Endroint runs models inside Sagemaker Deep Learning containers. In order to be compatible with Sagemaker container libraries ee need to compile our models in sagemaker container.

The list of Available Sagemaker Containers is here - (aws/deep-learning-containers)[https://github.com/aws/deep-learning-containers/blob/master/available_images.md]
