#include "iarduino_Bluetooth_HC05.h"
#include "misp.h"
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

// Setting the LCD shields pins
LiquidCrystal_I2C lcd(63, 20, 4);

String menuItems[] = {"Play", "Find List", "Load List", "Reset joy"};
// Navigation button variables
int readKey;
#define Bluetooth_K_pin    10
#define Joystick_LeftX     A0
#define Joystick_LeftY     A1
#define Joystick_RightX    A2
#define Joystick_RightY    A3
#define Keypad A6
#define Pot A7
#define Button1 4
#define Button2 5
struct tPacket packet;
byte data[PROTOCOL_MAX_PACKET];

iarduino_Bluetooth_HC05 hc05(Bluetooth_K_pin);// Создаём объект hc05(Arduino - any  <-> Bluetooth - K)
int bt;
int tCount1;
bool refresh;   //lcd clear On/Off
//leerJoystick
int joyRead;
int joyPos;
int lastJoyPos;
long lastDebounceTime = 0;
long debounceDelay = 70;                 //user define
//Control Joystick
bool PQCP;
bool editMode;
//sistema de menus
int mNivel1;
int mNivel2;
//editmode
byte n[19];
int lastN;
int lcdX;
//int lcdY;
bool exiT;


String bt_name[5];
int listPage = 0;
int maxlistPages = round(((sizeof(bt_name) / sizeof(String)) / 2));
String bt_addr[5];
// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) );
int cursorPosition = 0;
int cursorPosition1 = 0;
// Creates 3 custom characters for the menu display
byte downArrow[8] = {
		     0b00100, //   *
		     0b00100, //   *
		     0b00100, //   *
		     0b00100, //   *
		     0b00100, //   *
		     0b10101, // * * *
		     0b01110, //  ***
		     0b00100  //   *
};

byte upArrow[8] = {
		   0b00100, //   *
		   0b01110, //  ***
		   0b10101, // * * *
		   0b00100, //   *
		   0b00100, //   *
		   0b00100, //   *
		   0b00100, //   *
		   0b00100  //   *
};

byte menuCursor[8] = {
		      B01000, //  *
		      B00100, //   *
		      B00010, //    *
		      B00001, //     *
		      B00010, //    *
		      B00100, //   *
		      B01000, //  *
		      B00000  //
};


void bt_ok()
{
  bt = hc05.begin(Serial);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("begin: ");
  if (bt)
    {
      lcd.setCursor(9, 0);
      lcd.print("Ok");
    }
  else
    {
      lcd.setCursor(9, 1);
      lcd.print("Error");
    }
}

void bt_find()
{
  bt = hc05.find(20);
  //  Выводим результаты поиска:
  if (bt)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("found ");
      lcd.setCursor(7, 0);
      lcd.print(bt);
      lcd.setCursor(9, 0);
      lcd.print(" devices:");
      for (int j = 0; j < i; j++)
	{
	  lcd.setCursor(1, j + 1);
	  lcd.print(j + 1);
	  lcd.setCursor(2, j + 1);
	  lcd.print(")");
	  lcd.setCursor(4, j + 1);
	  lcd.print(hc05.findName[j]);
	  bt_name[j] = hc05.findName[j];
	  bt_addr[j] = hc05.findAddr[j];
	}
    }
  else
    {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Device not found.");
    }
  delay(1000);
}
bool conn = false;

void  bt_connect(int num)
{
  int ind=0;
  if (bt_name[num] == "No devices")
    {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("No device selected");
      lcd.setCursor(1, 1);
      lcd.print("Press B to Exit");
      delay(2000);
      goto label1;
    }
  else
    {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Master creating"); delay(10);
      bt = hc05.createMaster(bt_name[num], "9876");
      lcd.setCursor(1, 1);
      lcd.print("create master: ");
      if (bt)
	{
	  conn = true;
	  lcd.setCursor(15, 1);
	  lcd.print("Ok");
	}
      else
	{
	label1:
	  lcd.setCursor(15, 1);
	  lcd.print("Error");
	}
    }
}

