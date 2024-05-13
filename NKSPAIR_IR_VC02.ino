// ===============================| Including Libraries and calling classes |=======================================

    #include "RMaker.h"
    #include "WiFi.h"
    #include "WiFiProv.h"
    #include <Preferences.h> 
    #include <AceButton.h>
    #include <RCSwitch.h>
    #include <Ticker.h>
    #include <IRremote.h>
    #include "GroveOfflineSensor.h"
    using namespace ace_button;
    Preferences pref;
    RCSwitch rfReceiver;
    Ticker tick1, tick2, tick3, tick4;

// -------------------------------| Including Libraries and calling classes |--------------------------------


// ===============================| I/O Pins |=======================================

    uint32_t espChipId = 5;
    static uint8_t RelayPin1 = 23;  //D23
    static uint8_t RelayPin2 = 22;  //D22
    static uint8_t RelayPin3 = 21;  //D21
    static uint8_t RelayPin4 = 19;  //D19

    static uint8_t SwitchPin1 = 13;  //D13
    static uint8_t SwitchPin2 = 12;  //D12
    static uint8_t SwitchPin3 = 14;  //D14
    static uint8_t SwitchPin4 = 27;  //D27
    static uint8_t SwitchPin5 = 26;  //D26  

    static uint8_t FanRelay1 = 18;  //D18
    static uint8_t FanRelay2 = 5 ;  //D5
    static uint8_t FanRelay3 = 17;  //D17

    static uint8_t FanSwitch1 = 33;  //D33
    static uint8_t FanSwitch2 = 32;  //D32
    static uint8_t FanSwitch3 = 15;  //D15
    static uint8_t FanSwitch4 = 34;  //D34


    static uint8_t gpio_reset  = 0 ;   // Press BOOT to reset WiFi Details
    static uint8_t wifiLed     = 2 ;   //D2
    static uint8_t Buzzer       = 16;   //D16  pin connected with DHT
    static uint8_t RF_PIN      = 4;   //D4  pin connected with RF
    static uint8_t IR_RECV_PIN = 35;   // D35 (IR receiver pin)

    static uint8_t RX_pin = 3; //UART 0 RX
    static uint8_t TX_pin = 1; //UART 0 TX

// ----------------------------------------| I/O Pins |--------------------------------------------------


// ===============================| IR Values |=======================================

    IRrecv irrecv(IR_RECV_PIN);
    decode_results results;

  #define IR_Button_1   0xCF7095CD
  #define IR_Button_2   0xA215B7E2
  #define IR_Button_3   0xCE70943A
  #define IR_Button_4   0xA86E19D3
  #define IR_Button_5   0x9E856ADE
  #define IR_Fan_Up     0x5BBC35BE
  #define IR_Fan_Down   0xB23554F
  #define IR_All        0x499B750A

// ------------------------------| IR Values |------------------------------------------


// ===============================| RF Values |=======================================

    bool skip=0;
    bool update_rf_value=0;

    int RF_Button_A2;
    int RF_Button_A1; 
    int RF_Button_A3;
    int RF_Button_A4;
    int RF_Fan_A;
    int RF_Fan_Up_A;
    int RF_Fan_Down_A;
    int RF_All_A;


    int RF_Button_B1; 
    int RF_Button_B2;
    int RF_Button_B3;
    int RF_Button_B4;
    int RF_Fan_B;
    int RF_Fan_Up_B;
    int RF_Fan_Down_B;
    int RF_All_B;

    int RF_Button_C1; 
    int RF_Button_C2;
    int RF_Button_C3;
    int RF_Button_C4;
    int RF_Fan_C;
    int RF_Fan_Up_C;
    int RF_Fan_Down_C;
    int RF_All_C;

// ------------------------------| RF Values |------------------------------------------


// ===========================| Initilize the name in esprainmaker |==================================

    char nodeName[] = "SMARTLINK 00002";
    char deviceName_1[] = "Switch1";
    char deviceName_2[] = "Switch2";
    char deviceName_3[] = "Switch3";
    char deviceName_4[] = "Switch4";
    char deviceName_5[] = "Fan";
    const char *service_name = "NKSPAIR_SMARTLINK";
    const char *pop = "00002";
    SoftwareSerial groveSerial(RX_pin, TX_pin); 

// ---------------------------| Initilize the name in esprainmaker |-------------------------------------



// ===============================| Variables |=======================================

    int currSpeed = 0;
    int rf_counter1=1;
    unsigned long start_rf1;
    unsigned long end_rf1;
    int rf_counter2=1;
    unsigned long start_rf2;
    unsigned long end_rf2;
    int rf_counter3=1;
    unsigned long start_rf3;
    unsigned long end_rf3;
    int Reset_counter =1;
    unsigned long lastReceivedTime = 0;
    unsigned long start_reset;
    unsigned long end_reset;
    const unsigned long receiveDelay = 300;  // 300 milliseconds

// -----------------------------| Variables |------------------------------------


// ===============================| Relay State |=======================================

    bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
    bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
    bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
    bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4
    bool toggleState_5 = LOW; //Define integer to remember the toggle state for relay 5

    bool fanSpeed_0 = LOW;
    bool fanSpeed_1 = LOW; 
    bool fanSpeed_2 = LOW; 
    bool fanSpeed_3 = LOW; 
    bool fanSpeed_4 = LOW;
    int wifiFlag = 0;
    bool first_run = true;

// ------------------------------| Relay State |-----------------------------------------


