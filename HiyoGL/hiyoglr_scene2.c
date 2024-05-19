/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/* File: hiyoglr_scene2.c - FullPolygon Scene Code */

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
#define	HIYOGL_SCENE2_SRC
#include"hiyoglr.h"

typedef struct	{
	unsigned long	vertex[4];
	GLfloat			normal[3];
	GLfloat			specular[4];
	GLfloat			emission[4];
	GLuint			texid;
} HIYOGL_SCENE2_POLYGON;

typedef struct	{
	unsigned long	vernum;
	GLfloat			*vertex;
	unsigned long	normnum;
	GLfloat			*normal;
	unsigned long	polynum;
	HIYOGL_SCENE2_POLYGON	*polygon;
	unsigned long	polyscore;
} HIYOGL_SCENE2_CONE;


/* Scene Global -----------------------*/
static	GLfloat			conearg[3];
static	GLuint			coneid[3];
static	unsigned long	conescore[3];
static	GLfloat			texpnt[4][2] = {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

#define	CONE_VTXID(z,a)		((SCENE2_BACK_DIVARG*(z))+(a))
#define	CONE_POLYID(z,a)	((SCENE2_BACK_DIVARG*(z))+(a))

static GLuint hiyoGL_Scene2_Cone_GenerateVertex(HIYOGL_SCENE2_CONE *pcone, GLfloat height)
{
	int				d;
	int				a;
	int				id;
	unsigned long	vernum;
	GLfloat			r;
	GLfloat			z;
	GLfloat			rad;
	GLfloat			*pvertex;

	vernum = (SCENE2_BACK_DIVZ+1) * SCENE2_BACK_DIVARG * 3;
	pvertex = (GLfloat *)malloc(sizeof(GLfloat) * vernum);
	if(NULL == pvertex)	{
		return -0x01;
	}
	memset(pvertex, 0x00, sizeof(GLfloat) * (SCENE2_BACK_DIVZ+1) * SCENE2_BACK_DIVARG * 3);
	pcone->vertex = pvertex;
	pcone->vernum = vernum;

	for(d=0; d<(SCENE2_BACK_DIVZ+1); d++)	{
		z = -1.0f * height * (GLfloat)d / (GLfloat)SCENE2_BACK_DIVZ;
		r = -1.0f * (SCENE2_BACK_RADIUS_FRONT - SCENE2_BACK_RADIUS_END)
						* (GLfloat)d / (GLfloat)SCENE2_BACK_DIVZ + SCENE2_BACK_RADIUS_FRONT;

		for(a=0; a<SCENE2_BACK_DIVARG; a++)	{
			rad = (GLfloat)360.0f / 180.0f * Pi * (GLfloat)a  / (GLfloat)SCENE2_BACK_DIVARG;

			id = CONE_VTXID(d,a) * 3;
			*(pvertex + id + 0)	= r * sinf(rad);
			*(pvertex + id + 1)	= r * cosf(rad);
			*(pvertex + id + 2)	= z;
		}
	}

	return 0;
}

static void hiyoGL_Scene2_Cone_CalcPolyNormal(
				HIYOGL_SCENE2_CONE *pcone, HIYOGL_SCENE2_POLYGON *ply)
{
	GLfloat		*p[3];
	GLfloat		v[2][3];
	GLfloat		n[3];
	GLfloat		length;

	p[0] = pcone->vertex + (ply->vertex[0] * 3);
	p[1] = pcone->vertex + (ply->vertex[1] * 3);
	p[2] = pcone->vertex + (ply->vertex[2] * 3);

	v[0][0]	= *(p[0] + 0) - *(p[1] + 0);
	v[0][1]	= *(p[0] + 1) - *(p[1] + 1);
	v[0][2]	= *(p[0] + 2) - *(p[1] + 2);

	v[1][0]	= *(p[2] + 0) - *(p[1] + 0);
	v[1][1]	= *(p[2] + 1) - *(p[1] + 1);
	v[1][2]	= *(p[2] + 2) - *(p[1] + 2);

	n[0] = (v[0][1] * v[1][2]) - (v[0][2] * v[1][1]);
	n[1] = (v[0][2] * v[1][0]) - (v[0][0] * v[1][2]);
	n[2] = (v[0][0] * v[1][1]) - (v[0][1] * v[1][0]);

	length = sqrtf(n[0] * n[0]) + (n[1] * n[1]) + (n[2] * n[2]);

	ply->normal[0]	= n[0] / length;
	ply->normal[1]	= n[1] / length;
	ply->normal[2]	= n[2] / length;

	return;
}

static GLuint hiyoGL_Scene2_Cone_RegistPolygon(HIYOGL_SCENE2_CONE *pcone, GLuint texid[SCENE2_COREIMG_MAX], int col)
{
	unsigned long			d;
	unsigned long			dn;
	unsigned long			a;
	unsigned long			an;
	unsigned long			polynum;
	GLfloat					coltemp;
	HIYOGL_SCENE2_POLYGON	*polygon;
	HIYOGL_SCENE2_POLYGON	*ply;

	polynum	= SCENE2_BACK_DIVZ * SCENE2_BACK_DIVARG;
	polygon = (HIYOGL_SCENE2_POLYGON *)malloc(sizeof(HIYOGL_SCENE2_POLYGON) * polynum);
	if(NULL == polygon)	{
		return -0x01;
	}
	pcone->polygon	= polygon;
	pcone->polynum	= polynum;
	conescore[col]	= polynum / 100;
	memset(polygon, 0x00, sizeof(HIYOGL_SCENE2_POLYGON) * polynum);

	for(d=0,dn=1; d<SCENE2_BACK_DIVZ; d++,dn++)	{

		for(a=0; a<SCENE2_BACK_DIVARG; a++)	{
			an = (SCENE2_BACK_DIVARG-1==a) ? 0 : (a+1);

			ply = polygon + CONE_POLYID(d,a);
			ply->vertex[0] = CONE_VTXID(d,a);
			ply->vertex[1] = CONE_VTXID(d,an);
			ply->vertex[2] = CONE_VTXID(dn,an);
			ply->vertex[3] = CONE_VTXID(dn,a);

			coltemp	= (RandFloat() * 0.4f);
			ply->specular[0]	= coltemp;
			ply->specular[1]	= coltemp;
			ply->specular[2]	= coltemp;
			ply->specular[3]	= 1.0f;
			ply->specular[col]	= 0.4f + (RandFloat() * 0.6f);

			ply->emission[0]	= coltemp;
			ply->emission[1]	= coltemp;
			ply->emission[2]	= coltemp;
			ply->emission[3]	= 1.0f;
			ply->emission[col]	= 0.4f + (RandFloat() * 0.6f);

			hiyoGL_Scene2_Cone_CalcPolyNormal(pcone, ply);

			ply->texid	= texid[rand() % SCENE2_COREIMG_MAX];
		}
	}

	return 0;
}

static void hiyoGL_Scene2_DrawCone(HIYOGL_SCENE2_CONE *pcone)
{
	unsigned long	cnt;
	HIYOGL_SCENE2_POLYGON	*ply;
	GLfloat	diffuse[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat	ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat	shininess[] = { 3.0f };

	glEnable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	for(cnt=0; cnt<pcone->polynum; cnt++)	{
		ply = pcone->polygon + cnt;

		glBindTexture(GL_TEXTURE_2D, ply->texid);

		glBegin(GL_QUADS);
			glMaterialfv(GL_FRONT, GL_SPECULAR, ply->specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, ply->emission);

			glNormal3fv(ply->normal);
			glTexCoord2fv(texpnt[0]);	glArrayElement(ply->vertex[0]);
			glTexCoord2fv(texpnt[1]);	glArrayElement(ply->vertex[1]);
			glTexCoord2fv(texpnt[2]);	glArrayElement(ply->vertex[2]);
			glTexCoord2fv(texpnt[3]);	glArrayElement(ply->vertex[3]);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

	return;
}

static GLuint hiyoGL_Scene2_RegistCone(GLfloat height, GLuint texid[SCENE2_COREIMG_MAX], int colid)
{
	GLuint				gid;
	HIYOGL_SCENE2_CONE	tcone;

	memset(&tcone, 0x00, sizeof(HIYOGL_SCENE2_CONE));

	hiyoGL_Scene2_Cone_GenerateVertex(&tcone, height);
	hiyoGL_Scene2_Cone_RegistPolygon(&tcone, texid, colid);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, tcone.vertex);

	gid = glGenLists(1);
	glNewList(gid, GL_COMPILE);
		hiyoGL_Scene2_DrawCone(&tcone);
	glEndList();

	free(tcone.vertex);

	return gid;
}

static int hiyoGL_Scene2_Load_CoreImage(GLuint texid[SCENE2_COREIMG_MAX], char *texfname, int coreid)
{
	int					err;
	GC2_INFO			*gc2;
	unsigned char		*pdata;
	unsigned long		width;
	unsigned long		height;
	unsigned long		rawsize;
	GLuint				tid;

	// Load GC2 file ---
	gc2 = GC2_InitInfo();
	err = GC2_Load_File(gc2, texfname);
	if(0 != err)    {
		hiyoGL_PutLogString("[Common] ERROR! - Fail load GC2 TextureFile", texfname);
		return -1;
	}

	width = GC2Info_X(gc2);
	height = GC2Info_Y(gc2);
	rawsize = GC2Info_RawSize(gc2);
	pdata = GC2_CopyPictureData(rawsize, GC2Info_RawData(gc2));
	GC2_FreeInfo(gc2);

	if(NULL == pdata)	{
		hiyoGL_PutLogString("[Common] ERROR! - Can't alloc Picture data area", texfname);
		return -2;
	}

	// Regist Texture Data for OpenGL ---
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pdata);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pdata);

	texid[coreid]	= tid;

	return 0;
}

