# CMake project for AVR

## Usage example (MinGW)

Create build directory:

```bash
mkdir build && cd build
```

Generate makefile:

```bash
cmake -G "MinGW Makefiles" ..
```

Build:

```bash
mingw32-make
```

Flash:

```bash
mingw32-make flash
```



