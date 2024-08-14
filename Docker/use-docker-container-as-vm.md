# Use Docker container as VM (for development)

## Create Docker container with correct user_name, user_id and with shared folder

To use a Docker container as a virtual machine (VM) for development, the container's user should have the same username and user ID as your host OS.

The instructions below explain how to create a Docker container with correct user_name, user_id and with shared folder
```bash
# Create shared folder, .e.g. workspace. It should be owned by your user.
mkdir ~/workspace

# Run docker container with particular env vars and shared folder mapped to /home/$USER/workspace
docker run -ti --name u2204 \
-e U=$USER -e USER_ID=$UID \
-v ~/workspace:/home/$USER/workspace ubuntu:22.04

# Double check that container has user name and id env vars
echo $U $USER_ID

adduser $U
usermod -u $USER_ID $U
usermod -aG sudo $U

apt update
apt install -y sudo vim wget curl \
libssl-dev \
python3 python3-pip

### run visudo and add NOPASSWD: to Line 50
visudo
# Allow members of group sudo to execute any command
%sudo ALL=(ALL:ALL) NOPASSWD: ALL

cp /root/.bashrc /root/.profile /home/$U/
chown $U /home/$U/.bashrc /home/$U/.profile

su - $U
```

## Usefull commands, key combos
```bash
# List all containers stopped and started
docker ps -a

# Start previously stopped container
docker start <container_name>

# Attach to started container
docker attach <container_name>

# To detach
Control-P-Q (no need to hold P)

# Open the second shell to started container (in addition to attached shell)
docker exec -ti <container_name> /bin/bash

# List all images
docker images

# Get Docker image details
docker inspect <image_id|image_uri>
```
