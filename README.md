# Build Instructions
To configure the project, run:
```bash
$ cmake -S . -B build
```
and to compile, run:
```bash
$ cmake --build build
```

By default the project will be configured to cross-compile for the target and host-compiles the unit tests and documentation. This behaviour is controlled by the `ELEC3885_BUILD_TARGET`, `ELEC3885_BUILD_TESTS` and `ELEC3885_BUILD_DOCS` flags. For example, to configure to build the target executable and tests but not the documentation, run:
```bash
$ cmake -DELEC3885_BUILD_DOCS=OFF -S . -B build
```

## Flashing
The CMake project contains a custom `flash` target which can be used to flash the microcontroller. This depends on [stlink](https://github.com/stlink-org/stlink). To use, simply run:
```bash
$ cmake --build build --target flash
```
once the project has been configured
