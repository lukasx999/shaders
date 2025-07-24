configure:
    cmake -B build -GNinja

build: configure
    cmake --build build

run: build
    ./build/proj

check:
    glslangValidator shader.vert
    glslangValidator shader.frag
