void setTC(){
    int C4,C5,C6,C8,C9;
    int i=0;
    int miss=0;
    while(i<5){
        if(i==0){
            C4=touchRead(T4);
            C5=touchRead(T5);
            C6=touchRead(T6);
            C8=touchRead(T8);
            C9=touchRead(T9);
            i++;
        }else if(!touchchange(C4,C5,C6,C8,C9)){
            i=0;
            miss+=1;
        }else i++;
        if(miss>10){
            Serial.println("タッチポートの初期化に失敗しました");
        }
        delay(100);
    }
    TC4=C4;TC5=C5;TC6=C6;TC8=C8;TC9=C9;
    /*
    Serial.println(TC4);
    Serial.println(TC5);
    Serial.println(TC6);
    Serial.println(TC8);
    Serial.println(TC9);
    */
}

bool touchchange(int ctemp4,int ctemp5,int ctemp6,int ctemp8,int ctemp9){
    return (abs(touchRead(T4)-ctemp4)<3)&&(abs(touchRead(T5)-ctemp5)<3)&&(abs(touchRead(T6)-ctemp6)<3)&&(abs(touchRead(T8)-ctemp8)<3)&&(abs(touchRead(T9)-ctemp9)<3);
}

int touchDetect(){
    if(TC4-touchRead(T4)>TCth)TC4time++;
    else if(TC4time>5) return 4;
    else TC4time=0;
    if(TC5-touchRead(T5)>TCth)TC5time++;
    else if(TC5time>5) return 5;
    else TC5time=0;
    if(TC6-touchRead(T6)>TCth)TC6time++;
    else if(TC6time>5) return 6;
    else TC6time=0;
    if(TC8-touchRead(T8)>TCth)TC8time++;
    else if(TC8time>5) return 8;
    else TC8time=0;
    if(TC9-touchRead(T9)>TCth)TC9time++;
    else if(TC9time>5) return 9;
    else TC9time=0;
    return 0;
}

void TCtimeReset(){
    TC4time=0;
    TC5time=0;
    TC6time=0;
    TC8time=0;
    TC9time=0;
}

void touchFunction(){
        int t=touchDetect();
        
        if(t==5){
            if(alert){
                dfmp3.playFolderTrack(2,32);
                alert=false;
            }
            else if(playMode==1){
                dfmp3.playFolderTrack(2,31);
            }
            else if(autoPlay){
                digitalWrite(LED_LOOP,LOW);
                autoPlay=false;
            }else{
                digitalWrite(LED_LOOP,HIGH);
                autoPlay=true;
                playFinish=false;
            }
        }

        else if(t==8){
            if(playMode==1&&playFinish){
                dfmp3.playFolderTrack(2,30);
            }
            else if(shuffle){
                digitalWrite(LED_SHUFF,LOW);
                shuffle=false;
            }else{
                digitalWrite(LED_SHUFF,HIGH);
                shuffle=true;
            }
        }
        
        else if(t==6){
            if(TC6time>100){
                if(playMode==0){
                    playMode=1;
                    Serial.println("目覚ましモードだよ！");
                    dfmp3.playFolderTrack(2,27);
                }
                else if(playMode==1){
                    playMode=0;
                    Serial.println("mp3モードだよ！");
                    dfmp3.playFolderTrack(2,28);
                }
                previousTrack=0;
                autoPlay=false;
                playFinish=false;
                tempPause=false;
                shuffle=false;
            }
            else if(playMode==0){
                if(!tempPause&&previousTrack==0){
                    dfmp3.playFolderTrack(1,1);
                    previousTrack=1;
                }
                else if(tempPause){
                    dfmp3.start();
                    tempPause=false;
                }else{
                    dfmp3.pause();
                    tempPause=true;
                }
            }
            else if(playMode==1&&playFinish){
                dfmp3.setVolume(vol);
                dfmp3.playFolderTrack(2,29);
            }
        }
        else if(t==4){
            if(TC4time<50){
                vol-=5;
                setVolume();
            }else if(playMode==0) playPrevious();
        }
        else if(t==9){
            if(TC9time<50){
                vol+=5;
                setVolume();
            }else if(playMode==0) playNext();
        }
        if(t!=0)TCtimeReset();
}