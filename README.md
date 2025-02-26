<h1>Projeto1</h1>

> Status: Finished ✔️
> 
<h2>Sistema deteção de crianças</h2> 



* Zephyr RTOS
*  Nrf52840 DK
*  Nrf52840 Dongle
*  Raspberry Pi
*  Papertrail
*  Twilio

  <h3>BUILD + FLASHING</h3>
  
> NRF DK

```
west build -b nrf52840dk_nrf52840 Projeto_Dk --build-dir build_DK3.8
```
```
west flash --build-dir build_DK3.8
```



 > NRF Dongle
  
 ``` 
west build -b nrf52840dongle_nrf52840 Projeto_Dongle --build-dir build_Dongle6.2
```
  ``` 
nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build_Dongle6.2/zephyr/zephyr.hex --application-version 1 Projeto_Dongle6.2.zip
```

 ``` 
nrfutil dfu usb-serial -pkg Projeto_Dongle6.2.zip -p COM4
```
