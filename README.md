# PIMPLE

A music synthesizer inspired by the Acorn Music 500.

## Status

[![Build Status](https://github.com/AnotherJohnH/pico500/actions/workflows/build_on_push.yml/badge.svg)](https://github.com/AnotherJohnH/pico500/actions/workflows/build_on_push.yml)

## Software

### Dependencies

+ https://github.com/AnotherJohnH/Platform
+ arm-none-eabi-gcc
+ cmake
+ ninja (make works too)

### Checkout

This repo uses git sub-modules, so checkout using --recurse to clone all the
dependent source...

    git clone --recurse https://github.com/AnotherJohnH/PIMPLE.git

or

    git clone --recurse ssh://git@github.com/AnotherJohnH/PIMPLE.git

### Build

Being developed on MacOS but should build on Linux too.

    make

Build directly using cmake...

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DPLT_TARGET=rpipico2 -DCMAKE_TOOLCHAIN_FILE=Platform/MTL/rpipico2/toolchain.cmake ..
    make

Flashable image will be found under the build sub directory here...

    .../Source/PIMPLE.uf2

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