// Функция генерации пунктов меню.
void mainMenuDraw()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);

  lcd.print(menuItems[menuPage + 1]);
  lcd.setCursor(0, 3);
  lcd.print("ROBOclass joypad v0.1");

  if (cursorPosition == 00)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(2));
    }
  else
    if (cursorPosition > 0 and cursorPosition < (sizeof(menuItems) / sizeof(String)) - 1)
      {
	lcd.setCursor(15, 1);
	lcd.write(byte(2));
	lcd.setCursor(15, 0);
	lcd.write(byte(1));
      }
    else
      if (cursorPosition >= (sizeof(menuItems) / sizeof(String)) - 1)
	{
	  lcd.setCursor(15, 0);
	  lcd.write(byte(1));
	}
}

void list_cursor()
{
  for (int x = 1; x <= 2; x++)
    {
      lcd.setCursor(0, x);
      lcd.print(" ");
    }
  if (listPage % 2 == 0)
    {
      if (cursorPosition1 % 2 == 0)
	{  // Если страница меню и позиция курсора четные то курсор в 1 строке
	  lcd.setCursor(0, 0);
	  lcd.write(byte(0));
	}
      if (cursorPosition1 % 2 != 0)
	{  // Если страница меню четная а позиция курсора нет то курсор в 2 строке
	  lcd.setCursor(0, 1);
	  lcd.write(byte(0));
	}
    }
  if (listPage % 2 != 0)
    { // Если страница в меню нечетная а позиция курсора четная то курсор в 2 строке
      if (cursorPosition1 % 2 == 0)
	{  
	  lcd.setCursor(0, 1);
	  lcd.write(byte(0));
	}
      if (cursorPosition1 % 2 != 0)
	{  // Если страница меню и позиция курсора нечетные то курсор в 1 строке
	  lcd.setCursor(0, 0);
	  lcd.write(byte(0));
	}
    }
}

void list_conn()
{
  for (int j = 0; j < 5; j++)
    {
      if (bt_name[j] == NULL)
	{
	  bt_name[j] = "No devices";
	}
    }
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(bt_name[listPage]);
  lcd.setCursor(1, 1);
  lcd.print(bt_name[listPage + 1]);

  if (cursorPosition1 == 00)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(2));
    }
  else
    if (cursorPosition1 > 0 and cursorPosition1 < (sizeof(bt_name) / sizeof(String)) - 1)
      {
	lcd.setCursor(15, 1);
	lcd.write(byte(2));
	lcd.setCursor(15, 0);
	lcd.write(byte(1));
      }
    else
      if (cursorPosition1 >= (sizeof(bt_name) / sizeof(String)) - 1)
	{
	  lcd.setCursor(15, 0);
	  lcd.write(byte(1));
	}
}

// Перерисовка отображение текущей позиции курсора
void drawCursor()
{
  for (int x = 0; x < 2; x++)
    {     // Erases current cursor
      lcd.setCursor(0, x);
      lcd.print(" ");
    }
  if (menuPage % 2 == 0)
    {
      if (cursorPosition % 2 == 0)
	{
	  lcd.setCursor(0, 0);
	  lcd.write(byte(0));
	}
      if (cursorPosition % 2 != 0)
	{
	  lcd.setCursor(0, 1);
	  lcd.write(byte(0));
	}
    }
  if (menuPage % 2 != 0)
    {
      if (cursorPosition % 2 == 0)
	{  
		lcd.setCursor(0, 1);
	  	lcd.write(byte(0));
	}
      if (cursorPosition % 2 != 0)
	{  
	  lcd.setCursor(0, 0);
	  lcd.write(byte(0));
	}
    }
}

