#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D4  294
#define NOTE_D5  587
#define NOTE_DS4 311
#define NOTE_DS5 622
#define NOTE_E4  330
#define NOTE_E5  659
#define NOTE_FS4 370
#define NOTE_G4  392

// Nokia 5110 Display
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 10, 4, 3, 2);

// LEDs
const int green_LED = 6;
const int red_LED = 5;

// Buzzer
const byte buzzer = 7;

// Joystick Button
const int jButton = 0;

// Joystick Button Click
bool jButton_Previous = false;
bool jButton_Current = false;
bool jButton_Pressed = false;

// Reset Button
const byte rButton = 1;

// Reset Button Click
bool rButton_Previous = false;
bool rButton_Current = false;
bool rButton_Pressed = false;

// Joystick Analog
const int yAxis = A1;

// Joystick Analog Read
bool UP_Previous = false;
bool UP_Current = false;
bool UP_Pressed = false;

bool DOWN_Previous = false;
bool DOWN_Current = false;
bool DOWN_Pressed = false;

// Internal Player Score
byte score=0;

// Bird Position
int position_Bird = 15;

// Random Walls
unsigned char seed = 7;
unsigned char firstHole = 0;
unsigned char firstWall = 0;
unsigned char secondHole = 0;
unsigned char secondWall = 0;

unsigned char i = 0; 
byte Difficulty_Screen_Index = 3;
byte Lose_Play_Again_Screen_Index = 7;
byte Win_Play_Again_Screen_Index = 12;
bool Hit_A_Wall = false;
bool Pass_A_Wall = false;
bool Game_Lose = false;
bool Game_Win = false;

// Songs/Counters
//Game
const unsigned char game_Song_Pattern_Size = 47;
const int game_Melody[game_Song_Pattern_Size] = {NOTE_DS4, NOTE_E4, NOTE_FS4, 
NOTE_G4, NOTE_DS4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_B4, 
NOTE_AS4, NOTE_A4, NOTE_G4,  NOTE_E4, NOTE_G4, NOTE_B4, NOTE_E5,NOTE_DS5, NOTE_D5, 
NOTE_B4,  NOTE_G4, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4, 
NOTE_E5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_AS4, NOTE_A4, NOTE_AS4,
NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C5, NOTE_DS4,
};
unsigned char song_Game_Counter = game_Song_Pattern_Size;

// Try Again
const unsigned char tryAgain_Song_Pattern_Size = 9;
const int tryAgain_Melody[tryAgain_Song_Pattern_Size] = {200, 300, 400, 500, 600, 500, 400, 300, 200};

unsigned char song_tryAgain_Counter = tryAgain_Song_Pattern_Size;

// You Lose
const unsigned char youLose_Song_Pattern_Size = 9;
const int youLose_Melody[youLose_Song_Pattern_Size] = {NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_E5};
unsigned char song_youLose_Counter = youLose_Song_Pattern_Size;

// You Win
const unsigned char youWin_Song_Pattern_Size = 9;
const int youWin_Melody[youWin_Song_Pattern_Size] = {NOTE_D4, NOTE_DS5, NOTE_FS4, NOTE_C5, NOTE_B4, NOTE_DS4, NOTE_D5, NOTE_D4, NOTE_E5};
unsigned char song_youWin_Counter = youWin_Song_Pattern_Size;

// & Seg Display
// B[BA.CDEFG] 1 for ON 0 for OFF
byte seven_seg_digits[11] = { B11011110, //0
                              B10010000, //1
                              B11001101, //2
                              B11011001, //3
                              B10010011, //4
                              B01011011, //5
                              B01011111, //6
                              B11010000, //7
                              B11011111, //8
                              B11010011, //9
                              B00000000  //Blank
};

int latchPin = 11;
int clockPin = 9;
int dataPin = 12;
byte scoreDigit = 4; // Player Lives

// Tasks
typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;
const byte tasksNum = 5;
task tasks[tasksNum];

bool buttonPressed(int button) {
  if(digitalRead(button) == HIGH) {
    return true;
  }
  else {
    return false;
  }
}

// Read Buttons Click
void readJButton() {
  if (!buttonPressed(jButton)) {
    jButton_Current = true;
  }
  else {
    jButton_Current = false;
  }
}

void readRButton() {
  if (!buttonPressed(rButton)) {
    rButton_Current = true;
  }
  else {
    rButton_Current = false;
  }
}

