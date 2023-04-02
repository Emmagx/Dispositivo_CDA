#include <WiFiManager.h> 

void setup() {

WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    
    WiFiManager wiFiManager;

    // reset settings - wipe stored credentials for testing
    wiFiManager.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wiFiManager.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wiFiManager.autoConnect(); // auto generated AP name from chipid
    // res = wiFiManager.autoConnect("AutoConnectAP"); // anonymous ap
    res = wiFiManager.autoConnect("AutoConnectAP","12345678"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect!");
        // ESP.restart();
    } 
    else {
        Serial.println("Connected :)");
        // Serial.println("IP address: ");
        // Serial.println(WiFi.localIP());   
    }

}

void loop() {
    digitalWrite(2, LOW);
    delay(1000);
    digitalWrite(2, HIGH);
    delay(1000);
}
