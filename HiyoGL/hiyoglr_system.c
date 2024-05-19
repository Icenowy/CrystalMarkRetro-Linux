/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/*hiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyo

                           <CrystalMark Retro Module>

                              HiyoGL Version 1.00

hiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyohiyo*/

/* File: hiyogl.c - hiyoGL Main Src Code */

#if defined(_WIN32) || defined(_WIN64)
	#define _CRT_SECURE_NO_WARNINGS
	#include<windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<GL/gl.h>
#include<GL/glu.h>

#if defined(_WIN32) || defined(_WIN64)
//	#include<Timeapi.h>
	#pragma comment(lib, "winmm.lib")
#else
	// for FreeBSD / Linux 
	#if defined(__FreeBSD__)
		#include<sys/time.h>
		#include<sys/sysctl.h>
	#endif
	#if defined(__linux__)
		#include<time.h>
	#endif
#endif

#define	LIBGLDB_SERIAL	2004020101	/* Version 0.5 */
#include"hiyoglr_libgld.h"

#define	HIYOGL_SYSTEM_SRC
#include"hiyoglr.h"

/* ErrorLog OutPut Functinos ------------------------------*/
void hiyoGL_CreateLog(char *str)
{
	FILE	*fe;
#if defined(_WIN32) || defined(_WIN64)
	if((fe = _wfopen(logFilePath, L"w")) == NULL)
#else
	if((fe = fopen(HIYOGL_LOG_FILE, "w")) == NULL)
#endif
		{ return; }
	fprintf(fe, "%s\n", str);
	fclose(fe);

	return;
}

void hiyoGL_PutLog(char *str)
{
	FILE	*fe;
#if defined(_WIN32) || defined(_WIN64)
	if((fe = _wfopen(logFilePath, L"a")) == NULL)
#else
	if((fe = fopen(HIYOGL_LOG_FILE, "a")) == NULL)
#endif
		{ return; }
	fprintf(fe, "%s\n", str);
	fclose(fe);

	return;
}

void hiyoGL_PutLogString(char *title, char *str)
{
	FILE	*fe;
#if defined(_WIN32) || defined(_WIN64)
	if((fe = _wfopen(logFilePath, L"a")) == NULL)
#else
	if((fe = fopen(HIYOGL_LOG_FILE, "a")) == NULL)
#endif
		{ return; }
	fprintf(fe, "%s=%s\n", title, str);
	fclose(fe);

	return;
}

void hiyoGL_PutLogInt(char *title, int dat)
{
	FILE	*fe;
#if defined(_WIN32) || defined(_WIN64)
	if((fe = _wfopen(logFilePath, L"a")) == NULL)
#else
	if((fe = fopen(HIYOGL_LOG_FILE, "a")) == NULL)
#endif
		{ return; }
	fprintf(fe, "%s=%d\n", title, dat);
	fclose(fe);

	return;
}

void hiyoGL_PutLogDWord(char *title, unsigned long dat)
{
	FILE	*fe;
#if defined(_WIN32) || defined(_WIN64)
	if((fe = _wfopen(logFilePath, L"a")) == NULL)
#else
	if((fe = fopen(HIYOGL_LOG_FILE, "a")) == NULL)
#endif
		{ return; }
	fprintf(fe, "%s=%lu\n", title, dat);
	fclose(fe);

	return;
}
/* EOL of ErrorLog Output Functinos ------------------------*/

/* Get CPU model name --------------------------------------*/
void hiyoGL_GetCPUModelName(char *cpumodel, size_t buflen)
{

#if defined(_WIN32) || defined(_WIN64)
	DWORD	type = REG_SZ;
	ULONG	size = 256 * sizeof(CHAR);
	HKEY	hKey = NULL;
#elif defined(__FreeBSD__)
	int		mib[2];
	size_t	ctllen;
#endif

#if defined(_WIN32) || defined(_WIN64)
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
			0, KEY_READ, &hKey) == ERROR_SUCCESS)	{
		RegQueryValueExA(hKey, "ProcessorNameString", NULL, &type, (LPBYTE)cpumodel, &size);
		RegCloseKey(hKey);
	}

