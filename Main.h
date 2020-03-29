//存放camera和vector两个结构
#ifndef _MAIN_H
#define _MAIN_H

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "iostream"
#include "queue"
using namespace std;

#define CAMERASPEED	0.05			// The Camera Speed

#define SKYFRONT 0						// Give Front ID = 0
#define SKYBACK  1						// Give Back  ID = 1
#define SKYLEFT  2						// Give Left  ID = 2
#define SKYRIGHT 3						// Give Right ID = 3
#define SKYUP    4						// Give Up    ID = 4
#define SKYDOWN  5						// Give Down  ID = 5
#define		MAP_SIZE	  1024			// Size Of Our .RAW Height Map (NEW)
#define		STEP_SIZE	  8// Width And Height Of Each Quad (NEW)
#define		HEIGHT_RATIO  0.12f			// Ratio That The Y Is Scaled According To The X And Z (NEW)

typedef struct tVector3					// expanded 3D vector struct
{			
	tVector3() {}	// constructor
	tVector3 (float new_x, float new_y, float new_z) // initialize constructor	 
	{x = new_x; y = new_y; z = new_z;}
	// overload + operator so that we easier can add vectors
	tVector3 operator+(tVector3 vVector) {return tVector3(vVector.x+x, vVector.y+y, vVector.z+z);}
	// overload - operator that we easier can subtract vectors
	tVector3 operator-(tVector3 vVector) {return tVector3(x-vVector.x, y-vVector.y, z-vVector.z);}
	// overload * operator that we easier can multiply by scalars
	tVector3 operator*(float number)	 {return tVector3(x*number, y*number, z*number);}
	// overload / operator that we easier can divide by a scalar
	tVector3 operator/(float number)	 {return tVector3(x/number, y/number, z/number);}
	
	float x, y, z;						// 3D vector coordinates
}tVector3;

class CCamera 
{
	public:

		tVector3 mPos;	
		tVector3 mView;		
		tVector3 mUp;			

		int mWindowWidth;
		int mWindowHeight;

		void Mouse_Move();
		void Rotate_View(float x, float y, float z);
		void Move_Camera(float cameraspeed, int direction, int flag, int D, int S);
		void Move_Camera_up_down(float cameraspeed);
		void Position_Camera(float pos_x, float pos_y,float pos_z,
			 				 float view_x, float view_y, float view_z,
							 float up_x,   float up_y,   float up_z);
		void jump(float cameraspeed, int flag);
		int	Touch(float cameraspeed, float pos_x, float pos_z);
};

class Font
{
public:
	GLvoid BuildFont(GLvoid);						// 创建位图字体
	GLvoid KillFont(GLvoid);						// 删除显示列表
	GLvoid glPrint(float x,float y,const char *fmt, ...);					// 自定义GL输出字体函数

};

extern	HDC			hDC;				// Device context
extern	HGLRC		hRC;				// Permanent rendering context
extern	HWND		hWnd;				// Holds our window handle
extern	HINSTANCE	hInstance;			// Holds the instance of the application

void Keyboard_Input();
void Draw_Skybox(float x, float y, float z, float width, float height, float length);
void draw_things(float x, float y, float z, float width, float height, float length);
void load3dobj(char* dir,char* cn,int a);
#endif