// ===============================| Initializing The Acebutton |=======================================

    ButtonConfig config1;
    AceButton button1(&config1);
    ButtonConfig config2;
    AceButton button2(&config2);
    ButtonConfig config3;
    AceButton button3(&config3);
    ButtonConfig config4;
    AceButton button4(&config4);
    ButtonConfig config5;
    AceButton button5(&config5);

    void handleEvent1(AceButton*, uint8_t, uint8_t);
    void handleEvent2(AceButton*, uint8_t, uint8_t);
    void handleEvent3(AceButton*, uint8_t, uint8_t);
    void handleEvent4(AceButton*, uint8_t, uint8_t);
    void handleEvent5(AceButton*, uint8_t, uint8_t);

// -------------------------------| Initializing The Acebutton |-------------------------------------


// ===============================| Defining the EspRainmaker Elements |=======================================

    static Switch my_switch1;
    static Switch my_switch2;
    static Switch my_switch3;
    static Switch my_switch4;
    static Fan my_fan;

    void sysProvEvent(arduino_event_t *sys_event){ 
        switch (sys_event->event_id) {
            case ARDUINO_EVENT_PROV_START:
                #if CONFIG_IDF_TARGET_ESP32S2
                    Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
                    printQR(service_name, pop, "softap");
                #else
                    Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
                    printQR(service_name, pop, "ble");
                #endif
                break;
            case ARDUINO_EVENT_PROV_INIT:
                wifi_prov_mgr_disable_auto_stop(10000);
                break;
            case ARDUINO_EVENT_PROV_CRED_SUCCESS:
                wifi_prov_mgr_stop_provisioning();
                break;
            default:;
        }
    }


    void write_callback(Device *device,   Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx){
        const char *device_name = device->getDeviceName();
        const char *param_name = param->getParamName();

        if(strcmp(device_name, deviceName_1) == 0) {
            Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");
            if(strcmp(param_name, "Power") == 0) {
                Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
                toggleState_1 = val.val.b;
                (toggleState_1 == false) ? digitalWrite(RelayPin1, HIGH) : digitalWrite(RelayPin1, LOW);
                param->updateAndReport(val);
                pref.putBool("Relay1", toggleState_1);
            }
        } 

        else if(strcmp(device_name, deviceName_2) == 0) {
            Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");
            if(strcmp(param_name, "Power") == 0) {
                Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
                toggleState_2 = val.val.b;
                (toggleState_2 == false) ? digitalWrite(RelayPin2, HIGH) : digitalWrite(RelayPin2, LOW);
                param->updateAndReport(val);
                pref.putBool("Relay2", toggleState_2);
            }
        }

        else if(strcmp(device_name, deviceName_3) == 0) {
            Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");
            if(strcmp(param_name, "Power") == 0) {
                Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
                toggleState_3 = val.val.b;
                (toggleState_3 == false) ? digitalWrite(RelayPin3, HIGH) : digitalWrite(RelayPin3, LOW);
                param->updateAndReport(val);
                pref.putBool("Relay3", toggleState_3);
            }
        }

        else if(strcmp(device_name, deviceName_4) == 0) {
            Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");
            if(strcmp(param_name, "Power") == 0) {
                Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
                toggleState_4 = val.val.b;
                (toggleState_4 == false) ? digitalWrite(RelayPin4, HIGH) : digitalWrite(RelayPin4, LOW);
                param->updateAndReport(val);
                pref.putBool("Relay4", toggleState_4);
            }     
        } 

        else if(strcmp(device_name, deviceName_5) == 0) {
            if (strcmp(param_name, "Power") == 0){
                Serial.printf("Received Fan power = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
                toggleState_5 = val.val.b;
                (toggleState_5 == false) ? fanSpeedControl(0) : fanSpeedControl(currSpeed);
                param->updateAndReport(val);
                pref.putBool("Fan_Power", toggleState_5);
            }  
            if (strcmp(param_name, "My_Speed") == 0){
                Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
                currSpeed = val.val.i;
                if(toggleState_5 == 1){
                    fanSpeedControl(currSpeed);
                }
                param->updateAndReport(val);
                pref.putInt("Fan_Speed", currSpeed);
            }
        }
    }

// --------------------------------| Defining the EspRainmaker Elements |------------------------------------


// ===============================| IR Remote Control |=======================================

    void ir_remote(){
        if (irrecv.decode(&results)) {
            Serial.println(results.value);
            switch(results.value){
                case IR_Button_1:  
                    digitalWrite(RelayPin1, toggleState_1);
                    toggleState_1 = !toggleState_1;
                    pref.putBool("Relay1", toggleState_1);
                    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
                    delay(100);            
                    break;
                case IR_Button_2:  
                    digitalWrite(RelayPin2, toggleState_2);
                    toggleState_2 = !toggleState_2;
                    pref.putBool("Relay2", toggleState_2);
                    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
                    delay(100);            
                    break;
                case IR_Button_3:  
                    digitalWrite(RelayPin3, toggleState_3);
                    toggleState_3 = !toggleState_3;
                    pref.putBool("Relay3", toggleState_3);
                    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
                    delay(100);            
                    break;
                case IR_Button_4:  
                    digitalWrite(RelayPin4, toggleState_4);
                    toggleState_4 = !toggleState_4;
                    pref.putBool("Relay4", toggleState_4);
                    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
                    delay(100);            
                    break;
                case IR_Button_5:   
                    if(toggleState_5 == 0){
                        fanSpeedControl(currSpeed); //Turn ON Fan
                    }
                    else {
                        fanSpeedControl(0); //Turn OFF Fan
                    }
                    toggleState_5 = !toggleState_5;
                    pref.putBool("Fan_Power", toggleState_5);
                    my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
                    delay(100);            
                    break;
                case IR_Fan_Up: 
                    if(currSpeed < 4 && toggleState_5 == 1){
                        currSpeed = currSpeed + 1;
                        fanSpeedControl(currSpeed);
                        pref.putInt("Fan_Speed", currSpeed);
                        my_fan.updateAndReportParam("My_Speed", currSpeed);
                        delay(100); 
                    }
                    break;
                case IR_Fan_Down: 
                    if(currSpeed > 0 && toggleState_5 == 1){
                        currSpeed = currSpeed - 1;
                        fanSpeedControl(currSpeed);
                        pref.putInt("Fan_Speed", currSpeed);
                        my_fan.updateAndReportParam("My_Speed", currSpeed);
                        delay(100); 
                    }       
                    break;
                case IR_All:
                    Serial.println("OFF");
                    ESP.restart();
                    delay(100);
                default : break;         
            }   
            irrecv.resume();   
        } 
    }

// ---------------------------------| IR Remote Control |----------------------------------


// ===============================| RF Remote Control |=======================================

    void rf_remote(){
        if (rfReceiver.available()) {
            unsigned long currentTime = millis();
            if (currentTime - lastReceivedTime >= receiveDelay) {
                int value = rfReceiver.getReceivedValue();
                Serial.print("Value received= ");
                Serial.println(value);
                if(value==RF_Button_A1 || value==RF_Button_B1 || value==RF_Button_C1){  
                    digitalWrite(RelayPin1, toggleState_1);
                    toggleState_1 = !toggleState_1;
                    pref.putBool("Relay1", toggleState_1);
                    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
                    delay(100);            
                }
                if(value==RF_Button_A2 || value==RF_Button_B2 || value==RF_Button_C2){  
                    digitalWrite(RelayPin2, toggleState_2);
                    toggleState_2 = !toggleState_2;
                    pref.putBool("Relay2", toggleState_2);
                    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
                }
                if(value==RF_Button_A3 || value==RF_Button_B3 || value==RF_Button_C3){ 
                    digitalWrite(RelayPin3, toggleState_3);
                    toggleState_3 = !toggleState_3;
                    pref.putBool("Relay3", toggleState_3);
                    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
                    delay(100);            
                }
                if(value==RF_Button_A4 || value==RF_Button_B4 || value==RF_Button_C4){
                    digitalWrite(RelayPin4, toggleState_4);
                    toggleState_4 = !toggleState_4;
                    pref.putBool("Relay4", toggleState_4);
                    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
                    delay(100);            
                }
                if(value==RF_Fan_A || value==RF_Fan_B || value==RF_Fan_C){
                    if(toggleState_5 == 0){
                        fanSpeedControl(currSpeed); //Turn ON Fan
                    }
                    else {
                        fanSpeedControl(0); //Turn OFF Fan
                    }
                    toggleState_5 = !toggleState_5;
                    pref.putBool("Fan_Power", toggleState_5);
                    my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
                    delay(100);            
                }
                if(value==RF_Fan_Up_A || value==RF_Fan_Up_B || value==RF_Fan_Up_C){
                    if(currSpeed < 4 && toggleState_5 == 1){
                        currSpeed = currSpeed + 1;
                        fanSpeedControl(currSpeed);
                        pref.putInt("Fan_Speed", currSpeed);
                        my_fan.updateAndReportParam("My_Speed", currSpeed);
                        delay(100); 
                    }           
                }
                if(value==RF_Fan_Down_A || value==RF_Fan_Down_B || value==RF_Fan_Down_C){
                    if(currSpeed > 0 && toggleState_5 == 1){
                        currSpeed = currSpeed - 1;
                        fanSpeedControl(currSpeed);
                        pref.putInt("Fan_Speed", currSpeed);
                        my_fan.updateAndReportParam("My_Speed", currSpeed);
                        delay(100); 
                    }       
                }
                if(value==RF_All_A || value==RF_All_B || value==RF_All_C){
                    Serial.println("OFF");
                    ESP.restart();
                    delay(100);     
                }   
            } 
            lastReceivedTime = currentTime;
        }
        rfReceiver.resetAvailable();
    }

// ---------------------------------| RF Remote Control |----------------------------------


// ===============================| Update Rf Remote Code |=======================================


    void remote_select(int Remote_no, int value, char Rf_button[]){
        if(Rf_button=="Button1"){
            if(Remote_no==1){
                RF_Button_A1=value;
                pref.putInt("RF_Button_A1", value);
            }
            else if(Remote_no==2){
                RF_Button_B1=value;
                pref.putInt("RF_Button_B1", value);
            }
            else if(Remote_no==3){
                RF_Button_C1=value;
                pref.putInt("RF_Button_C1", value);
            }
        }

        else if(Rf_button=="Button2"){
            if(Remote_no==1){
                RF_Button_A2=value;
                pref.putInt("RF_Button_A2", value);
            }
            else if(Remote_no==2){
                RF_Button_B2=value;
                pref.putInt("RF_Button_B2", value);
            }
            else if(Remote_no==3){
                RF_Button_C2=value;
                pref.putInt("RF_Button_C2", value);
            }
        }

        else if(Rf_button=="Button3"){
            if(Remote_no==1){
                RF_Button_A3=value;
                pref.putInt("RF_Button_A3", value);
            }
            else if(Remote_no==2){
                RF_Button_B3=value;
                pref.putInt("RF_Button_B3", value);
            }
            else if(Remote_no==3){
                RF_Button_C3=value;
                pref.putInt("RF_Button_C3", value);
            }
        }

        else if(Rf_button=="Button4"){
            if(Remote_no==1){
                RF_Button_A4=value;
                pref.putInt("RF_Button_A4", value);
            }
            else if(Remote_no==2){
                RF_Button_B4=value;
                pref.putInt("RF_Button_B4", value);
            }
            else if(Remote_no==3){
                RF_Button_C4=value;
                pref.putInt("RF_Button_C4", value);
            }
        }

        else if(Rf_button=="Fan"){
            if(Remote_no==1){
                RF_Fan_A=value;
                pref.putInt("RF_Fan_A", value);
            }
            else if(Remote_no==2){
                RF_Fan_B=value;
                pref.putInt("RF_Fan_B", value);
            }
            else if(Remote_no==3){
                RF_Fan_C=value;
                pref.putInt("RF_Fan_C", value);
            }
        }

        else if(Rf_button=="FanUp"){
            if(Remote_no==1){
                RF_Fan_Up_A=value;
                pref.putInt("RF_Fan_Up_A", value);
            }
            else if(Remote_no==2){
                RF_Fan_Up_B=value;
                pref.putInt("RF_Fan_Up_B", value);
            }
            else if(Remote_no==3){
                RF_Fan_Up_C=value;
                pref.putInt("RF_Fan_Up_C", value);
            }
        }

        else if(Rf_button=="FanDown"){
            if(Remote_no==1){
                RF_Fan_Down_A=value;
                pref.putInt("RF_Fan_Down_A", value);
            }
            else if(Remote_no==2){
                RF_Fan_Down_B=value;
                pref.putInt("RF_Fan_Down_B", value);
            }
            else if(Remote_no==3){
                RF_Fan_Down_C=value;
                pref.putInt("RF_Fan_Down_C", value);
            }
        }

        else if(Rf_button=="All"){
            if(Remote_no==1){
                RF_All_A=value;
                pref.putInt("RF_All_A", value);
            }
            else if(Remote_no==2){
                RF_All_B=value;
                pref.putInt("RF_All_B", value);
            }
            else if(Remote_no==3){
                RF_All_C=value;
                pref.putInt("RF_All_C", value);
            }
        }
    }


    void update_RF(int Remote_no){
        skip=0;
        update_rf_value=1;
        tick1.detach();
        buzz(3,100);
        delay(2000);
        buzz(3,100);
        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "Button1");
                    Serial.println("Updated 1");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();    
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "Button2");
                    Serial.println("Updated 2");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "Button3");
                    Serial.println("Updated 3");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
            unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "Button4");
                    Serial.println("Updated 4");
                lastReceivedTime = currentTime;
                rfReceiver.resetAvailable();
                break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "Fan");
                    Serial.println("Updated Fan");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "FanUp");
                    Serial.println("Updated Fan_Up");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
                skip=0;
                delay(1000);
                buzz(2,300);
                break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "FanDown");
                    Serial.println("Updated Fan_Down");
                    lastReceivedTime = currentTime;
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }

        while(1){
            button1.check();
            button2.check();
            button3.check();
            button4.check();
            button5.check();
            if(skip==1){
            skip=0;
            delay(1000);
            buzz(4,100);
            update_rf_value=0;
            break;
            }
            if (rfReceiver.available()) {
                unsigned long currentTime = millis();
                if (currentTime - lastReceivedTime >= receiveDelay) {
                    int value = rfReceiver.getReceivedValue();
                    buzz(1,300);
                    remote_select(Remote_no, value, "All");
                    Serial.println("Updated RF_all");
                    delay(500);
                    buzz(4,100);
                    update_rf_value=0;
                    lastReceivedTime = currentTime;
                    delay(500);
                    rfReceiver.resetAvailable();
                    break;
                }
            }
        }
        tick1.attach_ms(16, rf_remote);
    }

