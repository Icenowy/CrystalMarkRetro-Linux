/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/* File: hiyogls0.c - Title Scene Code */

#if defined(_WIN32) || defined(_WIN64)
	#define _CRT_SECURE_NO_WARNINGS
	#include<windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<GL/gl.h>
#include"hiyoglr_libgld.h"
#define	HIYOGL_SCENE0_SRC
#include"hiyoglr.h"

/* Logo Panel Parts Struct ------------*/
typedef struct{
	GLfloat		pnt[3][3];
	GLfloat		texpnt[3][2];
	GLfloat		axis[3];
	GLfloat		mov[3];
	GLfloat		direc[3];
	GLfloat		darg;
} HIYOGL_SCENE0_PARTDATA;

/* Scene Global -----------------------*/
static	GLuint					mtexid;
static	HIYOGL_SCENE0_PARTDATA	*parts;
static	int						partsx;
static	int						partsy;

/* LogoParts Vertex Data Set --------------------*/
void hiyoGL_Scene0_PartsVertex(HIYOGL_SCENE0_PARTDATA *pptr, int id, int x, int y)
{
	pptr->pnt[id][0] = (GLfloat)8.9f - ((8.9f * 2.0f / partsx) * x);
	pptr->pnt[id][1] = (GLfloat)6.7f - ((6.7f * 2.0f / partsy) * y);
	pptr->pnt[id][2] = (GLfloat)0.0f;
	pptr->texpnt[id][0] = ((GLfloat)(partsx - x)) / ((GLfloat)partsx);
	pptr->texpnt[id][1] = ((GLfloat)(partsy - y)) / ((GLfloat)partsy);
	return;
}

/* LogoParts Center Calc. -----------------------*/
void hiyoGL_Scene0_PartsCenter(HIYOGL_SCENE0_PARTDATA *pptr)
{
	pptr->mov[0] = (pptr->pnt[0][0] + pptr->pnt[1][0] + pptr->pnt[2][0]) / 3.0f;
	pptr->mov[1] = (pptr->pnt[0][1] + pptr->pnt[1][1] + pptr->pnt[2][1]) / 3.0f;
	pptr->mov[2] = (pptr->pnt[0][2] + pptr->pnt[1][2] + pptr->pnt[2][2]) / 3.0f;

	pptr->pnt[0][0] -= pptr->mov[0];
	pptr->pnt[0][1] -= pptr->mov[1];
	pptr->pnt[0][2] -= pptr->mov[2];

	pptr->pnt[1][0] -= pptr->mov[0];
	pptr->pnt[1][1] -= pptr->mov[1];
	pptr->pnt[1][2] -= pptr->mov[2];

	pptr->pnt[2][0] -= pptr->mov[0];
	pptr->pnt[2][1] -= pptr->mov[1];
	pptr->pnt[2][2] -= pptr->mov[2];
	return;
}

/* LogoParts Data Set ---------------------------*/
void hiyoGL_Scene0_Parts(HIYOGL_SCENE0_PARTDATA *pptr)
{
	pptr->direc[0] = (RandFloat() * 10.0f) - 5.0f;
	pptr->direc[1] = (RandFloat() * 10.0f) - 5.0f;
	pptr->direc[2] = (RandFloat() * 18.0f) - 9.0f;

	pptr->axis[0] = (RandFloat() * 10.0f);
	pptr->axis[1] = (RandFloat() * 10.0f);
	pptr->axis[2] = (RandFloat() * 10.0f);

	pptr->darg = (RandFloat() * 180.0f) + 180.0f;
	return;
}

