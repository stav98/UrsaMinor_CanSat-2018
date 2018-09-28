#include <Servo.h>

#define SERVO_LEFT_PIN    21
#define SERVO_RIGHT_PIN   20
#define LEFT_MOTOR_SENS   A1
#define RIGHT_MOTOR_SENS  A2
#define servoZeroLeft     90  //Κάτω από 90 ανάποδα //160 εμπρος γρηγορα το zero είναι 90
#define servoZeroRight    90 //πάνω από 90 ανάποδα //0 εμπρος γρηγορα το zero είναι 20 (έπρεπε να είναι 90)


void servo_init(void);
void servo_stop(void);
void servo_disconnect(void);
void rover_fd(word);
void mv_servos_update(void);
void rover_lt(void);
void rover_rt(void);
void rover_turn(word, boolean);
void rover_bk(void);

Servo servo_left;
Servo servo_right;

const byte power_range[]      = { 10,  20,  40,  50,  60};
const int left_servo_limits[] = {420, 360, 260, 230, 440}; //60-200--440
const int right_servo_limits[] = {510, 400, 290, 250, 480};//60-210--500
byte servo_pwr = 0;
word mv_servo_dur = 0;
boolean left_servo_dir = false, right_servo_dir = false;

void servo_init()
{
 servo_left.attach(SERVO_LEFT_PIN);
 servo_right.attach(SERVO_RIGHT_PIN);
 servo_stop();
}

void servo_stop()
{
 servo_left.write(servoZeroLeft);
 servo_right.write(servoZeroRight);
}

void servo_disconnect()
{
 servo_left.detach();
 servo_right.detach();
}

void rover_fd(word meters)
{ 
 word d = meters * 2000; //Γιατί το ένα μέτρο είναι περίπου 2 sec
 servo_init();
 mv_servo_dur = d;
 servo_pwr = 60;
 left_servo_dir = right_servo_dir = true; //Μπροστά κίνηση
}
  
void mv_servos_update()
{
  static uint32_t wait_time_left, wait_time_right, start_time = 0;
  byte i, blocking_wheel = 0;
  word left_curr, right_curr;
  static word left_max, right_max, left_servo_max_lim, right_servo_max_lim;
  static boolean left_block_flag , right_block_flag;
  boolean stop_flag = false;
  if (servo_pwr > 0)
     {
      if (start_time == 0)
         {
          start_time = millis();
          left_max = right_max = 0;
          left_block_flag = right_block_flag = false;
          for (i = 0; i < sizeof(power_range); ++i)
               if (servo_pwr <= power_range[i])
                   break;
          left_servo_max_lim = left_servo_limits[i];
          right_servo_max_lim = right_servo_limits[i];
          //Ξεκίνα κίνηση των δύο servo
          if (left_servo_dir)  //Αν είναι true γύρνα εμπρός τον τροχό  
              servo_left.write(servoZeroLeft + servo_pwr); 
          else   //Αλλιώς γύρνα ανάποδα τον τροχό
              servo_left.write(servoZeroLeft - servo_pwr);
          if (right_servo_dir) //Αν είναι true γύρνα εμπρός τον τροχό 
              servo_right.write(servoZeroRight - servo_pwr);
          else   //Αλλιώς γύρνα ανάποδα τον τροχό
              servo_right.write(servoZeroRight + servo_pwr);
         }
      //Συνέχισε για όσο χρόνο υπάρχει στην καθολική mv_servo_dur  
      if (millis() <= start_time + mv_servo_dur)
         {
          //Διάβασε ένταση ρεύματος
          left_curr = analogRead(LEFT_MOTOR_SENS);
          right_curr = analogRead(RIGHT_MOTOR_SENS);
          //Στην αρχή περίμενε 200ms γιατί στο ξεκίνημα τραβάνε πολύ ρεύμα
          if ((millis() - start_time) > 200)
             {
              //Πάρε την μέγιστη τιμή και για τα δύο servos
              if(left_curr > left_max) left_max = left_curr;
              if(right_curr > right_max) right_max = right_curr;
             }
          /* Serial.print(millis() - start_time);
             Serial.print(" : ");
             Serial.print(left_curr);
             Serial.print(" - ");
             Serial.print(right_curr);
             Serial.print(" -> ");
             Serial.print(left_max);
             Serial.print(" - ");
             Serial.println(right_max);*/
      //Έλεγχος εμποδίου
      if (!full_power)
         {
          //---- Αριστερό Servo ----
          //Αν πέρασε το μέγιστο όριο και είναι η πρώτη φορά που έγινε
          if ((left_max > left_servo_max_lim) && (!left_block_flag))
             {
              left_max = 0;   //Μηδένισε ώστε να ανανεωθεί η τιμή
              wait_time_left = millis(); //Πάρε χρόνο
              left_block_flag = true; //Μη ξαναμπείς εδώ
             }
          //Αν πριν βρήκε μεγάλο ρεύμα και συνεχίζει να είναι μεγάλο μετά από 0,5sec τότε
          if ((left_max > left_servo_max_lim) && left_block_flag && (millis() - wait_time_left > 500))
             {
              //Serial.print("Left block");
              
              blocking_wheel = 1;
              stop_flag = true;  //Σταμάτα την κίνηση γιατί κόλλησε σε εμπόδιο
             }
          //---- Δεξί Servo ----
          //Αν πέρασε το μέγιστο όριο και είναι η πρώτη φορά που έγινε
          if ((right_max > right_servo_max_lim) && (!right_block_flag))
             {
              right_max = 0;   //Μηδένισε ώστε να ανανεωθεί η τιμή
              wait_time_right = millis(); //Πάρε χρόνο
              right_block_flag = true; //Μη ξαναμπείς εδώ
             }
          //Αν πριν βρήκε μεγάλο ρεύμα και συνεχίζει να είναι μεγάλο μετά από 0,5sec τότε
          if ((right_max > right_servo_max_lim) && right_block_flag && (millis() - wait_time_right > 500))
             {
              //Serial.print("Right block");
              
              stop_flag = true;  //Σταμάτα την κίνηση γιατί κόλλησε σε εμπόδιο
              blocking_wheel = 2;
             }
         }
        } //Έλεγχος εμποδίου
      else
         stop_flag = true;
      if (stop_flag)
         {
          servo_stop(); //Σταμάτησε την κίνηση
          start_time = 0;
          servo_disconnect();
          mv_servo_dur = 0;
          servo_pwr = 0;
          if (blocking_wheel == 1)
             {
              beep(1, 100);
              rover_bk();
              rover_turn(180, true);
              tx_response(PSTR("Left block"));
             }
          else if (blocking_wheel == 2)
             {
              beep(2, 100);
              rover_bk();
              rover_turn(180, false);
              tx_response(PSTR("Right block"));
             }
         }
     }
}

