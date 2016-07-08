#include <pgmspace.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
//#include <WebSocketsClient.h>
//#include <Hash.h>
#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_mfGFX.h"
#include "resources.h"
//#include "recipe.h"
#include <UTouch.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
#include <Wire.h>
#include "pcf8574.h"
//#include <RtcDS1307.h>
#include "FS.h"

/*
  RUN  (Temp °C Time min)
    0       MashIn
    1 - 2   Pause 1
    3 - 4   Pause 2
    5 - 6   Pause 3
    7 - 8   Pause 4
    9 - 10  Pause 5
    11      Boil Time 
    12      Time Hop  1
    13      Time Hop  2
    14      Time Hop  3
    15      FLAG Automode Started

  RECIPE
    16 - 31     Recipe Data  1 + Id
    32 - 47     Recipe Data  2 + Id
    48 - 63     Recipe Data  3 + Id
    64 - 79     Recipe Data  4 + Id
    80 - 95     Recipe Data  5 + Id
    96 - 111    Recipe Data  6 + Id
    112 - 127   Recipe Data  7 + Id
    128 - 143   Recipe Data  8 + Id
    144 - 159   Recipe Data  9 + Id
    160 - 175   Recipe Data 10 + Id
    176 - 185   Recipe Name  1
    186 - 195   Recipe Name  2
    196 - 205   Recipe Name  3
    206 - 215   Recipe Name  4
    216 - 225   Recipe Name  5
    226 - 235   Recipe Name  6
    236 - 245   Recipe Name  7
    246 - 255   Recipe Name  8
    256 - 265   Recipe Name  9
    266 - 275   Recipe Name 10
*/

//uint16_t logoStart = pgm_read_word(hbpro_logo);
//const uint16_t* logoData  = hbpro_logo;
//uint16_t logoStart = pgm_read_word(logoData);
const char* signMessage[] = {"Затирание","Пауза 1","Пауза 2","Пауза 3","Пауза 4","Пауза 5","Варка","Внос хмеля","°C","мин"};
const char* answer[] = {"Да","Нет"};
const uint16_t EEPROMsize   = 512;
const char* ssid     = "SSID";
const char* password = "PASSWORD";
const char* host = "dvc.hbpro.ru";
//const char* streamId   = "....................";
//const char* privateKey = "....................";

unsigned long connectingTime;
unsigned long startTime;
//таймаут подключения к WiFi в милисекундах
#define WIFI_CONNICTION_TIMEOUT  30000

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS  15
#define TFT_DC  2
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define T_CLK 14
#define T_CS  16
#define T_DIN 12
#define T_DO  13
#define T_IRQ 3
UTouch  myTouch(T_CLK, T_CS, T_DIN, T_DO, T_IRQ);
//Recipe recipe;
PCF8574 PCF_20(0x20);//26
  
struct coord {
  byte x[15];
  byte y[15];
};

const coord Pos={132,132,192,132,192,132,192,132,192,132,192,192,132,162,192, 84,103,103,122,122,141,141,160,160,179,179,198,217,217,217};

byte MenuDrawPosi = 1;
byte MenuNowPosi = 1;
byte RecipeMenuNowPos=0;
byte RecipeSaveNowPos=0;
String menuRecipe[11]={"Новый"};
byte amountNameRecipe=0;
byte nameRecipe[11];//={0,0,0,0,0,0,0,0,0,0};//11
byte idRecipe[10];//11 //numbRecipe
byte valueRecipe[15];
byte numItem;
//byte Previous_Menu=0;
byte xcur=0;
byte xinc=0;
byte xpl=0;
byte xlet=0;
byte xpl1=0;
byte xlet1=0;
byte nameRecipeWidth=0;
  
int szb=72;//width/height basic button
int sz12b=szb/2;//
int szbs=64;//width/height small button
int szi=48;//width/height icon
int szt=szbs;//touch button press 64x64
//grid 4x3
int x_1=43;//x column1
int x_2=121;//x column2
int x_3=199;//x column3
int x_4=277;//x column4
int y_1=42;//y row1
int y_2=120;//y row2
int y_3=198;//y row3

boolean WiFi_On = false;
boolean Load_1_On = false;
boolean Load_2_On = false;
boolean Main_Menu = true;
boolean RecipeSelect_Menu = false;
boolean Recipe_Menu = false;
boolean Graph_Menu = false;
boolean RecipeDelete_Menu = false;
boolean RecipeSave_Menu = false;
boolean Item_Edit = false;
boolean Item_Select = true;
boolean Letter_Edit = false;
boolean Letter_Select = true;
boolean AnswerNowPos;
boolean Settings_Menu = false;

float celsius, celsius2, celsiust;
Ticker tick;
Ticker ds0;

// Data wire is plugged into port 0 on the Arduino
#define ONE_WIRE_BUS 0
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature ds(&oneWire);

// Number of temperature devices found
int numberOfDevices;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 

// function to print a device address
void printAddress(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void getTemp(int i){
  ds.requestTemperaturesByIndex(i);
  tick.once_ms((750/ (1 << (12-TEMPERATURE_PRECISION))),readTemp,i );
}

void readTemp(int i){
  // get temperature
  //Serial.print("Temperature: ");
  celsius = roundf(ds.getTempCByIndex(i)*10)/10;
  Serial.println(celsius);  
}

void InitDisplay(){
    tft.begin();
    tft.setRotation(3);
}

String Uptime(){
  return TimeToString((millis()-startTime));
}

/*void SetRecipe() {
  recipe.name = "Hoegaarden";
  recipe.mashin = 45;
  recipe.step[0] = (STEP) { 45, 10 };
  recipe.step[1] = (STEP) { 60, 5 };
  recipe.step[2] = (STEP) { 66, 30 };
  recipe.step[3] = (STEP) { 72, 20 };
  recipe.step[4] = (STEP) { 75, 20 };
  recipe.boil = 90;
  recipe.hop[0] = (HOP) { 60 };
  recipe.hop[1] = (HOP) { 10 };
  recipe.hop[2] = (HOP) { 5 };
}*/

void ClearRect(){
  tft.fillRect(x_1-szbs/2, y_2-sz12b, szb*3+12, szb*2+12, ILI9341_BLACK);//szb*3+12, szb*2+6  //221, 150
}

void ClearHeaderAndRect(){
  tft.setFont(OPENSANS_18);
  tft.setTextColor(ILI9341_BLACK);
  tft.printAt(menuRecipe[MenuNowPosi-1], 160, 26, Center);
  tft.setTextColor(ILI9341_WHITE);
  ClearRect();
}

void DrawUpDownButton(){
  tft.fillRect(x_4-szbs/2, y_2-szbs/2, szbs, szbs, 0xE860);
  tft.drawBitmap(x_4-szi/2, y_2-szi/2, Icon_Up, szi,  szi, ILI9341_WHITE);
  tft.fillRect(x_4-szbs/2, y_3-szbs/2, szbs, szbs, 0x2D1A);
  tft.drawBitmap(x_4-szi/2, y_3-szi/2, Icon_Down, szi,  szi, ILI9341_WHITE);
}

void SelectMenuItem(){
  tft.setTextColor(ILI9341_BLACK,0xFE00);
  tft.printAt(menuRecipe[MenuNowPosi-1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi-1),Left);
  //Serial.println(idRecipe[MenuNowPosi-1]);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
}

void RecipeSelectMenu(){
  //InitDisplay();
  RecipeSelect_Menu=true;
  Main_Menu=false;
  Recipe_Menu=false;
  RecipeSave_Menu=false;
  RecipeDelete_Menu=false;
  tft.drawBitmap(x_1-szi/2, y_1-szi/2, Icon_Left, szi,  szi, ILI9341_WHITE);
  //tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0x4D6A);
  tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
  //DrawUpDownButton();
  tft.setFont(OPENSANS_18);
  tft.setTextColor(ILI9341_WHITE);
  tft.printAt("Рецепты", 160, 26, Center);  
  byte id = 0;//ID рецепта //numRecipe
  byte DrawI=1;
  numItem=0;
  EEPROM.begin(EEPROMsize);  
  for (byte i = 31; i < 176; i=i+16){
    //Найти все не пустые рецепты
    if (EEPROM.read(i) != 255){
      id=EEPROM.read(i);
      for (byte pos = 0; pos < 10; pos++){
        nameRecipe[pos]=EEPROM.read(176 + pos + ((id - 1) * 10));
        if (nameRecipe[pos]==255){
          nameRecipe[pos]=0;
          break;
        }
        //Serial.println(nameRecipe[pos]);
      }
      //Serial.println(id);
      menuRecipe[DrawI]=(char*)nameRecipe;//Сохранить название рецепта в массив имен menuRecipe
      idRecipe[DrawI]=id; //Сохранить ID рецепта в массив ID-шников idRecipe
      //Serial.println(sizeof(menuRecipe));
      //if (DrawI<5) tft.printAt(menuRecipe[DrawI],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*DrawI,Left);
      DrawI=DrawI+1;
      numItem=DrawI;//Количество сохраненных рецептов + нулевой, который Новый      
    }
  }
  //RunRecipe();
  //menuRecipe[MenuNowPosi-1].getBytes(nameRecipe,  menuRecipe[MenuNowPosi-1].length()+1);//toCharArray
  //Serial.println(menuRecipe[MenuNowPosi-1]);
  //Serial.println(menuRecipe[MenuNowPosi-1].length());
  //tft.printAt(menuRecipe[MenuNowPosi-2],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi-2),Left);
  //tft.printAt(menuRecipe[0],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(0),Left);
  if (MenuNowPosi<=5) {
    for(byte i=0; i<5; i++)
      tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    MenuDrawPosi=MenuNowPosi;
  }
  else {
    MenuDrawPosi=0;
    for(byte i=MenuNowPosi-5; i<MenuNowPosi; i++){
      tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*MenuDrawPosi,Left);
      MenuDrawPosi++;
    }
  }
  SelectMenuItem();
}