// Saves 1 click from buttons, no long press will register
void writeJButton() {
  if(jButton_Current) {
    if(!jButton_Previous) {
      jButton_Previous = true;
      jButton_Pressed = true;
    }
  }
  if(!jButton_Current) {
    jButton_Previous = false;
  }
}
void writeRButton() {
  if(rButton_Current) {
    if(!rButton_Previous) {
      rButton_Previous = true;
      rButton_Pressed = true;
    }
  }
  if(!rButton_Current) {
    rButton_Previous = false;
  }
}

void readAxis() {
  if(analogRead(yAxis) > 700){
    DOWN_Current = true;  
  }
  if(analogRead(yAxis) < 300){
    UP_Current = true;
  }
  if(analogRead(yAxis) <= 700 && analogRead(yAxis) >= 300){
    DOWN_Current = false;
    UP_Current = false;
  }
}

void writeAxis() {
  if(DOWN_Current) {
    if(!DOWN_Previous) {
      DOWN_Previous = true;
      DOWN_Pressed = true;
    }
  }
  if(!DOWN_Current) {
    DOWN_Previous = false;
  }
  if(UP_Current) {
    if(!UP_Previous) {
      UP_Previous = true;
      UP_Pressed = true;
    }
  }
  if(!UP_Current) {
    UP_Previous = false;
  }
}

// Linear congruential generator for Random Numbers
unsigned char getRandom(unsigned char& prev){
    unsigned char m = 42;
    unsigned long temp = millis();
    unsigned char a = 0;
    bitWrite(a, 0, bitRead(temp, 0));
    bitWrite(a, 1, bitRead(temp, 1));
    bitWrite(a, 2, bitRead(temp, 2));
    bitWrite(a, 3, bitRead(temp, 3));
    unsigned char c = 4;
    unsigned char X_n = prev;
    return ((a * X_n) + c) % m;
}

// Draw Bird
enum Difficulty_Level{Easy, Normal, Hard} level;
void draw_Flappy_Bird()
{
  display.clearDisplay();
    switch(level){
      case Easy:
      if(analogRead(yAxis) > 600 && position_Bird <=40 ){
        position_Bird = position_Bird+2;  
      }
      if(analogRead(yAxis) < 400 && position_Bird >2){
        position_Bird = position_Bird-2;
      }
      break;
      case Normal:
      if(analogRead(yAxis) > 600 && position_Bird <=40 ){
        position_Bird = position_Bird+2;  
      }
      if(analogRead(yAxis) < 400 && position_Bird >2){
        position_Bird = position_Bird-2;
      }
      break;
      case Hard:
      if(UP_Pressed && position_Bird >8){
        position_Bird = position_Bird-8;
        UP_Pressed = false;
      }
      else if(DOWN_Pressed && position_Bird <=40){
        position_Bird = position_Bird+2;
        DOWN_Pressed = false;
      }
      else{
        if(position_Bird <=40){
          position_Bird = position_Bird+1;
        }
      }
      break;
    }
    display.drawRoundRect(8, position_Bird, 9, 6, 4, BLACK);
    display.fillCircle(11, position_Bird+4, 1, BLACK);
    display.fillCircle(10, position_Bird+4, 1, BLACK);
    display.fillCircle(14, position_Bird+2, 1, BLACK);
}

// Draw Walls
void draw_Wals(unsigned char & wallPosition, unsigned char & wallHole){
  display.drawLine(wallPosition, wallHole+5, wallPosition, 46, BLACK); // Bottom wall
  display.drawLine(wallPosition, wallHole-5, wallPosition, 1, BLACK); // Top wall
  display.drawRect(1, 1, 83, 47, BLACK);
}

// Output to 7 Seg Display
void sevenSegWrite(byte digit){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);
  digitalWrite(latchPin, HIGH);
}

// Play Melody
void tone_Up(const int *arr, unsigned char & index, const unsigned char & arrLength) {
  if(index > 0) {
    index--;
    tone(buzzer, arr[index]);
  }
  else if(index == 0) {
    tone(buzzer, arr[index]);
    index = arrLength;
  }
}

enum Game_Controler_States{Game_Controler_INIT, //0
                           Game_Controler_Play, //1
                           Game_Try_Again,      //2
                           Game_Lost,           //3
                           Game_Won,            //4
                           Game_Reset,          //5
                           Game_RIP             //6
};

