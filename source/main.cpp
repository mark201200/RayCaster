/*
Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <3ds.h>
#include <cstring>
#include <cstdio>
#include "brew_bgr.h"
#define screenWidth 399
#define screenHeight 240
#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24
using namespace std;

int worldMap[mapWidth][mapHeight]=
{
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,1},
  {6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,6},
  {4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,3},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y)
{
	if(!spriteData)return;

	u16 fbWidth, fbHeight;
	u8* fbAdr=gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

	if(x+width<0 || x>=fbWidth)return;
	if(y+height<0 || y>=fbHeight)return;

	u16 xOffset=0, yOffset=0;
	u16 widthDrawn=width, heightDrawn=height;

	if(x<0)xOffset=-x;
	if(y<0)yOffset=-y;
	if(x+width>=fbWidth)widthDrawn=fbWidth-x;
	if(y+height>=fbHeight)heightDrawn=fbHeight-y;
	widthDrawn-=xOffset;
	heightDrawn-=yOffset;

	int j;
	for(j=yOffset; j<yOffset+heightDrawn; j++)
	{
		memcpy(&fbAdr[((x+xOffset)+(y+j)*fbWidth)*3], &spriteData[((xOffset)+(j)*width)*3], widthDrawn*3);
	}
}

void setPixel(gfxScreen_t screen, gfx3dSide_t side, u16 x, u8 ys ,u8 yf, u8 red, u8 green, u8 blue) {
    u8* fb=gfxGetFramebuffer(screen, side, NULL,NULL);
    for(int j=ys;j<yf;j++){
	fb[3 * (240 - j + (x - 1) * 240)] = blue;
	fb[3 * (240 - j + (x - 1) * 240) + 1] = green;
	fb[3 * (240 - j + (x - 1) * 240) + 2] = red;
}
}

void setSinglePixel(gfxScreen_t screen, gfx3dSide_t side, u16 x, u8 y, u8 red, u8 green, u8 blue) {
    u8* fb=gfxGetFramebuffer(screen, side, NULL,NULL);
	fb[3 * (240 - y + (x - 1) * 240)] = blue;
	fb[3 * (240 - y + (x - 1) * 240) + 1] = green;
	fb[3 * (240 - y + (x - 1) * 240) + 2] = red;
}

void setSinglePixelTest(gfxScreen_t screen, gfx3dSide_t side, u16 x, u8 y,u32 color) {
    u8* fb=gfxGetFramebuffer(screen, side, NULL,NULL);
	fb[3 * (240 - y + (x - 1) * 240)] = color;
}


void drawRectangle(gfxScreen_t screen, gfx3dSide_t side, u16 x1, u8 y1, u16 x2, u8 y2, u8 red, u8 green,
		u8 blue) {
		     u8* fb=gfxGetFramebuffer(screen, side, NULL,NULL);
	u16 y;
	while (x1 <= x2) {
		y = y1;
		while (y <= y2) {
			setSinglePixel(screen,side, x1, y, red, green, blue);
			y++;
		}
		x1++;
	}
}

void clrScreen(gfxScreen_t screen, gfx3dSide_t side) {
    u8* fb=gfxGetFramebuffer(screen, side, NULL,NULL);
	memset(fb, 0, 240 * 400 * 3);
}

void levelEditor(gfxScreen_t screen, gfx3dSide_t side){
    clrScreen(screen,side);
    int i,j;
    bool editing=true;
int lastx=10;
int lasty=10;
int selectedx=3;
int selectedy=3;
cout<<"Welcome to the level editor!\n Use the d-pad to select the block you want to edit."<<endl;
while(editing){
        int lastx=10;
int lasty=10;
        hidScanInput();
u32 kDown = hidKeysDown();
if(kDown & KEY_START) editing=false;
if(kDown & KEY_DUP) selectedy--;
if(kDown & KEY_DDOWN) selectedy++;
if(kDown & KEY_DLEFT) selectedx--;
if(kDown & KEY_DRIGHT) selectedx++;
if(selectedy>mapHeight) selectedy=mapHeight;
if(selectedx>mapWidth) selectedx=mapWidth;
if(selectedy<0) selectedy=0;
if(selectedx<0) selectedx=0;
if(kDown & KEY_A){
    worldMap[selectedx][selectedy]++;
    if(worldMap[selectedx][selectedy]>5) worldMap[selectedx][selectedy]=0;

}



 for(i=0;i<mapHeight;i++){
lastx=10;
    for(j=0;j<mapWidth;j++){

        switch(worldMap[j][i]){
    case 1:
        {
            drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,0,255,0);
            if(i==selectedy&&j==selectedx){
                //se selezionato
                drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
        }
         case 2:
        {
            drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,155,155,155);
            if(i==selectedy&&j==selectedx){
                //se selezionato
                drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
        }

         case 3:
        {
            drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,0,0,255);
            if(i==selectedy&&j==selectedx){
                //se selezionato
                drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
        }

         case 4:
        {
            drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,255,0,0);
            if(i==selectedy&&j==selectedx){
                //se selezionato
                drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
        }

         case 0:
        {
            drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,47,79,79);
            if(i==selectedy&&j==selectedx){
                //se selezionato
                drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
        }

         default:
            {
                drawRectangle(screen,side,lastx,lasty,lastx+6,lasty+6,155,55,240);
                if(i==selectedy&&j==selectedx){
             //se selezionato
             drawRectangle(screen,side,lastx+2,lasty+2,lastx+4,lasty+4,0,0,0);
            }

            break;
            }

        }

lastx=lastx+8;
    }
lasty=lasty+8;
 }

 }




}

void welcomeScreen(gfxScreen_t screen, gfx3dSide_t side){
    bool stayIn=true;

    gfxDrawSprite(screen,side,(u8*)brew_bgr,240,400,0,0);
while(stayIn){

hidScanInput();
u32 kDown = hidKeysDown();
if (kDown & KEY_A) stayIn=false;
if (kDown & KEY_B) {
        stayIn=false;
        levelEditor(screen,side);
}
}
}
u32 buffer[screenHeight][screenWidth];
int drawStart,drawEnd;
u32 color;
int main(int /*argc*/, char */*argv*/[])
{
  gfxInitDefault();
  PrintConsole bs;
  consoleInit(GFX_BOTTOM, &bs);
  consoleSelect(&bs);
  double posX = 22, posY = 12;  //x and y start position
  double dirX = -1, dirY = 0; //initial direction vector
  double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane
  vector<u32> texture[8];
  for(int i = 0; i < 8; i++) texture[i].resize(texWidth * texHeight);
  //generate some textures
  for(int x = 0; x < texWidth; x++)
  for(int y = 0; y < texHeight; y++)
  {
    int xorcolor = (x * 256 / texWidth) ^ (y * 256 / texHeight);
    //int xcolor = x * 256 / texWidth;
    int ycolor = y * 256 / texHeight;
    int xycolor = y * 128 / texHeight + x * 128 / texWidth;
    texture[0][texWidth * y + x] = 65536 * 254 * (x != y && x != texWidth - y); //flat red texture with black cross
    texture[1][texWidth * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
    texture[2][texWidth * y + x] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
    texture[3][texWidth * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
    texture[4][texWidth * y + x] = 256 * xorcolor; //xor green
    texture[5][texWidth * y + x] = 65536 * 192 * (x % 16 && y % 16); //red bricks
    texture[6][texWidth * y + x] = 65536 * ycolor; //red gradient
    texture[7][texWidth * y + x] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
  }
  int w=399;
  int h=240;
  int r,g,b;
  int x,y;
  bool welcomed=false;
  while(aptMainLoop())
  {

      if(!welcomed){
          gfxSetDoubleBuffering(GFX_TOP, false);
          welcomeScreen(GFX_TOP,GFX_LEFT);
          welcomed=true;
      }
      if(welcomed) gfxSetDoubleBuffering(GFX_TOP, true);

    //drawRectangle(GFX_TOP,GFX_LEFT,1,1,399,120,0,127,255);
   // drawRectangle(GFX_TOP,GFX_LEFT,1,120,399,240,101,67,33);
    for(x = 1; x < w; x++)
    {
      //calculate ray position and direction
      double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
      double rayPosX = posX;
      double rayPosY = posY;
      double rayDirX = dirX + planeX * cameraX;
      double rayDirY = dirY + planeY * cameraX;
      //which box of the map we're in
      int mapX = int(rayPosX);
      int mapY = int(rayPosY);

      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

       //length of ray from one x or y-side to next x or y-side
      double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
      double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?
      //calculate step and initial sideDist
      if (rayDirX < 0)
      {
        stepX = -1;
        sideDistX = (rayPosX - mapX) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (rayPosY - mapY) * deltaDistY;
      }
      else
      {
        stepY = 1;
        sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
      }
      //perform DDA
      while (hit == 0)
      {
        //jump to next map square, OR in x-direction, OR in y-direction
        if (sideDistX < sideDistY)
        {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else
        {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        //Check if ray has hit a wall
        if (worldMap[mapX][mapY] > 0) hit = 1;
      }
      //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
      if (side == 0) perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
      else           perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;

      //Calculate height of line to draw on screen
      int lineHeight = (int)(h / perpWallDist);

      //calculate lowest and highest pixel to fill in current stripe
      drawStart = -lineHeight / 2 + h / 2;
      if(drawStart < 0)drawStart = 0;
      drawEnd = lineHeight / 2 + h / 2;
      if(drawEnd >= h)drawEnd = h - 1;

      //choose wall color

      //texturing calculations
      int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

      //calculate value of wallX
      double wallX; //where exactly the wall was hit
      if (side == 0) wallX = rayPosY + perpWallDist * rayDirY;
      else           wallX = rayPosX + perpWallDist * rayDirX;
      wallX -= floor((wallX));

      //x coordinate on the texture
      int texX = int(wallX * double(texWidth));
      if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
      if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

      /*
       switch(worldMap[mapX][mapY])
      {
       case 1:
	{
		r = 0;
		g = 255;
		b = 0;
		break;
	}
	case 2:
	{
		r = 155;
		g = 155;
		b = 155;
		break;
	}
	case 3:
	{
		r = 0;
		g = 0;
		b = 255;
		break;
	}
	case 4:
	{
		r = 255;
		g = 0;
		b = 0;
		break;
	}
	default:
	{
		r = 155;
		g = 55;
		b = 240;
		break;
	}
      }

      //give x and y sides different brightness
      if (side == 1) {
            r=r/2;
            g=g/2;
            b=b/2;
      }
      */

      for(y = drawStart; y < drawEnd; y++)
      {
        int d = y * 256 - h * 128 + lineHeight * 128;  //256 and 128 factors to avoid floats
        int texY = ((d * texHeight) / lineHeight) / 256;
        color = texture[texNum][texHeight * texY + texX];//TODO:FIDDLE WITH THIS, ONLY BLUE TEXs WORK
        //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
        if(side == 1) color = (color >> 1) & 8355711;
        buffer[y][x] = color;
        color = color<<0;
        // cout<<color<<endl;
        setSinglePixelTest(GFX_TOP,GFX_LEFT,x,y,color);
      }
      setPixel(GFX_TOP, GFX_LEFT ,x, 1, drawStart, 0,127,255);
      setPixel(GFX_TOP, GFX_LEFT ,x, drawEnd, 239, 101,67,33);

    }
    clrScreen(GFX_TOP,GFX_LEFT);//netti apposto questo e vblank

gspWaitForVBlank();

      //draw the pixels of the stripe as a vertical line

      /*
      setPixel(GFX_TOP, GFX_LEFT ,x, drawStart, drawEnd, r,g,b);
      */




    //speed modifiers
    double moveSpeed = 0.005 * 5.0; //the constant value is in squares/second
    double rotSpeed = 0.005 * 3.0; //the constant value is in radians/second
    hidScanInput();
u32 kDown = hidKeysDown();
u32 held = hidKeysHeld();
    //move forward if no wall in front of you
    if (kDown & KEY_START)
			break;
    if (kDown & KEY_DUP||held & KEY_DUP)
    {
      if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
      if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
    }
    //move backwards if no wall behind you
    if (kDown & KEY_DDOWN||held & KEY_DDOWN)
    {
      if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
      if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
    }
    //rotate to the right
    if (kDown & KEY_DRIGHT||held & KEY_DRIGHT)
    {
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
      dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    //rotate to the left
    if (kDown & KEY_DLEFT||held & KEY_DLEFT)
    {
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
      dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
      planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }

    gfxFlushBuffers();
    gfxSwapBuffers();
  }
  gfxExit();
  return 0;
}