// --------------------------------| Update Rf Remote Code |--------------------------------------


// ===============================| All On/Off Function |=======================================

    void all_SwitchOff(){
        toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); pref.putBool("Relay1", toggleState_1); my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1); delay(100);
        toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); pref.putBool("Relay2", toggleState_2); my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2); delay(100);
        toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); pref.putBool("Relay3", toggleState_3); my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3); delay(100);
        toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); pref.putBool("Relay4", toggleState_4); my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4); delay(100);
        toggleState_5 = 0; fanSpeedControl(0); pref.putBool("Fan_Power", toggleState_5); my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5); delay(100);
    }

    void all_SwitchOn(){
        toggleState_1 = 1; digitalWrite(RelayPin1, LOW); pref.putBool("Relay1", toggleState_1); my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1); delay(100);
        toggleState_2 = 1; digitalWrite(RelayPin2, LOW); pref.putBool("Relay2", toggleState_2); my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2); delay(100);
        toggleState_3 = 1; digitalWrite(RelayPin3, LOW); pref.putBool("Relay3", toggleState_3); my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3); delay(100);
        toggleState_4 = 1; digitalWrite(RelayPin4, LOW); pref.putBool("Relay4", toggleState_4); my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4); delay(100);
        toggleState_5 = 1; fanSpeedControl(currSpeed); pref.putBool("Fan_Power", toggleState_5); my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5); delay(100);
    }