#elif defined(__FreeBSD__)
	mib[0]	= CTL_HW;
	mib[1]	= HW_MODEL;
	sysctl(mib, 2, NULL, &ctllen, NULL, 0);
	if((buflen-1) > ctllen)
		{ sysctl(mib, 2, cpumodel, &ctllen, NULL, 0); }
#elif defined(__linux__)
	strncpy(cpumodel, "TODO", buflen);
#endif

    return;
}

/* Get Max CPUs from Physical memory ------------*/
unsigned int hiyoGL_GetMaxCPU(void)
{
	unsigned int	cpus;
	unsigned long	mem;
#if defined(__FreeBSD__)
	int				mib[2];
	unsigned long	bsdmem;
	size_t			ctllen;
#elif defined(_WIN32) || defined(_WIN64)
	MEMORYSTATUSEX	statex;
#endif

#if defined(__FreeBSD__)
	bsdmem	= 0;
	mib[0]	= CTL_HW;
	mib[1]	= HW_PHYSMEM;
	ctllen	= sizeof(bsdmem);
	sysctl(mib, 2, &bsdmem, &ctllen, NULL, 0);

	mem		= bsdmem / 1024 / 1024;

#elif defined(_WIN32) || defined(_WIN64)
	statex.dwLength	= sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	mem		= (unsigned long)((unsigned long long)statex.ullTotalPhys / 1024 / 1024);

#else
	mem		= 4096;

#endif

	physmemory	= mem;

	if(512 >= mem)			{ cpus = (1<< 8); }
	else if(4096 >= mem)	{ cpus = (1<<16); }
	else if(65536 >= mem)	{ cpus = (1<<20); }
	else					{ cpus = (1<<24); }

	return cpus;
}

/* Init Timer -----------------------------------*/
int hiyoGL_InitTimer(void)
{
#if defined(_WIN32) || defined(_WIN64)
	dw_tmboot	= timeGetTime();
#else
	// for FreeBSD / Linux
	clock_gettime( CLOCK_REALTIME, &ts_boot );

	ts_boot.tv_nsec	= 1000000000 - ts_boot.tv_nsec;
#endif
	return 0;
}

/* Get Timer ------------------------------------*/
GLuint hiyoGL_GetTime(void)
{
	GLuint	elapsed;

#if defined(_WIN32) || defined(_WIN64)
	return (GLuint)(timeGetTime() - dw_tmboot);
#else
	// for FreeBSD / Linux
	struct	timespec	ts_now;
	long	nanodiff;

	clock_gettime( CLOCK_REALTIME, &ts_now );
	nanodiff	= ts_now.tv_nsec + ts_boot.tv_nsec;
	if( 1000000000 > nanodiff )
		{ elapsed	= ts_now.tv_sec - ts_boot.tv_sec; }
	else	{
		elapsed	= ts_now.tv_sec - ts_boot.tv_sec + 1;
		nanodiff	-= 1000000000;
	}
	
	elapsed	= (elapsed * 1000) + (nanodiff / 1000000);
#endif
	return elapsed;
}

/* Load Texture from GC2file --------------------*/
int hiyoGL_LoadRegistTexture(GLuint *texid, char *texfname)
{
	int				err;
	GC2_INFO		*gc2;
	unsigned char	*pdata;
	unsigned long	sizex;
	unsigned long	sizey;
	GLuint			tid;

	// Load GC2 file ---
	gc2 = GC2_InitInfo();
	err = GC2_Load_File(gc2, texfname);
	if(0 != err)    {
		hiyoGL_PutLogString("[Common] ERROR! - Fail load GC2 TextureFile", texfname);
		return -1;
	}
	pdata = GC2_CopyPictureData(GC2Info_RawSize(gc2), GC2Info_RawData(gc2));
	sizex = GC2Info_X(gc2);
	sizey = GC2Info_Y(gc2);
	GC2_FreeInfo(gc2);

	// Regist Texture Data for OpenGL ---
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizex, sizey, 0, GL_RGB, GL_UNSIGNED_BYTE, pdata);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pdata);

	*texid = tid;

	return 0;
}

/* ReShape Function -----------------------------*/
void hiyoGL_Reshape(int w,int h)
{
	scrwidth	= w;
	scrheight	= h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, ((GLfloat)w / (GLfloat)h ), 5.0 , PERSPECTIVE_END);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);

	return;
}

