PImage snapshot;
byte snapState = 0;
byte photoState = 0;
byte pnum = 0;

void snap_init()
{
  snapshot = loadImage("camera_S.jpg"); 
}

void show_snap()
{
 int snapx = 245, snapy = 1;
 scale(1.8);
 image(snapshot, snapx, snapy);
 translate(snapx + snapshot.width, snapy + snapshot.height);
 rotate(PI);
 image(snapshot, 0, 0);
}

void chk_snap_state()
{
 if (snapState > 0)
    {
     switch (snapState)
            {
             case 2:
                  img_data = "";
                  port.write("txpic\r");
                  snapState = 3;
                  break;
             case 4:
                  byte[] decodedBytes = Base64.decodeBase64(img_data);
                  saveBytes("input/snapshot.jpg", decodedBytes);
                  snapshot = loadImage("input/snapshot.jpg");
                  snapState = 0;
                  break;
            }
    }
}

void chk_photo_state()
{
 if (photoState > 0)
    {
     if (photoState == 2)
        {
         byte[] decodedBytes = Base64.decodeBase64(img_data);
         saveBytes("input/IMG_"+pnum+".jpg", decodedBytes);
         photoState = 0;
         pnum++;
        }
    }
}