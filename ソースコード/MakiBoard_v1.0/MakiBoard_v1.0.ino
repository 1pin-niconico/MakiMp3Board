#include "Arduino.h"
#include "EEPROM.h"
#include "BluetoothSerial.h"
#include "WiFi.h"
#include "DFMiniMp3.h"

class Mp3Notify; 
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 
DfMp3 dfmp3(Serial2);

#define LED6 19
#define LED5 18
#define LED4 5
#define LED3 27
#define LED2 26
#define LED1 25
#define LED_LOOP 23
#define LED_SHUFF 15

int playMode=0; //0:mp3再生モード,1:めざましモード
bool tempPause =false; //ポーズ中か否か
bool autoPlay = false; //自動再生モードか否か
bool playFinish = false; //再生が完了しているか否か
bool shuffle = false; //シャッフルモードか否か
bool alert = false;//目覚まし中かどうか
uint32_t timerEN=0;//タイマーモードの可否、上位8bitに目覚ましを鳴らす時刻、下位24bitに時報のEnableが格納
uint8_t previousTrack=0;//最後に再生したトラック
uint8_t f1Number;//フォルダ1内のトラック数
uint8_t f3Number;
uint8_t randomArray[256];

String SSID;//WiFi接続時のSSID格納変数
String PASS;//WiFi接続時のパスワード格納変数
//DFRobotDFPlayerMini myDFPlayer;
BluetoothSerial SerialBT;//Bluetooth
struct tm timeInfo;

int vol=20;//ボリューム0~30

int TC4,TC5,TC6,TC8,TC9;//タッチセンサの初期値
int TCth=10;//タッチセンサの反応閾値
uint32_t TC4time=0;//タッチセンサのタッチ時間
uint32_t TC5time=0;
uint32_t TC6time=0;
uint32_t TC8time=0;
uint32_t TC9time=0;

uint32_t stopTime = 0; //再生を終えた時刻
uint32_t previousCheckTime=0; //時刻監視を最後にした時刻
uint8_t previousHour; //今何時？
uint32_t volLEDsetTime=0;//ボリュームを最後に設定した時刻

uint32_t LEDbias=1800;
uint32_t LEDth=50;

class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

void setup() {
    Serial.begin(115200);//PCとの通信用
    //Serial2.begin(9600);//MP3playerとの通信用
    SerialBT.begin("MakiMp3Player");//Bluetoth用

    pinSetup();
    readEEPROM();
    MP3Setup();
   
    if(SSID.length()>0&&PASS.length()>0){
        connectWiFi();
        timeFix();
        previousHour=timeInfo.tm_hour;
    }
    setTC();

    //Serial.print("フォルダ内mp3数:");
    //Serial.println(f3Number);
    createArray();
}

void loop() {

    touchFunction();

    uint32_t ADC=analogRead(36);
    if(millis()-volLEDsetTime>3000)volumeLED(ADC);

    uint32_t temp=analogRead(35);
    if(playMode==1&&temp>100&&CTemp(temp)>40.0&&playFinish&&previousCheckTime-stopTime>600000){
        dfmp3.playFolderTrack(2,26);
        playFinish=false;
        stopTime=millis();
    }

    if(Serial.available()){
        delay(10);
        String data = Serial.readStringUntil(';');
        serialDecode(data);
    }

    if(millis()-previousCheckTime>1000){
        if(temp>100){
            SerialBT.print(CTemp(temp),1);
            SerialBT.println(" ℃");
        }
        previousCheckTime=millis();
        if(playMode==1){
            Alert();
        }
    }

    if(digitalRead(34)==HIGH)playFinish=true;
    else playFinish=false;
    
    if(autoPlay&&playFinish&&!tempPause)playNext();
    if(playFinish&&alert)playAlertMusic();
    delay(10);
}

void pinSetup(){
    pinMode(36,ANALOG);
    pinMode(35,ANALOG);
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
    pinMode(LED4,OUTPUT);
    pinMode(LED5,OUTPUT);
    pinMode(LED6,OUTPUT);
    pinMode(34,INPUT);
    pinMode(LED_LOOP,OUTPUT);
    pinMode(LED_SHUFF,OUTPUT);
}

float CTemp(uint32_t vt){
    return(vt-850-160)/32.0;
}