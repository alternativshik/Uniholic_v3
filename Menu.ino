//#include "U8glib.h"
//#include <EEPROM.h>
//указание пинов для использования дисплея, не обязательно брать пины аппаратного SPI
//U8GLIB_ST7920_128X64_1X u8g(36, 38, 39);
/*
//переменные для работы с клавиатурой
//пин к которому подключена панель управления
int KeyButton1Pin=A0;
//значение с панели управления
int KeyButton1Value=0;
//последнее время когда на панели не было нажатых кнопок
long KeyButton1TimePress=0;
//задержка перед считыванием состояния панели управления после нажатия
long KeyButton1Latency=100000;
//метка означающая что нажатие кнопки на панели было обработано
int KeyButton1WasChecked=0;
//время после которого удерживание кнопки начинает засчитываться как многократные быстрые нажатия
long KeyButton1RepeatLatency=1500000;
//вспомогательная переменная для обработки повторных нажатий, после каждого повторного срабатывания переменная увеличивается
//сдвигая тем самым време следующего повторного срабатывания зажатой кнопки
long KeyButton1RepeatTimePress=0;
//переменная для хранения времени между временем когда не было зажатых кнопок и временем проверки
long KeyButton1TimeFromPress=0;
//Переменные используются для периодической обработки нажатий клавиатуры
//время прошлой обработки клавиатуры
long KeyBoardTime1=0;
//текущее время
long KeyBoardTime2=0;
//период обработки клавиатуры
long KeyBoardTimeInterval=25000;*/

//Переменные отвечающие за меню
//Массив с названиями меню
const char* MenuNames[50];
//Тип элемента меню
//0-родительское меню
//1-целое число
//2-временной интервал (h:m:s, целое число но отображается как время)
//3-Вкл/Выкл (целое число, но отображается как On/Off)
//4-Расстояние (cm, целое число, но отображается как 0.хх метров)
int MenuTypeCode[50];
//Значение элемента меню
int MenuValue[50];
//Текущая позиция в меню, вложенность не важна т.к. меню представляет из себя список с ссылками на родительские 
//и дочерние пункты
int MenuNowPos=0;
//Режим редактирования (0-нет, просто выделен определенный параметр или пункт меню, 1-редактируем значение параметра)
int MenuEdit=0;
//номер элемента меню который является для данного родительским
//0-нет родительского элемента
int MenuParent[50];
//номер элемента меню который является для данного дочерним
//0-нет дочернего элемента
int MenuChild[50];
//Номер элемента дочернего меню который является первым
int MenuChildFirst[50];
//номер элемента дочернего меню который является последним
int MenuChildEnd[50];
//позиция меню в которой находится выделенный пункт на экране (например на экране отображается 3 пункта при этом выделен второй)
int MenuDrawPos=0;
//максимальное количество отображаемых на экране пунктов
//int MenuDrawCount=5;
//byte MenuDrawCount=((y_3+sz12b)-(y_2-sz12b))/tft.fontHeight();
byte MenuDrawCount(){
  return (152/tft.fontHeight());
}
/*
//переменные для таймера перерисовки
//время последней перерисовки
long DrawTime1=0;
//текущее время
long DrawTime2=0;
//интервал для перерисовки экрана
long DrawTimeInterval=200000;
*/

//#define MENU_DRAW_COUNT MenuDrawCount;

