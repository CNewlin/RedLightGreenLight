#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>
// declare the servos used for this project
Servo DetectServo; // servo that holds the distance sensor to detect if person has moved durring RED LIGHT 
Servo KillServo; // servo to swing bar to knock the player out
//set up counters
volatile int GameTime = 0;
volatile int Off_On_Count = 0;
volatile int NewGameToken = 1;
volatile int ArmToken = 0; // to tell if the arm has been spun mid game 0 means no and 1 means yes
volatile int GameWon = 0;
//put pin declarations here:
volatile int StartToken = 0;
int pinOff = 11;
int pinOn = 12;
const int trigPin = 30;
const int echoPin = 32;
long duration;
int distance;
int LastDistance;
int buzzer = 10;
int ResetGame = 2;
int screenclear = 0;
int gameoverclear = 0;
int newgameclear = 0;
// set up for button interupt
/*
Now the fuction NewGame sould function is that when a button interupt is pressed if the token is zero it will rest the field with the armed servo ready to swing
when the game detects movement and the head is turned around all ready for green light. After the field is set the LCD should say press again to start the game. When
it is pressed again the token is set to zero which then resets the clock and so the game starts again. 
*/
void NewGame(){
  if(NewGameToken == 0){
    Serial.println("Button Detected");
    NewGameToken = 1;
    GameTime = 0;
    
    DetectServo.write(90); //head facing away from playing board

    // +++++++++++++++++give enough time to reset DetectServo fully then move on+++++++++++++++++++++++++++++
    //+++++++++++++++++++END OF WHILE LOOP++++++++++++++++++++++++++++++++++++++++++

    KillServo.write(180);// set the arm back so it is ready to swipe the player agian 

    // +++++++++++++++++give enough time to reset KillServo fully then move on+++++++++++++++++++++++++++++
    //+++++++++++++++++++END OF WHILE LOOP++++++++++++++++++++++++++++++++++++++++++

    // DISPLAY ONCE THE SET UP IS DONE
    //lcd.print("PRESS AGAIN TO START");
  }
  else if(NewGameToken == 1){
    NewGameToken = 0;
	GameWon = 0;
    GameTime = 60; // start new game
  }
}
//LCD SCREEN SET UP
const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup(){
 //set pins as outputs
 pinMode(pinOff, OUTPUT);
 pinMode(pinOn, OUTPUT);
 pinMode(ResetGame, INPUT);
 pinMode(buzzer,OUTPUT);// set digital IO pin pattern, OUTPUT to be output 
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
 pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 DetectServo.attach(9); //set DetectServo to 44 since it has a PWM
 KillServo.attach(28); //set KillServo to 45 since it has a PWM
DetectServo.write(0);
KillServo.write(180);
//LCD SCREEN SET UP
lcd.begin(16, 2);
//set up button interruppt
 attachInterrupt(digitalPinToInterrupt(ResetGame), NewGame, RISING);
cli();//stop interrupts

//set timer1 interrupt at 1Hz
 TCCR1A = 0;// set entire TCCR1A register to 0
 TCCR1B = 0;// same for TCCR1B
 TCNT1  = 0;//initialize counter value to 0
  
  
 // set compare match register for 1hz increments
 OCR1A = (int)15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
 // turn on CTC (Clear Time and Compare Match) mode
 TCCR1B |= (1 << WGM12);
 // Set CS12 and CS10 bits for 1024 prescaler
 TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);  
 // enable timer compare interrupt
 TIMSK1 |= (1 << OCIE1A);
Serial.begin(9600);// connect to serial port, set baud rate at “9600”

sei();//allow interrupts

}//end setup

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if((GameTime >=0) && (ArmToken == 0)){
    newgameclear = 0;
    if (Off_On_Count < 2){ //green light
      digitalWrite(pinOn, HIGH); // green on
      digitalWrite(pinOff, LOW); // red off
      tone(buzzer,600);
      DetectServo.write(0);
    }
    else if( Off_On_Count < 5){ //red light
      digitalWrite(pinOn, LOW); //green off
      digitalWrite(pinOff, HIGH); //red on
      tone(buzzer,1000);
      DetectServo.write(90);
    }
    Off_On_Count++;
    if (Off_On_Count == 5){
      Off_On_Count = 0;
    }
  }
  else{ // game time is up so stop playing
    tone(buzzer,1500);
    digitalWrite(pinOff, LOW);
    digitalWrite(pinOn, LOW);
  }
  if(GameTime >= 0){
    GameTime--;
  }
}

void loop() {
  //lcd.print(GameTime);

	lastcheck = check
	check = digitalRead(pinOff); 


  if( (GameTime >= 0) && (ArmToken == 0)){

    //--------------------------IF statement for Red light to detect movement------------------------------

    if(digitalRead(pinOff) == HIGH){
		if(check != lastcheck){
			lastDistance = 0;
		}
      // Clears the trigPin
      delay(500);
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      LastDistance = distance;
      distance = duration * 0.034 / 2;
      //****************************************** IF STATEMENT TO SEE IF PLAYER MOVED DURING RED LIGHT********************************
      //if(DetectServo.read() == 0){
        
        if((abs(distance - LastDistance) > 1)&&(abs(distance -lastDistance) < 4)){// player moves 2 units knock player out
            ArmToken = 1;// change token so code cane move to end game section
            KillServo.write(90); // swing arm
            Serial.print("Arm Swung!");
        }
      //}
      // Prints the distance on the Serial Monitor {TAKE OUT IN FINAL CODE}
	  if(Distance <= 3){
		  GameWon = 1;
		  Serial.print("Game Wom!");
		}
      Serial.print("Last Distance: ");
      Serial.print(LastDistance);
      Serial.print("  Distance: ");
      Serial.println(distance);

    }
    LastDistance = 0; // reset lastDistance to get rid of false positives

    //------------------------------------------------------------------------------------------------------
    // COUNTER TO SHOW HOW MUCH TIME IS LEFT OF THE GAME
    //lcd.clear();
    lcd.setCursor(0, 0);
    if(GameTime < 10)
    {
      if(screenclear == 0)
      {
        lcd.clear();
        screenclear = 1;
      }
      
    }
    lcd.print(String(GameTime));
  }

  //+++++++++++++++++++++++++++++++END OF GAME++++++++++++++++++++++++++++++++++++++++++++
  
  else if ((GameTime <= 0) || (ArmToken == 1) || (GameWon == 1)){
    //lcd.clear();
    GameTime =0;
    GameTime = -1;
	if(GameWon == 1){
		lcd.print("Game Won");
	}
    if(gameoverclear == 0)
    {
      lcd.clear();
      gameoverclear = 1;
    }
    //----------------------------SET UP FOR NEW GAME-------------------------------------
    if(NewGameToken == 1){
        gameoverclear = 0;
		//GameWon = 0;
        if(newgameclear == 0)
        {
          lcd.clear();
          newgameclear = 1;
        }

      
    }
    //--------------------------------------------------------------------------------------
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}

