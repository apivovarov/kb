# Install Newer Python on Amazon Linux 2

This wiki explains how to install newer version of [Python](https://www.python.org/) on [Amazon Linux 2](https://aws.amazon.com/amazon-linux-2).

Below I'll show how to install Python 3.9.

Installing newer Python versions (3.10, 3.11, 3.12, etc) is similar.

## Install Python 3.9

### Install required packages

```bash
sudo yum update

# gcc, g++, etc
sudo yum groupinstall "Development Tools"

# Libs needed for Python compilation
sudo yum install \
  bzip2 bzip2-devel \
  gdbm gdbm-devel \
  libffi libffi-devel \
  ncurses ncurses-devel \
  openssl11 openssl11-devel \
  readline readline-devel \
  sqlite sqlite-devel \
  uuid libuuid libuuid-devel \
  xz xz-devel \
  zlib zlib-devel

# If plan to develop GUI apps install libs needed for tkinter.
# Libs size is not that small.... (so, it is optional)
sudo yum \
  tcl tcl-devel \
  tk tk-devel

# Additional convenience tools
sudo yum install \
  curl htop vim wget
```

### Downloads Python binaries

FTP to Download Python binaries - [https://www.python.org/ftp/python/](https://www.python.org/ftp/python/)

```bash
wget https://www.python.org/ftp/python/3.9.18/Python-3.9.18.tgz
tar zxf Python-3.9.18.tgz
cd Python-3.9.18
```

### Configure, make, install

```bash
# There are Two Options - Fast build or Release

# Option 1 - Fast build
./configure

# Option 2 - Release build with optimizations
./configure --enable-optimizations --with-lto

# To control installation Dir use --prefix=<path> in configure cmd
# Default is /usr/local
# Need More info? ./configure --help

# Build
make -j $(nproc)

# Install
sudo make altinstall

# By default python will be installed to /usr/local/bin
/usr/local/bin/python3.9
```

#### Check SSL version

Open Python shell

```bash
/usr/local/bin/python3.9
```

Run
```python
import ssl
print(ssl.OPENSSL_VERSION)

# It should be OpenSSL 1.1.1, not 1.0.2
# 'OpenSSL 1.1.1g FIPS  21 Apr 2020'
```

### Get pip
```bash
cd /tmp
wget https://bootstrap.pypa.io/get-pip.py

sudo python3.9 get-pip.py
```
