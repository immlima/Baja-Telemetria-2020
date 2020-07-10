#include <Wire.h>                                        // Biblioteca nativa da comunicaçao I2C
#include <LiquidCrystal_I2C.h>                           // Biblioteca LCD
#include "RTClib.h"                                      // Biblioteca RTC
#include <SD.h>                                          // Biblioteca Modulo SD

LiquidCrystal_I2C lcd(0x20,2,1,0,4,5,6,7,3,POSITIVE);    // objeto lcd, primeiro argumenro eh o endereço hex do lcd
RTC_DS1307 rtc;                                          // objeto rtc
File myFile;                                             // objeto myFile, usado pra no cartao SD

//**************PINOS**************//
#define pin_SD_cs 10                    // pino de ativaçao do modulo SD, esse pino eh usado pra abrir a comunicaçao
#define pin_Temp A0                     // pino de dados do sensor de temperatura
#define pin_Barra_Grafica_Rpm 6         // pino de saida para barra grafica de rpm
#define pin_velo 3                      // pino de pulsos da velocidade
#define pin_rpm 2                       // pino de pulsos da rpm

//**************VARIAVEIS GLOBAIS**************//
byte velocidade=0;          // variavel pra salvar dado de  velocidade
byte temp;                  // variavel pra salvar dado de temperatura do cvt
int rpm=0;                  // variaveis usadas pra salvar a temperatura do cvt e o rpm do motor
volatile byte pulsosRPM;    // variavel usada pra contar os pulsos obtidos da rotaçao do motor
volatile byte pulsosVELO=0; // variavel usada pra contar os pulsos obtidos da velocidade das rodas
unsigned long timeold=0;    // variavel usada pra salvar o tempo do processador na ultima passagem do ciclo
byte i=0;                   // variavel usada em laços de repetiçoes
byte segundop=0;            // variavel usada pra salvar o ultimo segundo q passou, usado pra calcular os milisegundos

//**************VARIAVEIS DE CRIAÇAO DOS NUMEROS GRANDES DO DISPLAY**************//
byte bar1[8]={B11100,B11110,B11110,B11110,B11110,B11110,B11110,B11100};
byte bar2[8]={B00111,B01111,B01111,B01111,B01111,B01111,B01111,B00111};
byte bar3[8]={B11111,B11111,B00000,B00000,B00000,B00000,B11111,B11111};
byte bar4[8]={B11110,B11100,B00000,B00000,B00000,B00000,B11000,B11100};
byte bar5[8]={B01111,B00111,B00000,B00000,B00000,B00000,B00011,B00111};
byte bar6[8]={B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte bar7[8]={B00000,B00000,B00000,B00000,B00000,B00000,B00111,B01111};
byte bar8[8]={B11111,B11111,B00000,B00000,B00000,B00000,B00000,B00000};

void setup(){
  Serial.begin(9600);                                              //inicio da comunicaçao Serial DEBUG
  temp=map(analogRead(pin_Temp),0,1023,-55,150);                   //coeta da temperatura

  pinMode(pin_Temp,INPUT);                                         //definindo como entrada o pino de temp
  pinMode(2,INPUT);                                                //definindo como entrada o pino de contagem dos pulsos de RPM
  pinMode(3,INPUT);                                                //definindo como entrada o pino de contagem dos pulsos de VELO
  pinMode(6,OUTPUT);                                               //definindo como saida o pino da barra grafica do rpm
  pinMode(pin_SD_cs,OUTPUT);                                       //definido como saida o pino de ativaçao do modulo SD
  attachInterrupt(digitalPinToInterrupt(pin_rpm),contRPM,RISING);  //definindo a monitoriamento de interrupt  dos pulsos do RPM
  attachInterrupt(digitalPinToInterrupt(pin_velo),contVELO,RISING);//definindo a monitoriamento de interrupt  dos pulsos do VELO

//*********SALVANDO NUMEROS GRANDES NA MEMORIA DO DISPLAY*********//
  lcd.createChar(1,bar1);lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);lcd.createChar(8,bar8);
  lcd.begin(16,2);


  if(!rtc.begin()){                                                           //se nao iniciou
    Serial.println("RTC nao encontarado");                                    // caso erro no modulo sd
  }

  if(!rtc.isrunning()){                                                       // se nao estiver rodando o RTC
    Serial.println("RTC descalibrado! / Horário Incorreto");                  // caso erro no horario/data
    //rtc.adjust(DateTime(*ano*,*mes*,*dia*,*horas*,*minutos*,*segundos*));   //recadastrar o horario
  }

  if(!SD.begin(pin_SD_cs)){                                                   // se nao iniciar o cartao sd
    Serial.println("Falha no cartao! / Cartao nao encontrado");               //erro no cartao sd
  }

  Serial.println("Sucesso na inicializacao!");                                // inicializacao do cartao completa
  myFile=SD.open("log.csv",FILE_WRITE);                                       //abrindo arquivo pra escrita

  if(myFile){                                                                 //se abriu o arquivo
    String header="Tempo em segundos;Hora e Data;Velocidade;RPM;Temperatura"; // transformando o cabeçalho em texto
    myFile.println(header);                                                   // salvando o cabeçalho no cartao sd
    myFile.close();                                                           // fechando o arquivo
  }else{
    Serial.println("Erro ao abrir arquivo");                                  // se nao erro
  }
}
void contRPM(){
  pulsosRPM++;                                                                // contagem de pulsos de rpm do motor
}
void contVELO(){
  pulsosVELO++;                                                               // contador de pulsos de velocidade da roda
}
//********DEFININDO O POSISONAMENDO DOS NUMEROS GRANDES********//
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