int Game_Controler_Tick(int state) {
    switch(state){ // Song State transitions
      case Game_Controler_INIT:
        if(i<=20) {
          tasks[1].state = 1;
          i++;
        }
        if(i>20 && i <=40) {
          tasks[1].state = 2;
          i++;
        }
        if(i > 40) {
          readAxis();
          writeAxis();
          readJButton();
          writeJButton();
          tasks[1].state = Difficulty_Screen_Index;
          if(UP_Pressed) {
            if(Difficulty_Screen_Index>3) {
              Difficulty_Screen_Index--;
            }
            else{
              Difficulty_Screen_Index = 5;
            }
            UP_Pressed = false;
          }
          if(DOWN_Pressed) {
            if(Difficulty_Screen_Index<5) {
              Difficulty_Screen_Index++;
            }
            else{
              Difficulty_Screen_Index = 3;
            }
            DOWN_Pressed = false;
          }
          if(jButton_Pressed){
            state = Game_Controler_Play;
            firstHole = getRandom(seed);
            firstWall=83;
            secondHole = getRandom(firstHole);
            secondWall=83;
            tasks[1].state = 11;
            tasks[2].state = 1;
            switch(Difficulty_Screen_Index){
              case 3:
                tasks[2].period = 40;
                level=0;
                break;
              case 4:
                tasks[2].period = 30;
                level=1;
                break;
              case 5:
                tasks[2].period = 50;
                level=2;
                break;
            }
            tasks[3].state = 3;
            tasks[4].state = 1;
            Difficulty_Screen_Index = 3;
            i = 0;
            jButton_Pressed = false;
          }
        }
        break;
      case Game_Controler_Play:
        if(Pass_A_Wall){
          tasks[3].state = 2;
          Pass_A_Wall = false;
        }
        if(Hit_A_Wall){
          song_Game_Counter = game_Song_Pattern_Size;
          song_tryAgain_Counter = tryAgain_Song_Pattern_Size;
          state = Game_Try_Again;
          tasks[1].state = 6;
          tasks[2].state = 0;
          tasks[3].state = 4;
          tasks[4].state = 2;
          Hit_A_Wall = false;
        }
        if(Game_Lose){
          song_youLose_Counter = youLose_Song_Pattern_Size;
          state = Game_Lost;
          tasks[1].state = 9;
          tasks[2].state = 2;
          tasks[3].state = 1;
          tasks[4].state = 0;
          Game_Lose = false;
          
        }
        if(Game_Win){
          song_youWin_Counter = youWin_Song_Pattern_Size;
          state = Game_Won;
          tasks[1].state = 10;
          tasks[2].state = 2;
          tasks[3].state = 1;
          tasks[4].state = 0;
          Game_Win = false;
        }
        if(rButton_Pressed){
          song_Game_Counter = game_Song_Pattern_Size;
          song_tryAgain_Counter = tryAgain_Song_Pattern_Size;
          state = Game_Reset;
          tasks[2].state = 2;
          tasks[3].state = 1;
          tasks[4].state = 0;
          rButton_Pressed = false;
        }
        break;
      case Game_Try_Again:
        i++;
        if(i%5==0){
          tasks[3].state = 4;
        }
        if(i>50) {
          state = Game_Controler_Play;
          tasks[1].state = 11;
          tasks[2].state = 1;
          tasks[3].state = 3;
          tasks[4].state = 1;
          i=0;
        }
        break;
      case Game_Lost:
        if(i==0){
        tasks[2].state = 0;
        tasks[4].state = 3;
        }
        if(i<=48){
          i++;
        }
        if(i>48) {
          tasks[4].state = 0;
          readAxis();
          writeAxis();
          readJButton();
          writeJButton();
          tasks[1].state = Lose_Play_Again_Screen_Index;
          if(UP_Pressed) {
            if(Lose_Play_Again_Screen_Index==8) {
              Lose_Play_Again_Screen_Index--;
            }
            else{
              Lose_Play_Again_Screen_Index = 8;
            }
            UP_Pressed = false;
          }
          if(DOWN_Pressed) {
            if(Lose_Play_Again_Screen_Index == 7) {
              Lose_Play_Again_Screen_Index++;
            }
            else{
              Lose_Play_Again_Screen_Index = 7;
            }
            DOWN_Pressed = false;
          }
          if(jButton_Pressed && Lose_Play_Again_Screen_Index == 7){
            state = Game_Controler_Play;
            tasks[1].state = 11;
            tasks[2].state = 1;
            tasks[3].state = 3; // sseven
            tasks[4].state = 1; // Song
            i = 0;
            Lose_Play_Again_Screen_Index = 7;
            jButton_Pressed = false;
          }
          else if(jButton_Pressed && Lose_Play_Again_Screen_Index == 8){
            state = Game_Controler_INIT;
            tasks[2].state = 0;
            tasks[3].state = 0;
            tasks[4].state = 0;
            i=41;
            Lose_Play_Again_Screen_Index = 7;
            jButton_Pressed = false;
          }
        }
        break;
      case Game_Won:
        if(i==0){
        tasks[2].state = 0;
        tasks[4].state = 4;
        }
        if(i<=50){
          i++;
        }
        if(i>50) {
          tasks[4].state = 0;
          readAxis();
          writeAxis();
          readJButton();
          writeJButton();
          tasks[1].state = Win_Play_Again_Screen_Index;
          if(UP_Pressed) {
            if(Win_Play_Again_Screen_Index==13) {
              Win_Play_Again_Screen_Index--;
            }
            else{
              Win_Play_Again_Screen_Index = 13;
            }
            UP_Pressed = false;
          }
          if(DOWN_Pressed) {
            if(Win_Play_Again_Screen_Index == 12) {
              Win_Play_Again_Screen_Index++;
            }
            else{
              Win_Play_Again_Screen_Index = 12;
            }
            DOWN_Pressed = false;
          }
          if(jButton_Pressed && Win_Play_Again_Screen_Index == 12){
            state = Game_Controler_INIT;
            tasks[2].state = 0;
            tasks[3].state = 0; // sseven
            tasks[4].state = 0; // Song
            i = 41;
            Win_Play_Again_Screen_Index = 12;
            jButton_Pressed = false;
          }
          else if(jButton_Pressed && Win_Play_Again_Screen_Index == 13){
            state = Game_RIP;
            display.clearDisplay();
            display.display();
            tasks[1].state = 0;
            tasks[2].state = 0;
            tasks[3].state = 0;
            tasks[4].state = 0;
            i=0;
            Win_Play_Again_Screen_Index = 12;
            jButton_Pressed = false;
          }
        }
        break;
      case Game_Reset:
        state = Game_Controler_INIT;
        break;
      case Game_RIP:
        readRButton();
        writeRButton();
        if(rButton_Pressed){
          state = Game_Controler_INIT;
          rButton_Pressed = false;
        }
        break;
    }
    return state;
}