// ---------------------------------| All On/Off Function |-----------------------------------


// ===============================| Update the Switch state |=======================================

    void getRelayState(){
        toggleState_1 = pref.getBool("Relay1", 0);
        digitalWrite(RelayPin1, !toggleState_1); 
        my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
        delay(200);
        toggleState_2 = pref.getBool("Relay2", 0);
        digitalWrite(RelayPin2, !toggleState_2); 
        my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
        delay(200);
        toggleState_3 = pref.getBool("Relay3", 0);
        digitalWrite(RelayPin3, !toggleState_3); 
        my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
        delay(200);
        toggleState_4 = pref.getBool("Relay4", 0);
        digitalWrite(RelayPin4, !toggleState_4); 
        my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
        delay(200);
        currSpeed = pref.getInt("Fan_Speed", 0);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        delay(200);
        toggleState_5 = pref.getBool("Fan_Power", 0);
        if(toggleState_5 == 1 && currSpeed > 0){
            fanSpeedControl(currSpeed); 
        }  
        my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
        delay(200);
    }  

// -------------------------------------| Update the Switch state |---------------------------------------


// ===============================| Buzzer Function |=======================================
    void buzz(int times, int stop){
        for(int i = 1; i<=times; i++){
            digitalWrite(Buzzer, HIGH);
            delay(stop);
            digitalWrite(Buzzer, LOW);
            if(i<times){
                delay(stop);
            }
        }
        Serial.println("DONE");
    }