int hiyoGL_Scene2_GenerateCone(void)
{
	int		cnt;
	int		err;
	GLuint	texid[SCENE2_COREIMG_MAX];

	// Load GC2 file ---
	for(cnt=0; cnt<SCENE2_COREIMG_MAX; cnt++)	{
		err = hiyoGL_Scene2_Load_CoreImage(texid, Scene2_CoreImageFile[cnt], cnt);
		if(err != 0)	{
			hiyoGL_PutLogString("[Scene2] ERROR! - Can't load texture image file",
								Scene2_CoreImageFile[cnt]);
			return -1;
		}
	}

	glEnable(GL_TEXTURE_2D);

	coneid[0] = hiyoGL_Scene2_RegistCone( SCENE2_BACK_DEPTH_END, texid, 0);
	coneid[1] = hiyoGL_Scene2_RegistCone( SCENE2_BACK_DEPTH_END -  5.0f, texid, 1);
	coneid[2] = hiyoGL_Scene2_RegistCone( SCENE2_BACK_DEPTH_END - 10.0f, texid, 2);

	return 0;
}

/* Add CPU Model --------------------------------*/
static void hiyoGL_Scene2_AddModel(unsigned int mid)
{
	HiyoGL_Model	*pmodel;

	pmodel	= pmdl + mid;

	pmodel->pos[0] = (GLfloat)((rand() % 40) - 20);
	pmodel->pos[1] = (GLfloat)((rand() % 40) - 20);
	pmodel->pos[2] = (GLfloat)(-1 * ((rand() %  SCENE2_CPU_BORN_POS_RANGE) + SCENE2_CPU_BORN_POS_MIN));
	pmodel->arg[0] = (GLfloat)(rand() % 360);
	pmodel->arg[1] = (GLfloat)(rand() % 360);
	pmodel->arg[2] = (GLfloat)(rand() % 360);

	pmodel->smodel = &model[modelids++];
	putmodels++;
	if(modelids>=regmodels)
		{ modelids = 0; }

	return;
}

