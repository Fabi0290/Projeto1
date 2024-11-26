# SPDX-License-Identifier: Apache-2.0

# Defina a versão mínima do CMake
cmake_minimum_required(VERSION 3.20.0)

# Encontre o Zephyr
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Defina o nome do projeto
project(Projeto1)

# Adicione o código fonte
target_sources(app PRIVATE src/main.c)

# Configurações específicas para o BLE
target_compile_definitions(app PRIVATE CONFIG_BT=y)  # Habilitar Bluetooth
target_compile_definitions(app PRIVATE CONFIG_BT_PERIPHERAL=y)  # Configurar o dispositivo como periférico BLE
target_compile_definitions(app PRIVATE CONFIG_BT_DEVICE_NAME="nRF_BLE_Device")  # Nome do dispositivo
