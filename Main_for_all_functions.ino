#include <Servo.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <ezButton.h>
//define bluetooth module
int devicebluetooth = 52;
int setpoint1 = 40;
//*************************************
//initialising limit switch
ezButton limitSwitch(48);
#define mailLED  49
//****************************************
//initializing touch sensor
int led = 3;
int sen = 8;
int val = 0;
//*************************************
//define motion detection sensor
#define trigPin 12
#define echoPin 13
#define pindetection 6
int ledstate = 0;
int theft_acknowledge=0;
//******************************
//initializing smoke detector
//#define MQ2pin (0)
#define smokeoff_button 10
const int Aoutpin = 0;
float sensorValue;  //variable to store sensor value
const int buzzer = 7;
int fireLED = 53;
const int pinread = 5;
int fire_acknowledge = 0;
//******************************
//initializing servo for door
Servo door;
int pos = 0;
//*****************************
//initializing servo for actualdoor
Servo door2;
int pos2 = 0;
//******************************
//initializing IR_garage sensor
const int pinIRd = 11;
const int pinIRa = A1;
int IRvalueA = 0;
int IRvalueD = 0;
int open_flag = 0;
int unlocked = 0;
//******************************
//initialising temperature sensor
int device = 4;
int setpoint = 26;
#define dataPin 4
dht DHT;
//******************************
//initialising Keypad and LCD
int lock = 0;
const int rs = 27, en = 26, d4 = 25, d5 = 24, d6 = 23, d7 = 22;   //pins connected to LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {37, 36, 35, 34}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {33, 32, 31}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );   //initilize Keypad
char key ;     //variable to get the input from user

int order = 0 ;   //variable to change the screen on LCD (Display or set T or set H or set Pos)
bool flag = 0 ;
int i = 0 ;
bool check = 0 ;
int count = 0;

int Test_array[4] ;  //array to store each char from the user
int Test = 0 ;     // convert array to a number

int saved_password[4] = {1, 2, 3, 4};
int current_password[4] ;
int new_password[4] ;
int door_pin = 41 ;
//******************************
void setup() {
  // put your setup code here, to run once:
  //Limit switch
  Serial.begin(9600);
  limitSwitch.setDebounceTime(50);// set debounce time to 50 milliseconds
  pinMode(mailLED, OUTPUT);
  //*****************************************************
  //touch sensor setup
  Serial.begin(9600);
  pinMode(sen, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(sen, LOW);
  digitalWrite(led, LOW);
  //**********************************
  //servo setup
  door.attach(9);
  //*********************************
  //servo home door setup
  door2.attach(41);
  //**********************************
  //IR_garage setup
  Serial.begin(9600);
  pinMode(pinIRd, INPUT);
  pinMode(pinIRa, INPUT);
  //***********************************
  //temperature sensor setup
  Serial.begin(9600);
  pinMode(device, OUTPUT);
  //Serial.print("Give input of Setpoint");
  // delay(2000);
  //***********************************
  //Keypad and LCD setup
  lcd.begin(16 , 2);      // initiaize lcd
  //pinMode(door_pin , OUTPUT) ;
  //*********************************
  //smoke detector setup
  pinMode(buzzer , OUTPUT);
  pinMode(fireLED , OUTPUT);
  pinMode(smokeoff_button, INPUT);
  //bluetooth module
  pinMode(devicebluetooth, OUTPUT);
  //motion detection setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pindetection, OUTPUT);
}

void loop() {

  // put your main code here, to run repeatedly:
  home_mode();
 distancedetection();
  if (lock == 1) {
    touch_sensor();
    temperature_sensor();
    smokedetector();
    bluetoothmodule();
    servo_garage();
    limit_switch();
  }
}
//********************************************************************************************************************

void touch_sensor()
{
  while (digitalRead(sen) == HIGH)
  {
    while ((digitalRead(sen) == HIGH) && (val < 100))
    {
      analogWrite(led, val);
      val++;
      delay(15);
    }
    delay(1000);
    while ((digitalRead(sen) == HIGH) && (val >= 0))
    {
      analogWrite(led, val);
      val--;
      delay(15);
    }
    delay(1000);
  }
}
//*********************************************************************
void servo_home()
{
  for (pos2 = 0; pos2 <= 100; pos2 += 1) { // goes from 0 degrees to 100 degrees
    // in steps of 1 degree
    door2.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(10);
  }// waits 10 ms for the servo to reach the position
  delay(3000);
  for (pos2 = 100; pos2 >= 0; pos2 -= 1)
  {
    // goes from 100 degrees to 0 degrees
    door2.write(pos2);   // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 10 ms for the servo to reach the position
  }
}

//***********************************************************************
void servo_garage()
{
  
  //Serial.print("Analog Reading=");
  //Serial.print(IRvalueA);
  //Serial.print("\t Digital Reading=");
  //Serial.println(IRvalueD);

  if ((IRvalueD == HIGH) && (open_flag == 0))
  {
    for (pos = 0; pos <= 100; pos += 1) { // goes from 0 degrees to 100 degrees
      // in steps of 1 degree
      door.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);
    }// waits 10 ms for the servo to reach the position
    open_flag = 1;
  }
  if ((IRvalueD == LOW) && (open_flag == 1))
  {
    for (pos = 100; pos >= 0; pos -= 1)
    {
      // goes from 100 degrees to 0 degrees
      door.write(pos);   // tell servo to go to position in variable 'pos'
      delay(10);                       // waits 10 ms for the servo to reach the position
    }

    open_flag = 0;
  }
  delay(50);

  IRvalueA = analogRead(pinIRa);
  IRvalueD = digitalRead(pinIRd);

}
//****************************************************************************
void temperature_sensor()
{
  int readData = DHT.read11(dataPin);
  float t = DHT.temperature;
  float h = DHT.humidity;

  ////Serial.print("Temperature = ");
  ////Serial.print(t);
  ////Serial.print(" *C ");
  ////Serial.print("    Humidity = ");
  ////Serial.print(h);
  ////Serial.println(" % ");
  Serial.print(t);
   Serial.print(",");
   Serial.print(h);
   Serial.print(",");

}
//********************************************************************************************************************//

