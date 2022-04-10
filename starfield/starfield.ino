// Subject: ESP 8266 Starfield demo for SSD1306 OLED display
// Author: PLRANG ART
// URL: https://plrang.com
// Technology: C++ Arduino, Converted from JAVA to C++ and modified. NodeMCU board used.
// V: 1.0.1 prototype, working version: 27-05-2017
// "Forked" from Benny: https://github.com/BennyQBD
// DOCS: https://www.youtube.com/watch?v=v7nrzvd9A5c

//  MIT License
// Copyright (c) [2017-2018] [Plrang Art]
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// In this working draft, there are some leftovers from the pure C++ version which I created as first, using the SDL library.
// Then I slowly started disabling and removing stuff unnecessary for the ESP / Arduino version.
// It's still a prototype, unoptimized intentionally, for possible further enhancements. Yet it works pretty well.
// There are also my "thinking process" comments left and comments from Benny.

//include <iostream>
#include <vector>

// OLED
//include "SSD1306.h" // alias for `include "SSD1306Wire.h"`

// For a connection via SPI include
#include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Spi.h"

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
 SSD1306Spi        display(D0, D2, D8);

//Screen dimension constants
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

class Stars3D
{
    /** How much the stars are spread out in 3D space, on average. */
    float m_spread;

    /** How quickly the stars move towards the camera */
    float m_speed;

    /** The star positions on the X axis */
    std::vector<float> m_starX;

    /** The star positions on the Y axis */
    std::vector<float> m_starY;

    /** The star positions on the Z axis */
    std::vector<float> m_starZ;

    /**
       Creates a new 3D star field in a usable state.
       @param numStars The number of stars in the star field
       @param spread   How much the stars spread out, on average.
       @param speed    How quickly the stars move towards the camera
    */
    
    public:

    Stars3D(int numStars, float spread, float speed)
    {

      m_spread = spread;
      m_speed = speed;

      m_starX.resize(numStars);
      m_starY.resize(numStars);
      m_starZ.resize(numStars);

      //std::cout <<  "INIT CLASS size:" << m_starX.size()<< " vs "<< numStars<< std::endl;

      //for(int i = 0; i < m_starX.size(); i++)

      for (int i = 0; i < numStars; i++)
      {
        //std::cout <<  "INIT:" << i << std::endl;
        InitStar(i);
      }
    }

    /*
       Initializes a star to a new pseudo-random location in 3D space.
       @param i The index of the star to initialize.
    */

    void InitStar(int i)
    {
      //The random values have 0.5 subtracted from them and are multiplied
      //by 2 to remap them from the range (0, 1) to (-1, 1).

      // rand() / (RAND_MAX)

      m_starX[i] = 2 * ((float)((double)rand() / (double)RAND_MAX) - 0.5f) * m_spread;
      m_starY[i] = 2 * ((float)((double)rand() / (double)RAND_MAX) - 0.5f) * m_spread;

      //For Z, the random value is only adjusted by a small amount to stop
      //a star from being generated at 0 on Z.
      m_starZ[i] = ((float)((double)rand() / (double)RAND_MAX) + 0.00001f) * m_spread;

      //std::cout <<  "INIT m_starY[i]" << m_starY[i]  << std::endl;

    }


    void UpdateAndRender(float delta)
    {

      //Stars are drawn on a black background
      //target.Clear((byte)0x00);

      float halfWidth  = SCREEN_WIDTH / 2.0f;
      float halfHeight  = SCREEN_HEIGHT / 2.0f;

      //SDL_Rect r;
      int green_val;

      for (unsigned int i = 0; i < m_starX.size(); i++)
      {

        //Update the Star.

        //Move the star towards the camera which is at 0 on Z.
        m_starZ[i] -= delta * m_speed;

        //std::cout << "delta * m_speed " << delta * m_speed << "\n";

        //If a star is at or behind the camera, generate a new position for it
        
        if (m_starZ[i] <= 0)
          {
          InitStar(i);
          }

        //Render the Star.

        //Multiplying the position by (size/2) and then adding (size/2)
        //remaps the positions from range (-1, 1) to (0, size)
        int x = (int)((m_starX[i] / m_starZ[i]) * halfWidth + halfWidth);
        int y = (int)((m_starY[i] / m_starZ[i]) * halfHeight + halfHeight);

        //If the star is not within range of the screen, then generate a
        //new position for it.
        //if(x < 0 || x >= target.GetWidth() || (y < 0 || y >= target.GetHeight()))

        if (x < 0 || x >= SCREEN_WIDTH || (y < 0 || y >= SCREEN_HEIGHT))
          {
            InitStar(i);
          }
          else
          {
            //Otherwise, it is safe to draw this star to the screen.
            //target.DrawPixel(x, y, (byte)0xFF, (byte)0xFF, (byte)0xFF, (byte)0xFF);
  
            // SDL_RenderDrawPoint(target, x, y);
            display.setPixel(x,y);
  
            /*
            r.w = -m_starZ[i] * 0.04 + 4;
            r.h = -m_starZ[i] * 0.04 + 4;
  
            r.x = x;
            r.y = y;
            */
            
            //green_val = (r.w * 50) + 30;
  
            /*
            SDL_SetRenderDrawColor(target, 32 + r.w * 25, green_val, 32 + r.w * 25, 255);
            SDL_RenderFillRect( target, &r );
            */
  
          }
      }
    }
};


using namespace std; // no more std:: - but the best is to use it in a function, not globally

Stars3D stars {256, 64.0f, 20.0f};//4096 // 3500 without optimalization ESP8266

float delta = 0;
int green_val, red_val;


void setup() {
   
  Serial.begin(115200);
  // put your setup code here, to run once:
  display.init();

  // display.flipScreenVertically();
  display.setContrast(255);

  display.setFont(ArialMT_Plain_10);
    
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f-12-12, "STARFIELD");
  display.drawString(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f-12, "by PLRANG");
  display.drawString(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, "ESP8266");
  display.drawString(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f+12, "SSD1306");
  display.drawString(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f+12+12, "C++");
  display.display();
  Serial.println("STARFIELD");
  
  display.display();
  delay(6000);
}

void loop() {
  // put your main code here, to run repeatedly:

display.clear();

//for (long i = 0; i < 10000000; i += 1)
    //while(1)
//  {

    //    green_val = abs(255 - (i / 5 + 255) % 512) / 2;
    //    red_val = abs(255 - (i / 5 + 255) % 512) / 4;

    /*
      SDL_SetRenderDrawColor(renderer, red_val, 0, green_val, 255);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer,128,255,200,255);

      SDL_PumpEvents();
    */

    delta = 0.055;
    stars.UpdateAndRender(delta);

    /*
      SDL_RenderPresent(renderer);
      SDL_Delay( 10 );
    */

// }
 
display.display();
delay(10);

}