/* Add Display Model ----------------------------*/
static void hiyoGL_Scene2_AddDispModel(void)
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
		{ hiyoGL_Scene2_AddModel(cnt); }
	return;
}

/* Display CPU Model ----------------------------*/
static void hiyoGL_Scene2_Put(unsigned int mid)
{
	HiyoGL_Model	*pmodel;

	pmodel	= pmdl + mid;

	pmodel->arg[0] += ((pmodel->arg[0]+3.0f >= 360.0f) ? (3.0f - 360.0f) : 3.0f);
	pmodel->arg[1] += ((pmodel->arg[1]+3.0f >= 360.0f) ? (3.0f - 360.0f) : 3.0f);
	pmodel->pos[2] += SCENE2_CPU_FLIGHT_SPEED;
	if(pmodel->pos[2] > 25.0f)	{
		pmodel->pos[0] = (GLfloat)((rand() % 40) - 20);
		pmodel->pos[1] = (GLfloat)((rand() % 40) - 20);
		pmodel->pos[2] = (GLfloat)(-1 * ((rand() % SCENE2_CPU_BORN_POS_RANGE)
										+ SCENE2_CPU_BORN_POS_MIN));
	}

	glPushMatrix();
		glTranslatef(pmodel->pos[0], pmodel->pos[1], pmodel->pos[2]);
		glRotatef(pmodel->arg[0], 10.0f,  0.0f, 0.0f);
		glRotatef(pmodel->arg[1],  0.0f, 10.0f, 0.0f);
		glScalef( fscalenow, fscalenow, fscalenow );
		spoly += LibGLD_Draw(pmodel->smodel);
	glPopMatrix();

	return;
}

