add_executable(${PROJECT_NAME}.elf system.c main.c syscalls.c interrupts.c startup.s)
target_include_directories(${PROJECT_NAME}.elf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    ${PROJECT_NAME}.elf PRIVATE
    CMSIS_Core
    CMSIS_Device
    HAL
    BSP
    qtff
)
set_target_properties(
    ${PROJECT_NAME}.elf PROPERTIES
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../STM32F429ZITx_FLASH.ld
)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(
    TARGET ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_C_OBJCOPY} ARGS -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT "Converting to binary"
)

add_custom_target(flash DEPENDS ${PROJECT_NAME}.bin)
add_custom_command(
    TARGET flash
    USES_TERMINAL COMMAND st-flash --reset write ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin 0x8000000
    COMMENT "Flashing firmware"
)