void RecipeMenu(){
  //InitDisplay();
  Recipe_Menu=true;
  Item_Select=true;
  Graph_Menu=false;
  RecipeDelete_Menu=false;
  ChangeIcon(RecipeMenuNowPos, &Item_Select);
//  if ((RecipeMenuNowPos>0) && (Item_Select)){
//    //Сменить иконку Далее на Edit
//    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
//    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
//    Item_Select=false;
//  }
  if (Item_Edit){
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_GREEN);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Active, szi,  szi, ILI9341_WHITE);
  }
  RecipeSelect_Menu=false;
  ClearRect();
  DrawTemplateRecipe();
  DrawItemsRecipe();
  
  //if (idRecipe[MenuNowPosi-1]!=0){
    //Скопировать из EEPROM рецепт по ID в RUN EEPROM
    //EEPROM.begin(EEPROMsize);
    //for(byte i=0; i<15; i++)
    //  if (EEPROM.read(i) != EEPROM.read((16+i)*idRecipe[MenuNowPosi-1]))
    //    EEPROM.write(i, EEPROM.read((16+i)*idRecipe[MenuNowPosi-1]));
    //EEPROM.commit();
  //}
  /*if(!Graph_Menu)
    RunRecipe();
  Graph_Menu=false;
  DrawItemsRecipe();*/    
}

void DrawTemplateRecipe(){
  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(OPENSANS_12);
  //const String signMessage[] = {"Затирание","Пауза 1","Пауза 2","Пауза 3","Пауза 4","Пауза 5","Варка","Внос хмеля","°C","мин"};
  tft.printAt(signMessage[0], x_1-szbs/2, y_2-sz12b, Left);
  tft.printAt(signMessage[8], 142, y_2-sz12b, Left);    
  int y_inc = tft.fontHeight();//19 for OPENSANS_12
  for (byte i = 1; i < 7; i++){
    tft.printAt(signMessage[i], x_1-szbs/2, y_2-sz12b+y_inc, Left);
    tft.printAt(signMessage[8], 142, y_2-sz12b+y_inc, Left); //°C
    tft.printAt(signMessage[9], x_3+sz12b-3, y_2-sz12b+y_inc, Right); //мин
    y_inc += tft.fontHeight();
  }
  tft.printAt(signMessage[7], x_1-szbs/2, y_2-sz12b+y_inc, Left);
  tft.printAt(signMessage[9], x_3+sz12b-3, y_2-sz12b+y_inc, Right);
  tft.printAt("/", 140, y_2-sz12b+y_inc, Right);
  tft.printAt("/", 170, y_2-sz12b+y_inc, Right);
}

void DrawItemsRecipe(){
  //Вывод значений
  for (byte i=0; i<15; i++)
    tft.printAt(String(valueRecipe[i], DEC),Pos.x[i],Pos.y[i],Right);
  tft.setTextColor(ILI9341_BLACK,0xFE00);
  if(RecipeMenuNowPos>0)
    tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
  //Serial.println(RecipeMenuNowPos);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  tft.printAt("100",132,y_2-sz12b+114,Right);  
}

void SaveRecipe(){
  EEPROM.begin(EEPROMsize);
  for(byte i=0; i<15; i++)
    if (valueRecipe[i] != EEPROM.read(i)){
      EEPROM.write(i, valueRecipe[i]);      
    }
  EEPROM.commit();
}

void SaveRecipeAs(){
  EEPROM.begin(EEPROMsize);
  for (byte i = 31; i < 176; i=i+16){
    //Найти первый свободный рецепт
    if (EEPROM.read(i) == 255){
      //ID рецепта
      byte id = (i+1)/16-1;      
      //Сохранить значения рецепта
      for(byte k=0; k<15; k++){
        if (valueRecipe[k] != EEPROM.read(id*16+k)){        
          EEPROM.write(id*16+k, valueRecipe[k]);                
        }
      }
      //Сохранить ID рецепта
      EEPROM.write(i ,id);
      //Сохранить имя рецепта
      for (byte l=0; l<10; l++){
        if (nameRecipe[l] != EEPROM.read(176+(id-1)*10+l)){
          EEPROM.write(176+(id-1)*10+l, nameRecipe[l]);      
        }
      }
    EEPROM.commit();
    break;
    }
  }
}

void RunRecipe() {
  /*EEPROM.begin(EEPROMsize);
  recipe.mashin = EEPROM.read(0);
  recipe.step[0] = (STEP) { EEPROM.read(1), EEPROM.read(2) };
  recipe.step[1] = (STEP) { EEPROM.read(3), EEPROM.read(4) };
  recipe.step[2] = (STEP) { EEPROM.read(5), EEPROM.read(6) };
  recipe.step[3] = (STEP) { EEPROM.read(7), EEPROM.read(8) };
  recipe.step[4] = (STEP) { EEPROM.read(9), EEPROM.read(10) };
  recipe.boil = EEPROM.read(11);
  recipe.hop[0] = (HOP) { EEPROM.read(12) };
  recipe.hop[1] = (HOP) { EEPROM.read(13) };
  recipe.hop[2] = (HOP) { EEPROM.read(14) };*/
  //Загрузка значений из EEPROM в массив valueRecipe
  EEPROM.begin(EEPROMsize);
  
  if (idRecipe[MenuNowPosi-1]==0){
    for (byte i=0; i<15; i++){
      if(EEPROM.read(i)==255)
        valueRecipe[i]=0;
      else         
        valueRecipe[i]=EEPROM.read(i);
    }
  }
  else{
    for (byte i=0; i<15; i++)
      valueRecipe[i]=EEPROM.read(16*idRecipe[MenuNowPosi-1]+i);
  }    
}

void DeleteRecipe(){
  EEPROM.begin(EEPROMsize);
//  byte id=EEPROM.read(16*idRecipe[MenuNowPosi-1]+15);
  //Удалить ID рецепта  
  EEPROM.write(16*idRecipe[MenuNowPosi-1]+15, 255);
  //Удалить имя рецепта
//  EEPROM.write(176+(id-1)*10, 32);
//  for (byte l=1; l<10; l++){
//    if (EEPROM.read(176+(id-1)*10+l)!=0){
//      EEPROM.write(176+(id-1)*10+l, 0);      
//      Serial.print("Сохраняем ячейку ");
//      Serial.println(176+(id-1)*10+l);
//    }
//  }
  EEPROM.commit();    
}

