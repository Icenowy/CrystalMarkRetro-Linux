/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/* File: hiyoglr_scene1.c - WireFrame Scene Code */

#if defined(_WIN32) || defined(_WIN64)
	#define	_CRT_SECURE_NO_WARNINGS
	#include<windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<GL/gl.h>
#include"hiyoglr_libgld.h"
#define	HIYOGL_SCENE1_SRC
#include"hiyoglr.h"

/* Add CPU Model --------------------------------*/
static void hiyoGL_Scene1_AddModel(unsigned int cnt)
{
	HiyoGL_Model	*pmodel;

	pmodel	= pmdl + cnt;

	pmodel->pos[0] = (GLfloat)((rand() % 35) + 10);
	pmodel->pos[1] = (GLfloat)((rand() % 40) - 20);
	pmodel->pos[2] = (GLfloat)((rand() % 150) - 75);
	pmodel->arg[0] = (GLfloat)(rand() % 360);
	pmodel->arg[1] = (GLfloat)(rand() % 360);

	pmodel->smodel = &model[modelids++];
	putmodels++;
	if(modelids>=regmodels)
		{ modelids = 0; }
	return;
}

/* Add Display Model ----------------------------*/
static void hiyoGL_Scene1_AddDispModel(void)
{
	unsigned int	cnt;
	unsigned int	max;

	if(CPUADD_LIMITFPS_DIV2 <= fpsavg)
		{ max = putmodels + putmodels; }
	else if((CPUADD_LIMITFPS_DIV8 > fpsavg)&&(8 < putmodels))
		{ max = putmodels + (putmodels / 8); }
	else if((CPUADD_LIMITFPS_DIV4 > fpsavg)&&(4 < putmodels))
		{ max = putmodels + (putmodels / 4); }
	else if((CPUADD_LIMITFPS_DIV2 > fpsavg)&&(2 < putmodels))
		{ max = putmodels + (putmodels / 2); }
	else
		{ max = putmodels + 1; }

	for(cnt=putmodels; cnt<max; cnt++)
		{ hiyoGL_Scene1_AddModel(cnt); }
	return;
}

/* Display CPU Model ----------------------------*/
static void hiyoGL_Scene1_Put(unsigned int mid)
{
	HiyoGL_Model	*pmodel;

	pmodel	= pmdl + mid;

	pmodel->arg[0] += ((pmodel->arg[0]+3.0f>=360.0f) ? (3.0f-360.0f) : 3.0f);
	pmodel->arg[1] += ((pmodel->arg[1]+3.0f>=360.0f) ? (3.0f-360.0f) : 3.0f);

	glPushMatrix();
		glRotatef(pmodel->arg[1], 0.0f, 10.0f, 0.0f);
		glRotatef(pmodel->arg[0], 10.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, pmodel->pos[0] );

		glRotatef(pmodel->arg[1], 0.0f, 10.0f, 0.0f);
		glRotatef(pmodel->arg[0], 10.0f, 0.0f, 10.0f);
		glScalef( fscalenow, fscalenow, fscalenow );
		spoly += LibGLD_DrawWireFrame(pmodel->smodel);
	glPopMatrix();

	return;
}

/* Display Scene Status -------------------------*/
void hiyoGL_Scene1_PutStatus(unsigned long long polys)
{
	char		tmpstr[128];

	if( GLUT == winmode )	{
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
 
		glDisable(GL_LIGHT0);  
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);   
 
		glColor3f(1.0f, 1.0f, 1.0f);
		snprintf(tmpstr, sizeof(tmpstr)-1, "[WireFrame Mode] %dx%d / %d bit",
					scrwidth, scrheight, scrcolor);
		hiyoGL_PutString(SCORELINE_X, SCORELINE_Y1, tmpstr);
 
		snprintf(tmpstr, sizeof(tmpstr)-1, "%.3f FPS / %d CPU", fpsavg, putmodels);
		hiyoGL_PutString(SCORELINE_X, SCORELINE_Y2, tmpstr);

		snprintf(tmpstr, sizeof(tmpstr)-1, "%llu00 Lines", (polys*3)/2);
		hiyoGL_PutString(SCORELINE_X, SCORELINE_Y3, tmpstr);

		glPopMatrix() ;
		glMatrixMode(GL_MODELVIEW) ;
		glPopMatrix() ;
		glPopAttrib();
	}

	return;
}

/* Terminate Scene1 -----------------------------*/
void hiyoGL_Scene1_Term(void)
{
	FILE		*fans = NULL;
	char		temp[256];
#if defined(_WIN32) || defined(_WIN64)
	errno_t		terr;
#endif

	hiyoGL_PutLog("[Scene1] Terminating process...");

	/* Score Write */
#if defined(_WIN32) || defined(_WIN64)
	if((terr = _wfopen_s(&fans, resultFilePath, L"w"))!=0)	{
		hiyoGL_PutLogString("[Scene1] ERROR! - Can't open result file.  errno", strerror(terr));
		exit(1);
	}
#else
	if((fans = fopen(HIYOGL_RESULT_FILE, "w")) == NULL)	{
		hiyoGL_PutLog("[Scene1] ERROR! - Can't open result file.");
		exit(1);
	}
#endif
	cpus[0] = putmodels;
	fprintf(fans, "[hiyoGL_Scene1]\n");
	fprintf(fans, "Scene1_Score=%llu\n", score[0]);
	fprintf(fans, "Scene1_Lines=%llu00\n", ((triangles[0] * 3) / 2));
	fprintf(fans, "Scene1_CPU=%d\n", putmodels);
	fclose(fans);

	/* Output score for Log */
	snprintf(temp, sizeof(temp)-1, "[Scene1] Scene1_Score=%llu", score[0]);
	hiyoGL_PutLog(temp);
	snprintf(temp, sizeof(temp)-1, "[Scene1] Scene1_Lines=%llu00", ((triangles[0] * 3) / 2));
	hiyoGL_PutLog(temp);
	snprintf(temp, sizeof(temp)-1, "[Scene1] Scene1_CPU=%d", putmodels);
	hiyoGL_PutLog(temp);

	stage = 1;
	spoly = 0;

	hiyoGL_Scene2_Init();
	return;
}

