/*
 *
 * macroScope LED Controller Program
 * Version 0.8 - 05-16-14
 * (C) 2014 Zack Phillips - Duke University
 * zp15@duke.edu / zkphil@gmail.com
 * This code is subject to the MIT License included in the LICENSE file
 * Portions of this code are licensed under the GNU GPL v2
 *
 */

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "include/mongoose.h"
#include "include/RgbMatrix.h"
#include "include/GpioProxy.h"
#include "include/DisplayUpdater.h"
#include "include/RgbMatrixContainer.h"
#include "include/Thread.h"

GpioProxy io;
RgbMatrix * m = NULL;
RgbMatrixContainer *updater = NULL;

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  if (ev == MG_REQUEST) 
     {

     // Example: curl http://127.0.0.1:8000/clearAllLEDs
     if (strcmp(conn->uri, "/clearAllLEDs") == 0) 
     {
        printf("Called ClearAllLEDs\n");
        m->clearDisplay();
        mg_printf_data(conn, "%s", "SUCCESS\n");
     }

     // Example: curl --data "rVal=128&bVal=128&bVal=128" http://127.0.0.1:8000/fillScreen
     if (strcmp(conn->uri, "/fillScreen") == 0) 
     {
        printf("Called fillScreen\n");
        char buf[100] = "";
        mg_get_var(conn, "rVal", buf, sizeof(buf));  // Get form variable
        int rVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "gVal", buf, sizeof(buf));  // Get form variable
        int gVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "bVal", buf, sizeof(buf));  // Get form variable
        int bVal = atoi(buf);                        // Set new value

        Color myColor;
        myColor.red = 255 * (rVal == 1);
        myColor.green = 255 * (gVal == 1);
        myColor.blue = 255 * (bVal == 1);

        m->fillScreen(myColor);
        mg_printf_data(conn, "%s", "SUCCESS\n");
     }

     //Example: curl --data "xPos=1&yPos=2&rVal=3&gVal=4&bVal=5" http://127.0.0.1:8000/singleLEDOn
     else if (strcmp(conn->uri, "/singleLEDOn") == 0)
     {
        char buf[100] = "";
        mg_get_var(conn, "xPos", buf, sizeof(buf));  // Get form variable
        int xPos = atoi(buf);                        // Set new value

        mg_get_var(conn, "yPos", buf, sizeof(buf));  // Get form variable
        int yPos = atoi(buf);                        // Set new value

        mg_get_var(conn, "rVal", buf, sizeof(buf));  // Get form variable
        int rVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "gVal", buf, sizeof(buf));  // Get form variable
        int gVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "bVal", buf, sizeof(buf));  // Get form variable
        int bVal = atoi(buf);                        // Set new value

        Color myColor;
        myColor.red = 255*(rVal == 1);
        myColor.green = 255*(gVal == 1);
        myColor.blue = 255 *(bVal == 1);
     
        printf("Called singleLEDOn, x:%d, y:%d, r:%d, g:%d, b:%d\n",xPos,yPos,rVal,gVal,bVal);
        mg_printf_data(conn, "%s", "SUCCESS\n");
	
	m->drawPixel(xPos,yPos,myColor);
     }
     //Example: curl --data "xPos=1&yPos=2&rVal=1&gVal=1&gVal=1&text=test_text" http://127.0.0.1:8000/writeText
     else if (strcmp(conn->uri, "/writeText") == 0)
     {
        m->clearDisplay();
        char buf[100] = "";
        mg_get_var(conn, "xPos", buf, sizeof(buf));  // Get form variable
        int xPos = atoi(buf);                        // Set new value

        mg_get_var(conn, "yPos", buf, sizeof(buf));  // Get form variable
        int yPos = atoi(buf);                        // Set new value

        mg_get_var(conn, "rVal", buf, sizeof(buf));  // Get form variable
        int rVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "gVal", buf, sizeof(buf));  // Get form variable
        int gVal = atoi(buf);                        // Set new value

        mg_get_var(conn, "bVal", buf, sizeof(buf));  // Get form variable
        int bVal = atoi(buf);                        // Set new value

        char text[100];
        int charLength = mg_get_var(conn, "text", buf, sizeof(buf));  // Get form variable
        strcpy(text,buf);                            // Copy text to new variable

        Color myColor;
        myColor.red = 255*(rVal == 1);
        myColor.green = 255*(gVal ==1);
        myColor.blue = 255*(bVal==1);
        m->setFontColor(myColor);
        m->setTextCursor(xPos, yPos);
        m->setFontSize(2);
        m->setWordWrap(0);
        for (int itr=0;itr<charLength;itr++)
        {
           m->writeChar(text[itr]);
        }

        mg_printf_data(conn, "%s", "SUCCESS\n");

    }
    //Example: curl --data "10101111011111" http://127.0.0.1:8000/showImage
    else if (strcmp(conn->uri, "/showImage") == 0)
    {
       /* Each pixel is passed as a string of integers of the format:
        * XXYYRGB
        * XX: the x position (uint)
        * YY: the y position (uint)
        * R: whether to illuminate the red LED (bool)
        * G: whether to illuminate the green LED (bool)
        * B: whether to illuminate the blue LED (bool)
        */

       m->clearDisplay();

       int pxLength = 7; // Length of image pixels in text buffer - based on above format.

       char buf[25000] = "";
       int bufLength = mg_get_var(conn, "image", buf, sizeof(buf));  // Get form variable
       printf("Called showImage, image is: %s\n",buf);
       int bufStart = 0;

       if (!(bufLength%pxLength))
       {
          for (int i=0; i<floor(bufLength/pxLength); i++)
          {
              char tmp[7];
              bufStart = i*pxLength;
              strncpy ( tmp, buf+bufStart, 7);
              int xPos = 10*(tmp[0]-'0')+tmp[1]-'0';
              int yPos = 10*(tmp[2]-'0')+tmp[3]-'0';
              int pxRed = tmp[4]-'0';
              int pxGreen = tmp[5]-'0';
              int pxBlue = tmp[6]-'0';

              std::cout << "Found Pixel: xPos: " << xPos << ", yPos: " << yPos << " R: " << pxRed << " G: " << pxGreen << " B: " << pxBlue << std::endl;
              Color myColor;
	      myColor.red = 255*pxRed;
	      myColor.green = 255*pxGreen;
              myColor.blue = 255*pxBlue;
	      m->drawPixel(xPos,yPos,myColor);
          }
       mg_printf_data(conn, "%s", "SUCCESS\n");
       }
       else
       {
          std::cout << "ERROR string of wrong format."<<std::endl;
          mg_printf_data(conn, "%s", "Image string of the wrong format: image=XXYYRGBXXYYRGB etc.. XX=int:xPos, YY=int:yPos, R=bool:red, G=bool:green, B=bool:blue \n");
       }   

    } 
    else 
    {
       printf("invalid syntax\n");
       printf("Single Pixel: \n");
       mg_printf_data(conn, "%s", "FAIL\n");
    }
    return MG_TRUE;
  } 
  else if (ev == MG_AUTH) 
  {
     return MG_TRUE;
  }

  return MG_FALSE;
}

int main(void) {
	
  // Mongoose Loop
  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8000");

  // Initialize GPIO
  if (!io.initialize())
    return 1;

  // Create new instance of RgbMatrix class
  m = new RgbMatrix(&io);
  updater = new DisplayUpdater(m);
  updater->start(10);

  // Serve requests and update LED display. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  while(1)
  {
    mg_poll_server(server, 1);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  // Stop LED Controller threads and wait for them to join.
  if (updater) delete updater;

  // Clear and refresh the display.
  m->clearDisplay();
  m->updateDisplay();

  delete m;

  return 0;
}