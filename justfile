set export

CC := "clang"
CXX := "clang++"

build:
    cmake --build build

format:
    find ./src -type f -name "*.cpp" | xargs clang-format -i

@run:
    ./build/darwv

setup:
    @rm -r build
    cmake -Bbuild . -GNinja