void smokedetector()
{
  sensorValue = analogRead(Aoutpin);
  ////Serial.print("Sensor Value: ");
  ////Serial.print(sensorValue);
  Serial.println(sensorValue);
  while ((sensorValue > 15000) && (!digitalRead(smokeoff_button)) && (!fire_acknowledge))
  {
    lcd.clear();
    ////Serial.print(" | Smoke detected!");
    digitalWrite(buzzer, !digitalRead(buzzer));
    digitalWrite(fireLED, !digitalRead(fireLED));
    lcd.setCursor(0, 0);
    lcd.print("FIRE!!!!!!");
    delay(500);
  }

  if (digitalRead(smokeoff_button))
  {
    fire_acknowledge = 1;
  }
  if (fire_acknowledge)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(fireLED, LOW);
  }
  if (sensorValue < 900)
    fire_acknowledge = 0;
  ////Serial.println("");
}
//********************************************************************************************************************//

void distancedetection()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  //Serial.print(distance);
  if (distance >= 5 || distance <= 0)
  {
    ledstate = 0;
    ////Serial.println("Out of range");
    //digitalWrite(pindetection, LOW);
  }
  else {
    if (ledstate == 0) {
      ledstate = 1;

      digitalWrite(pindetection, HIGH);
      //Serial.print(distance);
      //Serial.println(" cm");
    }
    else
    { ledstate = 0;
      digitalWrite(pindetection, LOW);
      //Serial.print(distance);
      //Serial.println(" cm");
    }
  }
     while(!lock &&(distance < 5)&&(!digitalRead(smokeoff_button))&&!(theft_acknowledge))
     {
      lcd.clear();
    //Serial.print(" BURGLAR ALARM!");
    digitalWrite(buzzer, !digitalRead(buzzer));
    digitalWrite(fireLED, !digitalRead(fireLED));
    lcd.setCursor(0, 0);
    lcd.print("BURGLAR ALARM!");
    digitalWrite(pindetection,LOW); //turnoff led for enterance derection (works for unlocked mode only)
    delay(500);
      
      }
   if (digitalRead(smokeoff_button))   //same button for fire alarm :)
  {
      theft_acknowledge = 1;
  }
  if (theft_acknowledge)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(fireLED, LOW);
  }
  if (distance >= 5)
    theft_acknowledge = 0;
  ////Serial.println(smokeoff_button);
  
}

//*************************************************
void bluetoothmodule() {
  // To check if data is available on Serial port
  if (Serial.available() > 0) {
    // To read a data in integer form from HC05 via serial communication protocol
    setpoint = Serial.parseInt();
    //Serial.print("Set Point Changed: ");
    //Serial.println(setpoint1);
  }
  int temperature = analogRead(A1);
  temperature = map(temperature, 0, 1023, 0, 500);
  //Serial.print(temperature);
  //Serial.print(",");
  //Serial.print("Temperature = ");
  //Serial.println(temperature);

  if (temperature >= setpoint1) {
    digitalWrite (device, HIGH);
    //Serial.println("Device is on");
  }
  else if (temperature < setpoint1) {
    digitalWrite (device, LOW);
    //Serial.println("Device is off");
  }
  delay(1000);
}
//*************************************************************************************
void Get_Order () {
  if ( key == '*' && flag == 0 ) {      //each time user press * the order number increases to change screen
    order = order + 1 ;                                        //  Display or set T or set H or set Pos)
    flag = 1 ;
    lcd.clear();
    i = 0;
    check = 0 ;
  }
  else if ( key != '*' ) {
    flag = 0 ;
  }

  if ( order == 2 ) {
    order = 0 ;
  }
}

