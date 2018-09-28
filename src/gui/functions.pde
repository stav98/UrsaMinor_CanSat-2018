

void ShowValues()
{
 textSize(30); 
 //Παρουσίαση θερμοκρασίας
 label1.Text = "Θερμοκρασία 1 : " + Temp1 + " º";
 label1.display();
 
 //Παρουσίαση Πίεσης
 label2.Text = "Πίεση: " + Pressure + " Pa";
 label2.display();
 
 //Παρουσίαση Υψομέτρου
 label3.Text = "Υψόμετρο: " + nf(Altitude, 1, 1) + " m";
 label3.display();
 
 //Παρουσίαση Υγρασίας
 label4.Text = "Υγρασία: " + Humidity + " %";
 label4.display();
 
 //Παρουσίαση θερμοκρασίας MPU6050
 label5.Text = "Μαγν. Πεδίο: " + Temp2 + " μT";
 label5.display();
 
 textSize(32); 
 fill(#640909);
 text(nf(hour(), 2) + ":" + nf(minute(), 2) + ":" + nf(second(), 2), 10, 532, 200, 60); 
}

void ShowBattery(int x, int y, int w, int h)
{
 int pole_width = w / 3; int pole_height = h / 10;
 textSize(w / 2); 
 fill(90);   //Χρώμα γεμίσματος
 strokeWeight(1);
 stroke(255, 255, 255); //Χρώμα border 
 rect(x, y, w, h); //Φτιάξε κυρίως πλαίσιο μπαταρίας
 rect(x + w / 2 - pole_width / 2 , (y - pole_height), pole_width, pole_height); //Φτιάξε τον πόλο
 label6.Text = Battery + " V";
 label6.display();
 int BatPerCent = BatteryCapacity(Battery);
 if (BatPerCent > 20)
      fill(#30F220); //Πράσινο
 else
      fill(#DB0F7C); //Κόκκινο
 int y_pix = (h * BatPerCent) / 100;
 rect(x, y + h-y_pix, w, y_pix);
 fill(255); //Χρώμα κειμένου
 textAlign(CENTER);
 pushMatrix();
 translate(x, y);
 rotate(PI/2); 
 text(str(BatPerCent) + " %", h/2, -w/3); 
 rotate(-PI/2);
 popMatrix();
}

int BatteryCapacity(float volts)
{
 int i, bat_per_cent;
 //float voltage[] = {4.2, 4.3, 4.4, 4.45, 4.5, 4.6, 4.7, 4.8, 4.9, 5.0, 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 6.0, 7.0, 8.0, 8.5};
 float voltage[] = {6.6, 6.7, 6.9, 7.1, 7.3, 7.5 };
 int percent[] =   { 0 ,   3,  20,  50,  80, 100 };
 if (volts >= 7.5)
     return 100;
 else if (volts >= 6.6)
    {
     for (i = 0; i < voltage.length; ++i)
          if (volts < voltage[i]) break;
     bat_per_cent = int(map(volts, voltage[i-1], voltage[i], percent[i-1], percent[i])); 
     return bat_per_cent;
    }
 else
    return 0;
}

void ShowSignal(int x, int y, int w, int h)
{
 stroke(255, 255, 255); 
 strokeWeight(1); 
 fill(90);
 triangle(x, y, x+w, y, x+w, y-h); //Μπροστά βελάκι
 label7.Text = RSSI + " dbm";
 label7.display();
 int SigStrength = Signal_2_Value(RSSI);
 if (SigStrength > 30)
      fill(#30F220); //Πράσινο
 else
      fill(#DB0F7C); //Κόκκινο
 int x_pix = (w * SigStrength) / 100;
 int y_pix = (h * SigStrength) / 100;
 triangle(x, y, x+x_pix, y, x+x_pix, y-y_pix);
}

int Signal_2_Value(int dbm)
{
 int i, signal;
 int sig[] =       {-90, -80, -70, -60, -50, -40 };
 int percent[] =   {  5 , 30,  60,  70,  80, 100 };
 if (dbm >= -40)
     return 100;
 else if (dbm >= -95)
    {
     for (i = 0; i < sig.length; ++i)
          if (dbm < sig[i]) break;
     signal = int(map(dbm, sig[i-1], sig[i], percent[i-1], percent[i])); 
     return signal;
    }
 else
    return 0;
}