/* Display Scene Status -------------------------*/
void hiyoGL_Scene2_PutStatus(unsigned long long polys)
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
		snprintf(tmpstr, sizeof(tmpstr)-1, "[Polygon Mode] %dx%d / %d bit",
					scrwidth, scrheight, scrcolor);
		hiyoGL_PutString(SCORELINE_X, SCORELINE_Y1, tmpstr);

		snprintf(tmpstr, sizeof(tmpstr)-1, "%.3f FPS / %d CPU", fpsavg, putmodels);
		hiyoGL_PutString(SCORELINE_X,  SCORELINE_Y2, tmpstr);

		snprintf(tmpstr, sizeof(tmpstr)-1, "%llu00 Polygons", (polys*3)/2);
		hiyoGL_PutString(SCORELINE_X,   SCORELINE_Y3, tmpstr);

		glPopMatrix() ;
		glMatrixMode(GL_MODELVIEW) ;
		glPopMatrix() ;
		glPopAttrib();
	}

	return;
}

/* Terminate Scene2 -----------------------------*/
void hiyoGL_Scene2_Term(void)
{
	FILE	*fans = NULL;
	char	temp[256];
#if defined(_WIN32) || defined(_WIN64)
	errno_t	terr;
#endif

	hiyoGL_PutLog("[Scene2] Terminating process...");

	/* Scene2 Score Write */
#if defined(_WIN32) || defined(_WIN64)
	if((terr = _wfopen_s(&fans, resultFilePath, L"a")) != 0)	{
		hiyoGL_PutLogString("[Scene2] ERROR! - Can't open result file.  errno", strerror(terr));
		exit(1);
	}
#else
	if((fans = fopen(HIYOGL_RESULT_FILE, "a")) == NULL)	{
		hiyoGL_PutLog("[Scene2] ERROR! - Can't open result file.");
		exit(1);
	}
#endif
	cpus[1] = putmodels;
	fprintf(fans, "\n[hiyoGL_Scene2]\n");
	fprintf(fans, "Scene2_Score=%llu\n", score[1]/10);
	fprintf(fans, "Scene2_Polygons=%llu00\n", triangles[1]);
	fprintf(fans, "Scene2_CPU=%d\n", putmodels);
	fclose(fans);

	/* Output score for Log */
	snprintf(temp, sizeof(temp)-1, "[Scene2] Scene2_Score=%llu", score[1]/10);
	hiyoGL_PutLog(temp);
	snprintf(temp, sizeof(temp)-1, "[Scene2] Scene2_Polygons=%llu00", triangles[1]);
	hiyoGL_PutLog(temp);
	snprintf(temp, sizeof(temp)-1, "[Scene2] Scene2_CPU=%d", putmodels);
	hiyoGL_PutLog(temp);

	return;
}

