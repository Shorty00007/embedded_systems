
#include <LiquidCrystal_I2C.h>

// DEVICES

LiquidCrystal_I2C lcd(0x27, 16, 2);
int buzzer_pin = 3;

int led_number = 3;
int led_red_pins[] =    {5, 8, 11};
int led_green_pins[] =  {6, 9, 12};
int led_input_pins[] =  {7, 10, 13};
int led_red_values[] =   {0, 0, 0};
int led_green_values[] = {0, 0, 0};


// PROPERTIES

int target_id = -1;
int time_to_hit = 1500;
int color_change_speed = 5;
bool errors_during_loop[3] = {0, 0, 0};
int points = 0;

// unused (for now)
int loop_no = 0;
int time_to_play = 18000;
bool game_ended = false;


// IO UTIL

void update_red_led(int id, int value) {
  led_red_values[id] = value;
  analogWrite(led_red_pins[id], value);
}

void update_green_led(int id, int value) {
  led_green_values[id] = value;
  analogWrite(led_green_pins[id], value);
}

// -1 if none or multiple buttons clicked
int scan_input() {
  int id = -1;
  for (int i = 0; i < led_number; i++) {
    if (!digitalRead(led_input_pins[i]))
      continue;

    if (id != -1)
      return -1;
    id = i;
  }
  return id;
}


// SYSTEM UTIL

void update_clock() {
    lcd.setCursor(7, 0);
    if (millis() > time_to_play) {
      game_ended = true;
      lcd.print("0s");
    }
    else {
      lcd.print((time_to_play - millis()) / 1000);
      lcd.print("s ");
    }
    
}

void print_points() {
  lcd.setCursor(9, 1);
  lcd.print(points);
  lcd.print("      ");
}

void add_points(int light_remaining) {
  points += light_remaining * 10;
  print_points();
  tone(buzzer_pin, 800);
  delay(250);
  noTone(buzzer_pin);
}

void penalty(int diode_id) {
  if (!errors_during_loop[diode_id]) {
    // if the diode has been pressed for the first time this loop 
    // (This method means that we can not get penalty twice for clicking the same diode during a single loop)
    
    points -= 450;
    print_points();

    errors_during_loop[diode_id] = 1;
    tone(buzzer_pin, 300);
    delay(500);
    time_to_play += 500;
    noTone(buzzer_pin);
  }
}


// GAME STATES

void pause_game() {
  double time;
  double start_time;
  target_id = -1;

  for (int i = 0; i < led_number; i++) {
    led_red_values[i] = 0;
    led_green_values[i] = 0;
    analogWrite(led_red_pins[i], 0);
  }
  
  int pause_time = rand() % (time_to_hit / 2) + 500;
  time = millis();
  start_time = millis() + pause_time;

  while (millis() < start_time) { 
    update_clock();
    int input = scan_input();
    if (input != -1) {
      penalty(input);
    }
  }
}

void light_a_target() {
  target_id = rand() % led_number;
  update_red_led(target_id, 60);
}

void playing_time () {
  double time = millis();
  double hit_time_window = time_to_hit;
  double end_time = millis() + hit_time_window;

  while (millis() < end_time) {
    update_clock();

    // input detection
    int input = scan_input();

    if (input == target_id) {
      add_points(led_red_values[target_id]);
      update_red_led(target_id, 0);

      // tick the green diode
      update_green_led(target_id, 50);
      delay(time_to_hit * 0.7);
      time_to_play += time_to_hit * 0.7;
      update_green_led(target_id, 0);

      break;
    }
    else if (input != -1)
      penalty(input);

    // dim led diode when nothing
    led_red_values[target_id] -= color_change_speed;
    if (led_red_values[target_id] < 0) {
      led_red_values[target_id] = 0;
    }
    analogWrite(led_red_pins[target_id], led_red_values[target_id]);
    
    delay(time_to_hit / (60 / color_change_speed)); //delay is dependent on time remaining
  }
}


// MAIN

void setup() {
  for (int i = 0; i < led_number; i++) {
    pinMode(led_red_pins[i], OUTPUT);
    pinMode(led_green_pins[i], OUTPUT);
    pinMode(led_input_pins[i], INPUT_PULLUP);
  }
  pinMode(buzzer_pin, OUTPUT);

  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.setCursor(0, 1);
  lcd.print("Points: ");
}

void loop() {
  if (game_ended){
    lcd.setCursor(0, 0);
    lcd.print("Game Over.    ");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(points);
    lcd.print("     ");
    delay(3000);
    return(0);
  }
  pause_game();
  light_a_target();
  playing_time();
  time_to_hit *= 0.92; //speed up the gamne
  loop_no ++;
  errors_during_loop[0] = 0; //reset error tracker
  errors_during_loop[1] = 0;
  errors_during_loop[2] = 0;
}