void RightPressing(){
  tft.printAt(menuRecipe[MenuNowPosi], 160, 26, Center);
}

void DownPressing(){
  //проверяем не является ли текущий пункт последним элементом
  if (MenuNowPosi<numItem){
    //если не является то двигаемся на 1 пункт вниз
    MenuNowPosi=MenuNowPosi+1;
    //проверяем упираемся ли мы в край экрана
    if (MenuDrawPosi<MenuDrawCount()){
      //если в край экрана не упираемся то также сдвигаем позицию на экране на 1 пункт вниз
      MenuDrawPosi=MenuDrawPosi+1;
      tft.printAt(menuRecipe[MenuNowPosi-2],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi-2),Left);
    }
    else {
      for(byte i=4; i>0; i--) {
        tft.setTextColor(ILI9341_BLACK);
        tft.printAt(menuRecipe[MenuNowPosi-(i+2)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi-(i+1)),Left);
        tft.setTextColor(ILI9341_WHITE);
        tft.printAt(menuRecipe[MenuNowPosi-(i+1)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi-(i+1)),Left);
      }
      tft.fillRect(x_1-sz12b, y_2-sz12b+tft.fontHeight()*4, szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
    }
  }
  else{
    MenuNowPosi=MenuDrawPosi=1;
    if (numItem>5){
      ClearRect();
      for(byte i=0; i<5; i++)
        tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    }
    else
      for(byte i=0; i<numItem; i++)
        tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
  }
}

void UpPressing(){
  //если текущая позиция в меню больше чем позиция первого элемента
  if (MenuNowPosi>1){
    //двигаемся на 1 пункт вверх
    MenuNowPosi=MenuNowPosi-1;
    //проверяем упираемся ли мы в край экрана
    if (MenuDrawPosi>1){
      //если в край экрана не упираемся то также сдвигаем позицию на экране на 1 пункт вверх
      MenuDrawPosi=MenuDrawPosi-1;
      tft.printAt(menuRecipe[MenuNowPosi],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi),Left);
    }
    else {
      for(byte i=4; i>0; i--) {
        tft.setTextColor(ILI9341_BLACK);
        tft.printAt(menuRecipe[MenuNowPosi+i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi+(i-1)),Left);
        tft.setTextColor(ILI9341_WHITE);
        tft.printAt(menuRecipe[MenuNowPosi+(i-1)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPosi+(i-1)),Left);
      }
    tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
    }
  }
  else {
    //MenuNowPosi=numItem;
    if (numItem>5){
      ClearRect();
      MenuNowPosi=numItem;
      MenuDrawPosi=5;
      for(byte i=0; i<MenuDrawPosi; i++)
        tft.printAt(menuRecipe[MenuNowPosi-5+i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    }  
    else if(numItem>0) {
      MenuNowPosi=MenuDrawPosi=numItem;
      for(byte i=0; i<MenuDrawPosi; i++)
        tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    //for(byte i=MenuNowPosi-5; i<MenuNowPosi; i++)
    // tft.printAt(menuRecipe[i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    }
  } 
}

void MainMenu(){
  Main_Menu=true;
  Letter_Select=true;
  RecipeSelect_Menu=false;
  //RecipeSave_Menu = false;
  Graph_Menu = false;
  Settings_Menu=false;
  InitDisplay();
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(x_1-sz12b, y_1-sz12b, szb*3+12, szb*2+6, ILI9341_DARKCYAN);
  
  tft.fillRect(7, 162, 72, 72, ILI9341_DARKGREEN);
  tft.drawBitmap(19, 174, Icon_Play, 48,  48, ILI9341_WHITE);
  tft.fillRect(85, 162, 72, 72, ILI9341_BLUE);
  if(WiFi_On) tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_WHITE);
  else tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_DARKGREY);
  tft.fillRect(163, 162, 72, 72, ILI9341_RED);
  tft.drawBitmap(175, 174, Icon_Settings, 48,  48, ILI9341_WHITE);
  
  tft.fillRect(241, 6, 72, 72, ILI9341_BLUE);
  if(Load_1_On) tft.drawBitmap(253, 18, Icon_Load, 48,  48, ILI9341_WHITE);
  else tft.drawBitmap(253, 18, Icon_Load, 48,  48, ILI9341_DARKGREY);
  tft.fillRect(241, 84, 72, 72, ILI9341_RED);
  if(Load_2_On) tft.drawBitmap(253, 96, Icon_Load, 48,  48, ILI9341_WHITE);
  else tft.drawBitmap(253, 96, Icon_Load, 48,  48, ILI9341_DARKGREY);
  tft.fillRect(241, 162, 72, 72, ILI9341_LIGHTGREY);
  tft.drawBitmap(253, 174, Icon_Load, 48,  48, ILI9341_DARKGREY);

  tft.setFont(OPENSANS_18); 
  tft.setCursor(150, 15);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("°C");

 /* tft.setFont(OPENSANS_16); 
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);
    //tft.println(" !\"#$%&'()*+,-./0123456789°:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]_abcdefghijklmnopqrstuvwxyz~АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя");
    tft.println("Итак, на 25 л готового сусла:");
    tft.println("Солод ячменный российский обыкновенный - 2.7 кг");
    tft.println("Крупа пшеничная - 2.4 кг");
    tft.println("Хлопья овсяные - 300 г");
    tft.println("Хмель Истринский 4% (какой был) - 30 г за 60 мин до конца варки, 15 г за 10 мин");
    tft.println("Цедра апельсина - 42 г за 5 мин до конца варки");
    tft.println("Кориандр - 14 г за 5 мин до конца варки");
    tft.println("Дрожжи Safbrew WB-06 - 11.5 г");
    tft.println("Вода - 32 л (начальный объем), 5 л (промывочная)");*/
}

void GraphMenu(){
  //InitDisplay();
  Graph_Menu=true;
  Recipe_Menu=false;
  RecipeDelete_Menu=false;
  RecipeSave_Menu=false;
  tft.fillRect(x_4-szbs/2, y_2-szbs/2, szbs, szbs, 0xE860);
  tft.drawBitmap(x_4-szi/2, y_2-szi/2, Icon_Delete, szi,  szi, ILI9341_WHITE);
  tft.fillRect(x_4-szbs/2, y_3-szbs/2, szbs, szbs, 0x2D1A);
  tft.drawBitmap(x_4-szi/2, y_3-szi/2, Icon_Save, szi,  szi, ILI9341_WHITE);
  //отрисовка фона графика
  tft.fillRect(x_1-szbs/2, y_2-sz12b,221, 150,ILI9341_LIGHTGREY);
  tft.drawRect(x_1-szbs/2, y_2-sz12b,221, 150,ILI9341_DARKGREY);
  //отрисовываем сетку
  for (byte i = 1; i < 5; i++){
    tft.drawLine((x_1-szbs/2)+i*44, y_2-sz12b,(x_1-szbs/2)+i*44, (y_2-sz12b)+149,ILI9341_DARKGREY);//Vertical
    tft.drawLine(x_1-szbs/2, (y_2-sz12b)+i*30,(x_1-szbs/2)+220, (y_2-sz12b)+i*30,ILI9341_DARKGREY);//Horizont
  }
  //координаты последней отрисованной точки
  int xl = 0;
  int yl = 0;
  //позиция начала координат
  int x0 = x_1-szbs/2+1;
  int y0 = 234;
  //масштаб
  float mx = 1.47;
  float my = 1.5;
  //масштабируем начальные координаты
  x0 = x0/mx;
  y0 = y0/my;
  //отрисовываем график
  //recipe.step[i].temp <--> valueRecipe[2*i+1]
  //recipe.step[i].time <--> valueRecipe[2*i+2]
  for(int i=0;i<5;i++){
    if(valueRecipe[2*i+2]!=0){
      if(xl == 0){
        for(int k=0;k<3;k++){
          tft.drawLine(mx*x0, my*(y0 - valueRecipe[2*i+1])+k, mx*(x0 + valueRecipe[2*i+2]), my*(y0 - valueRecipe[2*i+1])+k, ILI9341_RED); //первая пауза
        }
        xl = x0 + valueRecipe[2*i+2];
        yl = y0 - valueRecipe[2*i+1];    
      }
      else{
        for(int k=0;k<3;k++){
          tft.drawLine (mx*xl, my*yl+k, mx*(xl + (valueRecipe[2*i+1] - y0 + yl)), my*(y0 - valueRecipe[2*i+1])+k, ILI9341_RED);  //нагрев
          tft.drawLine (mx*(xl + (valueRecipe[2*i+1] - y0 + yl)), my*(y0 - valueRecipe[2*i+1])+k ,mx*(xl + (valueRecipe[2*i+1] - y0 + yl) + valueRecipe[2*i+2]), my*(y0 - valueRecipe[2*i+1])+k ,ILI9341_RED); //пауза
        }
        xl = xl + (valueRecipe[2*i+1] - y0 + yl) + valueRecipe[2*i+2];
        yl = y0 - valueRecipe[2*i+1];
      }
    }
  }
  //вывод делений на осях
  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(OPENSANS_10);
  for (byte i = 0; i < 5; i++){
    tft.printAt(String(i*30, DEC), (x_1-szbs/2)+i*44+3, y_3+sz12b-15, Left);
  }
  byte intVar=0;
  for (byte i = 4; i > 0; i--){ 
    intVar+=20; 
    tft.printAt(String(intVar, DEC), x_1-szbs/2+3, (y_2-sz12b)+i*30-15, Left);
  }
}

