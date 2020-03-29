#include "Main.h"

extern BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];
extern float scaleValue;
extern queue<float> q;
extern queue<float> j;
extern bool	jumping;
extern bool	crash[8];
extern bool	edge;
extern int modelnum;
int	model2;

void CCamera::Position_Camera(float pos_x,  float pos_y,  float pos_z,			//直接设定相机的位置      
							  float view_x, float view_y, float view_z,
							  float up_x,   float up_y,   float up_z)
{
	tVector3 vPos	= tVector3(pos_x,  pos_y,  pos_z);
	tVector3 vView	= tVector3(view_x, view_y, view_z);
	tVector3 vUp	= tVector3(up_x,   up_y,   up_z);
	mPos  = vPos;																
	mView = vView;																
	mUp   = vUp;														
}


int getHeight(BYTE *pHeightMap, int X, int Y)			// 从高度图读取高度信息
{
	int x = X % MAP_SIZE;								
	int y = Y % MAP_SIZE;								

	if(!pHeightMap) return 0;							

	return pHeightMap[x + (y * MAP_SIZE)];				
}


void CCamera::Move_Camera(float cameraspeed, int direction, int flag, int D, int S)	//键盘输入w，s，则朝着视角的方向移动相机的位置，改变pos和view，up不变
{
	modelnum = -1;
	if(jumping == FALSE)						//在jump时，不能控制行走
	{
		float y = mPos.y;
		if(!g_HeightMap) return;
		tVector3 vVector = tVector3(0,0,0);		
		vVector = mView - mPos;					
		if(flag == 1)							//flag指是否有两个方向键同时按住
		{
			if((mPos.x + vVector.z * cameraspeed + vVector.x * cameraspeed) < 1 || (mPos.z - vVector.x * cameraspeed + vVector.z * cameraspeed*D) < 1)
			{
				edge = TRUE;
				return;
			}
			float h = int(getHeight(g_HeightMap, int((mPos.x  + vVector.z * cameraspeed + vVector.x * cameraspeed *D)/scaleValue), int((mPos.z  - vVector.x * cameraspeed + vVector.z * cameraspeed*D)/scaleValue)))*HEIGHT_RATIO + 1.0;
			float a =  vVector.z * cameraspeed * S + vVector.x * cameraspeed * D;
			float b = (h - y);
			int i,j;
			if(b == 0)
			{
				if(vVector.x < 0)
				{
					 i = -1;
				}
				else
				{
					 i = 1;
				}
				if(vVector.z < 0)
				{
					 j = -1;
				}
				else
				{
					 j = 1;
				}
				float h1 = int(getHeight(g_HeightMap, int((mPos.x  + i)/scaleValue), int((mPos.z  + j)/scaleValue)))*HEIGHT_RATIO + 1.0;
				b = (h1 - y)/4 ;
			}
			
			float c =  -vVector.x * cameraspeed * S + vVector.z * cameraspeed * D;
			if(!Touch(cameraspeed, mPos.x + a, mPos.z + c)==0)
			{
				return;
			}
			if(b<3)
			{
				edge = FALSE;
				q.push(a/4);
				q.push(b/4);
				q.push(c/4);
			}
		}
		else if(direction == 1)					//direction判断是否单独A或者D按下
		{
			if(mPos.x + vVector.x * cameraspeed < 1 ||mPos.x + vVector.x * cameraspeed > 512|| mPos.z + vVector.z * cameraspeed < 1 ||mPos.z + vVector.z * cameraspeed >256)
			{
				edge = TRUE;
				return;
			}
			float h = int(getHeight(g_HeightMap, int((mPos.x  + vVector.z * cameraspeed)/scaleValue), int((mPos.z  - vVector.x * cameraspeed)/scaleValue)))*HEIGHT_RATIO + 1.0;
			float a =  vVector.z * cameraspeed;
			float b = (h - y);
			int i,j;
			if(b == 0)
			{
				if(vVector.x < 0)
				{
					 i = -1;
				}
				else
				{
					 i = 1;
				}
				if(vVector.z < 0)
				{
					 j = -1;
				}
				else
				{
					 j = 1;
				}
				float h1 = int(getHeight(g_HeightMap, int((mPos.x  + i)/scaleValue), int((mPos.z  + j)/scaleValue)))*HEIGHT_RATIO + 1.0;
				b = (h1 - y)/4 ;
			}
			
			float c = - vVector.x * cameraspeed;
			if(!Touch(cameraspeed, mPos.x + a, mPos.z + c)==0)
			{
				return;
			}
			if(b<3)
			{
				edge = FALSE;
				q.push(a/4);
				q.push(b/4);
				q.push(c/4);
			}
		}
		else
		{
			if(mPos.x + vVector.x * cameraspeed < 1 ||mPos.x + vVector.x * cameraspeed > 512|| mPos.z + vVector.z * cameraspeed < 1 ||mPos.z + vVector.z * cameraspeed >256)
			{
				edge = TRUE;
				return;
			}
			float h = int(getHeight(g_HeightMap, int((mPos.x  + vVector.x * cameraspeed)/scaleValue), int((mPos.z  + vVector.z * cameraspeed)/scaleValue)))*HEIGHT_RATIO + 1.0;
			float a =  vVector.x * cameraspeed;
			float b = (h - y);
			int i,j;
			if(b == 0)
			{
				if(vVector.x < 0)
				{
					 i = -1;
				}
				else
				{
					 i = 1;
				}
				if(vVector.z < 0)
				{
					 j = -1;
				}
				else
				{
					 j = 1;
				}
				float h1 = int(getHeight(g_HeightMap, int((mPos.x  + i)/scaleValue), int((mPos.z  + j)/scaleValue)))*HEIGHT_RATIO + 1.0;
				b = (h1 - y)/4 ;
			}
			
			float c = vVector.z * cameraspeed;
			if(!Touch(cameraspeed, mPos.x + a, mPos.z + c)==0)
			{
				return;
			}
			if(b<3)
			{
				edge = FALSE;
				q.push(a/4);
				q.push(b/4);
				q.push(c/4);
			}
		}
	}
}