enum Display_Print_States{ Display_INIT,              //0
                           Welcome_To_Print,          //1
                           Flappy_Bird_Print,         //2
                           Choose_Difficulty1_Print,  //3
                           Choose_Difficulty2_Print,  //4
                           Choose_Difficulty3_Print,  //5
                           Try_Again_Print,           //6
                           Lose_Play_Again_Yes_Print, //7
                           Lose_Play_Again_No_Print,  //8
                           You_Lose_Print,            //9
                           You_Win_Print,             //10
                           Display_Busy,              //11
                           Win_Play_Again_Yes_Print,  //12
                           Win_Play_Again_No_Print    //13
}; 
int Display_Print_Tick(int state) {
    switch(state){ 
      case Display_INIT:
       // Initial
       break;
      case Welcome_To_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(0,5);
        display.setTextSize(2);
        display.println("Welcome");
        display.setCursor(30,30);
        display.println("To");
        display.display();
       break;
      case Flappy_Bird_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(5,5);
        display.setTextSize(2);
        display.println("Flappy");
        display.setCursor(15,30);
        display.println("Bird!");
        display.display();
       break;
      case Choose_Difficulty1_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(13,1);
        display.setTextSize(0);
        display.println("Difficulty");
        display.setCursor(28,10);
        display.println("Level:");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(24,20);
        display.println(" EASY ");
        display.setTextColor(BLACK);
        display.setCursor(26,30);
        display.println("NORMAL");
        display.setCursor(30,40);
        display.println("HARD");
        display.display();
       break;
      case Choose_Difficulty2_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(13,1);
        display.setTextSize(0);
        display.println("Difficulty");
        display.setCursor(28,10);
        display.println("Level:");
        display.setCursor(30,20);
        display.println("EASY");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(20,30);
        display.println(" NORMAL ");
        display.setTextColor(BLACK);
        display.setCursor(30,40);
        display.println("HARD");
        display.display();
       break;
      case Choose_Difficulty3_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(13,1);
        display.setTextSize(0);
        display.println("Difficulty");
        display.setCursor(28,10);
        display.println("Level:");
        display.setCursor(30,20);
        display.println("EASY");
        display.setCursor(26,30);
        display.println("NORMAL");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(24,40);
        display.println(" HARD ");
        display.display();
       break;
      case Try_Again_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(20,5);
        display.setTextSize(2);
        display.println("Try");
        display.setCursor(12,25);
        display.println("Again");
        display.display();
       break;
      case Lose_Play_Again_Yes_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setTextSize(0);
        display.setCursor(12,0);
        display.println("Play Again ");
        display.setCursor(15,9);
        display.println("With Same");
        display.setCursor(10,18);
        display.println("Difficulty?");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(25,29);
        display.println(" YES ");
        display.setTextColor(BLACK);
        display.setCursor(28,39);
        display.println(" NO ");
        display.display();
       break;
      case Lose_Play_Again_No_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setTextSize(0);
        display.setCursor(12,0);
        display.println("Play Again ");
        display.setCursor(15,9);
        display.println("With Same");
        display.setCursor(10,18);
        display.println("Difficulty?");
        display.setTextColor(BLACK);
        display.setCursor(25,29);
        display.println(" YES ");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(28,39);
        display.println(" NO ");
        display.display();
       break;
      case You_Lose_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(22,5);
        display.setTextSize(2);
        display.println("You");
        display.setCursor(19,25);
        display.println("Lose");
        display.display();
       break;
      case You_Win_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(22,5);
        display.setTextSize(2);
        display.println("You");
        display.setCursor(20,25);
        display.println("Win!");
        display.display();
       break;
      case Display_Busy:
       // Standby
       break;
      case Win_Play_Again_Yes_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(10,6);
        display.setTextSize(0);
        display.println("Play Again?");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(25,20);
        display.println(" YES ");
        display.setTextColor(BLACK);
        display.setCursor(28,35);
        display.println(" NO ");
        display.display();
        break;
      case Win_Play_Again_No_Print:
        display.clearDisplay();
        display.setTextColor(BLACK);
        display.setCursor(10,6);
        display.setTextSize(0);
        display.println("Play Again?");
        display.setTextColor(BLACK);
        display.setCursor(25,20);
        display.println(" YES ");
        display.setTextColor(WHITE, BLACK);
        display.setCursor(28,35);
        display.println(" NO ");
        display.display();
        break;
    }
    return state;
}

