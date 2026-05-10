ESP32-S3 <--- USB OTG Adapter ---> USB Printer
### Arduino IDE setup
Add to Preferences -> Additional Boards Manager URLs:
```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```
Install the ESP32 from the library manager
- Board: ESP32S3 Dev Module
- PSRAM: OPI PSRAM (If your board version has it, otherwise "Disabled")



### Connecting the Printer
#### General:
1. Power off the ESP32-S3 and the printer
2. Connect the USB OTG adapter to the ESP32-S3
3. Connect the USB printer to the adapter
4. Power on the ESP32-S3
5. Power on the printer

#### On Windows:

### Printing

1. The server listens on port 9100
2. Add the ESP32-S3 as a network printer on your computer
3. Use the IP address and port 9100
4. Print from any application
  

Boards used and tested:

* ESP32-S3 SuperMini [AliExpress](https://www.aliexpress.com/item/1005006406538478.html)
* ESP32-S3WROOM1 N16R8 MCU Board [AliExpress](https://www.aliexpress.com/item/1005008796158734.html)

