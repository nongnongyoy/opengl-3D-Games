#include "Main.h"
#include "CLoad3DS.h"
#include <stdarg.h>			// 用来定义可变参数的头文件

#pragma comment(lib,"winmm.lib");


HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLfloat	cnt1;				// 字体移动计数器1
GLfloat	cnt2;				// 字体移动计数器2
int		modelnum = -1;
bool	start=FALSE;		//游戏开始标志
bool	end=FALSE;			//游戏结束标志
bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	flag=FALSE;
bool	jumping = FALSE;		//跳跃标志
bool	music_flag = TRUE;      //音乐开关标志
BYTE	g_HeightMap[MAP_SIZE*MAP_SIZE];					// Holds The Height Map Data (NEW)
bool	crash[8] =  {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};		//判断是否每一个模型都被碰到
bool	edge = FALSE;
float	scaleValue = 0.25f;								// Scale Value For The Terrain (NEW)


queue<float> q;					//用以保存行走时的点
queue<float> j;                 //用以保存跳跃时的点
GLuint texture, texture1;		//为地形分配纹理空间
UINT SkyboxTexture[6];			//为天空盒分配纹理空间

CCamera objCamera;				//声明相机对象
Font Cfont;
Font Cfont1;
Font Cfont2;

int     c = 0;										//帧数计数
float	wuqi =0.01f;								//雾气初始值
bool	f_wu =TRUE;                                 //雾气衰减模式
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// 雾气的模式
GLuint fogfilter= 0;								// 使用哪一种雾气
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};		// 雾的颜色设为白色


float  up = 0.0f;										 //气球上升的大小
CLoad3DS *gothicLoader=new(CLoad3DS);
t3DModel gothicModel;
CLoad3DS *gothicLoader1=new(CLoad3DS);
t3DModel gothicModel1;
CLoad3DS *gothicLoader2=new(CLoad3DS);
t3DModel gothicModel2;
CLoad3DS *gothicLoader3=new(CLoad3DS);
t3DModel gothicModel3;
CLoad3DS *gothicLoader4=new(CLoad3DS);
t3DModel gothicModel4;
CLoad3DS *gothicLoader5=new(CLoad3DS);
t3DModel gothicModel5;
CLoad3DS *gothicLoader6=new(CLoad3DS);
t3DModel gothicModel6;
CLoad3DS *gothicLoader7=new(CLoad3DS);
t3DModel gothicModel7;
CLoad3DS *gothicLoader8=new(CLoad3DS);
t3DModel gothicModel8;


float gothicTrans[10] = { 
	0, 0 , -30 ,     //表示在世界矩阵的位置
		0.3 , 0.3 , 0.3 ,      //表示xyz放大倍数
		0 , 0 , 0 , 0  //表示旋转
};


LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc


void CreateSkyTexture(UINT textureArray[], LPSTR filename, int id);

bool crash_true()
{
	for(int i=0;i<8;i++)
	{
		if(crash[i] == FALSE)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Loads The .RAW File And Stores It In pHeightMap
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.
	pFile = fopen( strName, "rb" );

	// Check To See If We Found The File And Could Open It
	if ( pFile == NULL )	
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, "Can't Find The Height Map!", "Error", MB_OK);
		return;
	}

	fread( pHeightMap, 1, nSize, pFile );

	// After We Read The Data, It's A Good Idea To Check If Everything Read Fine
	int result = ferror( pFile );

	// Check If We Received An Error
	if (result)
	{
		MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
	}

	// Close The File.
	fclose(pFile);
}


AUX_RGBImageRec *LoadBMP(char *Filename)	// 载入位图图象
{
	FILE *File=NULL;
	if (!Filename)								// 确保文件名已提供
	{
		return NULL;							// 如果没提供，返回 NULL
	}
	File=fopen(Filename,"r");
	if (File)								// 文件存在么?
	{
		fclose(File);							// 关闭句柄
		return auxDIBImageLoad(Filename);				// 载入位图并返回指针
	}
	return NULL;								// 如果载入失败，返回 NULL
}

