set -x
cd zlib
mkdir -p build
cmake -S . -B build
cmake --build build
cmake --build build --target test