/* Display Scene1 -------------------------------*/
void hiyoGL_Scene1_Display(void)
{
	unsigned int	cnt;
	GLuint			nowtime;
	GLfloat			seconds;
	GLfloat			fsin;
	GLfloat			fcos;

	/* Scene 1 Drawing */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	fcarg = ((fcarg-1.0f < 0.0f) ? (fcarg - 1.0f + 360.0f) : fcarg - 1.0f);
	fcos = cosf( fcarg * Pi / 180.0f );
	fsin = sinf( fcarg * Pi / 180.0f );
	glTranslatef(0.0f, 0.0f, -175.0f);
	glRotatef(fcarg, -1.5f * fcos, 10.0f, 1.5f * fsin );

	spoly += LibGLD_DrawWireFrame(&back);
	for(cnt=0; cnt<putmodels; cnt++)
		{ hiyoGL_Scene1_Put(cnt); }

	fpsframe++;
	nowtime = hiyoGL_GetTime();
	if(nowtime >= fpstime)	{
		fps		= (GLfloat)fpsframe / (nowtime - fpstime + FPSSTEP) * 1000.0f;
		fpssum	+= fps;
		fpscount++;
		fpsavg	= fpssum / (GLfloat)fpscount;
		fpstime = nowtime + FPSSTEP;
		fpsframe = 0;
	}
	if(stage==1)	{ npoly = spoly; }
	hiyoGL_Scene1_PutStatus(npoly);
	hiyoGL_SwapBuffer();

	if(nowtime >= nexttime)	{
		stagetime = nowtime;
		nexttime = nowtime + STEPTIME;
		if(stage==0)	{
			starttime = nowtime;
			spoly = 0;
			stage++;
			fpssum		= 0.0f;
			fpsavg		= 0.0f;
			fpscount	= 0;
			glDisable(GL_FOG);
			return;
		}
		else if(stage==2)	{
			hiyoGL_Scene1_Term();
			return;
		}

		if((fpsavg<STOPFPS)||(putmodels>=cpumax))	{
			stage++;
			triangles[0] = spoly;
			score[0] = (spoly * 10) / (unsigned long long)(nowtime-starttime);

			glEnable(GL_FOG);
			return;
		}
		hiyoGL_Scene1_AddDispModel();

		if( fscalenow > 1.0f )			{ fscalenow -= 0.2f; }
		else if( fscalenow > 0.2f )		{ fscalenow -= 0.1f; }
		else if( fscalenow > 0.01f )	{ fscalenow -= 0.01f; }

		fpssum		= 0.0f;
		fpsavg		= 0.0f;
		fpscount	= 0;
	}

	if(stage==0)	{
		seconds = (GLfloat)(nowtime - starttime);
		glFogf(GL_FOG_END, ((400.0f * seconds / (GLfloat)STEPTIME)+10.0f));
		glFogf(GL_FOG_START, ((350.0f * seconds / (GLfloat)STEPTIME)));
	}
	else if(stage==2)	{
		seconds = (GLfloat)(nowtime - stagetime);
		glFogf(GL_FOG_END, (410.0f - (410.0f * seconds / (GLfloat)STEPTIME)));
		glFogf(GL_FOG_START, (350.0f - (350.0f * seconds / (GLfloat)STEPTIME)));
	}

	return;
}

/* Init. Scene1 ---------------------------------*/
void hiyoGL_Scene1_Init(void)
{
	hiyoGL_PutLog("[Scene1] Booting...");
	stage		= 0;
	spoly		= 0;
	modelids	= 0;
	fcarg		= 0.0f;
	fscalenow	= 2.0f;
	putmodels	= 0;
	npoly		= 0;
	fps			= 0.0f;
	fpsframe	= 0;
	fpssum		= 0.0f;
	fpsavg		= 0.0f;
	fpscount	= 0;
	hiyoGL_Scene_Display = hiyoGL_Scene1_Display;
	hiyoGL_PutLog("[Scene1] Initalized Parameter");

	hiyoGL_Scene1_AddModel(0);
	hiyoGL_PutLog("[Scene1] Initalized CPU Model");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 10.0f);
	glFogfv(GL_FOG_COLOR, fogcolor);

	hiyoGL_PutLog("[Scene1] Start!");
	starttime = hiyoGL_GetTime();;
	nexttime = starttime + STEPTIME;
	fpstime = starttime + FPSSTEP;

	return;
}

/* EOF of hiyogls1.c */

