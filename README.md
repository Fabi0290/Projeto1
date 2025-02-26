Projeto1
  *Sistema deteção de crianças
  *Zephyr Rtos + nrf52840 dk + nrf52840 dongle
  *Raspberry Pi
  *app.databay + twilio

  //BUILDS

  //Dongle
  west build -b nrf52840dongle_nrf52840 Projeto_Dongle --build-dir build_Dongle6.2
nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build_Dongle6.2/zephyr/zephyr.hex --application-version 1 Projeto_Dongle6.2.zip
nrfutil dfu usb-serial -pkg Projeto_Dongle6.2.zip -p COM4
