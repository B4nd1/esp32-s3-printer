#include <Arduino.h>
#include <WiFi.h>
#include "usb/usb_host.h" 

const char* ssid     = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";
const int   PORT     = 9100;
const int   LED_PIN  = 8;

WiFiServer wifiServer(PORT);
WiFiClient wifiClient;
usb_host_client_handle_t ClientHandle;

enum SystemState { SEARCHING, READY, PRINTING };
SystemState currentState = SEARCHING;

class PrinterDriver {
public:
    usb_device_handle_t deviceHandle = NULL;
    uint8_t outEndpointAddress = 0;
    uint8_t interfaceNumber = 0;
    bool isReady = false;

    bool discoverPrinter(usb_device_handle_t devHdl) {
        const usb_config_desc_t* config_desc;
        if (usb_host_get_active_config_descriptor(devHdl, &config_desc) != ESP_OK) return false;

        const uint8_t *p = (const uint8_t *)config_desc;
        const uint8_t *end = (const uint8_t *)config_desc + config_desc->wTotalLength;

        // Reset state
        isReady = false;
        deviceHandle = devHdl;

        Serial0.println("Scanning USB Descriptors...");
        
        while (p < end) {
            if (p[1] == USB_B_DESCRIPTOR_TYPE_INTERFACE) {
                const usb_intf_desc_t *intf = (const usb_intf_desc_t *)p;
                interfaceNumber = intf->bInterfaceNumber;
                
                if (intf->bInterfaceClass == 0x07) {
                    Serial0.printf("Found Printer Interface: %d\n", interfaceNumber);
                    
                    const uint8_t *ep_p = p + intf->bLength;
                    while (ep_p < end && ep_p[1] != USB_B_DESCRIPTOR_TYPE_INTERFACE) {
                        if (ep_p[1] == USB_B_DESCRIPTOR_TYPE_ENDPOINT) {
                            const usb_ep_desc_t *ep = (const usb_ep_desc_t *)ep_p;
                            if (((ep->bmAttributes & 0x03) == USB_TRANSFER_TYPE_BULK) && 
                                ((ep->bEndpointAddress & 0x80) == 0)) {
                                
                                outEndpointAddress = ep->bEndpointAddress;
                                Serial0.printf("Found Bulk OUT Endpoint: 0x%02X\n", outEndpointAddress);
                                isReady = true;
                                return true;
                            }
                        }
                        ep_p += ep_p[0];
                    }
                }
            }
            p += p[0];
        }
        return false;
    }

    void send(uint8_t* data, size_t len) {
        if (!isReady || deviceHandle == NULL) return;
        usb_transfer_t* transfer;
        usb_host_transfer_alloc(len, 0, &transfer);
        if (transfer) {
            transfer->device_handle = deviceHandle;
            transfer->bEndpointAddress = outEndpointAddress;
            transfer->callback = [](usb_transfer_t *t) { usb_host_transfer_free(t); };
            transfer->num_bytes = len;
            memcpy(transfer->data_buffer, data, len);
            usb_host_transfer_submit(transfer);
        }
    }
};

PrinterDriver myPrinter;

void client_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg) {
    if (event_msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV) {
        usb_device_handle_t devHdl;
        if (usb_host_device_open(ClientHandle, event_msg->new_dev.address, &devHdl) == ESP_OK) {
            if (myPrinter.discoverPrinter(devHdl)) {
                usb_host_interface_claim(ClientHandle, devHdl, myPrinter.interfaceNumber, 0);
                currentState = READY;
                Serial0.println("Printer Status: READY");
            } else {
                usb_host_device_close(ClientHandle, devHdl);
            }
        }
    } 
    else if (event_msg->event == USB_HOST_CLIENT_EVENT_DEV_GONE) {
        myPrinter.isReady = false;
        myPrinter.deviceHandle = NULL;
        currentState = SEARCHING;
        Serial0.println("Printer Disconnected.");
    }
}

void updateLED() {
    static uint32_t lastToggle = 0;
    static bool ledState = false;
    uint32_t now = millis();

    switch (currentState) {
        case SEARCHING:
            if (now - lastToggle > 100) { // Fast Blink
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState);
                lastToggle = now;
            }
            break;
        case READY:
            digitalWrite(LED_PIN, HIGH); // Solid ON
            break;
        case PRINTING:
            if (now - lastToggle > 30) { // Flicker
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState);
                lastToggle = now;
            }
            break;
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial0.begin(115200, SERIAL_8N1, 44, 43);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        updateLED(); 
    }
    wifiServer.begin();
    
    const usb_host_config_t host_config = { .skip_phy_setup = false, .intr_flags = ESP_INTR_FLAG_LEVEL1 };
    usb_host_install(&host_config);
    
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async = { .client_event_callback = client_event_callback, .callback_arg = NULL }
    };
    usb_host_client_register(&client_config, &ClientHandle);
}

void loop() {
    usb_host_lib_handle_events(0, NULL);
    usb_host_client_handle_events(ClientHandle, 0);
    updateLED();

    WiFiClient newClient = wifiServer.available();
    if (newClient) wifiClient = newClient;

    if (wifiClient && wifiClient.connected()) {
        if (wifiClient.available()) {
            uint8_t buffer[512]; 
            int len = wifiClient.read(buffer, sizeof(buffer));
            if (len > 0 && myPrinter.isReady) {
                currentState = PRINTING;
                myPrinter.send(buffer, len);
                delay(10); 
                currentState = READY;
            }
        }
    }
}