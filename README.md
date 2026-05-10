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
1. Windows Settings -> Devices -> Printers & Scanners
2. Click "Add printer or scanner"
3. Wait for it to scan, then click "The printer that I want isn't listed"
4. Select "Add a printer using a TCP/IP address or hostname"
5. Device type: "TCP/IP Device"
6. Hostname or IP address: Enter the IP of your ESP32
7. Uncheck "Query the printer and automatically select the driver to use"
8. Click Next
9. Wait for it to fail to detect the printer
10. At device type select Custom and then next
11. Select you printer driver from the list and then click Next (Assuming you already have it installed)
12. Select "Keep the installed printer driver" and then click Next
13. Set you printers name and then click Next
14. Do not share the printer and click Next
15. Click Finish

### Printing

1. The server listens on port 9100
2. Add the ESP32-S3 as a network printer on your computer
3. Use the IP address and port 9100
4. Print from any application
  

### Boards used:

* ESP32-S3 SuperMini [AliExpress](https://www.aliexpress.com/item/1005006406538478.html)
* ESP32-S3WROOM1 N16R8 MCU Board [AliExpress](https://www.aliexpress.com/item/1005008796158734.html)

### Printer tested:

* Canon Pixma MG2550S [Cannon](https://www.canon-europe.com/support/consumer/products/printers/pixma/mg-series/pixma-mg2550s.html)