void MenuSetup()
{
//Настройка меню
//задаем начальное положение в меню
//MenuNowPos=1;
//Массив с названиями меню, индексами родительских элементов меню, начальными и конечными индексами дочерних элементов меню
//также задаем начальные значения параметров элементов и их тип
MenuNames[0]="Настройки";//Main Menu
MenuTypeCode[0]=0;
MenuValue[0]=0;
MenuParent[0]=0;
MenuChildFirst[0]=1;
MenuChildEnd[0]=4;

MenuNames[1]="Нагрев";//Menu1
MenuTypeCode[1]=0;
MenuValue[1]=0;
MenuParent[1]=0;
MenuChildFirst[1]=5;
MenuChildEnd[1]=11;

MenuNames[2]="Насос";//Menu2
MenuTypeCode[2]=0;
MenuValue[2]=0;
MenuParent[2]=0;
MenuChildFirst[2]=12;
MenuChildEnd[2]=14;

MenuNames[3]="Общие";//Menu3
MenuTypeCode[3]=0;
MenuValue[3]=0;
MenuParent[3]=0;
MenuChildFirst[3]=15;
MenuChildEnd[3]=17;

MenuNames[4]="Об устройстве";//Menu4
MenuTypeCode[4]=0;
MenuValue[4]=0;
MenuParent[4]=0;
MenuChildFirst[4]=18;
MenuChildEnd[4]=24;

MenuNames[5]="МАРТ";//Param1_1
MenuTypeCode[5]=2;
MenuValue[5]=0;
MenuParent[5]=1;
MenuChildFirst[5]=0;
MenuChildEnd[5]=0;

MenuNames[6]="P";//Param1_2
MenuTypeCode[6]=1;
MenuValue[6]=50;
MenuParent[6]=1;
MenuChildFirst[6]=0;
MenuChildEnd[6]=0;

MenuNames[7]="I";//Param1_3
MenuTypeCode[7]=1;
MenuValue[7]=1;
MenuParent[7]=1;
MenuChildFirst[7]=0;
MenuChildEnd[7]=0;

MenuNames[8]="D";//Param1_4
MenuTypeCode[8]=1;
MenuValue[8]=15;
MenuParent[8]=1;
MenuChildFirst[8]=0;
MenuChildEnd[8]=0;

MenuNames[9]="WindowSize";//Param1_5
MenuTypeCode[9]=6;
MenuValue[9]=3000;
MenuParent[9]=1;
MenuChildFirst[9]=0;
MenuChildEnd[9]=0;

MenuNames[10]="Delta";//Param1_6
MenuTypeCode[10]=9;
MenuValue[10]=1;
MenuParent[10]=1;
MenuChildFirst[10]=0;
MenuChildEnd[10]=0;

MenuNames[11]="Кипение";//Param1_7
MenuTypeCode[11]=8;
MenuValue[11]=98;
MenuParent[11]=1;
MenuChildFirst[11]=0;
MenuChildEnd[11]=0;

MenuNames[12]="Цикл";//Param2_1
MenuTypeCode[12]=7;
MenuValue[12]=10;
MenuParent[12]=2;
MenuChildFirst[12]=0;
MenuChildEnd[12]=0;

MenuNames[13]="Отдых";//Param2_2
MenuTypeCode[13]=7;
MenuValue[13]=2;
MenuParent[13]=2;
MenuChildFirst[13]=0;
MenuChildEnd[13]=0;

MenuNames[14]="Остановка";//Param2_3
MenuTypeCode[14]=8;
MenuValue[14]=88;
MenuParent[14]=2;
MenuChildFirst[14]=0;
MenuChildEnd[14]=0;

MenuNames[15]="Язык";//Param3_1
MenuTypeCode[15]=3;
MenuValue[15]=0;
MenuParent[15]=3;
MenuChildFirst[15]=0;
MenuChildEnd[15]=0;

MenuNames[16]="Шкала";//Param3_2
MenuTypeCode[16]=4;
MenuValue[16]=0;
MenuParent[16]=3;
MenuChildFirst[16]=0;
MenuChildEnd[16]=0;

MenuNames[17]="Звук";//Param3_3
MenuTypeCode[17]=5;
MenuValue[17]=0;
MenuParent[17]=3;
MenuChildFirst[17]=0;
MenuChildEnd[17]=0;

MenuNames[18]="Модель";//Param4_1
MenuTypeCode[18]=0;
MenuValue[18]=0;
MenuParent[18]=4;
MenuChildFirst[18]=0;
MenuChildEnd[18]=0;

MenuNames[19]="Версия";//Param4_2
MenuTypeCode[19]=0;
MenuValue[19]=0;
MenuParent[19]=4;
MenuChildFirst[19]=0;
MenuChildEnd[19]=0;

MenuNames[20]="Прошивка";//Param4_3
MenuTypeCode[20]=0;
MenuValue[20]=0;
MenuParent[20]=4;
MenuChildFirst[20]=0;
MenuChildEnd[20]=0;

MenuNames[21]="IP AP";//Param4_4
MenuTypeCode[21]=0;
MenuValue[21]=0;
MenuParent[21]=4;
MenuChildFirst[21]=0;
MenuChildEnd[21]=0;

MenuNames[22]="MAC AP";//Param4_5
MenuTypeCode[22]=0;
MenuValue[22]=0;
MenuParent[22]=4;
MenuChildFirst[22]=0;
MenuChildEnd[22]=0;

MenuNames[23]="MAC STA";//Param4_6
MenuTypeCode[23]=0;
MenuValue[23]=0;
MenuParent[23]=4;
MenuChildFirst[23]=0;
MenuChildEnd[23]=0;

MenuNames[24]="Uptime";//Param4_7
MenuTypeCode[24]=10;
MenuValue[24]=0;
MenuParent[24]=4;
MenuChildFirst[24]=0;
MenuChildEnd[24]=0;


/*int i=0;
for (i=0;i<50;i++){
  MenuValue[i]=(EEPROM.read(i*2-1) << 8);
  MenuValue[i]=MenuValue[i]+(EEPROM.read(i*2-2));
  }*/
}