// -------------------------------------| Buzzer Function |--------------------------------------------


// ===============================| Setup Function |=======================================

    void setup(){

        my_switch1=Switch(deviceName_1, &RelayPin1);
        my_switch2=Switch(deviceName_2, &RelayPin2);
        my_switch3=Switch(deviceName_3, &RelayPin3);
        my_switch4=Switch(deviceName_4, &RelayPin4);
        my_fan=Fan(deviceName_5);
        Serial.begin(115200);
        groveSerial.begin(115200);
        pref.begin("Data", false);
        pinMode(wifiLed, OUTPUT);
        pinMode(Buzzer, OUTPUT);
        


        pinMode(RelayPin1, OUTPUT);
        pinMode(RelayPin2, OUTPUT);
        pinMode(RelayPin3, OUTPUT);
        pinMode(RelayPin4, OUTPUT);
        pinMode(FanRelay1, OUTPUT);
        pinMode(FanRelay2, OUTPUT);
        pinMode(FanRelay3, OUTPUT);


        pinMode(SwitchPin1, INPUT_PULLUP);
        pinMode(SwitchPin2, INPUT_PULLUP);
        pinMode(SwitchPin3, INPUT_PULLUP);
        pinMode(SwitchPin4, INPUT_PULLUP);
        pinMode(SwitchPin5, INPUT_PULLUP);
        pinMode(FanSwitch1, INPUT_PULLUP);
        pinMode(FanSwitch2, INPUT_PULLUP);
        pinMode(FanSwitch3, INPUT_PULLUP);
        pinMode(FanSwitch4, INPUT_PULLUP);
        pinMode(gpio_reset, INPUT);

        digitalWrite(RelayPin1, !toggleState_1);
        digitalWrite(RelayPin2, !toggleState_2);
        digitalWrite(RelayPin3, !toggleState_3);
        digitalWrite(RelayPin4, !toggleState_4);
        digitalWrite(FanRelay1, HIGH);
        digitalWrite(FanRelay2, HIGH);
        digitalWrite(FanRelay3, HIGH);
        digitalWrite(wifiLed, LOW);
        digitalWrite(Buzzer, LOW);

        RF_Button_A1 = pref.getInt("RF_Button_A1", 0); 
        RF_Button_A2 = pref.getInt("RF_Button_A2", 0);
        RF_Button_A3 = pref.getInt("RF_Button_A3", 0);
        RF_Button_A4 = pref.getInt("RF_Button_A4", 0);
        RF_Fan_A = pref.getInt("RF_Fan_A", 0);
        RF_Fan_Up_A = pref.getInt("RF_Fan_Up_A", 0);
        RF_Fan_Down_A = pref.getInt("RF_Fan_Down_A", 0);
        RF_All_A = pref.getInt("RF_All_A", 0);

        RF_Button_B1 = pref.getInt("RF_Button_B1", 0); 
        RF_Button_B2 = pref.getInt("RF_Button_B2", 0);
        RF_Button_B3 = pref.getInt("RF_Button_B3", 0);
        RF_Button_B4 = pref.getInt("RF_Button_B4", 0);
        RF_Fan_B = pref.getInt("RF_Fan_B", 0);
        RF_Fan_Up_B = pref.getInt("RF_Fan_Up_B", 0);
        RF_Fan_Down_B = pref.getInt("RF_Fan_Down_B", 0);
        RF_All_B = pref.getInt("RF_All_B", 0);

        RF_Button_C1 = pref.getInt("RF_Button_C1", 0); 
        RF_Button_C2 = pref.getInt("RF_Button_C2", 0);
        RF_Button_C3 = pref.getInt("RF_Button_C3", 0);
        RF_Button_C4 = pref.getInt("RF_Button_C4", 0);
        RF_Fan_C = pref.getInt("RF_Fan_C", 0);
        RF_Fan_Up_C = pref.getInt("RF_Fan_Up_C", 0);
        RF_Fan_Down_C = pref.getInt("RF_Fan_Down_C", 0);
        RF_All_C = pref.getInt("RF_All_C", 0);

        config1.setEventHandler(button1Handler);
        config2.setEventHandler(button2Handler);
        config3.setEventHandler(button3Handler);
        config4.setEventHandler(button4Handler);
        config5.setEventHandler(button5Handler);

        button1.init(SwitchPin1);
        button2.init(SwitchPin2);
        button3.init(SwitchPin3);
        button4.init(SwitchPin4);
        button5.init(SwitchPin5);
        rfReceiver.enableReceive(RF_PIN);
        irrecv.enableIRIn(); // Enabling IR sensor


        tick1.attach_ms(16, rf_remote);
        tick2.attach_ms(16, fanRegularor);
        tick3.attach_ms(16, ir_remote);
        tick4.attach_ms(16, vc02Control);

        Node my_node;    
        my_node = RMaker.initNode(nodeName);

        
        //Standard switch device
        my_switch1.addCb(write_callback);
        my_switch2.addCb(write_callback);
        my_switch3.addCb(write_callback);
        my_switch4.addCb(write_callback);
        my_fan.addCb(write_callback);

        //Define Param Fan Speed
        Param speed("My_Speed",ESP_RMAKER_PARAM_RANGE , value(0), PROP_FLAG_READ | PROP_FLAG_WRITE);
        speed.addBounds(value(0), value(4), value(1));
        speed.addUIType(ESP_RMAKER_UI_SLIDER);
        my_fan.addParam(speed);

        //Add switch device to the node   
        my_node.addDevice(my_switch1);
        my_node.addDevice(my_switch2);
        my_node.addDevice(my_switch3);
        my_node.addDevice(my_switch4);
        my_node.addDevice(my_fan);
        
        delay(1000);

        RMaker.enableOTA(OTA_USING_PARAMS);
        RMaker.enableTZService();
        RMaker.enableSchedule();

        Serial.printf("\nStarting ESP-RainMaker\n");
        RMaker.start();
        buzz(1,300);
        WiFi.onEvent(sysProvEvent);
        #if CONFIG_IDF_TARGET_ESP32
            WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
        #else
            WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
        #endif
            getRelayState(); //fetch data from NVS Flash Memory
    }