/* Display Function -----------------------------*/
void hiyoGL_Scene2_Display(void)
{
	unsigned int	cnt;
	GLuint			nowtime;
	GLfloat			seconds;

	/* Scene 2 Drawing */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glTranslatef(0.0f, 0.0f, -25.0f);

	/* Draw Color Big Cone */
	glEnable(GL_BLEND);

	conearg[0] += ((conearg[0] + 0.1f >= 360.0f) ? (0.1f - 360.0f) : 0.1f);
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -25.0f);
		glRotatef(conearg[0], 0.0f, 0.0f, 10.0f);
		glCallList(coneid[0]);
		spoly += conescore[0];
	glPopMatrix();
	conearg[1] += ((conearg[1] + 0.3f >= 360.0f) ? (0.3f - 360.0f) : 0.3f);
	glPushMatrix();
		glRotatef(conearg[1], 0.0f, 0.0f, 10.0f);
		glCallList(coneid[1]);
		spoly += conescore[1];
	glPopMatrix();
	conearg[2] -= ((conearg[2] - 0.1f < 0.0f) ? (0.1f + 360.0f) : 0.1f);
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 20.0f);
		glRotatef(conearg[2], 0.0f, 0.0f, 10.0f);
		glCallList(coneid[2]);
		spoly += conescore[2];
	glPopMatrix();

	glDisable(GL_BLEND);

	fcarg -= ((fcarg - 0.6f < 0.0f) ? (0.6f + 360.0f) : 0.6f);
	glRotatef(fcarg, 0.0f, 0.0f, 10.0f);

	for(cnt=0; cnt<putmodels; cnt++)
		{ hiyoGL_Scene2_Put(cnt); }

	fpsframe++;
	nowtime = hiyoGL_GetTime();
	if(nowtime >= fpstime)	{
		fps		= (GLfloat)fpsframe / (GLfloat)(nowtime - fpstime + FPSSTEP) * 1000.0f;
		fpssum	+= fps;
		fpscount++;
		fpsavg	= fpssum / (GLfloat)fpscount;
		fpstime	= nowtime + FPSSTEP;
		fpsframe = 0;
	}
	if(stage==1)	{ npoly = spoly; }
	hiyoGL_Scene2_PutStatus(npoly);

	hiyoGL_SwapBuffer();

	if(stage==0)	{
		seconds = (GLfloat)(nowtime - starttime);
		if(seconds > STEPTIME)	{ seconds = STEPTIME; }
		glFogf(GL_FOG_END, ((SCENE2_FOG_END * seconds / (GLfloat)STEPTIME)+10.0f));
		glFogf(GL_FOG_START, ((SCENE2_FOG_FRONT * seconds / (GLfloat)STEPTIME)));
	}
	else if(stage==2)	{
		seconds = (GLfloat)(nowtime - stagetime);
		if(seconds > STEPTIME)	{ seconds = STEPTIME; }
		glFogf(GL_FOG_END, (SCENE2_FOG_END - (SCENE2_FOG_END * seconds / (GLfloat)STEPTIME)));
		glFogf(GL_FOG_START, (SCENE2_FOG_FRONT - (SCENE2_FOG_FRONT * seconds / (GLfloat)STEPTIME)));
	}

	if(nexttime <= nowtime)
		{ fscalenow = fscaletarg; }
	else
		{ fscalenow = fscaletarg + (fscalediff * (GLfloat)(nexttime - nowtime) / STEPTIME); }

	/* Scene 1 Evnet Proc. */
	if (nowtime >= nexttime)
		{
		stagetime = nowtime;
		nexttime = nowtime + STEPTIME;
		if(stage==0)	{
			starttime = nowtime;
			spoly = 0;
			stage++;
			fpssum		= 0.0f;
			fpsavg		= 0.0f;
			fpscount	= 0;
			return;
		}
		else if(stage==2)	{
			stage = 1;
			hiyoGL_Scene2_Term();
			hiyoGL_Term();
		}

		if((fpsavg<STOPFPS)||(putmodels>=cpumax))	{
			stage++;
			triangles[1] = spoly;
			score[1] = (spoly * 100) / (unsigned long long)(nowtime-starttime);
			return;
		}

		hiyoGL_Scene2_AddDispModel();

		fpssum		= 0.0f;
		fpsavg		= 0.0f;
		fpscount	= 0;

		if(putmodels <= (1<<SCENE2_CPU_SCALE_LASTBITS))	{
			fscaletarg = (GLfloat)((double)SCENE2_CPU_SCALE_MIN
					+ (SCENE2_CPU_SCALE_RANGE * ((GLfloat)SCENE2_CPU_SCALE_LASTBITS
						- log2((double)putmodels)) / (GLfloat)SCENE2_CPU_SCALE_LASTBITS));
			fscalediff	= fscalenow - fscaletarg;
		}
		else	{ fscalediff = 0.0f; }
	}

	return;
}

/* Init. Scene2 ---------------------------------*/
void hiyoGL_Scene2_Init(void)
{
	hiyoGL_PutLog("[Scene2] Booting...");
	stage		= 0;
	spoly		= 0;
	modelids	= 0;
	conearg[1]	= 0.0;
	fcarg		= 0.0;
	fscalenow	= SCENE2_CPU_SCALE_MIN + SCENE2_CPU_SCALE_RANGE;
	fscaletarg	= SCENE2_CPU_SCALE_MIN + SCENE2_CPU_SCALE_RANGE;
	npoly		= 0;
	fps			= 0.0f;
	fpsframe	= 0;
	fpssum		= 0.0f;
	fpsavg		= 0.0f;
	fpscount	= 0;
	putmodels	= 0;

	memset( pmdl, 0x00, sizeof( HiyoGL_Model ) * (cpumax + 1) );

	hiyoGL_Scene_Display = hiyoGL_Scene2_Display;
	hiyoGL_PutLog("[Scene2] Initialized Parameter");

	hiyoGL_Scene2_AddModel(0);
	hiyoGL_PutLog("[Scene2] Initialized CPU Model");

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 10.0f);
	glFogfv(GL_FOG_COLOR, fogcolor);
	glHint(GL_FOG_HINT, GL_FASTEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightambi);

	hiyoGL_PutLog("[Scene2] Start!");
	starttime	= hiyoGL_GetTime();
	nexttime	= starttime + STEPTIME;
	fpstime		= starttime + FPSSTEP;

	return;
}

/* EOF of hiyogls2.c */

