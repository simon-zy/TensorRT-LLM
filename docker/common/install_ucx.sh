
set -ex

wget https://github.com/openucx/ucx/releases/download/v1.17.0/ucx-1.17.0.tar.gz
tar xzf ucx-1.17.0.tar.gz
cd ucx-1.17.0

mkdir build && cd build
../configure
make -j16
make install