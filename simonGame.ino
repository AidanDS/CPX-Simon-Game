#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

AsyncDelay delay_2s;

float midi[127]; // array and value for using MIDI notes
int A_four = 440;

int winMel[5] = {71,72,79,83,84}; // melodies for winning and losing a level
int losMel[5] = {71,74,79,83,84};


const byte leftPin = 4; // buttons and switch pins
const byte rightPin = 5;
const byte switchPin = 7;

bool leftFlag = 0; // flags for interrupts
bool rightFlag = 0;
bool switchFlag = 0;

bool timeUp = 0; // bool that is used to skip parts of functions if a timer has run out

bool switchState; // bool that measures the value of the slide switch

int level = 0; // value that increases after each success and determines the limit for the patterns
int score = 0; // value that is displayed when the player loses; it increases after each success
int check = 1; // value used in checking if player is correct;
int gamePattern[1000] = {}; // array for the randomly generated pattern 
int playerPattern[1000] = {}; // array that tracks the player's entered pattern


void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  delay(50);
  randomSeed(CircuitPlayground.lightSensor());  // seeding the random function with an inconsistant value to get different patterns
  attachInterrupt(digitalPinToInterrupt(leftPin), leftPress, RISING); // setup interrupt functions
  attachInterrupt(digitalPinToInterrupt(rightPin), rightPress, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchFlick, CHANGE);
  switchState = CircuitPlayground.slideSwitch(); // read the switch position before start
  generateMIDI();   // run a function so that the MIDI notes work
  delay_2s.start(2000, AsyncDelay::MILLIS); // this timer is used to track whether the player takes too long
  while(!switchFlag){   // score and other elements require the use of the serial monitor. Must be open before start
    delay(10);
  }
}

void leftPress() {            // interrupt functions
  leftFlag = true;
}

void rightPress() {
  rightFlag = true;
}

void switchFlick() {
  switchFlag = true;
  switchState = CircuitPlayground.slideSwitch();
}

void startLevel() {               // This plays the level to remember
  Serial.println("---NEXT STAGE---");
  timeUp = 0;
  gamePattern[level] = random(2);
  delay(300);
  for(int i = 0; i <= level; i++) { // loops as many times as the level we are on
    if(switchFlag){                // using the swich will cancel the pattern
      break;
    }
    Serial.println(i+1);
    if (gamePattern[i] == 1){       // if value is 1, light up the right side of the board and play G4 key
      for (int r = 5; r < 10; r++) {
        CircuitPlayground.setPixelColor(r, 128,0,128);
      }
      CircuitPlayground.playTone(midi[67],300,false);
      CircuitPlayground.clearPixels();
    } else {                        // if value isn't 1 (must be 0 then), light up left side and play F4 key
      for (int l = 0; l <5; l++){
        CircuitPlayground.setPixelColor(l, 0,0,255);
      }
      CircuitPlayground.playTone(midi[65],300,false);
      CircuitPlayground.clearPixels();
    }
    delay(100);
  }
  
}

void recPlayer() {    // this function records what pattern the player enters
  delay_2s.restart();
  rightFlag = 0;    // set both button flags to 0 so that pressing them early doesn't mess with the game
  leftFlag = 0;
  for(int i = 0; i <= level; i++) {   //  records until enough values are entered
    if(switchFlag){
      break;
    }
    if (!switchFlag){     // skips if switch is pressed
    delay_2s.restart();
    while(!rightFlag&&!leftFlag) {    // wait for a button to be pressed, if 2 seconds pass, fail level
      delay(10);
      if(delay_2s.isExpired()) {
        levelFail();
        timeUp = 1;
        break;
      }
    }
    if (!timeUp&&!switchFlag){ // skips if switch was pressed or if time was up (it gets confused otherwise)
    if (rightFlag){
      for (int r = 5; r < 10; r++) {  // if right button is pressed, light up the right side of the board and play G4 key
        CircuitPlayground.setPixelColor(r, 128,0,128);
      }
      while(CircuitPlayground.rightButton()) {    // while loop makes the lights and sound stay on while button is held, makes buttons better
        CircuitPlayground.playTone(midi[67],10,false);
      }
      CircuitPlayground.clearPixels();
      playerPattern[i] = 1;     //records to array
      rightFlag=0;
      leftFlag=0;       // turn both flags off to avoid problems from pressing both.
      Serial.println("right");
    } else if (leftFlag) {
      for (int l = 0; l <5; l++){   // if left button is pressed, light up left side and play F4 key
        CircuitPlayground.setPixelColor(l, 0,0,255);
      }
      while(CircuitPlayground.leftButton()) {
        CircuitPlayground.playTone(midi[65],10,false);
      }
      CircuitPlayground.clearPixels();
      playerPattern[i] = 0;     //record to array
      leftFlag=0;
      rightFlag=0;
      Serial.println("left");
    }

    if (gamePattern[i] != playerPattern[i]){      // after entry is recorded, check to see if it matches the generated pattern
      break;
    } else {
      check = i;      // this value records each time an entry is correct
    }
    }
    }
  }

  if (!timeUp){       // skip checking if time ran out
  delay(200);
  if (check==level) {   // if player got all of them right, win; if not, lose
    levelWin();
    score = ((level+1) * (level +1) * (10));
    level++;
  } else {
    levelFail();
  }
  }
}


void levelWin() {     // this funcion plays each time a level is completed
  Serial.println("CORRECT!");
  int o = 1;
  for (int i = 5; i < 10; i++) {  // light up lights green from bottom to top
    CircuitPlayground.setPixelColor(i, 0, 255,0);
    CircuitPlayground.setPixelColor(i-o, 0, 255,0);

    CircuitPlayground.playTone(midi[winMel[i-5]], 100, false);
    o++;
    o++;
  }
  delay(200);
  CircuitPlayground.clearPixels();
}


void levelFail() {    // function runs when a mistake is made or time runs out
  Serial.println("WRONG!");
  level = 0;    // sets level and other associated variables back to starting values to begin again
  check = 1;
  Serial.print("Final Score:  ");   // prints score to serial monitor
  Serial.println(score);
  score=0;
  int o = 9;
  for (int i = 9; i > 4; i--) {     // lights up red from top to bottom
    CircuitPlayground.setPixelColor(i, 255, 0,0);
    CircuitPlayground.setPixelColor(i-o, 255, 0,0);
    CircuitPlayground.playTone(midi[losMel[i-5]], 100, false);
    o--;
    o--;
  }
  delay(500);
  CircuitPlayground.clearPixels();
}


void loop() {

  while(switchState){     // game remains stopped if switch is to the left
    delay(10);
  }
  switchFlag = 0; // turns off switch flag after skipping functions

  startLevel();   // plays the pattern to remember

  recPlayer();    // records and checks the player
  
}


void generateMIDI()   // generates the array of MIDI notes
{
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