// -------------------------------------| Setup Function |--------------------------------------------


// ===============================| Loop Function |=======================================

    void loop(){ 
        end_rf1=millis();
        end_rf2=millis();
        end_rf3=millis();
        if(digitalRead(gpio_reset) == LOW){
            buzz(4,300);
            RMakerFactoryReset(2);
        }
        if(end_rf1-start_rf1>4000){
            rf_counter1=1;
        }
        if(end_rf2-start_rf2>4000){
            rf_counter2=1;
        }
        if(end_rf3-start_rf3>4000){
            rf_counter3=1;
        }
        //Control Switches Manualy
        button1.check();
        button2.check();
        button3.check();
        button4.check();
        button5.check(); 
    }

// ------------------------------------| Loop Function |--------------------------------------



// ===============================| Manual Fan Regulator |=======================================

    void fanRegularor(){
    if (digitalRead(FanSwitch1) == HIGH && digitalRead(FanSwitch2) == HIGH && digitalRead(FanSwitch3) == HIGH && digitalRead(FanSwitch4) == HIGH  && fanSpeed_0 == LOW)
    // if (digitalRead(FanSwitch2) == HIGH && digitalRead(FanSwitch3) == HIGH && digitalRead(FanSwitch4) == HIGH  && fanSpeed_0 == LOW)
    {
        if(first_run == false){
        currSpeed = 0;
        if(toggleState_5 == 1){
            fanSpeedControl(0);
            Serial.println("0"); 
        }

        pref.putInt("Fan_Speed", currSpeed);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        }    
        fanSpeed_1 = LOW;
        fanSpeed_2 = LOW;
        fanSpeed_3 = LOW;
        fanSpeed_4 = LOW;
        fanSpeed_0 = HIGH;
    }
    if (digitalRead(FanSwitch1) == LOW && fanSpeed_1 == LOW)
    {
        if(first_run == false){
        currSpeed = 1;
        if(toggleState_5 == 1){
            fanSpeedControl(1); 
            Serial.println("1"); 
        }
        pref.putInt("Fan_Speed", currSpeed);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        }
        fanSpeed_1 = HIGH;
        fanSpeed_2 = LOW;
        fanSpeed_3 = LOW;
        fanSpeed_4 = LOW;
        fanSpeed_0 = LOW;
    }
    if (digitalRead(FanSwitch2) == LOW && digitalRead(FanSwitch3) == HIGH && fanSpeed_2 == LOW)
    {
        if(first_run == false){
        currSpeed = 2;
        if(toggleState_5 == 1){
            fanSpeedControl(2); 
            Serial.println("2"); 
        }
        pref.putInt("Fan_Speed", currSpeed);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        }
        fanSpeed_1 = LOW;
        fanSpeed_2 = HIGH;
        fanSpeed_3 = LOW;
        fanSpeed_4 = LOW;
        fanSpeed_0 = LOW;
    }
    if (digitalRead(FanSwitch2) == LOW && digitalRead(FanSwitch3) == LOW && fanSpeed_3 == LOW)
    {
        if(first_run == false){
        currSpeed = 3;
        if(toggleState_5 == 1){
            fanSpeedControl(3); 
            Serial.println("3"); 
        }
        pref.putInt("Fan_Speed", currSpeed);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        }
        fanSpeed_1 = LOW;
        fanSpeed_2 = LOW;
        fanSpeed_3 = HIGH;
        fanSpeed_4 = LOW;
        fanSpeed_0 = LOW;
    }
    if (digitalRead(FanSwitch4) == LOW  && fanSpeed_4 == LOW)
    {
        if(first_run == false){
        currSpeed = 4;
        if(toggleState_5 == 1){
            fanSpeedControl(4); 
            Serial.println("4"); 
        }
        pref.putInt("Fan_Speed", currSpeed);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        }
        fanSpeed_1 = LOW;
        fanSpeed_2 = LOW;
        fanSpeed_3 = LOW;
        fanSpeed_4 = HIGH;
        fanSpeed_0 = LOW;
    }
    first_run = false;
    }

// ------------------------------------| Manual Fan Regulator |-------------------------------------------



