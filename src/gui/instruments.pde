void Show_Instruments()
{
 Horizon(); 
 rotate(-radians(Bank)); 
 PitchScale(); //Οριζόντιες γραμμές με την μεταβολή του pitch στο κέντρο - | | | | | | - κάθετα
 Axis(); //Οι άξονες - σταυρόνημα 
 rotate(radians(Bank)); 
 Borders(); 
 Plane(); //Το κέντρο του μετρητή ---- ^ ---- 
 ShowAngles(); 
 Compass(); 
 ShowAzimuth(); 
}

void Horizon() 
{ 
  //scale(0.9);
  noStroke(); 
  //fill(0, 180, 255);    //Ουρανός Χρώμα μπλε 
  fill(#4188F7);
  rect(0, -100, 400, 600); //0, -100, 900, 1000
  fill(95, 55, 40);  //Έδαφος 
  float G_roll = radians(Bank);
  rotate(-G_roll); 
 // Pitch = 110;
  if(Pitch > -180 && Pitch < 90)
    {
     float G_pitch = Pitch * 2.22;
     rect(0, 200+G_pitch, 450, 400);
    }
  else
    {
     float G_pitch = (Pitch - 90) * 4.44;
     rect(0, -200, 450, -G_pitch);
    }
  
  rotate(G_roll); 
  rotate(-PI-PI/6); 
  SpanAngle = 120; 
  NumberOfScaleMajorDivisions=12; 
  NumberOfScaleMinorDivisions=24;  
  CircularScale(400); 
  rotate(PI+PI/6); 
  rotate(-PI/6);  
  CircularScale(400); 
  rotate(PI/6); 
}

void PitchScale() 
{  
  stroke(255); 
  fill(255); 
  strokeWeight(2); 
  textSize(14); 
  textAlign(CENTER); 
  for (int i=-4;i<5;i++) 
  {  
    if ((i==0)==false) 
    { 
      line(50, 25*i, -50, 25*i); 
    }  
    text(""+i*10, 75, 25*i, 100, 17); 
    text(""+i*10, -75, 25*i, 100, 17); 
  } 
  textAlign(CORNER); 
  strokeWeight(1); 
  for (int i=-9;i<10;i++) 
  {  
    if ((i==0)==false) 
    {    
      line(25, 12.5*i, -25, 12.5*i); 
    } 
  } 
}

void Axis() 
{ 
  stroke(255, 0, 0); 
  strokeWeight(2); 
  line(-100, 0, 100, 0); 
  line(0, 130, 0, -130); 
  fill(100, 255, 100); 
  stroke(0); 
  triangle(0, -140, -5, -120, 5, -120); 
  triangle(0, 140, -5, 120, 5, 120); 
}

void Borders() 
{ 
  noFill(); 
  stroke(bgColor); //stroke(0)
  //stroke(203);
  strokeWeight(200); 
  rect(0, 0, 550, 550); 
  strokeWeight(100); 
  ellipse(0, 0, 500, 500); 
  fill(bgColor); //fill(0)
  //fill(203);
  
  //stroke(bgColor);
  //strokeWeight(400); 
  //rect(0, 0, 1100, 1100); 
  
  noStroke(); 
  rect(510, 83, 600, 350); 
  rect(407, -210, 300, 250);
}

void Plane() 
{ 
  fill(0); 
  strokeWeight(1); 
  stroke(0, 255, 0); 
  triangle(-10, 0, 10, 0, 0, 12.5); 
  rect(50, 0, 80, 10); 
  rect(-50, 0, 80, 10); 
}

void CircularScale(float GaugeWidth) 
{ 
  //float GaugeWidth=400;  
  textSize(GaugeWidth/30); 
  float StrokeWidth=1; 
  float an; 
  float DivxPhasorCloser; 
  float DivxPhasorDistal; 
  float DivyPhasorCloser; 
  float DivyPhasorDistal; 
  strokeWeight(2*StrokeWidth); 
  stroke(255);
  float DivCloserPhasorLenght=GaugeWidth/2-GaugeWidth/9-StrokeWidth; 
  float DivDistalPhasorLenght=GaugeWidth/2-GaugeWidth/7.5-StrokeWidth;
  for (int Division=0;Division<NumberOfScaleMinorDivisions+1;Division++) 
  { 
    an=SpanAngle/2+Division*SpanAngle/NumberOfScaleMinorDivisions;  
    DivxPhasorCloser=DivCloserPhasorLenght*cos(radians(an)); 
    DivxPhasorDistal=DivDistalPhasorLenght*cos(radians(an)); 
    DivyPhasorCloser=DivCloserPhasorLenght*sin(radians(an)); 
    DivyPhasorDistal=DivDistalPhasorLenght*sin(radians(an));   
    line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal); 
  }
  DivCloserPhasorLenght=GaugeWidth/2-GaugeWidth/10-StrokeWidth; 
  DivDistalPhasorLenght=GaugeWidth/2-GaugeWidth/7.4-StrokeWidth;
  for (int Division=0;Division<NumberOfScaleMajorDivisions+1;Division++) 
  { 
    an=SpanAngle/2+Division*SpanAngle/NumberOfScaleMajorDivisions;  
    DivxPhasorCloser=DivCloserPhasorLenght*cos(radians(an)); 
    DivxPhasorDistal=DivDistalPhasorLenght*cos(radians(an)); 
    DivyPhasorCloser=DivCloserPhasorLenght*sin(radians(an)); 
    DivyPhasorDistal=DivDistalPhasorLenght*sin(radians(an)); 
    if (Division==NumberOfScaleMajorDivisions/2|Division==0|Division==NumberOfScaleMajorDivisions) 
    { 
      strokeWeight(15); 
      stroke(0); 
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal); 
      strokeWeight(8); 
      stroke(100, 255, 100); 
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal); 
    } 
    else 
    { 
      strokeWeight(3); 
      stroke(255); 
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal); 
    } 
  } 
}

