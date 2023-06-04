#include <Adafruit_Fingerprint.h> //finger
#include <Keypad.h>               //keypad
#include <LiquidCrystal_PCF8574.h>//lcd
#include <Servo.h>                //servo motor

#define Password_Length 5 //password

Servo myservo; //servo motor

LiquidCrystal_PCF8574 lcd(0x27);  

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int pos = 0;

char Data[Password_Length];
char Master[Password_Length] = "1505";
byte data_count = 0, master_count = 0;

bool Pass_is_good;
bool door = false;
char customKey;

int kp = 0 ;
int fin = 0 ;

int melody[] = { 262, 233, 415 }; 
int noteDurations[] = { 8,4,2 }; 
int melodyFail[] = { 110, 55 }; 
int noteDurationsFail[] = { 12, 2 }; 

/*---preparing keypad---*/

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = 
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {9, 10, 12, 13};

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);

  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {Serial.println("Found fingerprint sensor!");} 
  else { Serial.println("Did not find fingerprint sensor :(");  while (1) { delay(1); } }


  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");

  //等待指紋
  Serial.println("Waiting for valid finger...");
  
  myservo.attach(11, 2000, 2400);
  ServoClose();
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.print("Waiting ...");
  delay(3000);
  lcd.clear();
}

void loop()
{
  if (door == true)
  {
    customKey = customKeypad.getKey();
    if (customKey == '#')
    {
      lcd.clear();
      ServoClose();
      lcd.print("Door is closed");
      delay(3000);
      door = false;
    }
  }
  else
  {
    keyPad();
    getFingerprintIDez();
	if (kp == 999 && fin == 999)
  	{
    		realOpenDoor();
  	}
  	if (kp == 111 && fin == 111)
  	{
    		realCloseDoor();
  	}
	if (kp == 999 && fin == 111)
  	{
    		realCloseDoor();
  	}
  	if (kp == 111 && fin == 999)
  	{
    		realCloseDoor();
  	}
  }

}

void keyPad()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  
  customKey = customKeypad.getKey();
  if (customKey)
  {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Length - 1)
  {
    if (!strcmp(Data, Master))
    {
      kp = 999 ;
    }
    else
    {
      kp = 111 ;
    }
  }
}

void getFingerprintIDez() 
{
  
  uint8_t p = finger.getImage(); 
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed !");
    delay(3000);
    lcd.clear();
    return -1;
  }
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put Your Finger");
  if(finger.confidence > 100)
  {
    fin = 999 ;
  }
  else
  {
    fin = 111 ;
  }
}

void clearData()
{
  while (data_count != 0)
  { 
    Data[data_count--] = 0;
  }
  return;
}

void ServoClose()
{
  for (pos = 90; pos >= 0; pos -= 10) 
  { 
    myservo.write(pos);
  }
}

void ServoOpen()
{
  for (pos = 0; pos <= 90; pos += 10) 
  {
    myservo.write(pos);  
  }
}

void realOpenDoor()
{
  char idname[][16]={"Nobody","user","user","user"};

  lcd.clear();
  ServoOpen();
  lcd.setCursor(0, 0);
  lcd.print(" Door is Open ");
  door = true;
  delay(5000);
  playMusic();
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print(idname[finger.fingerID]);
  delay(3000);
  lcd.clear();
  lcd.print(" Time is up! ");
  delay(1000);
  ServoClose();
  kp = 0 ;
  fin = 0;
  door = false;
  delay(1000);
  lcd.clear();
  clearData();

}

void realCloseDoor()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Failed ");
  lcd.setCursor(0, 1);
  lcd.print(" Try Again ");
  playMusicFail();
  delay(3000);
  kp = 0 ;
  fin = 0;
  door = false;
  delay(1000);
  lcd.clear();
  clearData();
}


void playMusic()
{
  for (int thisNote = 0; thisNote < 3; thisNote++) 
  {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(4, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(4);
  }
}

void playMusicFail()
{
  for (int thisNote = 0; thisNote < 2; thisNote++) 
  {
    int noteDuration = 1000 / noteDurationsFail[thisNote];
    tone(4, melodyFail[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(4);
  }
}

