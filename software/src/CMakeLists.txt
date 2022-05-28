file(GLOB FILES *.c *.h)

add_library(src ${FILES})

    target_link_libraries(
    src
    pico_stdlib
    hardware_i2c 
    hardware_spi 
    hardware_timer
    pico_multicore 
    )


target_include_directories(src PUBLIC ./include/)