//**********FUNÇAO PRA CHAMAR NUMEROS GRANDES*********//
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

//*****FUNCAO PRA ESCREVER VELOCIDADE NO DISPLAY**********//
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
//*****FUNCAO PRA ESCREVER RPM NO DISPLAY**********//
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

//*****FUNCAO PRA ESCREVER TEMPERATURA NO DISPLAY**********//
void printTEMP(byte *temp_p){
  *temp_p=map(analogRead(pin_Temp),0,1023,-55,150);
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

//*****FUNCAO PRA ESCREVER TODOS OS DATOS NO CARTAO**********//
void printSD(){

  DateTime now=rtc.now();                                                // atualizando o valor de now

  if(now.second()==segundop){i++;}else{i=0; segundop=now.second();};     // calculo dos milisegundo

  String dataString=(String(now.unixtime())+","+String(i)+";"+String(now.day())+"/"+String(now.month())+"/"+String(now.year())+" "+String(now.hour())+"h:"+String(now.minute())+"m:"+String(now.second()) +"s:"+
  String(i)+"ms; "+String(velocidade)+";"+String(rpm)+";"+String(temp)); // texto q vai ser salvo no cartao

  myFile=SD.open("log.csv",FILE_WRITE);                                  // abrindo aquivo para escrita
  myFile.println(dataString);                                            // escrevendo no arquivo
  myFile.close();                                                        // fechando o arquivo
  Serial.println(dataString);                                            // debug no serial monitor
}

void loop(){
  Serial.println("    ");
  long ant=millis();

  //TEMP
  temp=map(analogRead(pin_Temp),0,1023,-55,150);


   //RPM
  detachInterrupt(0);                                              // pausa a contagem de pulsos pro calculo do rpm
  rpm=(60000/1/* PULSO POR CICLO */)/(millis()-timeold)*pulsosRPM; // calculo do rpm
  analogWrite(6, map(rpm,0,3600,0,255));                           // escrita por pwn na barra grafica
  pulsosRPM=0;                                                     // retoma o numero de pulsos a 0
  attachInterrupt(digitalPinToInterrupt(pin_rpm),contRPM,RISING);  // retorna a contagem

    //VELO
  detachInterrupt(1);                                                                             // pausa a contagem de pulsos pro calculo da velo
  velocidade=byte((/*perimetro da roda/ */ /* PULSO POR CICLO */1)/(millis()-timeold)*pulsosVELO);// calculo da velo
  pulsosVELO=0;                                                                                   // retoma o numero de pulsos a 0
  attachInterrupt(digitalPinToInterrupt(pin_velo),contVELO,RISING);                               // retorna a contagem


  timeold=millis(); // atuliza o tempo do ultimo calculo


  printVELO(&velocidade);
  printRPM(&rpm);
  printTEMP(&temp);
  printSD();

  Serial.println(millis()-ant);
  delay(100-(millis()-ant)); //delay de 100 milisegundos
  Serial.println(millis()-ant);
}
