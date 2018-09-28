byte txlogState = 0;
String img_data="";
void serialEvent(Serial port) //Διάβασε τα δεδομένα της σειριακής
{
 String tmp[], values[];
 String input = port.readStringUntil('\n');
 String S_data = "", G_data = "", T_data;
 //println(input);
 T_data = hour()+ ":" + minute() + ":" + second() + ";" + millis();
 if((input != null) && ((input.indexOf("S:")) > -1))
   {
    tmp = split(input, "S:");
    S_data = T_data + tmp[1];
    S_data = S_data.replace('.', ',');
    cansat_data.println(S_data);
    values = split(tmp[1], ";");
    //print(tmp[1]);
    if (values.length == 9)
       {
        Temp1 = float(values[1]);
        Pressure = int(values[2]);
        if (!first_pressure)
           {
             if (Pressure > 0)
                {
                 first_pressure = true;
                 pressure_offs = float(Pressure);
                }
           }
             
        //Altitude=(((float)-45846.2)*(pow(((float)mySensor.readFloatPressure()/(float)pressure_offs), 0.190263) - (float)1)), 6, 1, alt_buffer;
        Altitude = -45846.2 *(pow(float(values[2])/pressure_offs, 0.190263) - 1); 
        Humidity = float(values[3]);
        Battery = float(values[4]);
        float phi = float(trim(values[6]));
        float theta = float(trim(values[5])); 
        float psi = float(trim(values[7]));
        Temp2 = float(values[8]);
        //print(phi); //debug
        //print(theta); //debug
        //println(psi); //debug
        Phi = phi;
        Theta = theta;
        Psi = psi;
       }
   }
 else if ((input != null) && ((input.indexOf("Re:")) > -1))
   {
    println(input);
    tmp = split(input, "Re:");
    values = split(tmp[1], ":");
    if (values[0].indexOf("IMG") > -1)
       {
        if (values[1].indexOf("Save") > -1)
           {
            println("Saved to SD");
            if (snapState == 1)
                snapState = 2;
           }
        else if (values[1].indexOf("Trans") > -1)
           {
            println("Transminsion Complete");
            if (snapState == 3)
                snapState = 4;
           }
        else if (values[1].indexOf("Photo") > -1)
           {
            println("Photo Transminsion Complete");
            if (photoState == 1)
                photoState = 2;
           }
       }
    else if (values[0].indexOf("LND") > -1)
       {
        if (values[1].indexOf("Released") > -1)
           {
            println("Parachute Released");
            chute_radio.selected = false;
            chuteOnce = false;
           }
       }
     else if (values[0].indexOf("LOG") > -1)
       {
        if (values[1].indexOf("Trans") > -1)
           {
            println("Transminsion Complete");
            if (txlogState == 1)
                txlogState = 2;
           }
       }
   }
 else if ((input != null) && ((input.indexOf("G:")) > -1))
   {
    tmp = split(input, "G:");
    G_data = T_data + tmp[1];
    position_data.println(G_data);
    values = split(tmp[1], ";");
   // println(tmp[1]);
    //;152221.00;090418;3857.14533;N;02044.63438;E;A
    
    if (values.length == 8)
       {
        if (values[7].indexOf("A") > -1)
           {
            textSize(15); 
            fill(#0A428E);
            
        longitude = convert_pos(values[3]);
        latitude = convert_pos(values[5]);
        
        map_update();
           }
       }
   }
 else if ((input != null) && ((input.indexOf("$")) > -1))
   {
    tmp = split(input, "$");
    values = split(tmp[1], ";");
    if (values.length == 2)
       {
        img_data += values[1];
        //print(values[1]); //Debug
       }
   }

else if ((input != null) && ((input.indexOf("L>RSSI:")) > -1))
   {
    tmp = split(input, ":");
    values = split(tmp[1], "#");
    RSSI = int(trim(values[0]));
   }
}

String convert_pos(String input)
{
  float t = float(input);
  //println(t); //3857.141
  int part1 = int(t / 100); //38
  int part2 = int(((((t * 1000)/100000)-part1)*10000000)/60); //3857142.0 
  String s = part1+"."+part2;
  return s;
}

void chk_log_state()
{
 if (txlogState > 0)
    {
     if (txlogState == 2)
        {
         byte[] decodedBytes = Base64.decodeBase64(img_data);;
         saveBytes("input/DATA1.txt", decodedBytes);
         txlogState = 0;
        }
    }
}