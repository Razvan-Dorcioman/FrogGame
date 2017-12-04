#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LedControl.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

bool resetState = 1; //reset
bool startState = 1; //start
bool restartState = 1; //restart
bool bestScoreState = 1; //best scor

int nivel = 1;
int viata = 3;
int scor = 0;
int bestScore = 0;
int UD = 0;
int LR = 0;
int push = 6;
int ox = 0;
int oy = 4;
bool mouseUD = 0;
bool mouseLR = 0;

//timere
long int t0 = 0;
long int t1 = 0;
long int t2 = 0;
long int t3 = 0;
long int t4 = 0;
int t01;
int t02;
int t03;
int t04;

byte ledMatrix[8][8];

LedControl lc = LedControl(12, 11, 10, 1);

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 8); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode(push, INPUT_PULLUP);
}

void set(int x, int y, int state) {
  if (x > 7)
    x = 7;
  if (y > 7)
    y = 7;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  ledMatrix[x][y] = state;
}

void pushMatrix() {
  for ( int i = 0 ; i < 8 ; i++) {
    for (int j = 0 ; j < 8 ; j++) {
      lc.setLed(0, i, j, ledMatrix[i][j] % 2);
    }
  }
}

void clearMatrix() {
  for ( int i = 0 ; i < 8 ; i++)
    for (int j = 0 ; j < 8 ; j++)
      ledMatrix[i][j] = 0;

}

void timer() {

  if (millis() - t0 > 100) {
    moveDot();
    t0 = millis();
  }

  if (millis() - t1 > 400) {
    t01++;
    t1 = millis();
  }

  if (millis() - t2 > 300) {
    t02++;
    t2 = millis();
  }

  if (millis() - t3 > 200) {
    t03++;
    t3 = millis();
  }

  if (millis() - t4 > 100) {
    t04++;
    t4 = millis();
  }
}

void masina(int line, int sizeMas , long int timer, int directionMas) {

  long int t = timer;

  if (directionMas < 0) {
    set(line, ( t - 1) % 8, 0);

    for (int i = 0 ; i < sizeMas ; i++)
    {
      set(line, ( t + i) % 8 , 1);
    }
  }
  else
  {
    set(line, 7 - ((t - 1) % 8), 0);

    for (int i = 0 ; i < sizeMas ; i++)
    {
      set(line, 7 - ((t + i) % 8) , 1);
    }
  }

  checkIfDead(ox, oy);

  set(ox, oy, 7);
}

void moveMenu() {
  UD = analogRead(A0);
  LR = analogRead(A1);

  if (UD > 700) {
    mouseUD = 0;
  }
  if (UD < 500) {
    mouseUD = 1;
  }
  if (LR > 700) {
    mouseLR = 0;
  }
  if (LR < 500) {
    mouseLR = 1;
  }
}

void moveDot()
{
  UD = analogRead(A0);
  LR = analogRead(A1);

  int lastX = ox;
  int lastY = oy;

  if (UD > 700 && ox != 7) {
    ox++;
  }
  
  if (LR > 700 && oy != 7) {
    oy++;
  }
  
  if (LR < 500 && oy != 0) {
    oy--;
  }

  if (ox != lastX)
    scor += ox * nivel;

  set(lastX, lastY, 0);

  set(ox, oy, 7);
}

void level();
void start();
void reset();

void restart() {
  int pushState = digitalRead(push);
  
  if (pushState == LOW) {
    clearMatrix();
    pushMatrix();
    restartState = 0;
    ox = 0;
    oy = 4;
    viata = 3;
    scor = 0;
    nivel = 1;

    moveMenu();

    if (mouseUD == 0) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Menu:");
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 24);
      display.println(">  START");
      display.setCursor(0, 48);
      display.println("  Best");
      display.display();
    }
    else {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Menu:");
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 24);
      display.println("  START");
      display.setCursor(0, 48);
      display.println(">  Best");
      display.display();
    }


    if (mouseLR == 1 && mouseUD == 1) {
      bestScoreState = 0;
    }

    while (!bestScoreState) {
      moveMenu();
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 32);
      display.println("Best: ");
      display.setCursor(80, 32);
      display.println(bestScore, DEC);
      display.display();
      if (mouseLR == 0)
        bestScoreState = 1;
    }

    startState = 1;
    mouseLR = 0;
    if (mouseLR == 1 && mouseUD == 0) {
      startState = 0;
    }

    while (!startState) {
      start();
    }
  }
  pushState = HIGH;
}

