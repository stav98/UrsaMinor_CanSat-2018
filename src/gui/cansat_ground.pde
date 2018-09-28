//Ursa Minor - CanSat Team from EPAL Prevezas (c)2017 - 18

import processing.serial.*;
import cc.arduino.*;
import org.apache.commons.codec.binary.Base64;

int bgColor = #FAD456;
float Pitch; 
float Bank; 
float Azimuth; 
float ArtificialHoizonMagnificationFactor = 0.5; 
float SpanAngle=120; 
int NumberOfScaleMajorDivisions; 
int NumberOfScaleMinorDivisions; 
float pressure_offs = 101643.0;
boolean sampintOnce = false, chuteOnce = false, first_pressure = false;
int fd_step = 1;
//PVector v1, v2; 

Serial port;
PrintWriter cansat_data, position_data;

float Phi, Theta, Psi;
float Temp1, Temp2, Altitude, Humidity, Battery;
int Pressure, RSSI = -99;

void setup() 
{ 
 size(980, 570); //Διαστάσεις παραθύρου
 rectMode(CENTER); //Οι συντεταγμένες μετράνε από το κέντρο
 smooth(); 
 strokeCap(SQUARE); //Προαιρετικό
 println((Object[])Serial.list()); //Εμφάνιση λίστας με τις διαθέσιμες σειριακές θύρες
 port = new Serial(this, Serial.list()[1], 115200); //Σε ποια είναι συνδεδεμένος ο σταθμός εδάφους του CanSat
 port.bufferUntil('\n'); //Αν στο buffer έρθει \n τότε θα δημιουργηθεί συμβάν serialEvent στο serialTerminal
 CreateUI(); //Δημιουργεί το User Interface
 map_init(); //Αρχικοποιεί τον χάρτη
 snap_init();
 String file_t_stamp = hour() + "_" + minute() + "_" + second();
 cansat_data = createWriter("input/data_" + file_t_stamp + ".txt"); 
 position_data = createWriter("input/position_" + file_t_stamp + ".txt"); 
}

void draw() 
{  
  pushMatrix(); //Αποθηκεύει το σύστημα συντεταγμένων
  //translate(200, H / 2.1);  //Αλλάζει το κέντρο των σχημάτων 
  translate(180, 334);
  MakeAnglesDependentOnMPU6050(); 
  Show_Instruments();
  popMatrix();
  rectMode(CORNER); 
  ShowValues();
  ShowBattery(240, 15, 30, 70);
  ShowSignal(240, 146, 40, 30);
  UpdateControls();
  map_redraw();
  chk_snap_state();
  chk_log_state();
  chk_photo_state();
  show_snap();
}

void MakeAnglesDependentOnMPU6050() 
{ 
  Bank = -Phi; ///5; //-Phi/5 //Roll
  //Bank=0;
  //Pitch = map(Theta, -179.9, 179.9, -89.9, 89.9);
  Pitch = Theta; //*100 //Pitch
  //Pitch = 0;
  Azimuth = -Psi-90;
}