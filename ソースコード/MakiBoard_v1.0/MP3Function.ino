void MP3Setup(){
    dfmp3.begin();//MP3playerとの通信用
    dfmp3.reset(); 
    f1Number=dfmp3.getFolderTrackCount(1);
    f3Number=dfmp3.getFolderTrackCount(3);
    dfmp3.setVolume(vol);
}

void volumeLED(uint32_t amp){
    if(amp>=LEDbias+LEDth) digitalWrite(LED6,HIGH);
    else digitalWrite(LED6,LOW);
    if(amp>=LEDbias+LEDth*2) digitalWrite(LED5,HIGH);
    else digitalWrite(LED5,LOW);
    if(amp>=LEDbias+LEDth*3) digitalWrite(LED4,HIGH);
    else digitalWrite(LED4,LOW);
    if(amp>=LEDbias+LEDth*4) digitalWrite(LED3,HIGH);
    else digitalWrite(LED3,LOW);
    if(amp>=LEDbias+LEDth*5) digitalWrite(LED2,HIGH);
    else digitalWrite(LED2,LOW);
    if(amp>=LEDbias+LEDth*6) digitalWrite(LED1,HIGH);
    else digitalWrite(LED1,LOW);
}

void setVolume(){
    if(vol>30)vol=30;
    else if(vol<0)vol=0;
    digitalWrite(LED1,vol>25);
    digitalWrite(LED2,vol>20);
    digitalWrite(LED3,vol>15);
    digitalWrite(LED4,vol>10);
    digitalWrite(LED5,vol>5);
    digitalWrite(LED6,vol>0);

    dfmp3.setVolume(vol);
    EEPROM.put(200,vol);
    EEPROM.commit();
    Serial.print("今のボリュームは ");
    Serial.println(vol);
    volLEDsetTime=millis();
}

void Alert(){
    getLocalTime(&timeInfo);
        if(timeInfo.tm_hour==previousHour+1){
            previousHour=timeInfo.tm_hour;
            char s[20];
            sprintf(s, "%04d/%02d/%02d %02d:%02d:%02d",
                timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
                timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
            Serial.println(s);
            int timerON = (timerEN>>previousHour)%2;
            if(timerON==1 &&previousHour==timerEN>>24) {
                dfmp3.setVolume(30);
                delay(100);
                dfmp3.playFolderTrack(2,25);
                delay(100);
                if(f3Number>0){
                    alert=true;
                    previousTrack=0;
                }
            }
            else if(timerON==1) dfmp3.playFolderTrack(2,previousHour);
        }else if(timeInfo.tm_hour==0&&previousHour==23){
            connectWiFi();
            timeFix();
            previousHour=timeInfo.tm_hour;
            if(timerEN%2==1 &&previousHour==timerEN>>24) {
                dfmp3.setVolume(30);
                delay(100);
                dfmp3.playFolderTrack(2,25);
                delay(100);
                if(f3Number>0){
                    alert=true;
                    previousTrack=0;
                }
            }
            else if(timerEN%2==1) dfmp3.playFolderTrack(2,24);
        }
}

void playNext(){
    Serial.println("NEXT!");
    delay(500);
    uint8_t number;
    if(previousTrack==255||previousTrack>=f1Number||previousTrack==0)number=1;
    else number=previousTrack+1;
    if(number==1)createArray();
    if(shuffle)dfmp3.playFolderTrack(1,randomArray[number]);
    else dfmp3.playFolderTrack(1,number);
    previousTrack=number;
    tempPause=false;
    playFinish=false;
    delay(100);
}

void playAlertMusic(){
    delay(500);
    uint8_t number;
    if(previousTrack>=f3Number)number=1;
    else number=previousTrack+1;
    dfmp3.playFolderTrack(3,number);
    previousTrack=number;
    delay(100);
}

void playPrevious(){
    Serial.println("PREVIOUS!");
    delay(500);
    uint8_t number;
    if(previousTrack==1||previousTrack==0)number=f1Number;
    else number=previousTrack-1;
    if(number==f1Number)createArray();
    if(shuffle)dfmp3.playFolderTrack(1,randomArray[number]);
    else dfmp3.playFolderTrack(1,number);
    previousTrack=number;
    tempPause=false;
    playFinish=false;
    delay(100);
}

void createArray(){
    uint16_t i=0;
    for(i=0;i<=255;i++){
        if(i==0||i>f1Number)randomArray[i]=0;
        else randomArray[i]=i;
    }
    for(i=1;i<=f1Number;i++){
        uint8_t j=random(1,f1Number);
        uint8_t t=randomArray[i];
        randomArray[i]=randomArray[j];
        randomArray[j]=t;
    }
}