//********************************************************************************************************************//


void Enter_Password() {

  if (i == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Enter Password : " );
    lcd.setCursor(0, 1);
    lcd.print("- - - -" );
  }
  if (key == '0' || key == '1' || key == '2' || key == '3'
      || key == '4' || key == '5' || key == '6'
      || key == '7' || key == '8' || key == '9')
  {
    lcd.setCursor(2 * i, 1);
    lcd.print(key);
    delay(50);
    lcd.setCursor(2 * i, 1);
    lcd.print("*");
    current_password[i] = (int)key - 48 ;
    i = i + 1 ;
  }

  if (i == 4) {
    if ((current_password[0] == saved_password[0]) && (current_password[1] == saved_password[1]) &&
        (current_password[2] == saved_password[2]) && (current_password[3] == saved_password[3]) ) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome");
      lock = 1;
      servo_home();
      lcd.clear();
      i = 0 ;
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect Password");
      delay(1000);
      lcd.clear();
      i = 0 ;

    }
  }
}
//****************************************************
void Change_Password() {

  if (check == 0) {
    if (i == 0) {
      lcd.setCursor(0, 0);
      //lcd.print("*Change Password*" );
      //delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("Enter OLD Password :" );
      lcd.setCursor(0, 1);
      lcd.print("- - - -" );
    }

    if (key == '0' || key == '1' || key == '2' || key == '3'
        || key == '4' || key == '5' || key == '6'
        || key == '7' || key == '8' || key == '9') {
      lcd.setCursor(2 * i, 1);
      lcd.print(key);
      delay(300);
      lcd.setCursor(2 * i, 1);
      lcd.print("*");
      current_password[i] = (int)key - 48 ;
      i = i + 1 ;

    }

    if (i == 4) {
      if ((current_password[0] == saved_password[0]) && (current_password[1] == saved_password[1]) &&
          (current_password[2] == saved_password[2]) && (current_password[3] == saved_password[3]) ) {

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter New Password");
        lcd.setCursor(0, 1);
        lcd.print("- - - -" );
        check = 1;
        i = 0;
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Incorrect Password");
        delay(1000);
        lcd.clear();
        i = 0 ;
      }
    }
  }


  else if (check == 1) {

    if (key == '0' || key == '1' || key == '2' || key == '3'
        || key == '4' || key == '5' || key == '6'
        || key == '7' || key == '8' || key == '9') {
      lcd.setCursor(2 * i, 3);
      lcd.print(key);
      delay(300);
      lcd.setCursor(2 * i, 3);
      lcd.print("*");
      new_password[i] = (int)key - 48 ;
      i = i + 1 ;
    }

    if (i == 4) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password Changed");
      saved_password[0] = new_password[0];
      saved_password[1] = new_password[1];
      saved_password[2] = new_password[2];
      saved_password[3] = new_password[3];
      delay(1000);
      lcd.clear();
      i = 0 ;
      check = 0 ;
      order = 0 ;
    }
  }
}

//********************************************************************************************************************//
void home_mode()
{
  key = keypad.getKey();  // function to get a key from the keypad

  Get_Order () ;      //function to get an order from the user (Display or set T or set H or set Pos)

  if ( order == 0 ) {             // if order=0   display date on LCD
    Enter_Password();     //function to display data
  }

  else if ( order == 1 ) {
    Change_Password();
  }

}
//***********************************************************************************************************************
void limit_switch()
{
  int flag = 0;
  int count = 4;
  limitSwitch.loop(); // MUST call the loop() function first

  if (limitSwitch.isPressed())
  {
    //Serial.println("The limit switch: UNTOUCHED -> TOUCHED");
    flag = 1;
  }

  if (limitSwitch.isReleased())
  {
    //Serial.println("The limit switch: TOUCHED -> UNTOUCHED");
  }

  /*int state = limitSwitch.getState();
    if(state == HIGH)
    //Serial.println("The limit switch: UNTOUCHED");
    else
    //Serial.println("The limit switch: TOUCHED");
  */
  while ((flag == 1) && count >= 0)
  {
    digitalWrite(mailLED, !digitalRead(mailLED));
    delay(500);
    count--;
  }
  flag = 0;
  digitalWrite(mailLED, LOW);
}
//***********************************************************************************************************************