/* Logo Parts Init. -----------------------------*/
void hiyoGL_Scene0_PartsInit(void)
{
	int						err;
	int						cnt;
	int						bits;
	int						cx;
	HIYOGL_SCENE0_PARTDATA	*pptr;
	char					fname[256];

	for(bits=8; (24>bits)&&(((unsigned long)1<<bits)<physmemory); bits++);
	partsx	= SCENE0_PARTSX + ((bits - 8) * 20);

	for(bits=31; (8<bits)&&(!((1<<bits)&gltexmax)); bits--);
	if(18<bits)	{ bits = 18; }
	partsy	= SCENE0_PARTSY + ((bits - 8) * 18);

	if((parts=(HIYOGL_SCENE0_PARTDATA *)malloc(
			sizeof(HIYOGL_SCENE0_PARTDATA) * 2 * partsx * partsy))==NULL)
		{ hiyoGL_PutLog("[Scene0] ERROR! - Can't alloc parts data."); return; }
	pptr = parts;

	for(cnt=0; cnt<partsy; cnt++)	{
		for(cx=0; cx<partsx; cx++)		{
			/* Polygon 1 */
			hiyoGL_Scene0_PartsVertex(pptr, 0, cx  , cnt  );
			hiyoGL_Scene0_PartsVertex(pptr, 1, cx+1, cnt  );
			hiyoGL_Scene0_PartsVertex(pptr, 2, cx+1, cnt+1);
			hiyoGL_Scene0_PartsCenter(pptr);
			hiyoGL_Scene0_Parts(pptr);
			pptr++;
			/* Polygon 2 */
			hiyoGL_Scene0_PartsVertex(pptr, 0, cx  , cnt  );
			hiyoGL_Scene0_PartsVertex(pptr, 1, cx+1, cnt+1);
			hiyoGL_Scene0_PartsVertex(pptr, 2, cx  , cnt+1);
			hiyoGL_Scene0_PartsCenter(pptr);
			hiyoGL_Scene0_Parts(pptr);
			pptr++;
		}
	}

	/* Regist Logo Image */
	strncpy(fname, logofile, sizeof(fname)-7);
	if(1024 > gltexmax)			{ strncat(fname, "s.gc2", 6); }
	else if(2048 > gltexmax)	{ strncat(fname, "m.gc2", 6); }
	else						{ strncat(fname, "l.gc2", 6); }
	hiyoGL_PutLogString("[Scene0] Loading GC2 LogoFile ", fname);

	err = hiyoGL_LoadRegistTexture(&mtexid, fname);
	if(0 != err)	{
		hiyoGL_PutLogInt("[Scene0] ERROR! - Fail load GC2 LogoFile ", err);
		hiyoGL_Term();
	}

	glEnable(GL_TEXTURE_2D);

	return;
}

/* Diaplay Logo Parts (Moving) ------------------*/
void hiyoGL_Scene0_PartsDisplay_Moving(GLfloat difperc)
{	
	int							cnt;
	GLfloat						difs[3];
	HIYOGL_SCENE0_PARTDATA		*pptr = parts;

	for(cnt=0; cnt<(partsx * partsy * 2); cnt++)	{
		difs[0] = (pptr->direc[0] * difperc);
		difs[1] = (pptr->direc[1] * difperc);
		difs[2] = (pptr->direc[2] * difperc);

		glPushMatrix();
		glTranslatef((pptr->mov[0] + difs[0]), (pptr->mov[1] + difs[1]), (pptr->mov[2] + difs[2]));
		glRotatef((pptr->darg * difperc), pptr->axis[0], pptr->axis[1], pptr->axis[2]);
		glBegin(GL_TRIANGLES);
			glNormal3f(0.0f, 0.0f, 10.0f);
			glTexCoord2fv(pptr->texpnt[0]);
			glVertex3fv(pptr->pnt[0]);
			glTexCoord2fv(pptr->texpnt[1]);
			glVertex3fv(pptr->pnt[1]);
			glTexCoord2fv(pptr->texpnt[2]);
			glVertex3fv(pptr->pnt[2]);
		glEnd();
		glPopMatrix();
		pptr++;
	}
	return;
}

