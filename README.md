# M7: Mode 7 Rendering Module

M7 is a single-header module written in C99, designed to facilitate Mode 7 rendering using [raylib](https://www.raylib.com/).

## How to Use

The header file and the provided example are all you need to understand how to use M7 effectively.

If you'd like to compile the example while also cloning and building the latest version of raylib with M7, follow these steps:

```console
git clone --recursive https://github.com/Bigfoot71/M7.git
cd M7
make setup
make && ./M7Demo
```

If you initially cloned M7 without raylib and now wish to integrate raylib, you can do so within the M7 directory:

```console
git submodule update --init --recursive
```

Alternatively, you can clone the repository alone and directly use the header with your existing raylib setup.

**Enjoy 😄**
