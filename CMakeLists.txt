cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi.cmake CACHE STRING "")

project(ELEC3885 C CXX ASM)

add_definitions(-DSTM32F429xx)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    stm32cube
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF4
    GIT_TAG 52757b5e33259a088509a777a9e3a5b971194c7d # v1.27.1
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stm32cube)

# CMSIS Core
add_library(CMSIS_Core INTERFACE)
target_include_directories(CMSIS_Core INTERFACE ${stm32cube_SOURCE_DIR}/Drivers/CMSIS/Core/Include)

# CMSIS Device
set(cmsis_device_root ${stm32cube_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx)
add_library(CMSIS_Device INTERFACE)
target_include_directories(CMSIS_Device INTERFACE ${cmsis_device_root}/Include)
target_link_libraries(CMSIS_Device INTERFACE CMSIS_Core)

# HAL
set(hal_root ${stm32cube_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver)
add_library(
    HAL INTERFACE
)
target_sources(HAL INTERFACE
    ${hal_root}/Src/stm32f4xx_hal.c
    ${hal_root}/Src/stm32f4xx_hal_adc.c
    ${hal_root}/Src/stm32f4xx_hal_adc_ex.c
    ${hal_root}/Src/stm32f4xx_hal_can.c
    ${hal_root}/Src/stm32f4xx_hal_cec.c
    ${hal_root}/Src/stm32f4xx_hal_cortex.c
    ${hal_root}/Src/stm32f4xx_hal_crc.c
    ${hal_root}/Src/stm32f4xx_hal_cryp.c
    ${hal_root}/Src/stm32f4xx_hal_cryp_ex.c
    ${hal_root}/Src/stm32f4xx_hal_dac.c
    ${hal_root}/Src/stm32f4xx_hal_dac_ex.c
    ${hal_root}/Src/stm32f4xx_hal_dcmi.c
    ${hal_root}/Src/stm32f4xx_hal_dcmi_ex.c
    ${hal_root}/Src/stm32f4xx_hal_dfsdm.c
    ${hal_root}/Src/stm32f4xx_hal_dma.c
    ${hal_root}/Src/stm32f4xx_hal_dma2d.c
    ${hal_root}/Src/stm32f4xx_hal_dma_ex.c
    ${hal_root}/Src/stm32f4xx_hal_dsi.c
    ${hal_root}/Src/stm32f4xx_hal_eth.c
    ${hal_root}/Src/stm32f4xx_hal_exti.c
    ${hal_root}/Src/stm32f4xx_hal_flash.c
    ${hal_root}/Src/stm32f4xx_hal_flash_ex.c
    ${hal_root}/Src/stm32f4xx_hal_flash_ramfunc.c
    ${hal_root}/Src/stm32f4xx_hal_fmpi2c.c
    ${hal_root}/Src/stm32f4xx_hal_fmpi2c_ex.c
    ${hal_root}/Src/stm32f4xx_hal_fmpsmbus.c
    ${hal_root}/Src/stm32f4xx_hal_fmpsmbus_ex.c
    ${hal_root}/Src/stm32f4xx_hal_gpio.c
    ${hal_root}/Src/stm32f4xx_hal_hash.c
    ${hal_root}/Src/stm32f4xx_hal_hash_ex.c
    ${hal_root}/Src/stm32f4xx_hal_hcd.c
    ${hal_root}/Src/stm32f4xx_hal_i2c.c
    ${hal_root}/Src/stm32f4xx_hal_i2c_ex.c
    ${hal_root}/Src/stm32f4xx_hal_i2s.c
    ${hal_root}/Src/stm32f4xx_hal_i2s_ex.c
    ${hal_root}/Src/stm32f4xx_hal_irda.c
    ${hal_root}/Src/stm32f4xx_hal_iwdg.c
    ${hal_root}/Src/stm32f4xx_hal_lptim.c
    ${hal_root}/Src/stm32f4xx_hal_ltdc.c
    ${hal_root}/Src/stm32f4xx_hal_ltdc_ex.c
    ${hal_root}/Src/stm32f4xx_hal_mmc.c
    ${hal_root}/Src/stm32f4xx_hal_nand.c
    ${hal_root}/Src/stm32f4xx_hal_nor.c
    ${hal_root}/Src/stm32f4xx_hal_pccard.c
    ${hal_root}/Src/stm32f4xx_hal_pcd.c
    ${hal_root}/Src/stm32f4xx_hal_pcd_ex.c
    ${hal_root}/Src/stm32f4xx_hal_pwr.c
    ${hal_root}/Src/stm32f4xx_hal_pwr_ex.c
    ${hal_root}/Src/stm32f4xx_hal_qspi.c
    ${hal_root}/Src/stm32f4xx_hal_rcc.c
    ${hal_root}/Src/stm32f4xx_hal_rcc_ex.c
    ${hal_root}/Src/stm32f4xx_hal_rng.c
    ${hal_root}/Src/stm32f4xx_hal_rtc.c
    ${hal_root}/Src/stm32f4xx_hal_rtc_ex.c
    ${hal_root}/Src/stm32f4xx_hal_sai.c
    ${hal_root}/Src/stm32f4xx_hal_sai_ex.c
    ${hal_root}/Src/stm32f4xx_hal_sd.c
    ${hal_root}/Src/stm32f4xx_hal_sdram.c
    ${hal_root}/Src/stm32f4xx_hal_smartcard.c
    ${hal_root}/Src/stm32f4xx_hal_smbus.c
    ${hal_root}/Src/stm32f4xx_hal_spdifrx.c
    ${hal_root}/Src/stm32f4xx_hal_spi.c
    ${hal_root}/Src/stm32f4xx_hal_sram.c
    ${hal_root}/Src/stm32f4xx_hal_tim.c
    ${hal_root}/Src/stm32f4xx_hal_tim_ex.c
    ${hal_root}/Src/stm32f4xx_hal_uart.c
    ${hal_root}/Src/stm32f4xx_hal_usart.c
    ${hal_root}/Src/stm32f4xx_hal_wwdg.c
    ${hal_root}/Src/stm32f4xx_ll_adc.c
    ${hal_root}/Src/stm32f4xx_ll_crc.c
    ${hal_root}/Src/stm32f4xx_ll_dac.c
    ${hal_root}/Src/stm32f4xx_ll_dma.c
    ${hal_root}/Src/stm32f4xx_ll_dma2d.c
    ${hal_root}/Src/stm32f4xx_ll_exti.c
    ${hal_root}/Src/stm32f4xx_ll_fmc.c
    ${hal_root}/Src/stm32f4xx_ll_fmpi2c.c
    ${hal_root}/Src/stm32f4xx_ll_fsmc.c
    ${hal_root}/Src/stm32f4xx_ll_gpio.c
    ${hal_root}/Src/stm32f4xx_ll_i2c.c
    ${hal_root}/Src/stm32f4xx_ll_lptim.c
    ${hal_root}/Src/stm32f4xx_ll_pwr.c
    ${hal_root}/Src/stm32f4xx_ll_rcc.c
    ${hal_root}/Src/stm32f4xx_ll_rng.c
    ${hal_root}/Src/stm32f4xx_ll_rtc.c
    ${hal_root}/Src/stm32f4xx_ll_sdmmc.c
    ${hal_root}/Src/stm32f4xx_ll_spi.c
    ${hal_root}/Src/stm32f4xx_ll_tim.c
    ${hal_root}/Src/stm32f4xx_ll_usart.c
    ${hal_root}/Src/stm32f4xx_ll_usb.c
    ${hal_root}/Src/stm32f4xx_ll_utils.c
)
target_include_directories(HAL INTERFACE ${hal_root}/Inc)
target_link_libraries(HAL INTERFACE CMSIS_Device)

# BSP
set(bsp_root ${stm32cube_SOURCE_DIR}/Drivers/BSP/STM32F4xx_Nucleo_144)
add_library(BSP INTERFACE)
target_sources(BSP INTERFACE ${bsp_root}/stm32f4xx_nucleo_144.c)
target_include_directories(BSP INTERFACE ${bsp_root})
target_link_libraries(BSP INTERFACE HAL)

add_subdirectory(src)

if(CMAKE_TESTING_ENABLED)
    # GoogleTest
    set(CMAKE_CXX_STANDARD 14)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(CTest)
    include(GoogleTest)

    add_subdirectory(tests)
endif()

find_package(Doxygen)
configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating documentation"
)
