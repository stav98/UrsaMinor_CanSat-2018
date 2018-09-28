int zoom;
String longitude, latitude;
PImage google_maps;

float map_x;
float map_y;
boolean map_must_update = false;

void map_init()
{
 map_x = 738;
 map_y = 2;
 zoom = 19;
 longitude = "38.9524";
 latitude = "20.7438";
 google_maps = requestImage("https://maps.googleapis.com/maps/api/staticmap?size=240x240&zoom=5&maptype=satellite&markers=size:mid|color:red|38.9524,20.7438&key=AIzaSyAXT7Yv5vYAPKgR6UhkbNb1Hu7X28r8AaM", "png" );
}

void map_redraw()
{
 rectMode(CORNER);
 //stroke(0);
 //strokeWeight(3);
 //rect(map_x, map_y, 240, 240);
 image(google_maps, map_x, map_y);
 textSize(18); 
            fill(0);
           text(longitude + " - " + latitude, 860, 250);
 rectMode(CENTER);
  
 
}

void map_update()
{
 //if (map_must_update)
 //   {
     google_maps = requestImage("https://maps.googleapis.com/maps/api/staticmap?size=240x240&zoom="+zoom+"&maptype=satellite&markers=size:mid|color:red|"+longitude+","+latitude+"&key=AIzaSyAXT7Yv5vYAPKgR6UhkbNb1Hu7X28r8AaM", "png" );
     image(google_maps, map_x, map_y);
 //   }
}