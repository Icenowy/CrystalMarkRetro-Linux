/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/* File: hiyoglr.h - hiyoGLretro Main Header Code */

/*#include<windows.h>*/ 
#include<GL/gl.h>
#if defined(_WIN32) || defined(_WIN64)
#else
	#include<sys/time.h>
#endif

/* VERSION */
#if defined(_WIN32) || defined(_WIN64)
	#ifdef _M_ARM64
		#define HIYOGL_EDITION			 "ARM64"
		#define HIYOGL_EDITION_UTF16	L"ARM64"
	#elif _M_X64
		#define HIYOGL_EDITION			 "x64"
		#define HIYOGL_EDITION_UTF16	L"x64"
	#else
		#define HIYOGL_EDITION			 "x86"
		#define HIYOGL_EDITION_UTF16	L"x86"
	#endif
#elif defined(__FreeBSD__)
		#define HIYOGL_EDITION			"FreeBSD(amd64)"
#endif

#define HIYOGL_VERSION			"202403252135"
#define HIYOGL_VERSION_UTF16	L"1.0.0.2024"

/* Model & Data File Name */
#define	HIYOGL_LOGO_BASEFILE	"logo0"
#define	HIYOGL_MODEL_PENTIUM4	"willamette.gld"
#define	HIYOGL_MODEL_PALOMINO	"palomino.gld"
#define	HIYOGL_MODEL_BACK		"wireback.gld"

#if defined(_WIN32) || defined(_WIN64)
	#define	HIYOGL_RESULT_FILE		L"CMROGL.ini"
	#define	HIYOGL_LOG_FILE			L"CMROGL.log"
#else
	#define	HIYOGL_RESULT_FILE		"CrystalMarkRetroOGL.ini"
	#define	HIYOGL_LOG_FILE			"CrystalMarkRetroOGL.log"
#endif

/* Define Parameter */
#define	WIN_CLASSNAME				"HiyoGLRetro"
#define	CAPTION_TITLE				"CrystalMark Retro OpenGL Benchmark"
#define	WINDOW_DEFSIZE_X			1280
#define	WINDOW_DEFSIZE_Y			960
#define	PERSPECTIVE_END				1600.0
#define	FPSSTEP						100
#define	STEPTIME					4000
#define	STOPFPS						5.0
#define	CPUADD_LIMITFPS_DIV8		7.5f
#define	CPUADD_LIMITFPS_DIV4		10.0f
#define	CPUADD_LIMITFPS_DIV2		20.0f

#define	SCORELINE_X					-0.979f
#define	SCORELINE_Y1				0.95f
#define	SCORELINE_Y2				0.91f
#define	SCORELINE_Y3				0.87f

#define	SCENE0_PARTSX				40
#define	SCENE0_PARTSY				20
#define	SCENE0_STAGE0_TIME			3000
#define	SCENE0_STAGE1_TIME			5000
#define	SCENE0_STAGE2_TIME			1000

#define	SCENE2_BACK_DIVZ			48
#define	SCENE2_BACK_DIVARG			72
#define	SCENE2_COREIMG_MAX			32
#define	SCENE2_BACK_RADIUS_FRONT	150.0f
#define	SCENE2_BACK_RADIUS_END		30.0f
#define	SCENE2_CPU_FLIGHT_SPEED		2.5f
#define	SCENE2_CPU_BORN_POS_MIN		500
#define	SCENE2_CPU_BORN_POS_RANGE	500
#define	SCENE2_BACK_DEPTH_END		((GLfloat)PERSPECTIVE_END - 30.0f)
#define	SCENE2_FOG_END				((GLfloat)PERSPECTIVE_END - 30.0f)
#define	SCENE2_FOG_FRONT			500.0f
#define	SCENE2_CPU_SCALE_RANGE		0.74f
#define	SCENE2_CPU_SCALE_MIN		0.06f
#define	SCENE2_CPU_SCALE_LASTBITS	13


#ifdef HIYOGL_SCENE2_SRC
char *Scene2_CoreImageFile[] = {
	"alderlake.gc2",
	"argon.gc2",
	"baikaia57.gc2",
	"cannonlake.gc2",
	"chomperxt.gc2",
	"coffeelake.gc2",
	"coppermine.gc2",
	"dali.gc2",
	"durango.gc2",
	"fermi.gc2",
	"g80.gc2",
	"hawaii.gc2",
	"kabylake.gc2",
	"katmai.gc2",
	"knightslanding.gc2",
	"matisse.gc2",
	"navi14.gc2",
	"northwood.gc2",
	"oberon.gc2",
	"pascal.gc2",
	"pinnacleridge.gc2",
	"polaris.gc2",
	"raptorlake.gc2",
	"ravenridge.gc2",
	"rome.gc2",
	"sharptooth.gc2",
	"skylakesp.gc2",
	"tesla.gc2",
	"tualatin.gc2",
	"turing.gc2",
	"ultrasparct2.gc2",
	"vangogh.gc2"
};

#endif

typedef enum { GLUT, WINAPP }	WindowMode;
	
/* Model Struct */
typedef struct {
	GLfloat		pos[3];
	GLfloat		arg[3];
	Src_Model	*smodel;
} HiyoGL_Model;

/* Global Parameter */
#ifdef HIYOGL_MAIN_SRC
	#define	GLOBAL
#else
	#define	GLOBAL	extern
