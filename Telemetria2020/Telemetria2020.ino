#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <SD.h>

LiquidCrystal_I2C lcd(0x20,2,1,0,4,5,6,7,3,POSITIVE);
RTC_DS1307 rtc;
File myFile;
#define CS_pin 10

byte velocidade=0;
int temp,rpm=0;
volatile byte pulsosRPM;
volatile byte pulsosVELO;
unsigned long timeold;
byte i=0;
byte segundop=0;



byte bar1[8]={B11100,B11110,B11110,B11110,B11110,B11110,B11110,B11100};
byte bar2[8]={B00111,B01111,B01111,B01111,B01111,B01111,B01111,B00111};
byte bar3[8]={B11111,B11111,B00000,B00000,B00000,B00000,B11111,B11111};
byte bar4[8]={B11110,B11100,B00000,B00000,B00000,B00000,B11000,B11100};
byte bar5[8]={B01111,B00111,B00000,B00000,B00000,B00000,B00011,B00111};
byte bar6[8]={B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte bar7[8]={B00000,B00000,B00000,B00000,B00000,B00000,B00111,B01111};
byte bar8[8]={B11111,B11111,B00000,B00000,B00000,B00000,B00000,B00000};

void setup(){
  Serial.begin(9600);
  temp=map(analogRead(A0),0,1023,-55,150);

  rpm=0;
  pulsosVELO=0;
  timeold=0;

  pinMode(A0,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(6,OUTPUT);
  attachInterrupt(0,contRPM,RISING);
  attachInterrupt(1,contVELO,RISING);

  lcd.createChar(1,bar1);lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);lcd.createChar(8,bar8);
  lcd.begin(16,2);


  if(!rtc.begin()){
    Serial.println("RTC nao encontarado");
  }

  if(! rtc.isrunning()){
    Serial.println("RTC descalibrado!");
    //rtc.adjust(DateTime(*ano*,*mes*,*dia*,*horas*,*minutos*,*segundos*));
  }

  pinMode(CS_pin,OUTPUT);
  if(!SD.begin(CS_pin)){
    Serial.println("Falha no cartao!");
  }

  Serial.println("Sucesso na inicializacao!");
  myFile=SD.open("log.csv",FILE_WRITE);

  if(myFile){
    String header="Tempo em segundos;Hora e Data;Velocidade;RPM;Temperatura";
    myFile.println(header);
    myFile.close();
    Serial.println(header);
  }else{
    Serial.println("Erro ao abrir arquivo");
  }
}
void contRPM(){
  pulsosRPM++;
}
void contVELO(){
  pulsosVELO++;
}
void custom0(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(8);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(2);lcd.write(6);lcd.write(1);
}
void custom1(byte col){
  lcd.setCursor(col,0);
  lcd.write(32);lcd.write(32);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32);lcd.write(32);lcd.write(1);
}
void custom2(byte col){
  lcd.setCursor(col,0);
  lcd.write(5);lcd.write(3);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(2);lcd.write(6);lcd.write(6);
}
void custom3(byte col){
  lcd.setCursor(col,0);
  lcd.write(5);lcd.write(3);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(7);lcd.write(6);lcd.write(1);
}
void custom4(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(6);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32);lcd.write(32);lcd.write(1);
}
void custom5(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(3);lcd.write(4);
  lcd.setCursor(col,1);
  lcd.write(7);lcd.write(6);lcd.write(1);
}
void custom6(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(3);lcd.write(4);
  lcd.setCursor(col,1);
  lcd.write(2);lcd.write(6);lcd.write(1);
}
void custom7(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(8);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(32);lcd.write(32);lcd.write(1);
}
void custom8(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(3);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(2);lcd.write(6);lcd.write(1);
}
void custom9(byte col){
  lcd.setCursor(col,0);
  lcd.write(2);lcd.write(3);lcd.write(1);
  lcd.setCursor(col,1);
  lcd.write(7);lcd.write(6);lcd.write(1);
}
void printNumber(byte value,byte col){
  if(value==0){custom0(col);
  }if(value==1){custom1(col);
  }if(value==2){custom2(col);
  }if(value==3){custom3(col);
  }if(value==4){custom4(col);
  }if(value==5){custom5(col);
  }if(value==6){custom6(col);
  }if(value==7){custom7(col);
  }if(value==8){custom8(col);
  }if(value==9){custom9(col);
  }
}
void printVELO(byte *velo_p){
  byte c,d,u,number;
  number=*velo_p;
  if(number>99){
    c=(number-(number%100))/100;
    number=number%100;
  }else{
    c=0;
  }
  if(number>9){
    d=(number-(number%10))/10;
    number=number%10;
  }else{
    d=0;
  }
  u=number;
  lcd.setCursor(0,0);
  printNumber(d,0);
  printNumber(u,3);
}

void printRPM(int *rpm_p){
  byte m,c;
  int number;
  number=*rpm_p;
  if(number>999){
  m=(number-(number%1000))/1000;
  number=number%1000;
  }else{
    m=0;
  }
  if(number>99){
    c=(number-(number%100))/100;
  }else{
    c=0;
  }
  lcd.setCursor(0,0);
  printNumber(m,11);
  lcd.setCursor(14,1);
  lcd.print(",");
  lcd.print(c);
}

void printTEMP(int *temp_p){
  *temp_p=map(analogRead(A0),0,1023,-55,150);
  lcd.setCursor(7,0);
  lcd.print("Tmp");
  if(*temp_p>99|| *temp_p<-9){
    lcd.setCursor(7,1);
    lcd.print("    ");
    lcd.setCursor(7,1);
    lcd.print(*temp_p);
  }else{
    lcd.setCursor(7,1);
    lcd.print("    ");
    lcd.setCursor(8,1);
    lcd.print(*temp_p);
  }
}

void printSD(){

  DateTime now=rtc.now();

  if(now.second()==segundop){i++;}else{i=0; segundop=now.second();};
  String dataString=(String(now.unixtime())+","+String(i)+";"+String(now.day())+"/"+String(now.month())+"/"+String(now.year())+" "+String(now.hour())+"h:"+String(now.minute())+"m:"+String(now.second()) +"s:"+
  String(i)+"ms; "+String(velocidade)+";"+String(rpm)+";"+String(temp));

  myFile=SD.open("log.csv",FILE_WRITE);
  myFile.println(dataString);
  myFile.close();
  Serial.println(dataString);
}

void loop(){
  Serial.println("    ");
  long ant=millis();

  //TEMP
  temp=map(analogRead(A0),0,1023,-55,150);
   
   
   //RPM
  detachInterrupt(0);
  rpm=(60000/1/* PULSO POR CICLO */)/(millis()-timeold)*pulsosRPM;
  analogWrite(6, map(rpm,0,3600,0,255));
  pulsosRPM=0;
  attachInterrupt(0,contRPM,RISING);

    //VELO
  detachInterrupt(1);
  velocidade=byte((/*perimetro da roda/ */ /* PULSO POR CICLO */1)/(millis()-timeold)*pulsosVELO);
  pulsosVELO=0;
  attachInterrupt(1,contVELO,RISING);
 

  timeold=millis();


  printVELO(&velocidade);
  printRPM(&rpm);
  printTEMP(&temp);
  printSD();

  Serial.println(millis()-ant);
  delay(100-(millis()-ant));
  Serial.println(millis()-ant);
}