// Навигация
void up(bool q)
{
  switch (q)
    {
    case 0:
      if (menuPage == 0)
	{
	  cursorPosition = cursorPosition - 1;
	  cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) ) - 1);
	}
      if (menuPage % 2 == 0 and cursorPosition % 2 == 0)
	{
	  menuPage = menuPage - 1;
	  menuPage = constrain(menuPage, 0, maxMenuPages);
	}

      if (menuPage % 2 != 0 and cursorPosition % 2 != 0)
	{
	  menuPage = menuPage - 1;
	  menuPage = constrain(menuPage, 0, maxMenuPages);
	}
      cursorPosition = cursorPosition - 1;
      cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) ) - 1);
      break;

    case 1:
      if (listPage == 0)
	{
	  cursorPosition1 = cursorPosition1 - 1;
	  cursorPosition1 = constrain(cursorPosition1, 0, ((sizeof(menuItems) / sizeof(String)) ) - 1);
	}
      if (listPage % 2 == 0 and cursorPosition1 % 2 == 0)
	{
	  listPage = listPage - 1;
	  listPage = constrain(listPage, 0, maxlistPages);
	}

      if (listPage % 2 != 0 and cursorPosition1 % 2 != 0)
	{
	  listPage = listPage - 1;
	  listPage = constrain(listPage, 0, maxlistPages);
	}
      cursorPosition1 = cursorPosition1 - 1;
      cursorPosition1 = constrain(cursorPosition1, 0, ((sizeof(bt_name) / sizeof(String)) ) - 1);
      break;
    }
}

void down(bool q)
{
  switch (q)
    {
    case 0:
      if (menuPage % 2 == 0 and cursorPosition % 2 != 0)
	{
	  menuPage = menuPage + 1;
	  menuPage = constrain(menuPage, 0, maxMenuPages);
	}

      if (menuPage % 2 != 0 and cursorPosition % 2 == 0)
	{
	  menuPage = menuPage + 1;
	  menuPage = constrain(menuPage, 0, maxMenuPages);
	}

      cursorPosition = cursorPosition + 1;
      cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String))) - 1);
      break;
    case 1:
      if (listPage % 2 == 0 and cursorPosition1 % 2 != 0)
	{
	  listPage = listPage + 1;
	  listPage = constrain(listPage, 0, maxlistPages);
	}

      if (listPage % 2 != 0 and cursorPosition1 % 2 == 0)
	{
	  listPage = listPage + 1;
	  listPage = constrain(listPage, 0, maxlistPages);
	}

      cursorPosition1 = cursorPosition1 + 1;
      cursorPosition1 = constrain(cursorPosition1, 0, ((sizeof(bt_name) / sizeof(String))) - 1);
      break;
    }
}
void left() {}
void right() {}