// ===============================| Fan Speed Control |=======================================

    void fanSpeedControl(int fanSpeed){
    switch(fanSpeed){
        case 0:
        // delay(500);
        digitalWrite(FanRelay1, HIGH);
        digitalWrite(FanRelay2, HIGH);
        digitalWrite(FanRelay3, HIGH);        
        // delay(500);
        break;
        case 1:
        // delay(500);
        digitalWrite(FanRelay2, HIGH);
        digitalWrite(FanRelay3, HIGH);
        digitalWrite(FanRelay1, LOW);
        // delay(500);
        break;
        case 2:
        // delay(500);
        digitalWrite(FanRelay1, HIGH);
        digitalWrite(FanRelay3, HIGH);
        digitalWrite(FanRelay2, LOW);
        // delay(500);
        break;
        case 3:
        // delay(500);
        digitalWrite(FanRelay3, HIGH);
        digitalWrite(FanRelay1, LOW);
        digitalWrite(FanRelay2, LOW);
        // delay(500);
        break;
        case 4:
        // delay(500);
        digitalWrite(FanRelay1, HIGH);
        digitalWrite(FanRelay2, HIGH);
        digitalWrite(FanRelay3, LOW);
        // delay(500);
        break;          
        default : break;         
    } 
    }

// ----------------------------------| Fan Speed Control |-----------------------------------------

// ===========================| VC-02 Control |==================================
  void vc02Control(){
    uint8_t *voiceData = detectVoiceFromGroveSensor(&groveSerial);
    if(voiceData != NULL){
        String response = getCommandInString(voiceData);
            if(response == "switch one on"){
                toggleState_1 = 1;
                digitalWrite(RelayPin1, LOW);
                pref.putBool("Relay1", toggleState_1);
                my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);  
            }
            else if(response == "switch two on"){
                toggleState_2 = 1;
                digitalWrite(RelayPin2, LOW);
                pref.putBool("Relay2", toggleState_2);
                my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);  
            }
            else if(response == "switch three on"){
                toggleState_3 = 1;
                digitalWrite(RelayPin3, LOW);
                pref.putBool("Relay3", toggleState_3);
                my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);  
            }
            else if(response == "switch four on"){
                toggleState_4 = 1;
                digitalWrite(RelayPin4, LOW);
                pref.putBool("Relay4", toggleState_4);
                my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);  
            }
            else if(response == "switch one off"){
                toggleState_1 = 0;
                digitalWrite(RelayPin1, HIGH);
                pref.putBool("Relay1", toggleState_1);
                my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);  
            }
            else if(response == "switch two off"){
                toggleState_2 = 0;
                digitalWrite(RelayPin2, HIGH);
                pref.putBool("Relay2", toggleState_2);
                my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);  
            }
            else if(response == "switch three off"){
                toggleState_3 = 0;
                digitalWrite(RelayPin3, HIGH);
                pref.putBool("Relay3", toggleState_3);
                my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);  
            }
            else if(response == "switch four off"){
                toggleState_4 = 0;
                digitalWrite(RelayPin4, HIGH);
                pref.putBool("Relay4", toggleState_4);
                my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);  
            }
            else if(response == "Fan on"){
                toggleState_5 = 1;
                fanSpeedControl(currSpeed);
                pref.putBool("Fan_Power", toggleState_5);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
                my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
            }
            else if(response == "Fan off"){
                toggleState_5 = 0;
                fanSpeedControl(0);
                pref.putBool("Fan_Power", toggleState_5);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
                my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
            }
            else if(response == "fan at one"){
                currSpeed = 1;
                if(toggleState_5 == 1){
                    fanSpeedControl(1); 
                }
                pref.putInt("Fan_Speed", currSpeed);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
            }
            else if(response == "fan at two"){
                currSpeed = 2;
                if(toggleState_5 == 1){
                    fanSpeedControl(2);
                }
                pref.putInt("Fan_Speed", currSpeed);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
            }
            else if(response == "fan at three"){
                currSpeed = 3;
                if(toggleState_5 == 1){
                    fanSpeedControl(3);
                }
                pref.putInt("Fan_Speed", currSpeed);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
            }
            else if(response == "fan at four"){
                currSpeed = 4;
                if(toggleState_5 == 1){
                    fanSpeedControl(4); 
                }
                pref.putInt("Fan_Speed", currSpeed);
                my_fan.updateAndReportParam("My_Speed", currSpeed);
            }
            else if(response == "done"){
                all_SwitchOff();
            }
    }
    }
// ---------------------------| VC-02 Control |-------------------------------------

// ===============================| Manual Button-1 |=======================================

    void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    if(skip==0) skip=1;
    if(update_rf_value==0){
    Serial.println("EVENT1");
    switch (eventType) {
        case AceButton::kEventPressed:
        if(Reset_counter==1){
            start_reset=millis();
            Serial.println("Timer start");
        }
        if(Reset_counter==7){
            Reset_counter=0;
            end_reset=millis();
            Serial.println(end_reset-start_reset);
            if((end_reset-start_reset)<3000){
            Serial.println("Reset");
            delay(700);
            buzz(2, 100);
            RMakerFactoryReset(2);
            }
        }
        Serial.println("kEventPressed");
        toggleState_1 = 1;
        Reset_counter++;
        digitalWrite(RelayPin1, LOW);
        pref.putBool("Relay1", toggleState_1);
        my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
        break;
        case AceButton::kEventReleased:
        if(Reset_counter==1){
            start_reset=millis();
            Serial.println("Timer start");
        }
        if(Reset_counter==7){
            Reset_counter=0;
            end_reset=millis();
            Serial.println(end_reset-start_reset);
            if((end_reset-start_reset)<3000){
            Serial.println("Reset");
            buzz(2,100);
            RMakerFactoryReset(2);
            }
        }
        Reset_counter++;
        Serial.println("kEventReleased");
        toggleState_1 = 0;
        digitalWrite(RelayPin1, HIGH);
        pref.putBool("Relay1", toggleState_1);
        my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);  
        break;
    }}
    }