void rover_lt()
{
  servo_init();
  servo_left.write(servoZeroLeft); //from +20 to +70 max
  servo_right.write(servoZeroRight - 40); //-20 to -70 max
  delay(300);
  servo_stop();
  servo_disconnect();
}

void rover_rt()
{
  servo_init();
  servo_left.write(servoZeroLeft + 40); //from +20 to +70 max
  servo_right.write(servoZeroRight); //-20 to -70 max
  delay(300);
  servo_stop();
  servo_disconnect();
}

void rover_pitch_up()
{
 servo_init();
 servo_left.write(servoZeroLeft + 25);
 servo_right.write(servoZeroRight - 25); 
 delay(50);
 servo_stop();
 servo_disconnect();
}

void rover_pitch_down()
{
 servo_init();
 servo_left.write(servoZeroLeft - 25);
 servo_right.write(servoZeroRight + 25); 
 delay(50);
 servo_stop();
 servo_disconnect();
}

//Στροφή γύρω από τον άξονα. Οι τροχοί γυρίζουν αντίθετα.
void rover_turn(word deg, boolean dir)
{ 
  word del = map(deg, 0, 360, 0, 700); //Το 300ms είναι περίπου 170 μοίρες
  servo_init();
  servo_pwr = 50;
  mv_servo_dur = del;
  if (!dir) //false είναι αριστερά
     {
      left_servo_dir = true;
      right_servo_dir = false; 
     }
  else
     {
      left_servo_dir = false;
      right_servo_dir = true; 
     }
}

void rover_bk()
{
 servo_init();
 for (byte i = 0; i < 10; ++i)
     {
      servo_left.write(servoZeroLeft -60); //from +20 to +70 max
      servo_right.write(servoZeroRight + 60); //-20 to -70 max
      delay(200);
      servo_left.write(servoZeroLeft +20); //from +20 to +70 max
      servo_right.write(servoZeroRight - 20); //-20 to -70 max
      delay(200);
     }
 servo_left.write(servoZeroLeft +20); //from +20 to +70 max
 servo_right.write(servoZeroRight - 20); //-20 to -70 max
 delay(400);
 servo_stop();
 servo_disconnect();
}