int LoadGLTextures()	  // 载入位图(调用上面的代码)并转换成纹理
{
	int Status=FALSE;
	AUX_RGBImageRec *TextureImage[2];
	memset(TextureImage,0,sizeof(void *)*1);
	if (TextureImage[0]=LoadBMP("Data/texture/floor.bmp"))
	{
		Status=TRUE;
		glGenTextures(1, &texture);					// 创建纹理

		// 创建 MipMapped 纹理
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 

	}
	if (TextureImage[1]=LoadBMP("Data/texture/floor1.bmp"))
	{
		Status=TRUE;
		glGenTextures(1, &texture1);					// 创建纹理

		// 创建 MipMapped 纹理
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data); 

	}
	if (TextureImage[0])							// 纹理是否存在
	{
		if (TextureImage[0]->data)					// 纹理图像是否存在
		{
			free(TextureImage[0]->data);				// 释放纹理图像占用的内存
		}

		free(TextureImage[0]);						// 释放图像结构
	}
	if (TextureImage[1])							// 纹理是否存在
	{
		if (TextureImage[1]->data)					// 纹理图像是否存在
		{
			free(TextureImage[1]->data);				// 释放纹理图像占用的内存
		}

		free(TextureImage[1]);						// 释放图像结构
	}
	return Status;								// 返回 Status
}


GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	objCamera.mWindowWidth  = width;
																			
	gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);		//设置视口的大小

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	objCamera.mWindowWidth  = width;
	objCamera.mWindowHeight = height;

}

int Height(BYTE *pHeightMap, int X, int Y)				// This Returns The Height From A Height Map Index
{
	int x = X % MAP_SIZE;								// Error Check Our x Value
	int y = Y % MAP_SIZE;								// Error Check Our y Value

	if(!pHeightMap) return 0;							// Make Sure Our Data Is Valid

	return pHeightMap[x + (y * MAP_SIZE)];				// Index Into Our Height Array And Return The Height
}


int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glColor4f(1.0f,1.0f,1.0f,0.5f);						// 全亮度， 50% Alpha 混合
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// 基于源象素alpha通道值的半透明混合函数
   
	if (!LoadGLTextures())								// 调用纹理载入子例程
	{
		return FALSE;									// 如果未能载入，返回FALSE
	}
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glFogi(GL_FOG_MODE, fogMode[fogfilter]);		// 设置雾气的模式
	glFogfv(GL_FOG_COLOR, fogColor);				// 设置雾的颜色
	glFogf(GL_FOG_DENSITY, 0.0f);					// 设置雾的密度
	glHint(GL_FOG_HINT, GL_DONT_CARE);				// 设置系统如何计算雾气
	glFogf(GL_FOG_START, 0.0f);						// 雾气的开始位置
	glFogf(GL_FOG_END, 5.0f);						// 雾气的结束位置
	glEnable(GL_FOG);								// 使用雾气
	glFogf(GL_FOG_DENSITY,  wuqi);					// 设置雾的初始密度

	LoadRawFile("data/Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);
	
gothicLoader->Import3DS(&gothicModel, "data/3ds/GUTEMB_L.3DS");    //导入模型
	gothicLoader1->Import3DS(&gothicModel1, "data/3ds/house.3DS");    //导入模型
	gothicLoader2->Import3DS(&gothicModel2, "data/3ds/balloon.3DS");    //导入模型
	gothicLoader3->Import3DS(&gothicModel3, "data/3ds/balloon.3DS");    //导入模型
	gothicLoader4->Import3DS(&gothicModel4, "data/3ds/Loader.3DS");    //导入模型
	gothicLoader5->Import3DS(&gothicModel5, "data/3ds/M1.3DS");    //导入模型
	gothicLoader6->Import3DS(&gothicModel6, "data/3ds/直升机0.3DS");    //导入模型
	gothicLoader7->Import3DS(&gothicModel7, "data/3ds/townHouse.3DS");    //导入模型
	gothicLoader8->Import3DS(&gothicModel8, "data/3ds/轿车0.3DS");    //导入模型
	glEnable(GL_TEXTURE_2D);
	
	int h1 = Height(g_HeightMap, int(10/scaleValue), int(10/scaleValue));
	// 设置相机的位置
	objCamera.Position_Camera(10, h1*HEIGHT_RATIO + 1, 10,	20, h1*HEIGHT_RATIO + 1, 20, 10, 225, 10);
	
	// Load the Skybox textures
	CreateSkyTexture(SkyboxTexture,"data/texture/ft.bmp",  SKYFRONT);
	CreateSkyTexture(SkyboxTexture,"data/texture/bk.bmp",   SKYBACK);
	CreateSkyTexture(SkyboxTexture,"data/texture/lf.bmp",   SKYLEFT);
	CreateSkyTexture(SkyboxTexture,"data/texture/rt.bmp",  SKYRIGHT);
	CreateSkyTexture(SkyboxTexture,"data/texture/up.bmp",     SKYUP);
	CreateSkyTexture(SkyboxTexture,"data/texture/dn.bmp",   SKYDOWN);

    Cfont.BuildFont();	
	return TRUE;										// Initialization Went OK
}

														
void SetVertexColor(BYTE *pHeightMap, int x, int y)		// Sets The Color Value For A Particular Index, Depending On The Height Index
{
	if(!pHeightMap) return;								// Make Sure Our Height Data Is Valid

	float fColor = 2.0f + (Height(pHeightMap, x, y ) / 255.0f);

	// Assign This Blue Shade To The Current Vertex
	glColor3f(255., 255., fColor );
}

