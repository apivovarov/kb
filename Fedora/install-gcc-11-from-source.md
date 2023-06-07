# Build gcc 11 from source

## Infro

Cuda 11.8 (nvcc) supports gcc up to version 11.x (including)
Fedora 36 comes with gcc-12.x

In order to use nvcc fom cuda 11.8 on Fedora 36+ we need to instal gcc-11. RPM packages are not available. One solution is to use brew. Another solution is to simply build gcc from source - it is easy.


## Build
```bash
wget https://github.com/gcc-mirror/gcc/archive/refs/tags/releases/gcc-11.4.0.tar.gz

tar zxf gcc-11.4.0.tar.gz
rm gcc-11.4.0.tar.gz

# Download prerequisites
cd gcc-releases-gcc-11.4.0
./contrib/download_prerequisites

# build dir should be outside of gcc-releases-gcc-11.4.0
cd ..
mkdir build
cd build

../gcc-releases-gcc-11.4.0/configure \
-v \
--build=x86_64-linux-gnu \
--host=x86_64-linux-gnu \
--target=x86_64-linux-gnu \
--prefix=/usr/local/gcc-11 \
--program-suffix=-11 \
--enable-checking=release \
--enable-languages=c,c++ \
--disable-multilib \
--disable-bootstrap \
--enable-threads=posix \
--enable-64-bit-bfd \
--disable-lto

make -j$(nproc)
sudo make install-strip

export CC=/usr/local/gcc-11/bin/gcc-11
export CXX=/usr/local/gcc-11/bin/g++-11
```

# Test gcc

```c++
// test.cc
#include <iostream>

int main() {
  std::cout << "Hello\n";
  return 0;
}
```
``` bash
/usr/local/gcc-11/bin/g++-11 test.cc
./a.out
```
# Test nvcc
```bash
# tell nvcc to use g++-11
export NVCC_PREPEND_FLAGS='-ccbin /usr/local/gcc-11/bin/g++-11'

# test nvcc
wget https://gist.githubusercontent.com/apivovarov/bb99281bfbb864dda38a77110655cec2/raw/85acedf49e40d9c8cebe35a15e375e1b3d66a368/cuda_check.cu

nvcc cuda_check.cu -o cuda_check -lcuda
./cuda_check
```