enum Game_States{Game_INIT, Game_Play, Reset_Game_Variables};
int Game_Tick(int state) {
    switch(state){ 
      case Game_INIT:
        //Standby
        break;
      case Game_Play:
        readAxis();
        writeAxis();
        readRButton();
        writeRButton();
        if(firstWall>45){
        draw_Flappy_Bird();
        draw_Wals(firstWall, firstHole);
        display.display();
        firstWall--;
        }
        else if(firstWall<=45 && score == 5){
          draw_Flappy_Bird();
          draw_Wals(firstWall, firstHole);
          display.display();
          firstWall--;  
        }
        else if(firstWall<=45){
          draw_Flappy_Bird();
          draw_Wals(firstWall, firstHole);
          draw_Wals(secondWall, secondHole);
          display.display();
          firstWall--; 
          secondWall--; 
        }
        if(position_Bird>=firstHole-5 && position_Bird<=firstHole && firstWall==15) {
          firstHole = secondHole;
          firstWall=secondWall;
          secondHole = getRandom(firstHole);
          secondWall=83;
          score++;
          Pass_A_Wall = true;
          if(score==6){
            position_Bird = 15;
            Game_Win = true;
          }
        }
        else if((position_Bird<=firstHole-5 && firstWall==15) || (position_Bird>=firstHole && firstWall==15)) {
          firstHole = getRandom(secondHole);
          firstWall=83;
          secondHole = getRandom(firstHole);
          secondWall=83;
          score=0;
          if(scoreDigit>0){
            scoreDigit--;
            Hit_A_Wall = true;
          }
          else if(scoreDigit == 0){
            scoreDigit = 4;
            position_Bird = 15;
            Game_Lose = true;
          }
        }
        break;
        case Reset_Game_Variables:
          position_Bird = 15;
          firstHole = getRandom(secondHole);
          firstWall=83;
          secondHole = getRandom(firstHole);
          secondWall=83;
          score=0;
          scoreDigit = 4;
         break;    
       }
        return state;
    }