/* Display Function (Moving) --------------------*/
void hiyoGL_Scene0_Display_Moving(void)
{
	GLuint		nowtime;
	GLfloat		seconds;
	GLfloat		difperc;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	nowtime	= hiyoGL_GetTime();
	difperc = (((GLfloat)(nowtime - starttime))
				/ ((GLfloat)SCENE0_STAGE1_TIME + SCENE0_STAGE2_TIME));

	glTranslatef(0.0f, 0.0f, -25.0f + (difperc * difperc * 25.0f));
	glRotatef((540.0f * difperc * difperc), 0.0f, 0.0f, -1.0f);

	glRotatef((fcarg * difperc), (20.0f * difperc * difperc),
				(-10.0f * difperc * difperc), (-4.0f * difperc * difperc));
	hiyoGL_Scene0_PartsDisplay_Moving(difperc);
	hiyoGL_SwapBuffer();

	if (nowtime >= nexttime)	{
		if(stage==1)	{
			stage++;
			nexttime = nowtime + SCENE0_STAGE2_TIME;
			stagetime = nowtime;
			glEnable(GL_FOG);
			return;
		}
		else if(stage==2)	{
			stage = 1;
			glDisable(GL_FOG);
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glDisable(GL_TEXTURE_2D);
			free(parts);
			glDeleteTextures(1, &mtexid);
#ifdef DEBUG
			if(startmode==2)
				{ hiyoGL_Scene2_Init(); }
			else
				{ hiyoGL_Scene1_Init(); }
#else
			hiyoGL_Scene1_Init();
#endif
			return;
		}
	}

	if(stage==2)	{
		seconds = (GLfloat)(nowtime - stagetime);
		glFogf(GL_FOG_END,
				(40.0f - (38.0f * seconds / ((GLfloat)SCENE0_STAGE2_TIME))));
	}

	return;
}

/* Display LogoParts (Static) -------------------*/
void hiyoGL_Scene0_PartsDisplay_Static(void)
{	
	int						cnt;
	HIYOGL_SCENE0_PARTDATA	*pptr = parts;

	for(cnt=0; cnt<(partsx * partsy * 2); cnt++)	{
		glPushMatrix();
		glTranslatef(pptr->mov[0], pptr->mov[1], pptr->mov[2]);
		glBegin(GL_TRIANGLES);
			glNormal3f(0.0f, 0.0f, 10.0f);
			glTexCoord2fv(pptr->texpnt[0]);
			glVertex3fv(pptr->pnt[0]);
			glTexCoord2fv(pptr->texpnt[1]);
			glVertex3fv(pptr->pnt[1]);
			glTexCoord2fv(pptr->texpnt[2]);
			glVertex3fv(pptr->pnt[2]);
		glEnd();
		glPopMatrix();
		pptr++;
	}

	return;
}

/* Display Function (Static) --------------------*/
void hiyoGL_Scene0_Display_Static(void)
{
	GLuint		nowtime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glTranslatef(0.0f, 0.0f, -25.0f);
	hiyoGL_Scene0_PartsDisplay_Static();
	hiyoGL_SwapBuffer();

	nowtime	= hiyoGL_GetTime();
	if(nowtime >= nexttime)	{
		stage++;
		hiyoGL_Scene_Display = hiyoGL_Scene0_Display_Moving;
		nexttime = nowtime + SCENE0_STAGE1_TIME;
		starttime = nowtime;
		return;
	}

	return;
}

/* Title Scene Init. ----------------------------*/
void hiyoGL_Scene0_Init(void)
{
	hiyoGL_PutLog("[Scene0] Booting...");
	stage = 0;
	fcarg = 100.0f;
	hiyoGL_Scene_Display = hiyoGL_Scene0_Display_Static;
	hiyoGL_Scene0_PartsInit();
	hiyoGL_PutLog("[Scene0] Initialized Logo parts");

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 40.0f);
	glFogfv(GL_FOG_COLOR, fogcolor);
	/*glHint(GL_FOG_HINT, GL_FASTEST);*/

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightambi);
	hiyoGL_PutLog("[Scene0] Initialized Parameter");

	hiyoGL_PutLog("[Scene0] Start!");
	nexttime = hiyoGL_GetTime() + SCENE0_STAGE0_TIME;

	return;
}

/* EOF of hiyogls0.c */