void operateListMenu()
{
  int activeButton = 0;
  while (activeButton == 0)
    {
      int button;
      button = evaluateButton();
      switch (button)
	{
	case 0:
	  break;
	case 1:
	  activeButton = 1;
	  break;
	case 2:
	  button = 0;
	  up(1);
	  list_conn();
	  list_cursor();
	  // activeButton = 1;
	  break;
	case 3:
	  button = 0;
	  down(1);
	  list_conn();
	  list_cursor();
	  //activeButton = 1;
	  break;
	}
    }
}
void operateMainMenu()
{
  int activeButton = 0;
  while (activeButton == 0)
    {
      int button;
      button = evaluateButton();
      switch (button)
	{
	case 0:
	  // When button returns as 0 there is no action taken
	  break;
	case 1:
	  // This case will execute if the "forward" button is pressed
	  button = 0;
	  switch (cursorPosition)
	    {
	      // The case that is selected here is dependent on which menu page you are on and where the cursor is.
	    case 0:
	      menuItem1();
	      break;
	    case 1:
	      menuItem2();
	      break;
	    case 2:
	      menuItem3();
	      break;
	    case 3:
	      menuItem4();
	      break;
	    }
	  activeButton = 1;
	  mainMenuDraw();
	  drawCursor();
	  break;
	case 2:
	  button = 0;
	  up(0);
	  mainMenuDraw();
	  drawCursor();
	  activeButton = 1;
	  break;
	case 3:
	  button = 0;
	  down(0);
	  mainMenuDraw();
	  drawCursor();
	  activeButton = 1;
	  break;
	}
    }
}
int leeJoystick()
{
  int x = analogRead(Joystick_LeftX);
  int y = analogRead(Joystick_LeftY);
  if (x > 900)
    {
      joyRead = 1;        //x+
    }
  else
    if (x < 100)
      {
	joyRead = 2;          //x-
      }
    else
      if (y > 900)
	{
	  joyRead = 3;        //y+
	}
      else
	if (y < 100)
	  {
	     joyRead = 4;     //y-
	  }
	else
	  {
	    joyRead = 0;
	  }

  if (joyRead != lastJoyPos)
    {
      lastDebounceTime = millis();
    }
  if (((millis() - lastDebounceTime) > debounceDelay) && (joyRead != joyPos))
    {
      joyPos = joyRead;
      if (!PQCP)
	{
	  PQCP = 1;
	}
    }
  if (((millis() - lastDebounceTime) > (5 * debounceDelay)) && (joyPos == 3 || joyPos == 4))
    {
      joyPos = joyRead;                   //repeat time only for UP/DOWN
      if (!PQCP)
	{
	  PQCP = 1;
	}
    }
  lastJoyPos = joyRead;
}

int evaluateButton()
{
  leeJoystick();
  int result = 0;
  if (joyRead == 1)
    {
      result = 1; // Right
    }
  else
    if (joyRead == 3)
      {
	result = 2; // Up
      }
    else
      if (joyRead == 4)
	{
	  result = 3; // Down
	}
      else
	if (joyRead == 2)
	  {
	    result = 4; // Left
	  }
  return result;
}

void drawInstructions()
{
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.print(byte(1)); // Up arrow
  lcd.print("/");
  lcd.print(byte(2)); // Down arrow
  lcd.print(" buttons");
}

void menuItem1()
{ //меню подключения к устройству
 stop1:
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press A to start");
  lcd.setCursor(0, 1);
  lcd.print("Press B to return");

  while (activeButton == 0)
    {
      int button;

      button = evaluateButton();
      // Если нажата кнопка Назад
      if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
	{
	  button = 0;
	  activeButton = 1;

	}
      else
	if (analogRead(Keypad) >= 0 && analogRead(Keypad) < 400 )
	  {
	    // bt_find();
	    while (analogRead(Keypad) >= 0 && analogRead(Keypad) < 400)
	      {
		if (bt_name[0] == NULL )
		  {
		    lcd.clear();
		    lcd.setCursor(1, 0);
		    lcd.print("No devices");
		    delay(2000);
		    button = 0;
		    activeButton = 1;
		  }
		else
		  {
		    list_conn();
		    list_cursor();
		    operateListMenu();
		  }
	      }
	    bt_connect(cursorPosition1);
	    lcd.clear();
	    while (conn == true)
	      {
		lcd.setCursor(1, 0);
		lcd.print("Connected");
		lcd.setCursor(1, 1);
		lcd.print("Press B to ");
		lcd.setCursor(1, 2);
		lcd.print("disconnect");
		_misp_bluetooth(&packet, analogRead(Joystick_LeftY),analogRead(Joystick_RightY), digitalRead(Button1), analogRead(Pot));
		_protocol_p2d(&packet, data);
		Serial.write(data,packet._size+5);
		if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
		  {
		    Serial.write("S");
		    hc05.end();
		    conn = false;
		    lcd.clear();
		    lcd.setCursor(1, 0);
		    lcd.print("Disconnected");
		    lcd.setCursor(1, 2);
		    lcd.print("Press B to return");
		    lcd.setCursor(1, 1);
		    lcd.print("Press A to start");
		  }
	      }
	  }
    }
}

