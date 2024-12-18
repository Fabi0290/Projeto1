# SPDX-License-Identifier: Apache-2.0
#LINHA ADICIONAL PELO ERRO DO OBJCOPY
set(CMAKE_OBJCOPY "C:/Users/fabio/zephyr-sdk-0.17.0/arm-zephyr-eabi/arm-zephyr-eabi/bin/objcopy.exe")
# Defina a versão mínima do CMake
cmake_minimum_required(VERSION 3.20.0)

# Encontre o Zephyr
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Defina o nome do projeto
project(Projeto_Dongle)

# Adicione o código fonte principal
target_sources(app PRIVATE src/mainDG.c)