/* Check OpenGL environment ---------------------*/
void hiyoGL_CheckOpenGL(void)
{
	const GLubyte *glVer = glGetString(GL_VERSION);
	const GLubyte *glVendor = glGetString(GL_VENDOR);
	const GLubyte *glRenderer = glGetString(GL_RENDERER);

	hiyoGL_PutLogString("[OpenGL] OpenGL Version", (char *)glVer);
	hiyoGL_PutLogString("[OpenGL] OpenGL Vendor", (char *)glVendor);
	hiyoGL_PutLogString("[OpenGL] OpenGL Renderer", (char *)glRenderer);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gltexmax);
	hiyoGL_PutLogInt( "[OpenGL] Max Texture size", (int)gltexmax);
	return;
}

/* hiyoGL Main Init. (Load) ---------------------*/
int hiyoGL_MainInit(void)
{	
	int		err;

	cpumax	= hiyoGL_GetMaxCPU();
	hiyoGL_PutLogInt( "[System] Max CPU models", (int)cpumax);

	pmdl = malloc( (cpumax + 1) * sizeof( HiyoGL_Model ) );
	if( NULL == pmdl )	{
		hiyoGL_PutLog("[System] ERROR! - Can't alloc CPU models memory.");
		return -1;
	}
	memset( pmdl, 0x00, sizeof( HiyoGL_Model ) * (cpumax + 1) );

	hiyoGL_InitTimer();

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE,GL_SRC_ALPHA);
    
    regmodels = 0;

	hiyoGL_CheckOpenGL();
    
    /* Model Init. */
    LibGLD_Init(&(model[0]), HIYOGL_MODEL_PENTIUM4
        , SRCFLAG_ARRAY|SRCFLAG_OPTMEMORY1|SRCFLAG_OPTMEMORY2|SRCFLAG_WIREFRAME
        , LIBGLDB_SERIAL);
    regmodels++;
    LibGLD_Init(&(model[1]), HIYOGL_MODEL_PALOMINO
        , SRCFLAG_ARRAY|SRCFLAG_OPTMEMORY1|SRCFLAG_OPTMEMORY2|SRCFLAG_WIREFRAME
        , LIBGLDB_SERIAL);
    regmodels++;
    
    LibGLD_Init(&(back), HIYOGL_MODEL_BACK
        , SRCFLAG_ARRAY|SRCFLAG_OPTMEMORY1|SRCFLAG_OPTMEMORY2|SRCFLAG_WIREFRAME
        , LIBGLDB_SERIAL);

	err = hiyoGL_Scene2_GenerateCone();
	if(err != 0)	{
		hiyoGL_PutLog("[System] ERROR! - Can't init. Scene2 background.");
		return -2;
	}

    return 0;
}