void RenderHeightMap(BYTE pHeightMap[])					
{
	int X = 0, Y = 0;									
	int x, y, z;										
	if(!pHeightMap) return;								

	glBindTexture(GL_TEXTURE_2D, texture);      //绑定纹理  //用于绑定纹理									
	glBegin( GL_QUADS );														
	for ( X = 0; X < 1024; X += STEP_SIZE )
		for ( Y = 0; Y < 1024 ; Y += STEP_SIZE )
		{
			// Get The (X, Y, Z) Value For The Bottom Left Vertex
			x = X;						 	
			y = Height(pHeightMap, X, Y );	
			z = Y;							
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3i(x, y, z);						

			// Get The (X, Y, Z) Value For The Top Left Vertex
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3i(x, y, z);
			

			// Get The (X, Y, Z) Value For The Top Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3i(x, y, z);
			

			// Get The (X, Y, Z) Value For The Bottom Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3i(x, y, z);						
		}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture1);      //绑定纹理  //用于绑定纹理									
	glBegin( GL_QUADS );														
	for ( X = 1024; X < 2048; X += STEP_SIZE )
		for ( Y = 0; Y < 1024 ; Y += STEP_SIZE )
		{
			// Get The (X, Y, Z) Value For The Bottom Left Vertex
			x = X;						 	
			y = Height(pHeightMap, X, Y );	
			z = Y;							
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3i(x, y, z);						

			// Get The (X, Y, Z) Value For The Top Left Vertex
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3i(x, y, z);
			

			// Get The (X, Y, Z) Value For The Top Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3i(x, y, z);
			

			// Get The (X, Y, Z) Value For The Bottom Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;
			SetVertexColor(pHeightMap, x, z);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3i(x, y, z);						
		}
	glEnd();
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);					// Reset The Color
}