void SelectAnswerItem(){
  tft.setTextColor(ILI9341_BLACK,0xFE00);
  tft.printAt(answer[AnswerNowPos],(x_1-szbs/2)+221/2,(y_2-sz12b)+tft.fontHeight()*(AnswerNowPos+1),Center);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
}

void RecipeDeleteMenu(){
  //InitDisplay();
  RecipeDelete_Menu = true;
  Graph_Menu=false;
  ClearRect();
  DrawUpDownButton();
  tft.setFont(OPENSANS_18);
  tft.setTextColor(ILI9341_WHITE);
  tft.printAt("Удалить рецепт?",(x_1-szbs/2)+221/2,(y_2-sz12b),Center);
  for (byte i=0; i<2; i++)  tft.printAt(answer[i],(x_1-szbs/2)+221/2,(y_2-sz12b)+tft.fontHeight()*(i+1),Center);
  AnswerNowPos=0;
  SelectAnswerItem();
}

void RecipeSaveMenu(){
  RecipeSave_Menu = true;
  Letter_Select = true;
  Graph_Menu=false;
  ChangeIcon(RecipeSaveNowPos, &Letter_Select);
  if (Letter_Edit){
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_GREEN);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Active, szi,  szi, ILI9341_WHITE);
  }
  ClearRect();
  DrawUpDownButton();
  tft.setFont(OPENSANS_18);
  tft.setTextColor(ILI9341_WHITE);
  
  EEPROM.begin(EEPROMsize);
  boolean Free_Recipe=false;
  for (byte i = 31; i < 176; i=i+16){
    //Найти свободный рецепт
    if (EEPROM.read(i) == 255){
      Free_Recipe=true;
      break;
    }
  Free_Recipe=false;
  }
  if (Free_Recipe){
    tft.printAt("Сохранить как",x_1-szbs/2+221/2, y_2-sz12b,Center);
    //tft.printAt(menuRecipe[MenuNowPosi-1],x_1-szbs/2+221/2, y_2-sz12b+tft.fontHeight()*2,Center);
    CounNumbWidth();
    if (RecipeSaveNowPos!=0)
      MoveCursor();
    else
      PrintName();
  }
  else{
    ClearRect();
    tft.printAt("Память заполнена!!!",x_1-szbs/2+221/2, y_2-sz12b,Center);
    tft.printAt("Удалите ненужный",x_1-szbs/2+221/2, y_2-sz12b+tft.fontHeight()*2,Center);
    tft.printAt("рецепт",x_1-szbs/2+221/2, y_2-sz12b+tft.fontHeight()*3,Center);
    delay(5000);
    ClearHeaderAndRect();
    RecipeSelectMenu();
  }
}

void waitForIt(int x, int y, const uint8_t* Icon, uint16_t Basic_Color, uint16_t Push_Color){
  InitDisplay();
  tft.fillRect(x-sz12b, y-sz12b, szb, szb, Push_Color);
  tft.drawBitmap(x-szi/2, y-szi/2, Icon, szi,  szi, ILI9341_WHITE);
  delay(125); 
  while (myTouch.dataAvailable())
    delay(10);
  //myTouch.read();
  tft.fillRect(x-sz12b, y-sz12b, szb, szb, Basic_Color);
  tft.drawBitmap(x-szi/2, y-szi/2, Icon, szi,  szi, ILI9341_WHITE);
}

void waitForIt(){
  delay(125);
  while (myTouch.dataAvailable())
    delay(10);
}

void CounNumbWidth(){
  nameRecipeWidth=0;
  amountNameRecipe=0;
  for (byte i=0; i<10; i++){
    if (nameRecipe[i]==0) {
      for (byte k=i; k<11; k++)
        nameRecipe[k]=0;      
      break;
    }
    nameRecipeWidth=nameRecipeWidth+tft.charWidth(nameRecipe[i])+2;
    amountNameRecipe=amountNameRecipe+1;
  }
}

void MoveCursor(){
  //Стереть строку
  tft.fillRect(x_1-szbs/2,y_2-sz12b+tft.fontHeight()*2,szb*3+6,tft.fontHeight(),ILI9341_BLACK);
  //Установить курсор
  xinc=0;
  for (byte i=0; i<RecipeSaveNowPos-1; i++)
    xinc=xinc+tft.charWidth(nameRecipe[i])+2;
  xcur = x_1-szbs/2+221/2-(nameRecipeWidth-tft.charWidth(nameRecipe[RecipeSaveNowPos-1])+22)/2+xinc+2;
  tft.fillRect(xcur,y_2-sz12b+tft.fontHeight()*2,24,tft.fontHeight(),0xFE00);
  //Вывести название рецепта
  for (byte i=0; i<amountNameRecipe; i++){//+1  11
    if (nameRecipe[i]==0) {
      break;
    }
    if (i<RecipeSaveNowPos-1) {
      if (i==0)
        xpl1=x_1-szbs/2+221/2-(nameRecipeWidth-tft.charWidth(nameRecipe[RecipeSaveNowPos-1])+22)/2;
      else
        xlet1=xlet1+tft.charWidth(nameRecipe[i-1])+2;
      tft.drawChar(xpl1+xlet1+1,y_2-sz12b+tft.fontHeight()*2,nameRecipe[i],ILI9341_WHITE,ILI9341_BLACK,1);
    }
    if (i==RecipeSaveNowPos-1){
      xpl = xcur + (22 - tft.charWidth(nameRecipe[RecipeSaveNowPos-1]));
      tft.drawChar(xpl,y_2-sz12b+tft.fontHeight()*2,nameRecipe[RecipeSaveNowPos-1],ILI9341_BLACK,0xFE00,1);
    }
    if (i>RecipeSaveNowPos-1) {
      xlet=xlet+tft.charWidth(nameRecipe[i-1])+2;
      tft.drawChar(xpl+xlet,y_2-sz12b+tft.fontHeight()*2,nameRecipe[i],ILI9341_WHITE,ILI9341_BLACK,1);
    }
  }
  xlet=0;
  xlet1=0;
  //Serial.println((char*)nameRecipe);
}

void PrintName(){
  RecipeSaveNowPos=0;
  xinc=0;
  //Стереть строку
  tft.fillRect(x_1-szbs/2,y_2-sz12b+tft.fontHeight()*2,szb*3+6,tft.fontHeight(),ILI9341_BLACK);
  //Вывести название рецепта
  for (byte i=0; i<amountNameRecipe; i++){ //+1
    if (nameRecipe[i]==0) {
      break;
    }
    tft.drawChar(x_1-szbs/2+221/2-nameRecipeWidth/2+xlet,y_2-sz12b+tft.fontHeight()*2,nameRecipe[i],ILI9341_WHITE,ILI9341_BLACK,1);
    xlet=xlet+tft.charWidth(nameRecipe[i])+2;
  }
  xlet=0;
}