/*void setup()
{
  //настройка порта для клавиатуры
  pinMode(KeyButton1Pin, INPUT);
  //настройка дисплея
  u8g.setHardwareBackup(u8g_backup_avr_spi);
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);       
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);       
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
//формирование меню
MenuSetup();
}*/


void DrawMenu() {
  MenuNowPos=1;
  MenuDrawPos=0;
  //MenuDrawCount=150/tft.fontHeight();
  //tft.printAt("Рецепты", 160, 26, Center);
  //вывод названия родительского меню вверху экрана
  tft.printAt(MenuNames[MenuParent[MenuNowPos-MenuDrawPos]], 160, 26, Center);
  //Serial.println(MenuDrawCount());
  //переменная для вывода пунктов меню на экран
  int DrawI=0;
  //цикл для вывода пунктов меню на экран
  for(DrawI=0; DrawI<MenuDrawCount(); DrawI++) {
    if ((MenuChildFirst[MenuParent[MenuNowPos]]<=(MenuNowPos-MenuDrawPos+DrawI)) and (MenuChildEnd[MenuParent[MenuNowPos]]>=(MenuNowPos-MenuDrawPos+DrawI))) {
      tft.printAt(MenuNames[MenuNowPos-MenuDrawPos+DrawI],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*DrawI,Left);
        //tft.printAt(MenuNames[MenuNowPos-MenuDrawPos+DrawI],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*2+tft.fontHeight()*DrawI-tft.fontHeight()*MenuDrawPos,Left);
     
     
        /*u8g.setPrintPos(80, 21+10*DrawI);
        //Если целое число
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==1) 
          {
            u8g.print(MenuValue[MenuNowPos-MenuDrawPos+DrawI]);  
          }
        
        //Если тип временной интервал  
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==2) 
          {
            DrawHours=MenuValue[MenuNowPos-MenuDrawPos+DrawI] / 3600;
            DrawMinutes=(MenuValue[MenuNowPos-MenuDrawPos+DrawI] % 3600) / 60;
            DrawSeconds=(MenuValue[MenuNowPos-MenuDrawPos+DrawI] % 3600) % 60;
            u8g.print((String)DrawHours+":"+(String)DrawMinutes+":"+(String)DrawSeconds);
          }  
        //Если пункт меню бинарный
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==3) 
          { 
            if (MenuValue[MenuNowPos-MenuDrawPos+DrawI]==0) {u8g.print("Off");}
              else {u8g.print("On");}  
          }*/     
    }      
  }
  //если параметр сейчас не редактируется то отображение рамки вокруг выделенного пункта меню
  if (MenuEdit==0) {
    tft.setTextColor(ILI9341_BLACK,0xFE00);
    tft.printAt(MenuNames[MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*MenuDrawPos,Left);
    //tft.printAt(MenuNames[MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*2,Left);
    //u8g.drawLine( 3, 12+10*MenuDrawPos, 70, 12+10*MenuDrawPos);
    //u8g.drawLine(70, 12+10*MenuDrawPos, 70,22+10*MenuDrawPos);
    //u8g.drawLine( 3,22+10*MenuDrawPos, 70,22+10*MenuDrawPos);
    //u8g.drawLine( 3,  22+10*MenuDrawPos, 3,12+10*MenuDrawPos);
    tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  }  
    
  //если параметр сейчас редактируется то отображение рамки вокруг значения параметра
//  if (MenuEdit==1)
//    {
//      u8g.drawLine( 75, 12+10*MenuDrawPos, 122, 12+10*MenuDrawPos);
//      u8g.drawLine(122, 12+10*MenuDrawPos, 122,22+10*MenuDrawPos);
//      u8g.drawLine( 75,22+10*MenuDrawPos, 122,22+10*MenuDrawPos);
//      u8g.drawLine( 75,  22+10*MenuDrawPos, 75,12+10*MenuDrawPos);
//    }  

}

void draww(){
  tft.setTextColor(ILI9341_BLACK,0xFE00);
  tft.printAt(MenuNames[MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*MenuDrawPos,Left);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
}

/*void DrawMenu()
{
//временные переменные для отображения временных параметров  
int DrawHours=0;
int DrawMinutes=0;
int DrawSeconds=0;
  
u8g.setFont(u8g_font_fixed_v0);  
//вывод названия родительского меню вверху экрана
u8g.setPrintPos(5, 9);
u8g.print(MenuNames[MenuParent[MenuNowPos-MenuDrawPos]]);
u8g.drawLine( 0, 10, 123,10);
//переменная для вывода пунктов меню на экран
int DrawI=0;
//цикл для вывода пунктов меню на экран
for(DrawI=0; DrawI<MenuDrawCount;DrawI++)  
  {
    u8g.setPrintPos(5, 21+10*DrawI);
    if ((MenuChildFirst[MenuParent[MenuNowPos]]<=(MenuNowPos-MenuDrawPos+DrawI)) and 
      (MenuChildEnd[MenuParent[MenuNowPos]]>=(MenuNowPos-MenuDrawPos+DrawI)))
      { 
        u8g.print(MenuNames[MenuNowPos-MenuDrawPos+DrawI]);  
     
     
        u8g.setPrintPos(80, 21+10*DrawI);
        //Если целое число
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==1) 
          {
            u8g.print(MenuValue[MenuNowPos-MenuDrawPos+DrawI]);  
          }
        
        //Если тип временной интервал  
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==2) 
          {
            DrawHours=MenuValue[MenuNowPos-MenuDrawPos+DrawI] / 3600;
            DrawMinutes=(MenuValue[MenuNowPos-MenuDrawPos+DrawI] % 3600) / 60;
            DrawSeconds=(MenuValue[MenuNowPos-MenuDrawPos+DrawI] % 3600) % 60;
            u8g.print((String)DrawHours+":"+(String)DrawMinutes+":"+(String)DrawSeconds);
          }  
        //Если пункт меню бинарный
        if (MenuTypeCode[MenuNowPos-MenuDrawPos+DrawI]==3) 
          { 
            if (MenuValue[MenuNowPos-MenuDrawPos+DrawI]==0) {u8g.print("Off");}
              else {u8g.print("On");}  
          }     
      }
  }
  
  //если параметр сейчас не редактируется то отображение рамки вокруг выделенного пункта меню
  if (MenuEdit==0)
    {
    u8g.drawLine( 3, 12+10*MenuDrawPos, 70, 12+10*MenuDrawPos);
    u8g.drawLine(70, 12+10*MenuDrawPos, 70,22+10*MenuDrawPos);
    u8g.drawLine( 3,22+10*MenuDrawPos, 70,22+10*MenuDrawPos);
    u8g.drawLine( 3,  22+10*MenuDrawPos, 3,12+10*MenuDrawPos);
    }  
    
  //если параметр сейчас редактируется то отображение рамки вокруг значения параметра
  if (MenuEdit==1)
    {
      u8g.drawLine( 75, 12+10*MenuDrawPos, 122, 12+10*MenuDrawPos);
      u8g.drawLine(122, 12+10*MenuDrawPos, 122,22+10*MenuDrawPos);
      u8g.drawLine( 75,22+10*MenuDrawPos, 122,22+10*MenuDrawPos);
      u8g.drawLine( 75,  22+10*MenuDrawPos, 75,12+10*MenuDrawPos);
    }  
}*/


/*void Draw()
{
  u8g.firstPage();   
  do 
    { 
      //прорисовка статуса калибровки
      DrawMenu();
    } while( u8g.nextPage() );
}*/

/*
//при завершении редактирования пункта меню происходит обновление настроек
void UpdateSettings()
{
  //здесь происходит обновление настроек
  //допустим мы имеем программу которая мигает лампочкой с частотой speed
  //допустим этот параметр speed задается в элементе с индексом 4 тогда нам надо написать такой код:
  if (MenuNowPos==4) {
      Speed=MenuValue[MenuNowPos]
      }
  
  EEPROM.write(MenuNowPos*2-2, lowByte(MenuValue[MenuNowPos]));
  EEPROM.write(MenuNowPos*2-1,highByte(MenuValue[MenuNowPos]));
}*/

/*void Fillup(){            
            tft.begin();
            tft.setRotation(3);
            tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
}

void Filldown(){            
            tft.begin();
            tft.setRotation(3);
            tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
}*/
            
//Процедура для обработки нажатия кнопки "вверх"
void UpPress() {
  //если не находимся в режиме редактирования то кнопка используется для передвижения по меню
  if (MenuEdit==0) {
    //если текущая позиция в меню больше чем позиция первого элемента в этом меню то можно осуществлять передвижение.
    if (MenuChildFirst[MenuParent[MenuNowPos]]<MenuNowPos) {
      //осуществляем передвижение по меню на 1 пункт
      MenuNowPos=MenuNowPos-1;
      //при движении вверх проверяем где расположен выделенный пункт меню на дисплее
      //если выделенный пункт не упирается в край дисплея то также смещаем его на дисплее на 1 позицию
      if (MenuDrawPos>0) {
        MenuDrawPos=MenuDrawPos-1;
        tft.printAt(MenuNames[MenuNowPos+1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos+1),Left);  
      }
      /*else {
        tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
        //tft.setTextColor(ILI9341_BLACK,ILI9341_BLACK);
        //tft.printAt(MenuNames[MenuNowPos+1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos),Left);
        //draww();
//        for(int i=MenuDrawCount; i>=2;i--) {
//          tft.setTextColor(ILI9341_BLACK);
//          tft.printAt(MenuNames[MenuNowPos+i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos+(i-1)),Left);
//          tft.setTextColor(ILI9341_WHITE);
//          tft.printAt(MenuNames[MenuNowPos+(i-1)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos+(i-1)),Left);
//        }
      }*/
    }
    else {
      for(byte i=0; i<MenuChildEnd[MenuParent[MenuNowPos]]-MenuNowPos+1; i++)      
        tft.printAt(MenuNames[i+MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      //tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
      //tft.printAt(MenuNames[MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*0,Left);
      //MenuNowPos=MenuChildEnd[0];
      MenuNowPos=MenuChildEnd[MenuParent[MenuNowPos]];
      //MenuDrawPos=MenuChildEnd[0]-1;
      MenuDrawPos=MenuChildEnd[MenuParent[MenuNowPos]]-MenuChildFirst[MenuParent[MenuNowPos]];

      //for(byte i=0; i<MenuChildEnd[MenuParent[MenuNowPos]]-MenuNowPos+1; i++)      
      //  tft.printAt(MenuNames[i+MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      //for(byte i=0; i<MenuChildEnd[0]; i++)
      //  tft.printAt(MenuNames[i+1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);      
//      tft.fillRect(x_1-sz12b, y_2-sz12b, szb*3+12, tft.fontHeight(), ILI9341_BLACK);
//      tft.fillRect(x_1-sz12b, y_2-sz12b+tft.fontHeight()*(MenuDrawCount-1), szb*3+12, tft.fontHeight(), ILI9341_BLACK);
//      for(int i=MenuDrawCount; i>1;i--) {
//        tft.setTextColor(ILI9341_BLACK);
//        tft.printAt(MenuNames[i-1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*((i-2)),Left);
//        tft.setTextColor(ILI9341_WHITE);
//        tft.printAt(MenuNames[(MenuChildEnd[0]-MenuDrawCount)+(i-1)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*((i-2)),Left);
//        } 
//      draww();
    }
  }

  //Если находимся в режиме редактирования
  if (MenuEdit==1)
    {
      //проверяем какого типа меню и проверяем соответствующие ограничения, также контроллируем в зависимости от значения приращение
      //или уменьшение значения
      //Если тип целое число то максимального ограничения нет (добавить потом чтоб бралось максимальное значение из меню)
      if (MenuTypeCode[MenuNowPos]==1) 
        {
          MenuValue[MenuNowPos]=MenuValue[MenuNowPos]+1;
        }
      //Если тип временной интервал  
      if (MenuTypeCode[MenuNowPos]==2) 
        {
          MenuValue[MenuNowPos]=MenuValue[MenuNowPos]+1;
        }
      //Если пункт меню бинарный то инвертируем значение
      if (MenuTypeCode[MenuNowPos]==3) 
        {
          MenuValue[MenuNowPos]=(MenuValue[MenuNowPos]+1) % 2;  
        }
        tft.fillRect(x_1-sz12b, y_2-sz12b+tft.fontHeight()*(MenuDrawCount()-1), szb*3+12, tft.fontHeight(), ILI9341_BLACK);//szb*2+6
    }
}


//Процедура для обработки нажатия кнопки "вниз"
void DownPress() {
  //если не находимся в режиме редактирования
  if (MenuEdit==0)
  {
    //проверяем не является ли текущий пункт последним дочерним элементом
    if (MenuChildEnd[MenuParent[MenuNowPos]]>MenuNowPos) {
      //если не является то двигаемся на 1 пункт вниз
      MenuNowPos=MenuNowPos+1;
      //проверяем упираемся ли мы в край экрана. максимальное число элементов меню на экране задано переменной MenuDrawCount
      if ((MenuDrawPos<MenuDrawCount()-1) and (MenuDrawPos<MenuChildEnd[MenuParent[MenuNowPos]]-MenuChildFirst[MenuParent[MenuNowPos]])) {
        //если в край экрана не упираемся то также сдвигаем позицию на экране на 1 пункт вниз
        MenuDrawPos=MenuDrawPos+1;  
        tft.printAt(MenuNames[MenuNowPos-1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos-1),Left);  
      }
//      else {
//        //draww();
//        for(int i=MenuDrawCount; i>=2;i--) {
//          tft.setTextColor(ILI9341_BLACK);
//          tft.printAt(MenuNames[MenuNowPos-i],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos-(i-1)),Left);
//          tft.setTextColor(ILI9341_WHITE);
//          tft.printAt(MenuNames[MenuNowPos-(i-1)],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos-(i-1)),Left);
//        }
//      }
    }
    else {
      MenuNowPos=MenuChildFirst[MenuParent[MenuNowPos]];//1
      MenuDrawPos=0;
      //for(byte i=0; i<MenuChildEnd[0]; i++)
      //  tft.printAt(MenuNames[i+1],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);      
      for(byte i=0; i<MenuChildEnd[MenuParent[MenuNowPos]]-MenuNowPos+1; i++)
        tft.printAt(MenuNames[i+MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
    }
  }
  //если находимся в режиме редактирования 
  if (MenuEdit==1) {
    //tft.printAt(MenuValue[MenuNowPos-1],x_2+szbs/2, y_2-sz12b+tft.fontHeight()*(MenuDrawPos-1),Right);
   /* //проверяем какого типа меню и проверяем соответствующие ограничения, также контроллируем в зависимости от значения приращение
    //или уменьшение значения
    if (MenuTypeCode[MenuNowPos]==1) {
      if (MenuValue[MenuNowPos]>0) {
        MenuValue[MenuNowPos]=MenuValue[MenuNowPos]-1;
      }
    }
    //Если тип временной интервал  
    if (MenuTypeCode[MenuNowPos]==2) {
      if (MenuValue[MenuNowPos]>0) {
        MenuValue[MenuNowPos]=MenuValue[MenuNowPos]-1;
      }
    }
    //Если пункт меню бинарный то инвертируем значение
    if (MenuTypeCode[MenuNowPos]==3) {
      MenuValue[MenuNowPos]=(MenuValue[MenuNowPos]+1) % 2;  
    }*/
  }  
}  
  
/*  
//Процедура для обработки нажатия кнопки "влево"  
void LeftPress()
{
  //если не находимся в режиме редактирования
    if (MenuEdit==0)
      { 
        //если пункт меню содержит ненулевой индекс родителя (т.е. мы находимся внутри другого меню)
        if (MenuParent[MenuNowPos]>0)
          { 
            //то переходим на индекс родительского пункта меню
            MenuNowPos=MenuParent[MenuNowPos];
            //установка позиции на экарне, если количество пунктов меньше чем влезает на экране то выделенный пункт будет в самом низу но не в конце
            //иначе будет в самом конце
            if (MenuChildEnd[MenuParent[MenuNowPos]]-MenuChildFirst[MenuParent[MenuNowPos]]<MenuDrawCount())
              {
                MenuDrawPos=MenuNowPos-MenuChildFirst[MenuParent[MenuNowPos]];
              }
              else 
                {
                  MenuDrawPos=MenuDrawCount()-1;
                }
          }
      }
    //если находимся в режиме редактирования то выключаем режим редактирования  
    if (MenuEdit==1)
      {  
      MenuEdit=0;
      //запускаем процедуру для обновления настроек. это необходимо для того чтобы параметры меню которые мы поменяли начали действовать
      //в программе для которой мы используем это меню
      //UpdateSettings();
      }
}
*/  
  
//Процедура для обработки нажатия кнопки "вправо"   
void RightPress()
{
  //если код типа элемента отличается от нуля (т.е. выделенный элемент является параметром) то включаем режим редактирования
  if (MenuTypeCode[MenuNowPos]>0)
    {
    MenuEdit=1;  
    }
  //если код типа элемента равен нулю значит в данный момент выделен пункт меню и мы можем войти в него  
  if (MenuTypeCode[MenuNowPos]==0) {
    tft.setTextColor(ILI9341_BLACK);
    tft.printAt(MenuNames[0], 160, 26, Center);
    tft.setTextColor(ILI9341_WHITE);
    tft.printAt(MenuNames[MenuNowPos], 160, 26, Center);      
    //обнуляем позицию выделенного пункта на экране
    MenuDrawPos=0;  
    //переходим на первый дочерний элемент для текущего элемента
    //byte MenuNowPosChildEnd=MenuChildEnd[MenuNowPos];
    MenuNowPos=MenuChildFirst[MenuNowPos];
    ClearRect();
    tft.setFont(OPENSANS_12);
    //MenuDrawCount=152/tft.fontHeight();
    //Serial.println(MenuDrawCount());
    //for(byte i=0; i<MenuNowPosChildEnd-MenuNowPos+1; i++){
    for(byte i=0; i<MenuChildEnd[MenuParent[MenuNowPos]]-MenuNowPos+1; i++){      
      tft.printAt(MenuNames[i+MenuNowPos],x_1-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      if (MenuTypeCode[i+MenuNowPos]==1) {
        tft.printAt(String(MenuValue[i+MenuNowPos],DEC),x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
      }
      if (MenuTypeCode[i+MenuNowPos]==2) {
        if (MenuValue[i+MenuNowPos]==0)
          tft.printAt("PID",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
        else
          tft.printAt("Гист.",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
      }
      if (MenuTypeCode[i+MenuNowPos]==3) {
        if (MenuValue[i+MenuNowPos]==0)
          tft.printAt("RU",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
        else
          tft.printAt("EN",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
      }
      if (MenuTypeCode[i+MenuNowPos]==4) {
        if (MenuValue[i+MenuNowPos]==0)
          tft.printAt("°C",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
        else
          tft.printAt("°F",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
      }  
      if (MenuTypeCode[i+MenuNowPos]==5) {
        if (MenuValue[i+MenuNowPos]==0)
          tft.printAt("ON",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
        else
          tft.printAt("OFF",x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
      }                 
      if (MenuTypeCode[i+MenuNowPos]==6) {
        PrintValue(i);
        tft.printAt("мс",x_3-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      }
      if (MenuTypeCode[i+MenuNowPos]==7) {
        PrintValue(i);
        tft.printAt("мин",x_3-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      }                  
      if (MenuTypeCode[i+MenuNowPos]==8) {
        PrintValue(i);
        tft.printAt("°C",x_3-szbs/2, y_2-sz12b+tft.fontHeight()*i,Left);
      }
      if (MenuTypeCode[i+MenuNowPos]==10) {
        //tft.printAt(String(MenuValue[i+MenuNowPos],DEC),x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
        while(!myTouch.dataAvailable()){
          delay(1000);
          tft.fillRect(x_2, y_2-sz12b+tft.fontHeight()*i, szb, tft.fontHeight(), ILI9341_BLACK);
          tft.printAt(Uptime(),x_2, y_2-sz12b+tft.fontHeight()*i,Left);
        }
      }              
      //tft.printAt(String(MenuValue[i+MenuNowPos],DEC),x_3+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
    }
  }  
}

void PrintValue(byte i){
  tft.printAt(String(MenuValue[i+MenuNowPos],DEC),x_2+szbs/2, y_2-sz12b+tft.fontHeight()*i,Right);
}

  
/*
void ButtonPress()
{
  //130
  if ((KeyButton1Value>125) and (KeyButton1Value<135))
    {
      UpPress(); 
    }
  //258  
  if ((KeyButton1Value>255) and (KeyButton1Value<262))
    {
      DownPress();
    }
  //65  
  if ((KeyButton1Value>60) and (KeyButton1Value<70))
    {
    LeftPress();  
    }
  //195  
  if ((KeyButton1Value>190) and (KeyButton1Value<200))
    {
    RightPress();
    }
}


void KeyBoardCalculate()
{
  //Часть отработки нажатия клавиши
  KeyButton1Value=analogRead(KeyButton1Pin); 
  //если сигнал с кнопки нулевой то обнуляем метку обработки нажатия
  if ((KeyButton1Value<=50) or (KeyButton1Value>=1000))
    {
      //Сохраняем время последнего сигнала без нажатой кнопки
      KeyButton1TimePress=micros(); 
      KeyButton1WasChecked=0;
      KeyButton1RepeatTimePress=0;
    } 
        
  KeyButton1TimeFromPress=micros()-KeyButton1TimePress;
  //исключаем шумы
  if ((KeyButton1Value>50) and (KeyButton1Value<1000))
    {
      //отработка первого нажатия
      if ( ((KeyButton1TimeFromPress)>KeyButton1Latency) and (KeyButton1WasChecked==0))
        {
           ButtonPress();
           KeyButton1WasChecked=1;
           KeyButton1RepeatTimePress=0;
        }
      //отработка повторных нажатий  
      if ( ((KeyButton1TimeFromPress)>(KeyButton1RepeatLatency+KeyButton1RepeatTimePress)) and (KeyButton1WasChecked==1))
        {
           ButtonPress();
           //различная скорость обработки повторений, если держим кнопку меньше 5 секунд то повторная обработка кнопки раз в 0.4с, 
           //если кнопка удерживается дольше 5 секунд то время следующего повторного срабатывания не увеличивается и происходит максимально быстро
           if (KeyButton1TimeFromPress<(KeyButton1RepeatLatency+5000000)) {
             KeyButton1RepeatTimePress=KeyButton1RepeatTimePress+400000;
             }
        }
    }
}

void DrawTemplateRecipe_()
{
    //tft.setFont(OPENSANS_18);
    //tft.printAt(MenuNames[MenuParent[MenuNowPos-MenuDrawPos]], 160, 26, Center);
    //Serial.println(MenuNames[MenuParent[MenuNowPos-MenuDrawPos]]);
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
}*/
/*void loop()
{
  //проверка таймера для обработки графики, аналогично с обработкой клавиатуры 
  //обновление графики происходит не чаще заданного интервала DrawTimeInterval
  DrawTime2=micros();
  if ((DrawTime2-DrawTime1)>DrawTimeInterval) 
    {
      DrawTime1=DrawTime2;
      Draw();
    }
  //проверка таймера для обработки нажатий клавиатуры
  //обработка нажатий происходит не чаще заданного интервала KeyBoardTimeInterval
  KeyBoardTime2=micros();
  if ((KeyBoardTime2-KeyBoardTime1)>KeyBoardTimeInterval) 
    {
      KeyBoardTime1=KeyBoardTime2;
      KeyBoardCalculate();
    }
}*/
