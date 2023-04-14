
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int led1_Red = 5;
int led1_Green = 6;
int led1_Input = 7;
int led1_R = 0;
int led1_G = 0;

int led2_Red = 8;
int led2_Green = 9;
int led2_Input = 10;
int led2_R = 0;
int led2_G = 0;

int led3_Red = 11;
int led3_Green = 12;
int led3_Input = 13;
int led3_R = 0;
int led3_G = 0;

int buzzer = 3;

bool phase = false; //for testing
int color_change_speed = 5;
int target = 0;
int time_to_hit = 1500;
int loop_no = 0;
bool errors_during_loop[3] = {0, 0, 0};

int points = 0;

void setup() {
  
  pinMode(led1_Red, OUTPUT); 
  pinMode(led1_Green, OUTPUT); 
  pinMode(led1_Input, INPUT_PULLUP); 

  pinMode(led2_Red, OUTPUT); 
  pinMode(led2_Green, OUTPUT); 
  pinMode(led2_Input, INPUT_PULLUP); 

  pinMode(led3_Red, OUTPUT); 
  pinMode(led3_Green, OUTPUT); 
  pinMode(led3_Input, INPUT_PULLUP); 

  pinMode(buzzer, OUTPUT);

  lcd.begin();
  lcd.print("Whack the Moles!");
  lcd.setCursor(0, 1);
  lcd.print("Points: ");
  
  
}

void loop() {
  pause_game();
  light_a_target();
  playing_time();
  time_to_hit * 0.92; //speed up the gamne
  loop_no ++;
  errors_during_loop[0] = 0; //reset error tracker
  errors_during_loop[1] = 0;
  errors_during_loop[2] = 0;
}

void pause_game() {
  double time;
  double start_time;
  target = 0;

  led1_R = 0;
  led2_R = 0;
  led3_R = 0;
  led1_G = 0;
  led2_G = 0;
  led3_G = 0;
  analogWrite(led1_Red, 0);
  analogWrite(led2_Red, 0);
  analogWrite(led3_Red, 0);
  
  int pause_time = rand() % 1000 + 500;
  time = millis();
  start_time = millis() + pause_time;

  while (millis() < start_time) { 
    if (digitalRead(led1_Input) || digitalRead(led2_Input) || digitalRead(led3_Input)) { //if button pressed in pause time -> penalty for the player
      penalty(); //impose penalty
      delay(250);
    }
  }
}

void light_a_target() {
  target = rand() % 3 + 1;
  if (target == 1) {
    led1_R = 60;
    analogWrite(led1_Red, led1_R);
  }
  if (target == 2) {
    led2_R = 60;
    analogWrite(led2_Red, led2_R);
  }
  if (target == 3) {
    led3_R = 60;
    analogWrite(led3_Red, led3_R);
  }
}

void playing_time () {
  double time = millis();
  double hit_time_window = time_to_hit;
  double end_time = millis() + hit_time_window;

  while (millis() < end_time) {
    if (digitalRead(led1_Input)) {
      if (target == 1) { // player pressed the right button
        add_points(led1_R); // add points for player
        led1_R = 0;
        analogWrite(led1_Red, led1_R);
        led1_G = 50;
        analogWrite(led1_Green, led1_G);
        delay(time_to_hit * 0.7);
        led1_G = 0;
        analogWrite(led1_Green, led1_G);
        break;
        
      } else { // player pressed the wrong button
          penalty(0); //impose penalty
      }
    }

    else if (digitalRead(led2_Input)) {
      if (target == 2) { // player pressed the right button
        add_points(led2_R); // add points for player
        led2_R = 0;
        analogWrite(led2_Red, led2_R);
        led2_G = 50;
        analogWrite(led2_Green, led2_G);
        delay(time_to_hit * 0.7);
        led2_G = 0;
        analogWrite(led2_Green, led2_G);
        break;
        
      } else { // player pressed the wrong button
          penalty(1); //impose penalty
      }
    } 

    else if (digitalRead(led3_Input)) {
      if (target == 3) { // player pressed the right button
        add_points(led3_R); // add points for player
        led3_R = 0;
        analogWrite(led3_Red, led3_R);
        led3_G = 50;
        analogWrite(led3_Green, led3_G);
        delay(time_to_hit * 0.7);
        led3_G = 0;
        analogWrite(led3_Green, led3_G);
        break;
        
      } else { // player pressed the wrong button
          penalty(2); //impose penalty
      }
    }
    if (target == 1) {
      led1_R -= color_change_speed;
      if (led1_R < 0){
        led1_R = 0;
      }
      analogWrite(led1_Red, led1_R);
    }

    if (target == 2) {
      led2_R -= color_change_speed;
      if (led2_R < 0){
        led2_R = 0;
      }
      analogWrite(led2_Red, led2_R);
    }

    if (target == 3) {
      led3_R -= color_change_speed;
      if (led3_R < 0){
        led3_R = 0;
      }
      analogWrite(led3_Red, led3_R);
    }
    
    delay(time_to_hit / (60 / color_change_speed)); //delay is dependent on time remaining
  }
}

void penalty(int diode_no) {
  if (!errors_during_loop[diode_no]) { //if the diode has been pressed for the first time this loop 
  //(This method means that we can not get penalty twice for clicking the same diode during a single loop)
    points -= 450;
    lcd.setCursor(9, 1);
    lcd.print("      ");
    lcd.print(points);

    errors_during_loop[diode_no] = 1;
    tone(buzzer, 300);
    delay(500);
    noTone(buzzer);

  }
}
void add_points(int light_remaining) {
  points += light_remaining * 10;
  lcd.setCursor(9, 1);
  lcd.print("      ");
  lcd.print(points);
  tone(buzzer, 800);
  delay(250);
  noTone(buzzer);
}




