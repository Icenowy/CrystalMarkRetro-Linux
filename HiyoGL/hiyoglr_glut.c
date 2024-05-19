/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/* File: hiyoglr_glut.c - hiyoGL Main Src Code (glut Ver.) */

#if defined(_WIN32) || defined(_WIN64)
	#define	_CRT_SECURE_NO_WARNINGS
	#include<windows.h>
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<GL/gl.h>

#if defined(_WIN32) || defined(_WIN64)
	#define FREEGLUT_STATIC
#endif
#include "GL/glut.h"

#include"hiyoglr_libgld.h"

#define	HIYOGL_MAIN_SRC
#include"hiyoglr.h"

/* Display GL String ----------------------------*/
void hiyoGL_PutString(GLfloat x, GLfloat y, char *str)
{
	glRasterPos3f(x, y, 0.0f);

	while (*str) 
		{ glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str++); }

	return;
}

/* glut SwapBuffer Function ---------------------*/
void hiyoGL_SwapBuffer(void)
{
	glutSwapBuffers();
	return;
}

/* Terminate Program with glut ------------------*/
void hiyoGL_TermProgram(void)
{
	exit( 0 );
	return;
}

/* glut Display Function ------------------------*/
void hiyoGL_Display(void)
{
	hiyoGL_Scene_Display();
	return;
}

/* glut Idle Function ---------------------------*/
void hiyoGL_Idle(void)
{
	glutPostRedisplay();
	return;
}

/* glut KeyBorad Function - DEBUG! --------------*/
#ifdef DEBUG
void hiyoGL_keyboard(unsigned char key,int x,int y)
{
	switch(key)		{
		case 'f':
			glutFullScreen();
			break;
		case '1':
			startmode	= 1;
			break;
		case '2':
			startmode	= 2;
			break;
		case 'q':
			exit(0);
			break;
	}
	glutPostRedisplay();

	return;
}
#endif

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27: // ESC
		// glutLeaveMainLoop();
		exit(0);
		break;
	}
	glutPostRedisplay();

	return;
}

/* main -----------------------------------------*/
int main(int argc,char *argv[])
{
	int		err;
	char	temp[256];
#if defined(_WIN32) || defined(_WIN64)
	TCHAR	*ptrEnd;
	TCHAR	path[MAX_PATH];
	HDC		hDC;
#endif

#if defined(_WIN32) || defined(_WIN64)
	/* Set Result & log file path */
	// path[0] = '\n';
	GetTempPathW(MAX_PATH, tempPath);
	wsprintfW(resultFilePath, L"%s%s", tempPath, HIYOGL_RESULT_FILE);
	wsprintfW(logFilePath, L"%s%s", tempPath, HIYOGL_LOG_FILE);

	GetModuleFileName(NULL, path, MAX_PATH);
	if((ptrEnd = wcsrchr(path, '\\')) != NULL)
		{ *ptrEnd = '\0'; }

	SetCurrentDirectory(path);
#endif

	snprintf(temp, sizeof(temp)-1, "[System] hiyoGL Retro %s (%s)",
					HIYOGL_VERSION, HIYOGL_EDITION);
	hiyoGL_CreateLog(temp);

#ifdef DEBUG
	hiyoGL_PutLog( "** DEBUG MODE **" );
	startmode	= 1;
#endif

	/* Set Scene0 title logo filename */
	logofile = HIYOGL_LOGO_BASEFILE;
	if( 2 == argc )	{
		if( 256-7 < strnlen(argv[1], 256) )	{
			hiyoGL_PutLog( "[System] ERROR! - Too much long logo BaseFilename." );
			return 1;
		}
		logofile = argv[1];
	}
	hiyoGL_PutLogString( "[System] LogoFile BaseName", logofile );

	/* Init. glut */
	srand(13);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	scrwidth	= WINDOW_DEFSIZE_X;
	scrheight	= WINDOW_DEFSIZE_Y;
	glutInitWindowSize( WINDOW_DEFSIZE_X, WINDOW_DEFSIZE_Y );
	glutInitWindowPosition( 30, 30 );
	glutCreateWindow( (const char *)CAPTION_TITLE );

#if defined(_WIN32) || defined(_WIN64)
	hDC = GetDC(NULL);
	scrcolor	= GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	ReleaseDC(NULL, hDC);
#else
	scrcolor	= glutGet(GLUT_WINDOW_RED_SIZE) + glutGet(GLUT_WINDOW_GREEN_SIZE)
					+ glutGet(GLUT_WINDOW_BLUE_SIZE);
#endif
 
	/* Init. models */
	winmode	= GLUT;
	err = hiyoGL_MainInit();
	if(0 != err)	{
		hiyoGL_PutLogInt( "[System] ERROR! - Failure init. objects.", err );
		return 2;
	}

#if defined(_WIN32) || defined(_WIN64)
	glutKeyboardFunc(keyboard);
#else

#ifdef DEBUG
	glutKeyboardFunc(hiyoGL_keyboard);
#endif

#endif

	glutReshapeFunc(hiyoGL_Reshape);
	glutDisplayFunc(hiyoGL_Display);
	glutIdleFunc(hiyoGL_Idle);
 
	hiyoGL_Scene0_Init();

#ifndef DEBUG
	glutFullScreen();
#endif
	glutMainLoop();

	return 0;
}

/* EOF of hiyoglr_glut.c */