void Settings(){
  
}

//void ChangeIcon(){
//  if ((RecipeSaveNowPos>0) && (Letter_Select)){
//    //Сменить иконку Далее на Edit
//    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
//    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
//    Letter_Select=false;
//  }
//  if (RecipeSaveNowPos==0){
//    //Сменить иконку Edit на Далее
//    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
//    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
//    Letter_Select=true;
//  }
//}

void ChangeIcon(byte NowPos, boolean *Select){
  if ((NowPos>0) && (*Select)){
    //Сменить иконку Далее на Edit
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
    *Select=false;
  }
  if (NowPos==0){
    //Сменить иконку Edit на Далее
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
    *Select=true;
  }
}

boolean EditTemperatureItems() {
  if ((RecipeMenuNowPos==1)||(RecipeMenuNowPos==2)||(RecipeMenuNowPos==4)||(RecipeMenuNowPos==6)||(RecipeMenuNowPos==8)||(RecipeMenuNowPos==10))
    return true;
  else
    return false;
  }

void push_button(int nbtn){
/*  if ((nbtn==1)&&(!Main_Menu)){
    while (myTouch.dataAvailable())
    delay(10);
    //myTouch.read();
    InitDisplay();
    tft.fillScreen(ILI9341_BLACK);
    MainMenu();
  }*/
  
  if ((nbtn==1)&&(RecipeSelect_Menu)){
    waitForIt();
    //RecipeSelect_Menu=false;
    MainMenu();
    return;//Previous_Menu--;
  }

  if ((nbtn==1)&&(Settings_Menu)){
    waitForIt();
    //RecipeSelect_Menu=false;
    MainMenu();
    return;//Previous_Menu--;
  }
  
  if ((nbtn==1)&&(Recipe_Menu)){
    waitForIt();
    //Recipe_Menu=false;
    InitDisplay();
    ClearHeaderAndRect();
//    tft.setFont(OPENSANS_18);
//    tft.setTextColor(ILI9341_BLACK);
//    tft.printAt(menuRecipe[MenuNowPosi-1], 160, 26, Center);
//    tft.setTextColor(ILI9341_WHITE);
//    ClearRect();
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
    RecipeSelectMenu();
    return;//Previous_Menu--;
  }

  if ((nbtn==1)&&(Graph_Menu)){
    waitForIt();
    InitDisplay();
    //Graph_Menu=false;
    //Serial.println("Back to Recipe Menu");
    RecipeMenu();
    DrawUpDownButton();
    return;//Previous_Menu--;
  }

  if ((nbtn==1)&&(RecipeDelete_Menu)){
    waitForIt();
    InitDisplay();
    GraphMenu();
    //return;
  }  

  if ((nbtn==1)&&(RecipeSave_Menu)){
    waitForIt();
    InitDisplay();
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);    
    GraphMenu();
    //return;
  }    
  
  if ((nbtn==4)&&(Main_Menu)){
    waitForIt(x_4,y_1,Icon_Load,ILI9341_BLUE,0x055E);//Load 1
    EEPROM.begin(EEPROMsize);
    for (int i=0; i<EEPROMsize; i++){  
      Serial.print(i);
      Serial.print(" ");
      Serial.println(EEPROM.read(i));
    }
    if (Load_1_On){
      tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Load, szi,  szi, ILI9341_DARKGREY);//Off
      Load_1_On = false;
      PCF_20.write(1, 0);
    }
    else{
      tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Load, szi,  szi, ILI9341_WHITE);//On
      Load_1_On = true;
      PCF_20.write(1, 1);
    }
  }    
  
  if ((nbtn==8)&&(Main_Menu)){
    waitForIt(x_4,y_2,Icon_Load,ILI9341_RED,0xEA8A);//Load 2
    if (Load_2_On){
      tft.drawBitmap(x_4-szi/2, y_2-szi/2, Icon_Load, szi,  szi, ILI9341_DARKGREY);//Off
      Load_2_On = false;
      PCF_20.write(2, 0);
    }
    else{
      tft.drawBitmap(x_4-szi/2, y_2-szi/2, Icon_Load, szi,  szi, ILI9341_WHITE);//On
      Load_2_On = true;
      PCF_20.write(2, 1);
    }
  }

  /*if ((nbtn==4)&&(!Main_Menu)){
    InitDisplay();
    tft.setTextColor(ILI9341_BLACK);
    tft.printAt("Рецепты", 160, 26, Center);
    tft.setTextColor(ILI9341_WHITE);
    tft.printAt(menuRecipe[MenuNowPosi-1], 160, 26, Center);
    
    MashGraph();
    
    //tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, szb*2+6, ILI9341_BLACK);
    //RightPress();
    //DrawTemplateRecipe();
  }*/

  if ((nbtn==4)&&(RecipeSelect_Menu)){
    waitForIt();
    InitDisplay();
    tft.setTextColor(ILI9341_BLACK);
    tft.printAt("Рецепты", 160, 26, Center);
    tft.setTextColor(ILI9341_WHITE);
    tft.printAt(menuRecipe[MenuNowPosi-1], 160, 26, Center);
    for (byte i=0; i<11; i++)
      nameRecipe[i]=0;
    menuRecipe[MenuNowPosi-1].getBytes(nameRecipe,  menuRecipe[MenuNowPosi-1].length()+1);
    RunRecipe();
    RecipeMenu();
    return;
  }  

  if ((nbtn==4)&&(Recipe_Menu)&&(!Item_Edit)){
    //RecipeSelect_Menu=false;
    waitForIt();
    InitDisplay();
    if (RecipeMenuNowPos>0) Item_Select=true;
    else Item_Select=false;
    if(!Item_Select){
      GraphMenu();
    }
    else {
      Item_Edit=true;
      tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_GREEN);
      tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Active, szi,  szi, ILI9341_WHITE);
    }
    return;
  }

  if ((nbtn==4)&&(Graph_Menu)){
    waitForIt();
    //InitDisplay();
    SaveRecipe();
    MainMenu();
    //return;    
  }

  if ((nbtn==4)&&(Item_Edit)){
    waitForIt();
    InitDisplay();
    Item_Edit=false;
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
    Item_Select=false;    
  }

  if ((nbtn==4)&&(RecipeDelete_Menu)){
    waitForIt();
    InitDisplay();
    RecipeDelete_Menu=false;
    if (AnswerNowPos==0) {
      //Очистить/Удалить рецепт
      DeleteRecipe();
      ClearHeaderAndRect();
//      tft.setTextColor(ILI9341_BLACK);
//      tft.printAt(menuRecipe[MenuNowPosi-1], 160, 26, Center);
//      tft.setTextColor(ILI9341_WHITE);
//      ClearRect();
      //numItem=numItem-1;
      for (byte i=1; i<11; i++)
        menuRecipe[i]="";
      MenuNowPosi=1;
      MenuDrawPosi=1;      
      RecipeSelectMenu();
    }
    else
      RecipeMenu();
  }

  if ((nbtn==4)&&(RecipeSave_Menu)&&(!Letter_Edit)){
    //RecipeSelect_Menu=false;
    waitForIt();
    InitDisplay();
    if (RecipeSaveNowPos>0) Letter_Select=true;
    else Letter_Select=false;
    if(!Letter_Select){
      //Letter_Select=true;
      SaveRecipeAs();
      ClearHeaderAndRect();
      //MainMenu();
      //numItem=numItem-1;
      MenuNowPosi=1;
      MenuDrawPosi=1;      
      RecipeSelectMenu();
    }
    else {
      Letter_Edit=true;
      tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_GREEN);
      tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Active, szi,  szi, ILI9341_WHITE);
    }
    return;
  } 

  if ((nbtn==4)&&(Letter_Edit)){
    waitForIt();
    InitDisplay();
    Letter_Edit=false;
    tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
    Letter_Select=false;    
  }

  if ((nbtn==8)&&(RecipeSelect_Menu)){
    InitDisplay();
    UpPressing();
    //UpPress();
    //InitDisplay();
    SelectMenuItem();
//    if (myTouch.dataAvailable()) {
//      delay(50);
//      myTouch.InitTouch();
//      myTouch.read();
//      //break;
//      }
    //draww();
    //DrawMenu();
  }     
    
  if ((nbtn==9)&&(Main_Menu)){
    waitForIt(x_1,y_3,Icon_Play,ILI9341_DARKGREEN,ILI9341_GREEN);//Play
    //DrawMenu();
    tft.fillScreen(ILI9341_BLACK);
    DrawUpDownButton();
    RecipeSelectMenu();
    Main_Menu=false;
    //while (!myTouch.dataAvailable())//Если ничего не нажато
    //  delay(10);
    myTouch.InitTouch();
  }
    
  if ((nbtn==10)&&(Main_Menu)){
    waitForIt(x_2,y_3,Icon_WiFi,ILI9341_BLUE,0x055E);//WiFi
    if (WiFi_On){
      WiFi.disconnect(); 
      tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_DARKGREY);//Off
      WiFi_On = false;
    }
    else{
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
      connectingTime=millis();
      while (WiFi.status() != WL_CONNECTED) {
        delay(125);
        Serial.print(".");
        tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_WHITE);
        delay(125);
        tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_DARKGREY);
        if ((millis()-connectingTime)>WIFI_CONNICTION_TIMEOUT) break;
      }
      if (WiFi.status() == WL_CONNECTED){
        Serial.println("");
        Serial.println("WiFi connected");  
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        tft.drawBitmap(97, 174, Icon_WiFi, 48,  48, ILI9341_WHITE);//On
        WiFi_On = true;        
      }
    }
  }
 
  if ((nbtn==11)&&(Main_Menu)){
    waitForIt(x_3,y_3,Icon_Settings,ILI9341_RED,0xEA8A);//Settings
    tft.fillScreen(ILI9341_BLACK);
    tft.drawBitmap(x_1-szi/2, y_1-szi/2, Icon_Left, szi,  szi, ILI9341_WHITE);
    tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
    DrawUpDownButton();
    tft.setFont(OPENSANS_18);
    tft.setTextColor(ILI9341_WHITE);
    //String message =  String("Настройки");
    //Serial.print(message.length());
    //tft.printAt(message, 160, 26+29, Center);
    MenuSetup();
    DrawMenu();
    //tft.printAt("Настройки", 160, 26, Center);
    Main_Menu=false;
    Settings_Menu=true;
    //return;
  }

  if ((nbtn==4)&&(Settings_Menu)){
    InitDisplay();
    RightPress();
    draww();
  }  

  if ((nbtn==12)&&(Settings_Menu)){
    InitDisplay();
    DownPress();
    draww();
  }

  if ((nbtn==8)&&(Settings_Menu)){
    InitDisplay();
    UpPress();
    draww();
  }

  if ((nbtn==12)&&(RecipeSelect_Menu)){
    InitDisplay();
    DownPressing();
    //DownPress();
    //InitDisplay();
    SelectMenuItem();
//    if (myTouch.dataAvailable()){
//      delay(50);
//      myTouch.InitTouch();
//      myTouch.read();
//      //break;
//      }    
    //draww();
    //DrawMenu();
  }
  
  if ((nbtn==12)&&(Recipe_Menu)){
    InitDisplay();
    //если не находимся в режиме редактирования то кнопка используется для передвижения по меню
    if (Item_Edit==false){
      //проверяем не является ли текущий пункт последним элементом
      if (RecipeMenuNowPos<15){
        //если не является то двигаемся на 1 пункт вниз
        RecipeMenuNowPos=RecipeMenuNowPos+1;
        tft.setTextColor(ILI9341_BLACK,0xFE00);
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
        //Serial.println(RecipeMenuNowPos);
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        if (RecipeMenuNowPos>1) tft.printAt(String(valueRecipe[RecipeMenuNowPos-2], DEC),Pos.x[RecipeMenuNowPos-2],Pos.y[RecipeMenuNowPos-2],Right);
      }
      else {
        //Item_Select=true;
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
        RecipeMenuNowPos=0;
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        tft.printAt(String(valueRecipe[RecipeMenuNowPos+14], DEC),Pos.x[RecipeMenuNowPos+14],Pos.y[RecipeMenuNowPos+14],Right);        
      }
      ChangeIcon(RecipeMenuNowPos, &Item_Select);
//      if ((RecipeMenuNowPos>0) && (Item_Select)){
//        //Сменить иконку Далее на Edit
//        tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
//        tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
//        Item_Select=false;
//      }
//      if (RecipeMenuNowPos==0){
//        tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
//        tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
//        Item_Select=true;
//      }
     
    }
    //если находимся в режиме редактирования 
    if (Item_Edit==true) {
      //if (valueRecipe[RecipeMenuNowPos-1]>0) 
      {
        valueRecipe[RecipeMenuNowPos-1]=valueRecipe[RecipeMenuNowPos-1]-1;
        if (EditTemperatureItems()) {
          if ((valueRecipe[RecipeMenuNowPos-1]==255)||(valueRecipe[RecipeMenuNowPos-1]<20)) valueRecipe[RecipeMenuNowPos-1]=80;
        }        
        //if ((((RecipeMenuNowPos==1)||(RecipeMenuNowPos==2)||(RecipeMenuNowPos==4)||(RecipeMenuNowPos==6)||(RecipeMenuNowPos==8)||(RecipeMenuNowPos==10))&&(valueRecipe[RecipeMenuNowPos-1]==255))||(valueRecipe[RecipeMenuNowPos-1]<20))
        //  valueRecipe[RecipeMenuNowPos-1]=80;
        else
          if (valueRecipe[RecipeMenuNowPos-1]==255) valueRecipe[RecipeMenuNowPos-1]=120;
        tft.fillRect(Pos.x[RecipeMenuNowPos-1]-26,Pos.y[RecipeMenuNowPos-1], 26, tft.fontHeight(), ILI9341_BLACK);        
        //tft.fillRect(Pos.x[RecipeMenuNowPos-1]-tft.stringWidth(String(valueRecipe[RecipeMenuNowPos-1]+1, DEC)),Pos.y[RecipeMenuNowPos-1], tft.stringWidth(String(valueRecipe[RecipeMenuNowPos-1]+1, DEC))+2, tft.fontHeight(), ILI9341_BLACK);
        tft.setTextColor(ILI9341_BLACK,0xFE00);
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
      }
    }
  }

  if ((nbtn==8)&&(Graph_Menu)){
    waitForIt();
    InitDisplay();
    RecipeDeleteMenu();
  }

  if ((nbtn==12)&&(Graph_Menu)){
    waitForIt();
    InitDisplay();
    RecipeSaveMenu();
//    CounNumbWidth();
//    if (RecipeSaveNowPos!=0){
//      MoveCursor();      
//    }
//    else{
//      PrintName();
//    }
    return;
  }  

  if ((nbtn==8)&&(RecipeDelete_Menu)){
    waitForIt();
    InitDisplay();
    if (AnswerNowPos==1) {
      tft.printAt(answer[AnswerNowPos],(x_1-szbs/2)+221/2,(y_2-sz12b)+tft.fontHeight()*(AnswerNowPos+1),Center);
      AnswerNowPos=0;
      SelectAnswerItem();
    }
  }  

  if ((nbtn==12)&&(RecipeDelete_Menu)){
    waitForIt();
    InitDisplay();
    if (AnswerNowPos==0) {
      tft.printAt(answer[AnswerNowPos],(x_1-szbs/2)+221/2,(y_2-sz12b)+tft.fontHeight()*(AnswerNowPos+1),Center);
      AnswerNowPos=1;
      SelectAnswerItem();
    }
  }    

  if ((nbtn==8)&&(Recipe_Menu)){
    //Сменить иконку Далее на Edit
    InitDisplay();
    //если не находимся в режиме редактирования то кнопка используется для передвижения по меню
    if (Item_Edit==false){
      //если текущая позиция в меню больше чем позиция первого элемента
      if (RecipeMenuNowPos>0){
        //двигаемся на 1 пункт вверх
        RecipeMenuNowPos=RecipeMenuNowPos-1;
        if (RecipeMenuNowPos!=0){
          tft.setTextColor(ILI9341_BLACK,0xFE00);
          tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
        }
        //Serial.println(RecipeMenuNowPos);
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        if (RecipeMenuNowPos<15) tft.printAt(String(valueRecipe[RecipeMenuNowPos], DEC),Pos.x[RecipeMenuNowPos],Pos.y[RecipeMenuNowPos],Right);
      }
      else {
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);        
        RecipeMenuNowPos=15;
        tft.setTextColor(ILI9341_BLACK,0xFE00);
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
      }
      ChangeIcon(RecipeMenuNowPos, &Item_Select);
//      if ((RecipeMenuNowPos>0) && (Item_Select)){
//        //Сменить иконку Далее на Edit
//        tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, 0xFE00);
//        tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Edit, szi,  szi, ILI9341_WHITE);
//        Item_Select=false;
//      }
//      if (RecipeMenuNowPos==0){
//        tft.fillRect(x_4-szbs/2, y_1-szbs/2, szbs, szbs, ILI9341_BLACK);
//        tft.drawBitmap(x_4-szi/2, y_1-szi/2, Icon_Right, szi,  szi, ILI9341_WHITE);
//        Item_Select=true;
//      }       
    }
    //если находимся в режиме редактирования 
    if (Item_Edit==true) {

      //if (valueRecipe[RecipeMenuNowPos-1]>0) 
      {
        valueRecipe[RecipeMenuNowPos-1]=valueRecipe[RecipeMenuNowPos-1]+1;
        if (EditTemperatureItems()) {
          if ((valueRecipe[RecipeMenuNowPos-1]>80)||(valueRecipe[RecipeMenuNowPos-1]<20)) valueRecipe[RecipeMenuNowPos-1]=20;
        }
        else
          if (valueRecipe[RecipeMenuNowPos-1]>120) valueRecipe[RecipeMenuNowPos-1]=0;
        tft.fillRect(Pos.x[RecipeMenuNowPos-1]-26,Pos.y[RecipeMenuNowPos-1], 26, tft.fontHeight(), ILI9341_BLACK);
        //tft.fillRect(Pos.x[RecipeMenuNowPos-1]-tft.stringWidth(String(valueRecipe[RecipeMenuNowPos-1]-1, DEC)),Pos.y[RecipeMenuNowPos-1], tft.stringWidth(String(valueRecipe[RecipeMenuNowPos-1]-1, DEC))+2, tft.fontHeight(), ILI9341_BLACK);
        tft.setTextColor(ILI9341_BLACK,0xFE00);
        tft.printAt(String(valueRecipe[RecipeMenuNowPos-1], DEC),Pos.x[RecipeMenuNowPos-1],Pos.y[RecipeMenuNowPos-1],Right);
      }
    }    
  }


  if ((nbtn==12)&&(RecipeSave_Menu)){
    InitDisplay();
    //если не находимся в режиме редактирования то кнопка используется для передвижения по символам
    if (Letter_Edit==false){
      CounNumbWidth();
      //проверяем не является ли текущий пункт последним элементом
      if ((RecipeSaveNowPos<amountNameRecipe+1)&&(RecipeSaveNowPos<10)){
        //если не является то двигаемся по символам вниз/вперед
        RecipeSaveNowPos=RecipeSaveNowPos+1;
        MoveCursor();
      }
      else {
        //Letter_Select=true;
        PrintName();
      }
      ChangeIcon(RecipeSaveNowPos, &Letter_Select);
    }
    //если находимся в режиме редактирования 
    if (Letter_Edit==true) {
      
      if ((nameRecipe[RecipeSaveNowPos-1]<=192)&&(nameRecipe[RecipeSaveNowPos-1]>177)) nameRecipe[RecipeSaveNowPos-1] = 177;
      if ((nameRecipe[RecipeSaveNowPos-1]<=176)&&(nameRecipe[RecipeSaveNowPos-1]>127)) nameRecipe[RecipeSaveNowPos-1] = 127;
      if ((nameRecipe[RecipeSaveNowPos-1]<=93) &&(nameRecipe[RecipeSaveNowPos-1]>92))  nameRecipe[RecipeSaveNowPos-1] = 92;
      if ((nameRecipe[RecipeSaveNowPos-1]<=32) &&(nameRecipe[RecipeSaveNowPos-1]>1))   nameRecipe[RecipeSaveNowPos-1] = 1;

      nameRecipe[RecipeSaveNowPos-1]=nameRecipe[RecipeSaveNowPos-1]-1;
      tft.fillRect(xcur,y_2-sz12b+tft.fontHeight()*2,24,tft.fontHeight(),0xFE00);
     
      if (nameRecipe[RecipeSaveNowPos-1]==0)
        tft.fillRect(xcur,y_2-sz12b+tft.fontHeight()*2,24,tft.fontHeight(),ILI9341_BLACK);
      else 
        tft.drawChar(xcur + (22 - tft.charWidth(nameRecipe[RecipeSaveNowPos-1])),y_2-sz12b+tft.fontHeight()*2,nameRecipe[RecipeSaveNowPos-1],ILI9341_BLACK,0xFE00,1);
    
    }
  
  }

  if ((nbtn==8)&&(RecipeSave_Menu)){
    InitDisplay();
    //если не находимся в режиме редактирования то кнопка используется для передвижения по символам
    if (Letter_Edit==false){
      CounNumbWidth();
      //проверяем не является ли текущий пункт последним элементом
      if (RecipeSaveNowPos>1){
        //если не является то двигаемся по символам вверх/назад
        RecipeSaveNowPos=RecipeSaveNowPos-1;
        MoveCursor();
      }
      else {
        PrintName();
      }
      ChangeIcon(RecipeSaveNowPos, &Letter_Select);
    }
    //если находимся в режиме редактирования 
    if (Letter_Edit==true) {
      
      if ((nameRecipe[RecipeSaveNowPos-1]>=176)&&(nameRecipe[RecipeSaveNowPos-1]<192)) nameRecipe[RecipeSaveNowPos-1] = 191;
      if ((nameRecipe[RecipeSaveNowPos-1]>=126)&&(nameRecipe[RecipeSaveNowPos-1]<176)) nameRecipe[RecipeSaveNowPos-1] = 175;
      if ((nameRecipe[RecipeSaveNowPos-1]>=91) &&(nameRecipe[RecipeSaveNowPos-1]<93))  nameRecipe[RecipeSaveNowPos-1] = 92;
      if ((nameRecipe[RecipeSaveNowPos-1]>=0)  &&(nameRecipe[RecipeSaveNowPos-1]<32))  nameRecipe[RecipeSaveNowPos-1] = 31;

      nameRecipe[RecipeSaveNowPos-1]=nameRecipe[RecipeSaveNowPos-1]+1;
      tft.fillRect(xcur,y_2-sz12b+tft.fontHeight()*2,24,tft.fontHeight(),0xFE00);
      
      if (nameRecipe[RecipeSaveNowPos-1]==0)
        tft.fillRect(xcur,y_2-sz12b+tft.fontHeight()*2,24,tft.fontHeight(),ILI9341_BLACK);
      else 
        tft.drawChar(xcur + (22 - tft.charWidth(nameRecipe[RecipeSaveNowPos-1])),y_2-sz12b+tft.fontHeight()*2,nameRecipe[RecipeSaveNowPos-1],ILI9341_BLACK,0xFE00,1);
    
    }
  
  }  
  
}

