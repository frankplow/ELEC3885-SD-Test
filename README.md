## Build Instructions
### Compiling for Target
To configure the project compiling for the target, run:
```bash
$ rm -rf build
$ cmake -S . -B build
```
and to compile, run:
```bash
$ cmake --build build
```

### Testing
Unit testing is disabled by default. When unit testing, cross compilation is disabled as the test is run on the host machine.

To configure for unit testing, run:
```bash
$ rm -rf build
$ cmake -DCMAKE_TESTING_ENABLED=1 -DCMAKE_TOOLCHAIN_FILE=cmake/host.cmake -S . -B build
```
and to compile and run the tests, run:
```bash
$ cmake --build build --target check
```

## Flashing
The CMake project contains a custom `flash` target which can be used to flash the microcontroller. This depends on [stlink](https://github.com/stlink-org/stlink). To use, simply run:
```bash
$ cmake --build build --target flash
```
