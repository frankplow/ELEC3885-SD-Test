add_executable(elec3885 system_stm32f4xx.c main.c syscalls.c interrupts.c startup_stm32f429xx.s)
target_include_directories(elec3885 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(
    elec3885 PRIVATE
    CMSIS_Core
    CMSIS_Device
    HAL
    BSP
)
set_target_properties(
    elec3885 PROPERTIES
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../STM32F429ZITx_FLASH.ld
    SUFFIX ".elf"
)