void ShowAngles() 
{ 
  textSize(15); 
  fill(#0A428E); 
  //noStroke();
  stroke(255, 255, 255);
  rect(-80, 190, 140, 25); 
  rect(80, 190, 140, 25); 
  fill(255); 
  //Pitch = Pitch / 5; 
  //Pitch = Pitch / 4.44; 
  int Pitch1 = round(Pitch);  
  //text("Pitch:  " + Pitch1 + " º", -20, 391, 500, 60); 
  text("Pitch:  " + Pitch + " º", -40, 210, 200, 60); 
  //text("Roll:  "  + round((Bank * 100)) + " º", 280, 391, 500, 60); 
  text("Roll:  "  + round(Bank) + " º", 130, 210, 200, 60); 
}

void Compass() 
{ 
  translate(300, 60);
  scale(.8);
  //scale(CompassMagnificationFactor); 
  noFill(); 
  stroke(255);
  strokeWeight(5);
  ellipse(0, 0, 275, 275);
  //--fill(40);
  stroke(100); 
  strokeWeight(30); 
  ellipse(0, 0, 245, 245); 
  strokeWeight(20); //50 
  stroke(50); 
  fill(0, 0, 40); 
  ellipse(0, 0, 203, 203); 
  for (int k=255;k>0;k=k-5) 
  { 
    noStroke(); 
    fill(0, 0, 255-k); 
    ellipse(0, 0, k*.7, k*.7); 
  } 
  //strokeWeight(20); 
  NumberOfScaleMajorDivisions=18; 
  NumberOfScaleMinorDivisions=36;  
  SpanAngle=180; 
  CircularScale(270); 
  rotate(PI); 
  SpanAngle=180; 
  CircularScale(270); 
  rotate(-PI); 
  fill(255); 
  textSize(24); 
  textAlign(CENTER); 
  text("W", -125, 0, 30, 30); 
  text("E", 125, 0, 30, 30); 
  text("N", 0, -125, 30, 30); 
  text("S", 0, 125, 30, 30); 
  textSize(18); 
  text("ΠΥΞΙΔΑ", 0, -40, 200, 80); 
  rotate(PI/4); 
  textSize(16); 
  text("NW", -125, 0, 50, 25); 
  text("SE", 125, 0, 50, 25); 
  text("NE", 0, -120, 50, 25); 
  text("SW", 0, 127, 50, 25); 
  rotate(-PI/4); 
  CompassPointer(); 
}

void CompassPointer() 
{ 
  rotate(PI + radians(Azimuth));  
  stroke(0); 
  strokeWeight(2); 
  //fill(100, 255, 100);
  fill(#DADB12);
  triangle(-8, -85, 8, -85, 0, 100); //Κύριος άξονας δείκτη
  triangle(-7, 70, 7, 70, 0, 95); //Μπροστά βελάκι
  ellipse(0, 0, 20, 20);         //Πίσω τριγωνάκι μαύρο
  fill(0, 0, 50); 
  noStroke(); 
  ellipse(0, 0, 3, 3); 
  triangle(-8, -85, 8, -85, 0, -75); 
  rotate(-PI - radians(Azimuth)); 
}

void ShowAzimuth() 
{ 
  fill(50); 
  noStroke(); 
  rect(7, 162, 190, 30); 
  int Azimuth1=round(Azimuth); 
  textAlign(CORNER); 
  textSize(20); 
  fill(255); 
  text("Azimuth:  " + Azimuth1 + " º", 20, 166, 190, 30); 
}