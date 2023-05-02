## Install AlmaLinux 9
### Desktop
```bash
sudo dnf install yum-utils
sudo dnf config-manager --set-enabled crb
sudo dnf install epel-release
sudo dnf upgrade

sudo dnf group install Workstation
sudo systemctl set-default graphical
sudo reboot

sudo dnf install xrdp
sudo systemctl start xrdp
sudo systemctl enable xrdp

sudo systemctl start firewalld
sudo systemctl enable firewalld
sudo firewall-cmd --permanent --zone=public --add-service=rdp
sudo systemctl restart firewalld
```

### Development Tools
```bash
sudo dnf group install "Development Tools"
sudo dnf install tmux htop wget curl vim cmake
```