/* Terminate hiyoGL -----------------------------*/
void hiyoGL_Term(void)
{
    FILE		*fans;
	char		cpumodel[128];
	char		temp[256];
	const GLubyte *glVer = glGetString(GL_VERSION);
	const GLubyte *glVendor = glGetString(GL_VENDOR);
	const GLubyte *glRenderer = glGetString(GL_RENDERER);
#if defined(_WIN32) || defined(_WIN64)
	TCHAR		*ptrEnd;
	TCHAR		path[MAX_PATH];
	TCHAR		currentResultFilePath[MAX_PATH];
	TCHAR		currentLogFilePath[MAX_PATH];
	errno_t		terr;
#endif

	hiyoGL_GetCPUModelName(cpumodel, sizeof(cpumodel));

    /* System Info. Write */
#if defined(_WIN32) || defined(_WIN64)
	if((terr = _wfopen_s(&fans, resultFilePath, L"a")) != 0)	{
		hiyoGL_PutLogString("[System] ERROR! - Can't open result file  errno", strerror(terr));
		exit(1);
	}
#else
	if((fans = fopen(HIYOGL_RESULT_FILE, "a")) == NULL)	{
		hiyoGL_PutLog("[System] ERROR! - Can't open result file.");
		exit(1);
	}
#endif

	fprintf(fans, "\n[hiyoGL_SystemInformation]\n");
	fprintf(fans, "hiyoGLretro=%s (%s)\n", HIYOGL_VERSION, HIYOGL_EDITION);
    fprintf(fans, "DisplaySize=%dx%d\n", scrwidth, scrheight);
    fprintf(fans, "DisplayColor=%d\n", scrcolor);
    fprintf(fans, "OpenGL_Version=%s\n", (char *)glVer);
    fprintf(fans, "OpenGL_Vendor=%s\n", (char *)glVendor);
    fprintf(fans, "OpenGL_Renderer=%s\n", (char *)glRenderer);
    fprintf(fans, "CPUModel=%s\n", cpumodel);
    fprintf(fans, "PhysicalMemory=%lu\n", physmemory);
	fclose(fans);

	/* Output score for Log */
	snprintf(temp, sizeof(temp)-1, "[SysInf] DisplaySize=%dx%d", scrwidth, scrheight);
	hiyoGL_PutLog(temp);
	hiyoGL_PutLogInt("[SysInf] DisplayColor", scrcolor);
	hiyoGL_PutLogString("[SysInf] OpenGL_Version", (char*)glVer);
	hiyoGL_PutLogString("[SysInf] OpenGL_Vendor", (char*)glVendor);
	hiyoGL_PutLogString("[SysInf] OpenGL_Renderer", (char*)glRenderer);
	hiyoGL_PutLogString("[SysInf] CPUModel", cpumodel);
	hiyoGL_PutLogDWord("[SysInf] PhysicalMemory", physmemory);

#if defined(_WIN32) || defined(_WIN64)

	hiyoGL_PutLog("[System] Copy result&log files");

	GetModuleFileName(NULL, path, MAX_PATH);
	if((ptrEnd = wcsrchr(path, '\\')) != NULL)
		{ *ptrEnd = '\0'; }

	wsprintf(currentResultFilePath, L"%s\\%s", path, HIYOGL_RESULT_FILE);
	wsprintf(currentLogFilePath, L"%s\\%s", path, HIYOGL_LOG_FILE);

	CopyFile(resultFilePath, currentResultFilePath, FALSE);
	CopyFile(logFilePath, currentLogFilePath, FALSE);

	hiyoGL_PutLog("[System] Complete copy result&log files");

	/// Memory Map for Benchmark result
	TCHAR name[32];
	wsprintfW(name, L"CrystalMarkRetro");
	TCHAR result[1024];
	TCHAR openGLVersion[256];
	TCHAR openGLVendor[256];
	TCHAR openGLRenderer[256];

	MultiByteToWideChar(CP_ACP, 0, glVer, -1, openGLVersion, 256);
	MultiByteToWideChar(CP_ACP, 0, glVendor, -1, openGLVendor, 256);
	MultiByteToWideChar(CP_ACP, 0, glRenderer, -1, openGLRenderer, 256);

	wsprintfW(result, 
L"\
Scene1_Score=%I64d\n\
Scene1_Lines=%I64d\n\
Scene1_CPU=%I64d\n\
Scene2_Score=%I64d\n\
Scene2_Polygons=%I64d\n\
Scene2_CPU=%I64d\n\
HiyoGL_Version=%s\n\
OpenGL_Version=%s\n\
OpenGL_Vendor=%s\n\
OpenGL_Renderer=%s\n\
", 
score[0],
((triangles[0] * 3) / 2),
cpus[0],
score[1] / 10,
triangles[1],
cpus[1],
HIYOGL_VERSION_UTF16,
openGLVersion,
openGLVendor,
openGLRenderer
);

	auto size = 2048;

	HANDLE hSharedMemory = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name);
	if (hSharedMemory != NULL)
	{
		TCHAR* pMemory = (TCHAR*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (pMemory != NULL)
		{
			RtlCopyMemory(pMemory, result, size);
			UnmapViewOfFile(pMemory);
			CloseHandle(hSharedMemory);
		}
	}

#endif

    /* Model Memory Free */
    LibGLD_Free(&back);
    LibGLD_Free(&(model[1]));
    LibGLD_Free(&(model[0])); 

	free( pmdl );

	hiyoGL_TermProgram();
    return;
}

/* EOF of hiyoGL.c */