void CCamera::Move_Camera_up_down(float cameraspeed)		//键盘输入page_up  down，则在垂直方向上移动相机的位置，改变pos和view，up不变
{
	tVector3 vVector = tVector3(0,0,0);		
	vVector = mView - mPos;					
	
	// forward positive cameraspeed and backward negative -cameraspeed.
	mPos.y  = mPos.y  + vVector.y * cameraspeed;
	mView.y = mView.y + vVector.y * cameraspeed;
}


void CCamera::Rotate_View(float x, float y, float z)		//鼠标旋转时，改变view的辅助函数
{
	tVector3 vVector = mView - mPos;

	if(x) 
	{
		mView.z = (float)(mPos.z + sin(x)*vVector.y + cos(x)*vVector.z);
		mView.y = (float)(mPos.y + cos(x)*vVector.y - sin(x)*vVector.z);
	}
	if(y) 
	{
		mView.z = (float)(mPos.z + sin(y)*vVector.x + cos(y)*vVector.z);
		mView.x = (float)(mPos.x + cos(y)*vVector.x - sin(y)*vVector.z);
	}
	if(z) 
	{
		mView.x = (float)(mPos.x + sin(z)*vVector.y + cos(z)*vVector.x);		
		mView.y = (float)(mPos.y + cos(z)*vVector.y - sin(z)*vVector.x);
	}
}


