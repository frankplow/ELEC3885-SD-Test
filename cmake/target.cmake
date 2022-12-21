if((CMAKE_SYSTEM_NAME STREQUAL Generic) AND (CMAKE_SYSTEM_PROCESSOR STREQUAL arm))
    include(stmcube)
    add_definitions(-DSTM32F429xx)

    add_subdirectory(src)
else()
    # taken from: https://stackoverflow.com/questions/25533831/passing-cmake-variables-to-externalproject-add
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    foreach(CACHE_VAR ${CACHE_VARS})
      get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
      if(CACHE_VAR_HELPSTRING STREQUAL "No help, variable specified on the command line.")
        get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
        if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
          set(CACHE_VAR_TYPE)
        else()
          set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
        endif()
        list(APPEND CMAKE_ARGS "-D${CACHE_VAR}${CACHE_VAR_TYPE}=${${CACHE_VAR}}")
      endif()
    endforeach()

    include(ExternalProject)

    set(toolchain_path ${PROJECT_SOURCE_DIR}/cmake/arm-none-eabi.cmake)
    set(target_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/target)

    ExternalProject_Add(
        target
        SOURCE_DIR ${PROJECT_SOURCE_DIR}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/target
        INSTALL_COMMAND ""
        BUILD_ALWAYS ON
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${toolchain_path}
            -DELEC3885_BUILD_TESTS:BOOL=OFF
            -DELEC3885_BUILD_DOCS:BOOL=OFF
    )

    add_custom_target(flash DEPENDS target)
    add_custom_command(
        TARGET flash
        USES_TERMINAL COMMAND st-flash --reset write ${target_binary_dir}/src/${PROJECT_NAME}.bin 0x8000000
        COMMENT "Flashing firmware"
    )
endif()