enum Seven_Seg_States{Seven_Seg_LEDs_INIT,     //0
                     Display_LEDs_OFF,  //1
                     Green_LED_ON,      //2
                     Green_LED_OFF,     //3
                     Red_LED_ON,        //4
                     Red_LED_OFF        //5
};

int Seven_Seg_LEDs_Tick(int state) {
    switch(state){ // Song State transitions
      case Seven_Seg_LEDs_INIT:
        // Standby
        break;
      case Display_LEDs_OFF:
        sevenSegWrite(10);
        digitalWrite(green_LED, LOW);
        digitalWrite(red_LED, LOW);
        break;
      case Green_LED_ON:
        sevenSegWrite(scoreDigit);
        digitalWrite(green_LED, HIGH);
        state = Green_LED_OFF;
        break;
      case Green_LED_OFF:
        sevenSegWrite(scoreDigit);
        digitalWrite(green_LED, LOW);
        digitalWrite(red_LED, LOW);
       break;
      case Red_LED_ON:
        sevenSegWrite(10);
        digitalWrite(red_LED, HIGH);
        state = Red_LED_OFF;
        break; 
      case Red_LED_OFF:
        sevenSegWrite(10);
        digitalWrite(red_LED, LOW);
        break;
    }
    return state;
}

// Song SM
enum Song_States{Song_INIT,      //0
                 Song_Game,      //1
                 Song_Try_Again, //2
                 Song_YouLose,   //3
                 Song_YouWin     //4
};
int Song_Tick(int state) {
    switch(state){ // Song State transitions
      case Song_INIT:
        noTone(buzzer);
        break;
      case Song_Game:
        tone_Up(game_Melody, song_Game_Counter, game_Song_Pattern_Size);
        break;
      case Song_Try_Again:
        tone_Up(tryAgain_Melody, song_tryAgain_Counter, tryAgain_Song_Pattern_Size);
        break;
      case Song_YouLose:
        tone_Up(youLose_Melody, song_youLose_Counter, youLose_Song_Pattern_Size);
        break;
      case Song_YouWin:
        tone_Up(youWin_Melody, song_youWin_Counter, youWin_Song_Pattern_Size);
        break;
    }
    return state;
}

void setup() {
  Serial.begin(9600);
  pinMode(red_LED, OUTPUT);
  pinMode(green_LED, OUTPUT);
  display.begin();
  display.clearDisplay();
  display.display();
  display.setContrast(50);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(jButton, INPUT_PULLUP);
  pinMode(rButton, INPUT_PULLUP);
  unsigned char task_counter = 0;
  tasks[task_counter].state = Game_Controler_INIT;
  tasks[task_counter].period = 100;
  tasks[task_counter].elapsedTime = 0;
  tasks[task_counter].TickFct = &Game_Controler_Tick;
  task_counter++;
  tasks[task_counter].state = Display_INIT;
  tasks[task_counter].period = 100;
  tasks[task_counter].elapsedTime = 0;
  tasks[task_counter].TickFct = &Display_Print_Tick;
  task_counter++;
  tasks[task_counter].state = Game_INIT;
  tasks[task_counter].period = 100;
  tasks[task_counter].elapsedTime = 0;
  tasks[task_counter].TickFct = &Game_Tick;
  task_counter++;
  tasks[task_counter].state = Seven_Seg_LEDs_INIT;
  tasks[task_counter].period = 250;
  tasks[task_counter].elapsedTime = 0;
  tasks[task_counter].TickFct = &Seven_Seg_LEDs_Tick;
  task_counter++;
  tasks[task_counter].state = Song_INIT;
  tasks[task_counter].period = 500;
  tasks[task_counter].elapsedTime = 0;
  tasks[task_counter].TickFct = &Song_Tick;
}

void loop() {
  for (unsigned char loop_counter = 0; loop_counter < tasksNum; ++loop_counter) {
    if ( (millis() - tasks[loop_counter].elapsedTime) >= tasks[loop_counter].period) {
        tasks[loop_counter].state = tasks[loop_counter].TickFct(tasks[loop_counter].state);
        tasks[loop_counter].elapsedTime = millis();
    }
  }
}