//Возвращает время в формате "HH:mm:ss" из миллисекунд
char * TimeToString(unsigned long t){
  static char str[10];
  //unsigned short ms = t % 1000;
  unsigned char s = (t/1000) % 60;
  unsigned char m = (t / 1000 / 60) % 60;
  long h = (t / 1000 / 60 / 60);
  sprintf(str, "%02ld:%02d:%02d", h, m, s);
  return str;
}

void WiFi_send(){
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/settemp.php?temp=";
  url += celsiust;
  //url += "?private_key=";
  //url += privateKey;
  //url += "&value=";
  //url += value;
  
  //String request = "GET /settemp.php?temp=" + String(temp_char) + " HTTP/1.1\r\n";

  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
 /* while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection"); */
}

void setup() {
  //ESP.wdtDisable();
  Serial.begin(115200);
  Wire.begin();//4,5
  PCF_20.write8(0);
  SPIFFS.begin();
  //myTouch.setPrecision(PREC_EXTREME); 
  
  InitDisplay();
  tft.fillScreen(0x8B6C);
  bmpDraw("/HBPro_logo.bmp", 96, 56);
  //playMelody();
  Beep(3,300);

/*  tft.setAddrWindow(96,56,223,183);
  for (int px = 0; px < 16384; px++)
    //logoStart++;
    tft.pushColor(pgm_read_word(&(hbpro_logo[px])));*/
  
  //MenuSetup();
    
  myTouch.InitTouch();
  while (!myTouch.dataAvailable())
    delay(10);

  //connectingTime;
  startTime=millis();
  MainMenu();

  myTouch.InitTouch();

//  EEPROM.begin(EEPROMsize);
//  EEPROM.write(0, 45);
//  EEPROM.write(1, 45);
//  EEPROM.write(2, 10);
//  EEPROM.write(3, 60);
//  EEPROM.write(4, 5);
//  EEPROM.write(5, 66);
//  EEPROM.write(6, 30);
//  EEPROM.write(7, 72);
//  EEPROM.write(8, 20);
//  EEPROM.write(9, 75);
//  EEPROM.write(10, 20);
//  EEPROM.write(11, 90);
//  EEPROM.write(12, 60);
//  EEPROM.write(13, 10);
//  EEPROM.write(14, 5);
//  EEPROM.write(15, 1);
//  EEPROM.write(16, 45);
//  EEPROM.write(17, 45);
//  EEPROM.write(18, 10);
//  EEPROM.write(19, 60);
//  EEPROM.write(20, 5);
//  EEPROM.write(21, 66);
//  EEPROM.write(22, 30);
//  EEPROM.write(23, 72);
//  EEPROM.write(24, 20);
//  EEPROM.write(25, 75);
//  EEPROM.write(26, 20);
//  EEPROM.write(27, 90);
//  EEPROM.write(28, 60);
//  EEPROM.write(29, 10);
//  EEPROM.write(30, 5);
//  EEPROM.write(31, 1);
//  EEPROM.write(47, 2);
//  EEPROM.write(79, 4);
//  EEPROM.write(176, 72);//H
//  EEPROM.write(177, 111);//o
//  EEPROM.write(178, 101);//e
//  EEPROM.write(179, 103);//g
//  EEPROM.write(180, 97);//a
//  EEPROM.write(181, 97);//a
//  EEPROM.write(182, 114);//r
//  EEPROM.write(183, 100);//d
//  EEPROM.write(184, 101);//e
//  EEPROM.write(185, 110);//n
//  EEPROM.write(186, 209);С
//  EEPROM.write(187, 226);в
//  EEPROM.write(188, 229);е
//  EEPROM.write(189, 242);т
//  EEPROM.write(190, 235);л
//  EEPROM.write(191, 238);о
//  EEPROM.write(192, 229);е
//  EEPROM.write(206, 240);
//  EEPROM.write(207, 241);
//  EEPROM.write(208, 242);
//  EEPROM.commit();

  // Start up the library
  ds.begin();

  // Grab a count of devices on the wire
  numberOfDevices = ds.getDeviceCount();

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (ds.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

   // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(ds.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      ds.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      
      Serial.print("Resolution actually set to: ");
      Serial.print(ds.getResolution(tempDeviceAddress), DEC); 
      Serial.println();
    }
    else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

  //disable sync reading temperature
  ds.setWaitForConversion(false);
  
  ds0.attach(1,getTemp,0);
}