void checkIfDead(int x , int y) {
  int pushState = digitalRead(push);

  if (ledMatrix[ox][oy] == 1) {

    clearMatrix();

    set(x, y, 0);
    viata--;

    if (viata) {
      if (nivel > 1) {
        scor -= scor / nivel;
      }
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("DEAD!");
      display.setTextSize(2);
      display.setCursor(0, 24);
      display.println("Lives: ");
      display.setCursor(80, 24);
      display.println(viata);
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.println("Press to restart!");
      display.display();

      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          if (i == j || i + j == 7)
            set(i, j, 1);

      pushMatrix();

      resetState = 1;
      while (resetState) {
        reset();
      }
    }
    else
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Game over!!");
      display.setCursor(0, 24);
      display.println("Score: ");
      display.setCursor(80, 24);
      display.println(scor, DEC);
      display.setTextSize(1);
      display.setCursor(0, 48);
      display.println("Press to exit!");
      display.display();
      set(ox, oy, 0);

      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          if (i == 5 || i == 4 || j == 3 || j == 4)
            set(i, j, 1);

      pushMatrix();

      if (scor > bestScore)
        bestScore = scor;

      restartState = 1;
      while (restartState) {
        restart();
      }
    }
  }

  pushState = HIGH;

}

void reset() {
  int pushState = digitalRead(push);

  clearMatrix();

  if (pushState == LOW) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Level: ");
    display.setCursor(80, 0);
    display.println(nivel, DEC);
    display.setCursor(0, 24);
    display.println("Lives: ");
    display.setCursor(80, 24);
    display.println(viata, DEC);
    display.setCursor(0, 48);
    display.println("Score: ");
    display.setCursor(80, 48);
    display.println(scor, DEC);
    display.display();
    clearMatrix();
    ox = 0;
    oy = 4;
    level();
    resetState = 0;
    pushMatrix();
  }
  pushState = HIGH;
}

void level() {
  timer();

  if (nivel == 1) {
    masina(1, 3, t01, -1);
    masina(3, 2, t02, 1);
    masina(5, 4, t02, -1);
    masina(7, 3, t01, 1);

    checkIfDead(ox, oy);
  }
  else if (nivel == 2)
  {
    masina(1, 2, t02, 1);
    masina(2, 3, t03, -1);
    masina(4, 4, t01, -1);
    masina(5, 1, t03, 1);
    masina(6, 3, t02, -1);

    checkIfDead(ox, oy);
  }
  else if (nivel == 3) {
    masina(1, 4, t02, -1);
    masina(3, 2, t04, 1);
    masina(5, 3, t03, -1);
    masina(6, 2, t02, -1);
    masina(7, 2, t01, 1);

    checkIfDead(ox, oy);
  }
  else if (nivel == 4) {
    masina(1, 2, t03, 1);
    masina(2, 1, t02, -1);
    masina(3, 3, t01, -1);
    masina(4, 4, t02, 1);
    masina(5, 2, t02, -1);
    masina(6, 1, t01, 1);
    masina(7, 2, t02, -1);

    checkIfDead(ox, oy);
  }
  else if (nivel == 5) {

    masina(1, 3, t01, -1);
    masina(2, 1, t04, 1);
    masina(3, 2, t03, -1);
    masina(4, 4, t02, 1);
    masina(5, 3, t03, -1);
    masina(6, 2, t04, 1);

    checkIfDead(ox, oy);
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("WIN!");
    display.setCursor(0, 24);
    display.println("Score: ");
    display.setCursor(80, 24);
    display.println(scor, DEC);
    display.setTextSize(1);
    display.setCursor(0, 48);
    display.println("Press to exit!");
    display.display();
    clearMatrix();
    pushMatrix();

    if (scor > bestScore)
      bestScore = scor;

    restartState = 1;
    while (restartState) {
      restart();
    }
  }
  checkIfWin(ox);
  pushMatrix();
}

void checkIfWin(int x) {
  if (ox == 7) {
    nivel++;
    scor += viata * 100;
    clearMatrix();
    set(ox, oy, 0);
    ox = 0;
    oy = 4;
    delay(200);
    level();
    pushMatrix();
  }
}

void start() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Level: ");
  display.setCursor(80, 0);
  display.println(nivel, DEC);
  display.setCursor(0, 24);
  display.println("Lives: ");
  display.setCursor(80, 24);
  display.println(viata, DEC);
  display.setCursor(0, 48);
  display.println("Score: ");
  display.setCursor(80, 48);
  display.println(scor, DEC);
  display.display();
  level();
}

void meniu() {
  moveMenu();
  clearMatrix();
  pushMatrix();

  if (mouseUD == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Menu:");
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println(">  START");
    display.setCursor(0, 48);
    display.println("  Best");
    display.display();
  }
  else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Menu:");
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println("  START");
    display.setCursor(0, 48);
    display.println(">  Best");
    display.display();
  }

  if (mouseLR == 1 && mouseUD == 1) {
    bestScoreState = 0;
  }

  while (!bestScoreState) {
    moveMenu();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    display.println("Best: ");
    display.setCursor(80, 32);
    display.println(bestScore, DEC);
    display.display();

    if (mouseLR == 0) {
      bestScoreState = 1;
      clearMatrix();
    }
  }

  if (mouseLR == 1 && mouseUD == 0) {
    startState = 0;
    clearMatrix();
  }

  while (!startState) {
    start();
  }
}

void loop() {
  meniu();
}
