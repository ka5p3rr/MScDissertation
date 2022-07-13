set -x
cd zip
mkdir -p build
cmake -S . -B build
cmake --build build
cmake --build build --target test