#endif
GLOBAL	Src_Model		model[4];
GLOBAL	Src_Model		back;
GLOBAL	HiyoGL_Model	*pmdl;
GLOBAL	void 			(* hiyoGL_Scene_Display)(void);
GLOBAL	int				stage;
#ifdef DEBUG
	GLOBAL	int			startmode;
#endif

GLOBAL	int				scrwidth;
GLOBAL	int				scrheight;
GLOBAL	int				scrcolor;
GLOBAL	GLint			gltexmax;
GLOBAL	unsigned long	physmemory;

GLOBAL	GLuint			nexttime;
GLOBAL	GLuint			stagetime;
GLOBAL	GLuint			starttime;
GLOBAL	GLuint			boottime;

GLOBAL	GLfloat			fcarg;

GLOBAL	GLfloat			fscalenow;
GLOBAL	GLfloat			fscalediff;
GLOBAL	GLfloat			fscaletarg;

GLOBAL	GLuint			fpsframe;
GLOBAL	GLuint			fpstime;
GLOBAL	GLfloat			fps;
GLOBAL	GLfloat			fpssum;
GLOBAL	GLfloat			fpsavg;

GLOBAL	unsigned int	cpumax;
GLOBAL	unsigned int	fpscount;
GLOBAL	unsigned int	putmodels;
GLOBAL	unsigned int	regmodels;
GLOBAL	unsigned int	modelids;
GLOBAL	unsigned long long	spoly;
GLOBAL	unsigned long long	npoly;
GLOBAL	unsigned long long	score[2];
GLOBAL	unsigned long long	triangles[2];
GLOBAL	unsigned long long	cpus[2];
GLOBAL	WindowMode		winmode;
GLOBAL	char			*logofile;
#if defined(_WIN32) || defined(_WIN64)
	GLOBAL	TCHAR		tempPath[MAX_PATH];
	GLOBAL	TCHAR		resultFilePath[MAX_PATH];
	GLOBAL	TCHAR		logFilePath[MAX_PATH];
	GLOBAL	DWORD		dw_tmboot;
#else
	GLOBAL	struct timespec	ts_boot;
#endif

static	GLfloat			lightpos[]	= {0.0, 0.0, 0.0, 1.0};
static	GLfloat			lightspec[]	= {1.0, 1.0, 1.0, 1.0};
static	GLfloat			lightdiff[]	= {1.0, 1.0, 1.0, 1.0};
static 	GLfloat			lightambi[]	= {0.0, 0.0, 0.0, 1.0};
static	GLfloat			fogcolor[]	= {0.0, 0.0, 0.0, 1.0}; 

/* Function Prototype */
#ifdef HIYOGL_MAIN_SRC
	#define	FUNC_MAIN	
#else
	#define	FUNC_MAIN	extern
#endif
FUNC_MAIN void hiyoGL_PutString(GLfloat, GLfloat, char *);
FUNC_MAIN void hiyoGL_SwapBuffer(void);
FUNC_MAIN void hiyoGL_TermProgram(void);

#ifdef DEBUG
	#define Debug_PutLog(n)			hiyoGL_PutLog(n)
	#define Debug_PutLogString(m,n)	hiyoGL_PutLogString((m),(n))
	#define Debug_PutLogInt(m,n)	hiyoGL_PutLogInt((m),(n))
#else
	#define Debug_PutLog(n)
	#define Debug_PutLogString(m,n)
	#define Debug_PutLogInt(m,n)
#endif

#ifdef HIYOGL_SYSTEM_SRC
	#define	FUNC_SYSTEM
#else
	#define	FUNC_SYSTEM	extern
#endif
FUNC_SYSTEM void hiyoGL_CreateLog(char *);
FUNC_SYSTEM void hiyoGL_PutLog(char *);
FUNC_SYSTEM void hiyoGL_PutLogString(char *,char *);
FUNC_SYSTEM void hiyoGL_PutLogInt(char *,int);
FUNC_SYSTEM void hiyoGL_PutLogDWord(char *,unsigned long);
FUNC_SYSTEM GLuint hiyoGL_GetTime(void);
FUNC_SYSTEM int hiyoGL_LoadRegistTexture(GLuint *, char *);
FUNC_SYSTEM void hiyoGL_Reshape(int,int);
FUNC_SYSTEM int hiyoGL_MainInit(void);
FUNC_SYSTEM void hiyoGL_Term(void);

#ifdef HIYOGL_SCENE0_SRC
	#define	FUNC_SCENE0
#else
	#define	FUNC_SCENE0	extern
#endif
FUNC_SCENE0 void hiyoGL_Scene0_Init(void);

#ifdef HIYOGL_SCENE1_SRC
	#define	FUNC_SCENE1	
#else
	#define	FUNC_SCENE1	extern
#endif
FUNC_SCENE1 void hiyoGL_Scene1_Display(void);
FUNC_SCENE1 void hiyoGL_Scene1_Init(void);

#ifdef HIYOGL_SCENE2_SRC
	#define	FUNC_SCENE2	
#else
	#define	FUNC_SCENE2	extern
#endif
FUNC_SCENE2 int hiyoGL_Scene2_GenerateCone(void);
FUNC_SCENE2 void hiyoGL_Scene2_Display(void);
FUNC_SCENE2 void hiyoGL_Scene2_Init(void);

#define	Pi				(3.141592865f)
#define	RandFloat()	(((GLfloat)(rand() % RAND_MAX)) / (GLfloat)RAND_MAX)

/* EOF of hiyogl.h */

