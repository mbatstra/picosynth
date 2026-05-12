cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico2_w)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(picosynth C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(picosynth
  src/main.c
  src/kps.c
)

target_link_libraries(picosynth PRIVATE
  pico_stdlib
  hardware_gpio
  pico_audio_i2s
  pico_rand
  pico_time
  pico_util
)

target_compile_definitions(picosynth PRIVATE
  PICO_AUDIO_I2S_MONO_INPUT=1
  USE_AUDIO_I2S=1
)

target_include_directories(picosynth PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/inc
)

pico_enable_stdio_usb(picosynth 1)
pico_add_extra_outputs(picosynth)
