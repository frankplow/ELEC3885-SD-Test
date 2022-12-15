cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake)

project(ELEC3885 C ASM)

# -mcpu=cortex-m4 is self-explanatory
# -mfpu=-fpv4-sp-d16 specifies the properties of the FPU (VFPv4 instructions, single-precision, 16 double-precision registers)
# -mfloat-abi=hard allows the generation of floating-point instructions
# --specs=nosys.specs provides dummy implementations for syscalls
# --specs=nano.specs selects the nano version of newlib
add_definitions(-DSTM32F429xx)
add_compile_options(
    "-mcpu=cortex-m4"
    "-mfpu=fpv4-sp-d16"
    "-mfloat-abi=hard"
    "--specs=nosys.specs"
    "--specs=nano.specs"
)
add_link_options(
    "-mcpu=cortex-m4"
    "-mfpu=fpv4-sp-d16"
    "-mfloat-abi=hard"
    "--specs=nosys.specs"
    "--specs=nano.specs"
)

add_subdirectory(src)