void CCamera::Mouse_Move()								//鼠标移动时改变view，pos和up都不变
{
	POINT mousePos;	
	int mid_x = mWindowWidth  >> 1;		
	int mid_y = mWindowHeight >> 1;	
	float angle_y  = 0.0f;				
	float angle_z  = 0.0f;							
	
	GetCursorPos(&mousePos);			// Get the 2D mouse cursor (x,y) position					
	
	if( (mousePos.x == mid_x) && (mousePos.y == mid_y) ) return;

	SetCursorPos(mid_x, mid_y);			// Set the mouse cursor in the middle of the window						

	// Get the direction from the mouse, and bring the number down to a reasonable amount
	angle_y = (float)( (mid_x - mousePos.x) ) / 1000;		
	angle_z = (float)( (mid_y - mousePos.y) ) / 1000;

	// The higher the number(acceleration) is the faster the camera looks around.
	mView.y += angle_z * 4;

	// limit the rotation around the x-axis
	if((mView.y - mPos.y) > 8)  mView.y = mPos.y + 8;
	if((mView.y - mPos.y) <-8)  mView.y = mPos.y - 8;
	
	Rotate_View(0, -angle_y, 0);		
}

void CCamera::jump(float cameraspeed, int flag)
{
	jumping = TRUE;
	if(flag==0)                           //flag==0标志原地跳跃
	{
		for(int i=0; i<35; i++)
		{
			j.push(0);
			j.push(0.1);
			j.push(0);
		}
		for(int k=0; k<0; k++)
		{
			j.push(0);
			j.push(0);
			j.push(0);
		}
		for(int m=0; m<35; m++)
		{
			j.push(0);
			j.push(-0.1);
			j.push(0);
		}
	}
	if(flag==1)								//flag==1标志向前跳跃
	{
		if(!g_HeightMap) return;
		tVector3 vVector1 = tVector3(0,0,0);		
		vVector1 = mView - mPos;
		for(int i=0; i<35; i++)
		{
			j.push(vVector1.x*cameraspeed*0.25);
			j.push(0.1);
			j.push(vVector1.z*cameraspeed*0.25);
		}
		for(int k=0; k<0; k++)
		{
			j.push(0);
			j.push(0);
			j.push(0);
		}
		for(int m=0; m<35; m++)
		{
			j.push(vVector1.x*cameraspeed*0.25);
			j.push(-0.1);
			j.push(vVector1.z*cameraspeed*0.25);
		}
	}
}


int CCamera::Touch(float cameraspeed, float pos_x, float pos_z)
{
	
	int nvert[8] = {11, 6, 4, 4, 5, 4, 4, 8};
	float vertx[8][11] = {{61, 74, 75, 80, 84, 85, 88, 88, 76, 63, 60},{149, 149, 143, 143, 180, 180},{193, 209, 249, 251},{46, 46, 35, 35},{146, 153, 156, 156, 145},{149, 145, 149, 154},{283, 283, 286, 286},{277, 278, 279, 279, 289, 289, 290, 290}};
	float vertz[8][11] = {{51, 50, 38, 34, 35, 38, 38, 51, 57, 58, 53},{119, 134, 135, 151, 150, 119},{201, 185, 190, 236},{143, 138, 138, 143},{231, 236, 231, 226, 226},{201, 195, 189, 195},{147, 152, 152, 147},{166, 188, 188, 181, 181, 188, 188, 166}};
	tVector3 vVector = tVector3(0,0,0);	
	vVector = mView - mPos;
	int i, j, k, c = 0;
	for(k = 0; k < 8; k++) {
		for (i = 0, j = nvert[k]-1; i < nvert[k]; j = i++) {
			if ( ((vertz[k][i]>pos_z + vVector.z * cameraspeed) != (vertz[k][j]>pos_z + vVector.z * cameraspeed)) &&
				(pos_x < (vertx[k][j]-vertx[k][i]) * (pos_z -vertz[k][i]) / (vertz[k][j]-vertz[k][i]) + vertx[k][i]) ){
			   c = !c;
			   model2= k;
			  }
			
		}
     	if(c){
				modelnum =model2;
				if(modelnum!=-1)
				{
					crash[modelnum] = TRUE;
				}
			}
	}
	return c;
}

