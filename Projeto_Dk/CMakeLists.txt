cmake_minimum_required(VERSION 3.20.0)
#LINHA ADICIONAL PELO ERRO DO OBJCOPY
set(CMAKE_OBJCOPY "C:/Users/fabio/zephyr-sdk-0.17.0/arm-zephyr-eabi/arm-zephyr-eabi/bin/objcopy.exe")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Projeto_Dk)

target_sources(app PRIVATE src/mainDk.c)