// -----------------------------------| Manual Button-1 |-----------------------------------------


// ===============================| Manual Button-2 |=======================================

    void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    Serial.println("EVENT2");
    if(skip==0) skip=1;
    if(update_rf_value==0){
    switch (eventType) {
        case AceButton::kEventPressed:
        if(rf_counter1==1){
            start_rf1=millis();
            Serial.println("Timer start");
        }
        if(rf_counter1==7){
            rf_counter1=0;
            end_rf1=millis();
            Serial.println(end_rf1-start_rf1);
            if((end_rf1-start_rf1)<3000){
            Serial.println("Reset 1");
            update_RF(1);
            }
        }
        Serial.println("kEventPressed");
        toggleState_2 = 1;
        rf_counter1++;
        digitalWrite(RelayPin2, LOW);
        pref.putBool("Relay2", toggleState_2);
        my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);  
        break;
        case AceButton::kEventReleased:
        Serial.println("kEventReleased");
        if(rf_counter1==1){
            start_rf1=millis();
            Serial.println("Timer start");
        }
        if(rf_counter1==7){
            rf_counter1=0;
            end_rf1=millis();
            Serial.println(end_rf1-start_rf1);
            if((end_rf1-start_rf1)<3000){
            Serial.println("Reset 2");
            update_RF(1);
            }
        }
        rf_counter1++;
        toggleState_2 = 0;
        digitalWrite(RelayPin2, HIGH);
        pref.putBool("Relay2", toggleState_2);
        my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);   
        break;
    }
    }
    }

// --------------------------------------| Manual Button-2 |-------------------------------------------


// ===============================| Manual Button-3 |=======================================

    void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
        if(skip==0) skip=1;
    if(update_rf_value==0){
    Serial.println("EVENT3");
    switch (eventType) {
        case AceButton::kEventPressed:
        if(rf_counter2==1){
            start_rf2=millis();
            Serial.println("Timer start");
        }
        if(rf_counter2==7){
            rf_counter2=0;
            end_rf2=millis();
            Serial.println(end_rf2-start_rf2);
            if((end_rf2-start_rf2)<3000){
            Serial.println("Reset 1");
            update_RF(2);
            }
        }
        Serial.println("kEventPressed");
        rf_counter2++;
        toggleState_3 = 1;
        digitalWrite(RelayPin3, LOW);
        pref.putBool("Relay3", toggleState_3);
        my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);  
        break;
        case AceButton::kEventReleased:
        if(rf_counter2==1){
            start_rf2=millis();
            Serial.println("Timer start");
        }
        if(rf_counter2==7){
            rf_counter2=0;
            end_rf2=millis();
            Serial.println(end_rf2-start_rf2);
            if((end_rf2-start_rf2)<3000){
            Serial.println("Reset 1");
            update_RF(2);
            }
        }
        rf_counter2++;
        Serial.println("kEventReleased");
        toggleState_3 = 0;
        digitalWrite(RelayPin3, HIGH);
        pref.putBool("Relay3", toggleState_3);
        my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);  
        break;
            }
        }
    }

// ----------------------------------| Manual Button-3 |----------------------------------------


// ===============================| Manual Button-4 |=======================================

    void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
        if(skip==0) skip=1;
    if(update_rf_value==0){
    Serial.println("EVENT4");
    switch (eventType) {
        case AceButton::kEventPressed:
        if(rf_counter3==1){
            start_rf3=millis();
            Serial.println("Timer start");
        }
        if(rf_counter3==7){
            rf_counter3=0;
            end_rf3=millis();
            Serial.println(end_rf3-start_rf3);
            if((end_rf3-start_rf3)<3000){
            Serial.println("Reset 1");
            update_RF(3);
            }
        }
        rf_counter3++;
        Serial.println("kEventPressed");
        toggleState_4 = 1;
        digitalWrite(RelayPin4, LOW);
        pref.putBool("Relay4", toggleState_4);
        my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
        break;
        case AceButton::kEventReleased:
            if(rf_counter3==1){
            start_rf3=millis();
            Serial.println("Timer start");
        }
        if(rf_counter3==7){
            rf_counter3=0;
            end_rf3=millis();
            Serial.println(end_rf3-start_rf3);
            if((end_rf3-start_rf3)<3000){
            Serial.println("Reset 1");
            update_RF(3);
            }
        }
        rf_counter3++;
        Serial.println("kEventReleased");
        toggleState_4 = 0;
        digitalWrite(RelayPin4, HIGH);
        pref.putBool("Relay4", toggleState_4);
        my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
        break;
    }
    }
    }

// -----------------------------------| Manual Button-4 |--------------------------------------



// ===============================| Manual Button-Fan |=======================================

    void button5Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    Serial.println("EVENT5");
    if(skip==0) skip=1;
    if(update_rf_value==0){
    switch (eventType) {
        case AceButton::kEventPressed:
        Serial.println("kEventPressed");
        toggleState_5 = 1;
        fanSpeedControl(currSpeed);
        pref.putBool("Fan_Power", toggleState_5);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
        break;
        case AceButton::kEventReleased:
        Serial.println("kEventReleased");
        toggleState_5 = 0;
        fanSpeedControl(0);
        pref.putBool("Fan_Power", toggleState_5);
        my_fan.updateAndReportParam("My_Speed", currSpeed);
        my_fan.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5); 
        break;
    }}
    }

// -----------------------------| Manual Button-Fan |----------------------------------