void menuItem2()
{ // Меню поиска устройств
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press A to find");
  lcd.setCursor(0, 1);
  lcd.print("Press B to return");

  while (activeButton == 0)
    {
      int button;
      button = evaluateButton();
      // Если нажата кнопка Назад
      if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
	{
	  button = 0;
	  activeButton = 1;
	}
      else
	if (analogRead(Keypad) >= 0 && analogRead(Keypad) < 400 )
	  {
	    bt_ok();
	    bt_find();
	    //bt_connect(0);
	    delay(1000);
	    activeButton = 1;
	  }
    }
}

void menuItem3()
{ // Меню загрузки списка устройств
 label:
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press A to load");
  lcd.setCursor(0, 1);
  lcd.print("Press B to return");

  while (activeButton == 0)
    {
      int button;

      button = evaluateButton();
      // Если нажата кнопка Назад
      if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
	{
	  button = 0;
	  activeButton = 1;
	}
      if (analogRead(Keypad) >= 0 && analogRead(Keypad) < 400 )
	{
	  lcd.clear();
	  lcd.setCursor(1,0);
	  lcd.print("Coming soon");
	  delay(3000);
	  button = 0;
	  activeButton = 1;
	}
      else
	if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
	  {
	    lcd.clear();
	    activeButton = 1;
	  }
    }
}

void menuItem4()
{ 
  int activeButton = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press A to reset joy");
  lcd.setCursor(0, 1);
  lcd.print("Press B to return");
  while (activeButton == 0)
    {
      int button;
      button = evaluateButton();
      // Если нажата кнопка Назад
      if (analogRead(Keypad) > 400 && analogRead(Keypad) < 600)
		{
	  		button = 0;
	  		activeButton = 1;
		}
    }
}

void controlJoystick()
{
  leeJoystick();
  if (PQCP)
    {
      PQCP = 0;
      if (joyPos == 5)
	{
	  editMode = !editMode;
	}
      switch (editMode)
	{
	case 1:
	  lcd.blink();
	  if (joyPos == 4 && n[lcdX] < 9)
	    {
	      n[lcdX]++;   //Up
	      refresh = 0;
	    }
	  if (joyPos == 3 && n[lcdX] > 0)
	    {
	      n[lcdX]--;   //Down
	      refresh = 0;
	    }
	  if (joyPos == 1 && lcdX < 19)
	    {
	      lcdX++;        //Right
	      refresh = 0;
	    }
	  if (joyPos == 2 && lcdX > 0) {
	    lcdX--;         //Left
	    refresh = 0;
	  }
	  break;
	case 0:
	  lcd.noBlink();
	  if (mNivel1 < 3 && joyPos == 3)
	    {
	      mNivel1++;    //Up
	      refresh = 1;
	      mNivel2 = 0;
	    }
	  if (mNivel1 > 0 && joyPos == 4)
	    {
	      mNivel1--;    //Down
	      mNivel2 = 0;
	      refresh = 1;
	    }
	  if (mNivel2 < 4 && joyPos == 1)
	    {
	      mNivel2++;   //Right
	      refresh = 1;
	    }
	  if (mNivel2 > 1 && joyPos == 2)
	    {
	      mNivel2--;    //Left
	      refresh = 1;
	    }
	}//!edit
    }//PQCP
}
void setup()
{
  // Initializes serial communication and Pin button
  Serial.begin(38400);
  pinMode(Joystick_LeftX,INPUT);
  pinMode(Joystick_LeftY,INPUT);
  pinMode(Joystick_RightX,INPUT);
  pinMode(Joystick_RightY,INPUT);
  pinMode(Keypad,INPUT);
  pinMode(Pot,INPUT);
  pinMode(Button1,INPUT);
  pinMode(Button2,INPUT);
  // Initializes and clears the LCD screen
  lcd.begin();
  lcd.clear();

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.setCursor(0, 3);
  lcd.print("ROBOclass joypad v0.1a");
}

void loop()
{
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}