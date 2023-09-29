void connectWiFi(){
    WiFi.begin(SSID.c_str(),PASS.c_str());
    int i=0;
    while (WiFi.status() != WL_CONNECTED)  {
        Serial.print(".");
        delay(500);
        i++;
        if(i>10)break;
    }
    if(i>10){
        Serial.println("WiFiError");
    }else{
        Serial.print("Connected to WiFi:");
        Serial.println(SSID);
    }

}

void timeFix(){
    if(WiFi.status() == WL_CONNECTED){
        configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com");
        getLocalTime(&timeInfo);
        WiFi.disconnect();
    }
}

void readEEPROM(){
    char ssid[100];
    char pass[100];

    EEPROM.begin(300);
    EEPROM.get(0,ssid);
    EEPROM.get(100,pass);
    EEPROM.get(200,vol);
    EEPROM.get(201,playMode);
    EEPROM.get(202,timerEN);
    if(playMode==0xFF)playMode=0;
    if(timerEN==0xFFFFFFFF)timerEN=0;

    if(vol<0||vol>30)vol=20;
    if(playMode<0||playMode>3)playMode=0;
    for(int i=0; i<100&&ssid[i]!=0xFF; i++){
        SSID+=String(ssid[i]);
    }
    for(int i=0; i<100&&pass[i]!=0xFF; i++){
        PASS+=String(pass[i]);
    }
    Serial.print("SSID:");
    Serial.println(SSID);
    Serial.print("PASS:");
    Serial.println(PASS);
}

void writeEEPROM(){
    char ssid[100];
    char pass[100];
    for(int i=0; i<100; i++){
        if(i<SSID.length())ssid[i]=SSID[i];
        else ssid[i]=0xFF;
    }
    for(int i=0; i<100; i++){
        if(i<PASS.length())pass[i]=PASS[i];
        else pass[i]=0xFF;
    }
    EEPROM.put(0,ssid);
    EEPROM.put(100,pass);
    EEPROM.commit();
}