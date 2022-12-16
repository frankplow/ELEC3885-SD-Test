set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set compiler
set(triple arm-none-eabi)
set(CMAKE_C_COMPILER ${triple}-gcc)
set(CMAKE_CXX_COMPILER ${triple}-g++)
set(CMAKE_C_OBJCOPY ${triple}-objcopy)

# test the compiler can compile a static library rather than an executable
# an executable build will fail as no syscalls are implemented
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# configure cmake search
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
