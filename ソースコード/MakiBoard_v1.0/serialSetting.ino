void serialDecode(String str){
    if(str.indexOf("WIFI")!=-1){
        int n1=str.indexOf(",");
        int n2=str.indexOf(",",n1+1);
        Serial.println(n1);
        Serial.println(n2);
        if(n1>0,n2>0){
            SSID=str.substring(n1+1,n2);
            PASS=str.substring(n2+1,str.length());
            Serial.println(SSID);
            Serial.println(PASS);
            writeEEPROM();
        }
    }
    else if(str.indexOf("TIMER")!=-1){
        uint32_t temp=timerEN;
        int n1=str.indexOf(",");
        int n2=str.indexOf(",",n1+1);
        if(n1>0,n2>0){
            String settingTime=str.substring(n1+1,n2);
            String body=str.substring(n2+1,str.length());
            if(settingTime=="ALL"&&body=="OFF"){
                timerEN=timerEN&0xFF000000;
                Serial.println("時報設定を全時間OFFにしたよ！");
            }
            else if(settingTime=="ALL"&&body=="ON"){
                timerEN=timerEN|0x00FFFFFF;
                Serial.println("時報設定を全時間ONにしたよ！");
            }
            else{
                uint8_t settingHour=settingTime.toInt();
                if(settingHour>=0&&settingHour<24){
                    if(body=="ALERT"){
                        temp=(timerEN|uint32_t(pow(2,settingHour)))&0x00FFFFFF;
                        timerEN=(settingHour<<24)|temp;
                        Serial.print("目覚ましを");
                        Serial.print(settingHour);
                        Serial.println("時に設定したよ！");
                    }
                    else if(body=="ON"){
                        timerEN=timerEN|uint32_t(pow(2,settingHour));
                        Serial.print(settingHour);
                        Serial.println("時に時報を鳴らすよう設定したよ！");
                    }
                    else if(body=="OFF"){
                        timerEN=timerEN&~uint32_t(pow(2,settingHour));
                        Serial.print(settingHour);
                        Serial.println("時の時報を鳴らさないように設定したよ！");
                    }
                    
                }
                else Serial.println("エラー、不正な時刻だよ!");
            }
            EEPROM.put(202,timerEN);
            EEPROM.commit();
            //Serial.println(timerEN>>24);
            //Serial.println(timerEN&0x00FFFFFF);
        }
    }
}
