#include "Main.h"

GLuint	base;				// 绘制字体的显示列表的开始位置
GLvoid Font::BuildFont(GLvoid)						// 创建位图字体
{
	HFONT	font;						// 字体句柄
	HFONT	oldfont;						// 旧的字体句柄

	base = glGenLists(96);					// 创建96个显示列表
	font = CreateFont(	-26,					// 字体高度
			        	  0,				// 字体宽度
						  0,				// 字体的旋转角度 Angle Of Escapement
		                  0,				// 字体底线的旋转角度Orientation Angle
						  FW_BOLD,				// 字体的重量	
                          FALSE,				// 是否使用斜体
			              FALSE,				// 是否使用下划线
				          FALSE,				// 是否使用删除线
                          ANSI_CHARSET,			// 设置字符集
						  OUT_TT_PRECIS,			// 输出精度
						  CLIP_DEFAULT_PRECIS,		// 裁剪精度
						  ANTIALIASED_QUALITY,		// 输出质量
					      FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						  "Courier New");			// 字体名称
	oldfont = (HFONT)SelectObject(hDC, font);			// 选择我们需要的字体
	wglUseFontBitmaps(hDC, 32, 96, base);			// 创建96个显示列表，绘制从ASCII码为32-128的字符
	SelectObject(hDC, oldfont);					// 选择原来的字体
	DeleteObject(font);					// 删除字体
}

GLvoid Font::KillFont(GLvoid)						// 删除显示列表
{
 	glDeleteLists(base, 96);					//删除96个显示列表
}


GLvoid Font::glPrint(float x,float y,const char *fmt, ...)					// 自定义GL输出字体函数
{
    char		text[256];				// 保存文字串
	va_list		ap;					// 指向一个变量列表的指针
if (fmt == NULL)						// 如果无输入则返回
		return;	
  	glRasterPos2f(x, y);
						
va_start(ap, fmt);						// 分析可变参数
	    vsprintf(text, fmt, ap);				// 把参数值写入字符串
	va_end(ap);						// 结束分析
glPushAttrib(GL_LIST_BIT);					// 把显示列表属性压入属性堆栈
	glListBase(base - 32);					// 设置显示列表的基础值
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);		// 调用显示列表绘制字符串
	glPopAttrib();						// 弹出属性堆栈
}