void loop(void) {
  delay(10);
  //myTouch.InitTouch();
  while (myTouch.dataAvailable()){
    delay(10);
    myTouch.InitTouch();
    Serial.print("Touch Data");
    myTouch.read();
    long x, y;
    x = myTouch.getX();
    y = myTouch.getY();
    if ((x!=-1) and (y!=-1)){
      Serial.print(" ");
      Serial.print(x);
      Serial.print(", ");
      Serial.println(y);
    
      if ((y>=y_1-szt/2) && (y<=y_1+szt/2)) //row1
      {
        if ((x>=x_1-szt/2) && (x<=x_1+szt/2))  // Touch Button: 1
        {
          push_button(1);
        }
        if ((x>=x_2-szt/2) && (x<=x_2+szt/2))  // Touch Button: 2
        {
          //push_button(2);
        }
        if ((x>=x_3-szt/2) && (x<=x_3+szt/2))  // Touch Button: 3
        {
          //push_button(3);
        }
        if ((x>=x_4-szt/2) && (x<=x_4+szt/2))  // Touch Button: 4
        {
          push_button(4);
        }
      }
      if ((y>=y_2-szt/2) && (y<=y_2+szt/2)) //row2
      {
        if ((x>=x_1-szt/2) && (x<=x_1+szt/2))  // Touch Button: 5
        {
          //push_button(5);
        }
        if ((x>=x_2-szt/2) && (x<=x_2+szt/2))  // Touch Button: 6
        {
          //push_button(6);
        }
        if ((x>=x_3-szt/2) && (x<=x_3+szt/2))  // Touch Button: 7
        {
          //push_button(7);
        }
        if ((x>=x_4-szt/2) && (x<=x_4+szt/2))  // Touch Button: 8
        {
          push_button(8);
        }
      }
      if ((y>=y_3-szt/2) && (y<=y_3+szt/2)) //row3
      {
        if ((x>=x_1-szt/2) && (x<=x_1+szt/2))  // Touch Button: 9
        {
          push_button(9);
        }
        if ((x>=x_2-szt/2) && (x<=x_2+szt/2))  // Touch Button: 10
        {
          push_button(10);
        }
        if ((x>=x_3-szt/2) && (x<=x_3+szt/2))  // Touch Button: 11
        {
          push_button(11);
        }
        if ((x>=x_4-szt/2) && (x<=x_4+szt/2))  // Touch Button: 12
        {
          push_button(12);
        }
      }
    }
  }

  if(Main_Menu){
    celsiust = celsius;
    if (celsius2 != celsiust) {
      InitDisplay();
      tft.setFont(ROBOTO_48); 
      tft.setCursor(50, 18);
      tft.setTextColor(ILI9341_DARKCYAN);
      if (celsius2 == -127) {
        tft.print("--.-");
      }
      else {
        tft.print(celsius2,1);
      }
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(50, 18);
      if (celsiust == -127) {
        tft.print("--.-");
      }
      else {
        tft.print(celsiust,1);
      }
      celsius2 = celsiust;
      Serial.print("Free RAM = ");
      Serial.println(ESP.getFreeHeap());
      //Serial.print("Uptime = ");
      //Serial.println(TimeToString((millis()-startTime)));
      //Serial.println(Uptime());
      if (WiFi_On) WiFi_send();
    }
  }
// Wire.beginTransmission(0x26);
// Wire.write(0);
// Wire.endTransmission();
// delay(250);
// Wire.beginTransmission(0x26);
// Wire.write(255);
// Wire.endTransmission();

  
  /*  for (int i=0; i<8; i++)
  {
    PCF_20.write(i, 1);
    delay(100);
    PCF_20.write(i, 0);
    delay(100);
  }*/
/* tft.begin();
 tft.setRotation(3);
    //tft.fillRect(7, 162, 72, 72, 0x045A);
    tft.drawBitmap(19, 174, Icon_Heat2, 48,  48, ILI9341_RED);
    tft.drawBitmap(19, 174, Icon_Heat1, 48,  48, ILI9341_WHITE);
  delay(250);
    //tft.fillRect(7, 162, 72, 72, 0x045A);
    tft.drawBitmap(19, 174, Icon_Heat1, 48,  48, ILI9341_RED);
    tft.drawBitmap(19, 174, Icon_Heat2, 48,  48, ILI9341_WHITE);     
  delay(250);*/
}