int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(0.0f,0.0f,-1.0f);				// 移入屏幕一个单位
    // 根据字体位置设置颜色
	glColor3f(0.1f,0.4f,0.2f);
   
	if(end == FALSE)
	{
		if(start == TRUE)					//判断是否开始游戏
		{
			/*****************************/	 //字体函数的实现

			switch(modelnum){
	case 0:{Cfont2.glPrint(-0.9f,0.3f,"You were born here on March 8, 1944. You and your twin brother Johnnie grew up ");
		    Cfont2.glPrint(-0.9f,0.2f,"together. You were introverted and sickly from an early age.");
			Cfont2.glPrint(-0.9f,0.1f,"'We are brothers, brothers should take care of each other.' Johnnie wiped the tears");
			Cfont2.glPrint(-0.9f,0.0f,"from your eyes every time he finished.You love writing and dream of being a writer.");
			break;}
	case 2:{Cfont2.glPrint(-0.9f,0.3f,"This hot air balloon looks so familiar, you remember, this is where you first met her.");
		    Cfont2.glPrint(-0.9f,0.2f," At the bar mitzvah 14 years ago.'Hello, my name is Diana. Nice to meet you.'");
			Cfont2.glPrint(-0.9f,0.1f,"you see her red hair, this is the first time you have been so close to her");
            Cfont2.glPrint(-0.9f,0.0f,"her faint rose fragrance makes you breathless.");
			Cfont2.glPrint(-0.9f,-0.1f,"'Come on, today, today is your last chance. You'll never see her again.'");
			break;}
	case 1:{Cfont2.glPrint(-0.9f,0.3f,"May 13, 1971.  This black iron house is a prison for prisoners of war.");
	     	Cfont2.glPrint(-0.9f,0.2f,"'Did you hear that the war is almost over? Maybe in two weeks we'll be home again.'");
			Cfont2.glPrint(-0.9f,0.1f,"Said Johnnie excitedly.You take a puff in silence and nod.");
			Cfont2.glPrint(-0.9f,0.0f,"You know, thanks to this brilliant brother, you've been spared so many front lines.");
			Cfont2.glPrint(-0.9f,-0.1f,"'We are brothers, brothers should take care of each other.'");
		    break;}
	case 3:{Cfont2.glPrint(-0.9f,0.3f,"This is the reaper that your grandparents owned back home in the country,");
	     	Cfont2.glPrint(-0.9f,0.2f,"and it always reminds you of the way your grandmother held you and told you a story. .");
			Cfont2.glPrint(-0.9f,0.1f,"You often tell people that you had a wonderful childhood.");
		    break;}
	case 4:{Cfont2.glPrint(-0.9f,0.3f,"In the 1969 draft lottery, all men between the ages of 20 and 30were forced to serve in Vietnam.");
	     	Cfont2.glPrint(-0.9f,0.2f,"It is a ridiculous fate that those who once opposed the war now participate in it.");
			Cfont2.glPrint(-0.9f,0.1f,"Helicopters will be sent to the front you, you took this piece of land in southeast Asia");
			Cfont2.glPrint(-0.9f,0.0f,"With your back to a tearful Diana and a crying Hannah, you walk out the door and head for the battlefield.");
			Cfont2.glPrint(-0.9f,-0.1f,"It's a nightmare you can't wake up from.");
		    break;}
	case 5:{Cfont2.glPrint(-0.9f,0.3f,"In the 1969 draft lottery, all men between the ages of 20 and 30were forced to serve in Vietnam.");
	     	Cfont2.glPrint(-0.9f,0.2f,"It is a ridiculous fate that those who once opposed the war now participate in it.");
			Cfont2.glPrint(-0.9f,0.1f,"Helicopters will be sent to the front you, you took this piece of land in southeast Asia");
			Cfont2.glPrint(-0.9f,0.0f,"With your back to a tearful Diana and a crying Hannah, you walk out the door and head for the battlefield.");
			Cfont2.glPrint(-0.9f,-0.1f,"It's a nightmare you can't wake up from.");
		    break;}
    case 7:{Cfont2.glPrint(-0.9f,0.3f,"You recognize this house at once. It's your home. Your real home.");
	     	Cfont2.glPrint(-0.9f,0.2f,"On February 17, 1967, you married Diana in a big church ceremony, and soon after, Diana was pregnant.");
			Cfont2.glPrint(-0.9f,0.1f,"Life is moving into a whole new phase, as you would expect. You borrowed a lot of");
			Cfont2.glPrint(-0.9f,0.0f,"money to buy a house in the city, which you built brick by brick. ");
			Cfont2.glPrint(-0.9f,-0.1f,"You start working day and night on your draft, hoping to earn more money. ");
		    break;}
	case 6:{Cfont2.glPrint(-0.9f,0.3f,"You can't get out of the shadow of the war, the purgatory of the scene that flashes in front of your eyes");
	     	Cfont2.glPrint(-0.9f,0.2f,"You begin to numb yourself with alcohol and become violent, beating Diana and Hannah. ");
			Cfont2.glPrint(-0.9f,0.1f,"'Dad, are we going to buy birthday presents?'");
			Cfont2.glPrint(-0.9f,0.0f,"'Alex, you've been drinking too much. Look ahead.'");
			Cfont2.glPrint(-0.9f,-0.1f,"'Shut up, you bitch, I'm not drunk!'You say, putting your foot on the gas pedal...");
		    break;}
	}
			if(crash_true()==FALSE)
			{
				/*****************************/						//雾气渐变功能的实现
				if(wuqi>=0.11)								
				{
					f_wu = FALSE;
				}
				if(f_wu == FALSE)
				{
					if(wuqi<=0.1&&music_flag==TRUE)
					{	
						//mciSendString("stop start", NULL, 0, NULL);
						//mciSendString("open data/music/bgm1.mp3 alias start1", NULL, 0, NULL);
						//mciSendString("play start1 repeat", NULL, 0, NULL);
						music_flag=FALSE;
					}
					if(c%1==0)
					{
						if(wuqi>0.05)
						{
							wuqi -=0.0001;
							glFogf(GL_FOG_DENSITY,  0);			// 设置雾的密度
						}
						else
						{	
							f_wu = TRUE;
						}
					}
				}
				if(f_wu == TRUE)
				{
					if(c%1==0)
					{
						wuqi +=0.0001;
						glFogf(GL_FOG_DENSITY,  0);				// 设置雾的密度
					}
				}
			}

			if(crash_true()==TRUE)
			{
					glFogf(GL_FOG_DENSITY,  0);				// 设置雾的密度
					if(wuqi>0)
					{
						wuqi -= 0.0001;
					}
					if(wuqi <0)
					{
						wuqi =0;
					}
			}

			c++;

			/*****************************/
			if(jumping == TRUE)									//跳跃功能的实现
			{ 
				if(!j.empty())
				{
					float t1=j.front();
					j.pop();
					float t2=j.front();
					j.pop();
					float t3=j.front();
					j.pop();
					float h = Height(g_HeightMap, int((objCamera.mPos.x + t1)/scaleValue), int((objCamera.mPos.z + t3)/scaleValue))*HEIGHT_RATIO + 1.0;
					if((objCamera.mPos.y + t2)< h)
					{
						//t1=0;
						t2=(h - objCamera.mPos.y);
						//t3=0;
						while(!j.empty())
						{
							j.pop();
						}
					}
					objCamera.Position_Camera(objCamera.mPos.x + t1, objCamera.mPos.y + t2, objCamera.mPos.z + t3,	
											  objCamera.mView.x + t1, objCamera.mView.y + t2, objCamera.mView.z+ t3,
											  objCamera.mUp.x , objCamera.mUp.y , objCamera.mUp.z );
				}
				else if(j.empty() & objCamera.mPos.y > (Height(g_HeightMap, int(objCamera.mPos.x/scaleValue), int(objCamera.mPos.z/scaleValue))*HEIGHT_RATIO + 1.0))
				{
					tVector3 vVector2 = tVector3(0,0,0);		// init a new view vector
					vVector2 = objCamera.mView - objCamera.mPos;
					objCamera.Position_Camera(objCamera.mPos.x + vVector2.x*CAMERASPEED*0.25, objCamera.mPos.y - 0.1, objCamera.mPos.z + vVector2.z*CAMERASPEED*0.25,	
											  objCamera.mView.x + vVector2.x*CAMERASPEED*0.25, objCamera.mView.y - 0.1, objCamera.mView.z+ vVector2.z*CAMERASPEED*0.25,
											  objCamera.mUp.x , objCamera.mUp.y , objCamera.mUp.z );
				}
				else
				{
					jumping = FALSE;
				}
			}

			/*****************************/
			else												//行走功能的实现
			{	
				//new_hight = Height(g_HeightMap, objCamera.mPos.z, objCamera.mPos.z );
				if(!q.empty())
				{
					float t1=q.front();
					q.pop();
					float t2=q.front();
					q.pop();
					float t3=q.front();
					q.pop();
					objCamera.Position_Camera(objCamera.mPos.x + t1, objCamera.mPos.y + t2, objCamera.mPos.z + t3,	
											  objCamera.mView.x + t1, objCamera.mView.y + t2, objCamera.mView.z + t3,
											  objCamera.mUp.x , objCamera.mUp.y, objCamera.mUp.z);
				}
			}

		


			/*****************************/					//判断是否在边界附近
			if(edge == TRUE)
			{
				if(crash_true()==TRUE)
				{
					glFogf(GL_FOG_DENSITY,  wuqi);				// 设置雾的密度
					if(wuqi>0)
					{
						wuqi -= 0.001;
					}
					if(wuqi <0)
					{
						wuqi =0;
					}
					Cfont.glPrint(-0.40,0.2,"you succeed! press 'ecs' to quit...");		//输出文字到屏幕
				}
				else
				{
					Cfont.glPrint(-0.40,0.2,"you can not move forward, please go back!");		//输出初始文字到屏幕
				}
			}
		}
		else
		{
			 Cfont.glPrint(-0.40,0.2,"input 'enter' to begin...");		//输出初始文字到屏幕
		}
	}
	

	// use this function for opengl target camera
	gluLookAt(objCamera.mPos.x,  objCamera.mPos.y  ,  objCamera.mPos.z,//该函数是移动相机的位置，而glTranslatef()函数用以改变物体的位置，即绘制物体的中心
			  objCamera.mView.x , objCamera.mView.y , objCamera.mView.z,	
			  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);

	
	glPushMatrix();
	changeObject( gothicTrans );
	drawModel(gothicModel,g_HeightMap,true,false, 1000*scaleValue, 650*HEIGHT_RATIO, 1000*scaleValue, 1.5);
	drawModel(gothicModel1,g_HeightMap,true,false, 2200*scaleValue, 310*HEIGHT_RATIO, 2200*scaleValue, 0.5);
	drawModel(gothicModel2,g_HeightMap,true,false, 3300*scaleValue, 800*HEIGHT_RATIO + up, 3300*scaleValue, 5);
	drawModel(gothicModel3,g_HeightMap,true,false, 3000*scaleValue, 1000*HEIGHT_RATIO + up, 3000*scaleValue, 5);
	drawModel(gothicModel4,g_HeightMap,true,false, 520*scaleValue, 450*HEIGHT_RATIO, 2300*scaleValue, 0.15);
	drawModel(gothicModel5,g_HeightMap,true,false, 2000*scaleValue, 380*HEIGHT_RATIO, 3500*scaleValue, 0.2);
	drawModel(gothicModel6,g_HeightMap,true,false, 2000*scaleValue, 400*HEIGHT_RATIO, 3000*scaleValue, 0.5);
	drawModel(gothicModel7,g_HeightMap,true,false, 3800*scaleValue, 550*HEIGHT_RATIO, 2700*scaleValue, 0.02);
	drawModel(gothicModel8,g_HeightMap,true,false, 3800*scaleValue, 380*HEIGHT_RATIO, 2400*scaleValue, 1);
	glPopMatrix();
	up += 0.02f;


	//Cfont.glPrint("%f,%f",objCamera.mPos.x,objCamera.mPos.z);

	/*****************************/					//跳出边界后的情况雾逐渐消散
	if((objCamera.mPos.x<0 || objCamera.mPos.x >512 ||objCamera.mPos.z < 0 || objCamera.mPos.z > 256)&&(jumping==FALSE))
	{
		objCamera.mPos.y -= 0.01;
		objCamera.mView.y -= 0.01;
		glFogf(GL_FOG_DENSITY,  wuqi);				// 设置雾的密度
		if(wuqi>0)
		{
			wuqi -= 0.0001;
		}
		if(wuqi <0)
		{
			wuqi =0;
		}
		Cfont.glPrint(-0.40,0.2,"you failed! press 'ecs' to quit...");		//输出文字到屏幕
		end = TRUE;
	}

	Draw_Skybox(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,500,250,500);// Draw the Skybox
	
	glScalef(scaleValue, HEIGHT_RATIO, scaleValue);									// 缩放变换

	RenderHeightMap(g_HeightMap);													// Render The Height Map
	
	glFlush ();	

	return TRUE;																	// Keep Going
}



GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
	Cfont.KillFont();	
}


BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","OPENGL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}



LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:

		  switch(wParam) 
		  {		  
			case VK_ESCAPE:
				PostQuitMessage(0);						
				break;
		  }
		  break;
		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("APRON TUTORIALS",1920, 1080, 16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}
	char buf[50];
	MCIERROR mciError;
	mciSendString("open data/music/bgm1.mp3 alias start", NULL, 0, NULL);
	mciSendString("play start repeat", NULL, 0, NULL);
	
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// 翻译消息
				DispatchMessage(&msg);				// 发送消息
			}
		}
		else										// If There Are No Messages
		{	
			//键盘交互
			if (GetKeyState(VK_RETURN) & 0x80 )				//回车键游戏开始		
			{
				start = TRUE;
			}
			if(start == TRUE && end == FALSE)
			{
				if (GetKeyState(VK_SPACE) & 0x80 & GetKeyState('W') & 0x80)	//空格键跳跃		
				{
					if(jumping == FALSE)
					{
						objCamera.jump(CAMERASPEED, 1);
					}
				}
				else if ((GetKeyState(VK_SPACE) & 0x80))					//空格键跳跃		
				{
					if(jumping == FALSE)
					{
						objCamera.jump(CAMERASPEED, 0);
					}
				}
				if((GetKeyState('A') & 0x80 & GetKeyState('W') & 0x80))
				{	
					objCamera.Move_Camera(CAMERASPEED , 0, 1, 1, 1);
				}
				else if(GetKeyState('D') & 0x80 & GetKeyState('W') & 0x80)
				{
					objCamera.Move_Camera(-CAMERASPEED , 0, 1, -1, 1);
				}
				else if((GetKeyState('A') & 0x80 & GetKeyState('S') & 0x80))
				{	
					objCamera.Move_Camera(-CAMERASPEED , 0, 1, 1, -1);
				}
				else if(GetKeyState('D') & 0x80 & GetKeyState('S') & 0x80)
				{
					objCamera.Move_Camera(CAMERASPEED , 0, 1, -1, -1);
				}
				else if((GetKeyState(VK_LSHIFT) & 0x80) & (GetKeyState('W') & 0x80))   //shift键加速
				{	
					objCamera.Move_Camera(CAMERASPEED + 0.01, 0, 0, 1, 1);
				}
				else if((GetKeyState('W') & 0x80))										//0x80是指按键是否松开
				{	
					objCamera.Move_Camera(CAMERASPEED, 0, 0, 1, 1);
				}
				else if((GetKeyState(VK_LSHIFT) & 0x80) & (GetKeyState('S') & 0x80))   //shift键加速
				{	
					objCamera.Move_Camera(-CAMERASPEED - 0.01, 0, 0, 1, 1);
				}
				else if((GetKeyState('S') & 0x80))
				{
					objCamera.Move_Camera(-CAMERASPEED, 0, 0, 1, 1);
				}

				else if((GetKeyState('A') & 0x80))
				{	
					objCamera.Move_Camera(CAMERASPEED , 1, 0, 1, 1);
				}

				else if((GetKeyState('D') & 0x80))
				{
					objCamera.Move_Camera(-CAMERASPEED , 1, 0, 1, 1);
				}
				if(GetKeyState(VK_PRIOR)& 0x80)
				{
					objCamera.Move_Camera_up_down(-CAMERASPEED);
				}
				if(GetKeyState(VK_NEXT)& 0x80)
				{
					objCamera.Move_Camera_up_down(CAMERASPEED);
				}
			}

			//鼠标交互
			objCamera.Mouse_Move();

			//绘制场景
			if (active )	// Active?  Was There A Quit Received?
			{
				if(keys[VK_ESCAPE])
				{
					done=TRUE;							// ESC or DrawGLScene Signalled A Quit
				}
			
				else									// Not Time To Quit, Update Screen
				{
					DrawGLScene();
					SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("APRON TUTORIALS",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}
	
	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}



void Draw_Skybox(float x, float y, float z, float width, float height, float length) //传入的x，y，z代表skybox的中心
{
	// Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;


	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYFRONT]);      //绑定纹理
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);    
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYBACK]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYLEFT]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYRIGHT]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYUP]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length);           //右里
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);    //右外
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);    //左外
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height,	z);           //左里
	glEnd();

	// Draw Down side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYDOWN]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

}

void CreateSkyTexture(UINT textureArray[], LPSTR filename, int id)
{

	_AUX_RGBImageRec *Image;				

    if(Image = auxDIBImageLoadA( (const char*) filename ))
	{
		glGenTextures(1, &textureArray[id]);

		glBindTexture(GL_TEXTURE_2D, textureArray[id]);    //创建mipmapped纹理
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,
                              Image->sizeY, GL_RGB,
                              GL_UNSIGNED_BYTE, Image->data);
	}
  	if(Image)
	{										
		if (Image->data)
			delete Image->data;			
		delete Image;
	}
}


