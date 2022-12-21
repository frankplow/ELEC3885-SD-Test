set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set compiler
set(triple arm-none-eabi)
set(CMAKE_C_COMPILER ${triple}-gcc)
set(CMAKE_CXX_COMPILER ${triple}-g++)
set(CMAKE_C_OBJCOPY ${triple}-objcopy)

# -mcpu=cortex-m4 is self-explanatory
# -mfpu=-fpv4-sp-d16 specifies the properties of the FPU (VFPv4 instructions, single-precision, 16 double-precision registers)
# -mfloat-abi=hard allows the generation of floating-point instructions
# --specs=nosys.specs provides dummy implementations for syscalls
# --specs=nano.specs selects the nano version of newlib
set(CMAKE_C_FLAGS
    "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nosys.specs --specs=nano.specs"
    CACHE STRING "" FORCE
)
set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITx_FLASH.ld")

# test the compiler can compile a static library rather than an executable
# an executable build will fail as no syscalls are implemented
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# configure cmake search
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
