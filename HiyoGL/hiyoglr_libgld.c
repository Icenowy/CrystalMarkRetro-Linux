/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

/*
	libgld.c
*/

#if defined(_WIN32) || defined(_WIN64)
	#define	_CRT_SECURE_NO_WARNINGS
	#include<windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<GL/gl.h>

#define SRC_LIBGLD
#include"hiyoglr_libgld.h"
#include"hiyoglr.h"

#define	LIBGLDC_SERIAL	2004020101


static	void (* LibGLD_SwitchTexture[12])(void);
static	void (* LibGLD_BindTexture[3])(SRC_POLYGON *);
static	void (* LibGLD_DrawPolygon[7])(SRC_POLYGON *);
static	SRC_STATUS	sstat;

static	GC2_BIT_DATA	*signmap[4];
static	GC2_BIT_DATA	*digits[4];
static	unsigned char	*vertical[4];
static	unsigned char	*workmap[4];
static	unsigned char	datatop[4];



/* GC2 Inner Module =============================*/
GC2_INFO *GC2_InitInfo(void)
{
	GC2_INFO	*infop;

	if((infop=(GC2_INFO *)malloc(sizeof(GC2_INFO)))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc GC2 Header");
		return NULL;
	}

	GC2Info_Ver(infop)			= GC2_VER1;
	GC2Info_Colors(infop)		= GC2_NODATA;
	GC2Info_X(infop)			= 0;
	GC2Info_Y(infop)			= 0;
	GC2Info_Flag(infop)			= GC2_NOFLAG;
	GC2Info_RawData(infop)		= NULL;
	GC2Info_RawSize(infop)		= 0;
	GC2Info_Ext1Size(infop)		= 0;
	GC2Info_Ext1Data(infop)		= NULL;
	memset(GC2Info_Author(infop), 0x00, 64);
	memcpy(GC2Info_FileHeader(infop), GC2_DEF_FILEHEADER_STRING, 16);
	memset(GC2Info_Ext1Header(infop), 0x00, 16);

	return infop;
}

void GC2_FreeRaw(GC2_INFO *infop)
{
	if(GC2Info_RawData(infop) != NULL)
		{ free(GC2Info_RawData(infop)); }
	GC2Info_RawData(infop) = NULL;
	GC2Info_RawSize(infop) = 0;
	return;
}

void GC2_FreeExt1(GC2_INFO *infop)
{
	if(GC2Info_Ext1Data(infop)!=NULL)	{
		free(GC2Info_Ext1Data(infop));
		GC2Info_Ext1Data(infop) = NULL;
		GC2Info_Ext1Size(infop)	= 0;
		memset(GC2Info_Ext1Header(infop), 0x00, 16);
	}
	return;
}

unsigned char *GC2_CopyPictureData(unsigned long dsize, BinData src)
{
	unsigned char *dest = NULL;

	if((dest=(unsigned char *)malloc(dsize))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc copy Picture Data area");
		return NULL;
	}
	memcpy(dest, src, dsize);
	return dest;
}

static int GC2_Load_Header(GC2_INFO *gc2, unsigned char *header)
{
	unsigned char	*hptr;
	char	fheader[17];

	hptr = header;

	memcpy(fheader, hptr, 16);					hptr += 16;
	GC2Info_Ver(gc2) = GC2_GetWord(hptr);		hptr +=  2;
	GC2Info_Colors(gc2) = GC2_GetWord(hptr);	hptr +=  2;
	GC2Info_X(gc2) = GC2_GetDWord(hptr);		hptr +=  4;
	GC2Info_Y(gc2) = GC2_GetDWord(hptr);		hptr +=  4;
	GC2Info_RawSize(gc2) = GC2_GetDWord(hptr);	hptr +=  4;
	memcpy(GC2Info_Author(gc2), hptr, 64);		hptr += 64;
	GC2Info_Flag(gc2) = GC2_GetWord(hptr);		hptr +=  2;
	GC2Info_Ext1Size(gc2) = GC2_GetDWord(hptr);	hptr +=  4;
	hptr += 256;

	/* Error Check */
	if(strncmp(fheader, GC2Info_FileHeader(gc2), 16))	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't load GC2 Header");
		return -1;
	}

	return 0;
}

GC2_BIT_DATA *GC2_InitBitData(unsigned long bsize)
{
	GC2_BIT_DATA	*bdt = NULL;

	if((bdt=(GC2_BIT_DATA *)malloc(sizeof(GC2_BIT_DATA)))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc GC2_BIT_DATA area");
		goto GC2_InitBitData_Err1;
	}

	bdt->bdata = NULL;
	bdt->bptr = NULL;
	bdt->bloc = 7;

	if((bdt->bdata=(unsigned char *)malloc(bsize))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc bitdata area");
		return NULL;
	}
	bdt->bptr = bdt->bdata;
	memset(bdt->bdata, 0x00, bsize);

GC2_InitBitData_Err1:
	return bdt;
}

void GC2_FreeBitData(GC2_BIT_DATA *bdt)
{
	free(bdt->bdata);
	free(bdt);
	return;
}

unsigned char GC2_ReadBit(GC2_BIT_DATA *bdt, int bits)
{
	int				cnt;
	unsigned char	data = 0x00;

	for(cnt=(bits-1); cnt>=0; cnt--)	{
		data |=  (((*(bdt->bptr) >> bdt->bloc) & 0x01) << cnt);
		if(bdt->bloc==0)
			{ bdt->bloc = 7; bdt->bptr++; }
		else
			{ bdt->bloc--; }
	}
	return data;
}

static int GC2_Load2_Init(GC2_INFO *infop, int planes)
{
	int				cnt;
	unsigned long	vertmaxsize;
	unsigned long	signsize;
	unsigned long	digitsize;

	signsize = GC2Info_X(infop) * GC2Info_Y(infop) / 8;
	digitsize = ((signsize / 8) + 1) * 3;
	vertmaxsize = GC2Info_X(infop) * GC2Info_Y(infop);

	for(cnt=0; cnt<planes; cnt++)	{
		signmap[cnt] = GC2_InitBitData(signsize);
		digits[cnt] = GC2_InitBitData(digitsize);

		if((vertical[cnt]=(unsigned char *)malloc(vertmaxsize))==NULL)	{
			hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc vertical array");
			return -1;
		}

		if((workmap[cnt]=(unsigned char *)malloc(vertmaxsize))==NULL)	{
			hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc workmap array");
			return -2;
		}
	}
	return 0;
}

static int GC2_Load2_Free(GC2_INFO *infop, int planes)
{
	int		cnt;

	for(cnt=0; cnt<planes; cnt++)	{
		GC2_FreeBitData(signmap[cnt]);
		GC2_FreeBitData(digits[cnt]);
		free(vertical[cnt]);
		free(workmap[cnt]);
	}
	return 0;
}

static int GC2_Load2_MemLoad(GC2_INFO *infop, unsigned char *rdata, int planes)
{
	int				cnt;
	unsigned long	signsize;
	unsigned long	digitsize;
	unsigned long	vertsize[4];
	unsigned char	*rptr;

	rptr = rdata;

	signsize = GC2_GetDWord(rptr);		rptr += 4;
	digitsize = GC2_GetDWord(rptr);		rptr += 4;
	for(cnt=0; cnt<planes; cnt++)	{
		vertsize[cnt] = GC2_GetDWord(rptr);
		rptr += 4;
	}
	for(cnt=0; cnt<planes; cnt++)	{
		datatop[cnt] = GC2_GetByte(rptr);
		rptr++;
	}
	for(cnt=0; cnt<planes; cnt++)	{
		memcpy(signmap[cnt]->bdata, rptr, signsize);
		rptr += signsize;
	}
	for(cnt=0; cnt<planes; cnt++)	{
		memcpy(digits[cnt]->bdata, rptr, digitsize);
		rptr += digitsize;
	}
	for(cnt=0; cnt<planes; cnt++)	{
		memcpy(vertical[cnt], rptr, vertsize[cnt]);
		rptr += vertsize[cnt];
	}
	return 0;
}

static int GC2_Load2_Phase3(GC2_INFO *infop, int planes)
{
	unsigned long	cnt;
	unsigned char	*rptr;
	unsigned char	*gptr;
	unsigned char	*bptr;
	unsigned long	signsize;

	if(planes<3)	{ return 0; }
	signsize = GC2Info_X(infop) * GC2Info_Y(infop) / 8;

	rptr = signmap[0]->bdata;
	gptr = signmap[1]->bdata;
	bptr = signmap[2]->bdata;
	for(cnt=0; cnt<signsize; cnt++)	{
		*gptr++ ^= *rptr;
		*bptr++ ^= *rptr++;
	}
	return 0;
}

static int GC2_Load2_UnPressBlock(unsigned char data[8])
{
	unsigned char	srcbin[8];
	int				cnt;
	int				dig;

	for(dig=0; dig<8; dig++)	{
		srcbin[dig] = 0x00;
		for(cnt=0; cnt<8; cnt++)
			{ srcbin[dig] |= (((data[cnt] >> (7-dig)) & 0x01) << (7-cnt)); }
	}
	memcpy(data, srcbin, 8);
	return 0;
}

static int GC2_Load2_Phase2(GC2_INFO *infop, int planes)
{
	int				cnt;
	unsigned long	x;
	unsigned long	y;
	unsigned char	*vptr[4];
	unsigned char	*wptr[4];
	unsigned char	nums;

	for(cnt=0; cnt<planes; cnt++)	{
		vptr[cnt] = vertical[cnt];
		wptr[cnt] = workmap[cnt];
	}

	for(y=0; y<GC2Info_Y(infop); y++)	{
		for(x=0; x<GC2Info_X(infop); x+=8)	{
			for(cnt=0; cnt<planes; cnt++)		{
				nums = GC2_ReadBit(digits[cnt], 3);
				memset(wptr[cnt], 0x00, (7-nums));
				memcpy((wptr[cnt]+(7-nums)), vptr[cnt], (nums+1));
				vptr[cnt] += (nums + 1);
				GC2_Load2_UnPressBlock(wptr[cnt]);
				wptr[cnt] += 8;
			}
		}
	}
	return 0;
}

static unsigned char GC2_Load2_Phase1_GetData(int pl, unsigned char nows, unsigned char refs)
{	
	unsigned char	data;
	if(GC2_ReadBit(signmap[pl], 1)==1)	{ data = refs - nows; }
	else								{ data = nows + refs; }
	return data;
}

static unsigned char *GC2_Load2_Phase1(GC2_INFO *infop, int planes)
{	
	unsigned long	x;
	unsigned long	y;
	int				cnt;
	int				lines;
	unsigned int	tpp;
	unsigned char	*wptr[4];
	unsigned char	*pdata;
	unsigned char	*pptr;
	unsigned long	realsize = 0;

	lines = GC2Info_X(infop) * planes;
	realsize = GC2Info_X(infop) * GC2Info_Y(infop) * planes;
	if((pdata=(unsigned char *)malloc(realsize))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc bitmap data area.");
		return NULL;
	}
	pptr = pdata;

	for(cnt=0; cnt<planes; cnt++)	{
		wptr[cnt] = workmap[cnt];
		*pptr++ = datatop[cnt];
		tpp = GC2_ReadBit(signmap[cnt], 1);
		wptr[cnt]++;
	}

	for(x=1; x<GC2Info_X(infop); x++)	{
		for(cnt=0; cnt<planes; cnt++)	{
			*pptr = GC2_Load2_Phase1_GetData(cnt, (*wptr[cnt]), (*(pptr-planes)));
			pptr++;
			wptr[cnt]++;
		}
	}
	for(y=1; y<GC2Info_Y(infop); y++)	{
		for(cnt=0; cnt<planes; cnt++)	{
			*pptr = GC2_Load2_Phase1_GetData(cnt, (*wptr[cnt]), (*(pptr - lines)));
			pptr++;
			wptr[cnt]++;
		}

		for(x=1; x<GC2Info_X(infop); x++)	{
			for(cnt=0; cnt<planes; cnt++)	{
				tpp = ((*(pptr - lines)) + (*(pptr-planes))) / 2;
				*pptr++ = GC2_Load2_Phase1_GetData(cnt, (*wptr[cnt]), tpp);
				wptr[cnt]++;
			}
		}
	}
	return pdata;
}

unsigned char *GC2_Load_RawData_V200(GC2_INFO *gc2, unsigned char *rdata)
{
	int				planes;
	unsigned char	*pdata = NULL;

	if((gc2==NULL)||(rdata==NULL))	{ return NULL; }

	planes = GC2Info_Colors(gc2);
	if(isGC2_Alpha(gc2))	{ planes++; }

	if(GC2_Load2_Init(gc2, planes))	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't Init. RawData");
		return NULL;
	}
	GC2_Load2_MemLoad(gc2,rdata, planes);
	GC2_Load2_Phase3(gc2, planes);
	GC2_Load2_Phase2(gc2, planes);
	if((pdata = GC2_Load2_Phase1(gc2, planes))==NULL)
		{ hiyoGL_PutLog("    [GC2] ERROR! - Can't load GC2 Phase1"); }
	GC2_Load2_Free(gc2, planes);

	GC2Info_RawSize(gc2) = GC2Info_X(gc2) * GC2Info_Y(gc2) * planes;
	return pdata;
}


static BinData GC2_Load_RawData_V100(GC2_INFO *gc2, unsigned char *rdata)
{
	if((gc2==NULL)||(rdata==NULL))	{
		hiyoGL_PutLog("    [GC2] ERROR! - NULL ptr. operand.");
		return NULL;
	}
	return GC2_CopyPictureData(GC2Info_RawSize(gc2), rdata);
}

static int GC2_Load_RawData(GC2_INFO *gc2, unsigned char *rdata)
{
	if((gc2==NULL)||(rdata==NULL))	{
		hiyoGL_PutLog("    [GC2] ERROR! - NULL ptr. operand.");
		return 1;
	}
	if(GC2Info_Ver(gc2)==GC2_VERSION_100)
		{ GC2Info_RawData(gc2) = GC2_Load_RawData_V100(gc2, rdata); }
	else if(GC2Info_Ver(gc2)==GC2_VERSION_200)
		{ GC2Info_RawData(gc2) = GC2_Load_RawData_V200(gc2, rdata); }
	return 0;
}

static int GC2_Load_Ext1Data(GC2_INFO *gc2, unsigned char *bdata)
{
	memcpy(GC2Info_Ext1Header(gc2), bdata, 16);

	if((GC2Info_Ext1Data(gc2)=(unsigned char *)malloc(GC2Info_Ext1Size(gc2)))==NULL)	{
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc GC2 Ext. Header area.");
		return 1;
	}
	memcpy(GC2Info_Ext1Data(gc2), (bdata+16), GC2Info_Ext1Size(gc2));
	return 0;
}

int GC2_Load_File(GC2_INFO *gc2, char *fname)
{
	int				err = 0;
	FILE			*fr;
	unsigned char	header[GC2_FILE_HEADER_SIZE];
	unsigned char	*rdata;
	char			tmpstr[128];

	tmpstr[0]	= '-';
	tmpstr[1]	= '\0';

	if(fname==NULL)	{
		err = 10;
		hiyoGL_PutLog("    [GC2] ERROR! - Filename operand is NULL");
		goto GC2_Load_File_Err;
	}

	if((fr=fopen(fname, "rb"))==NULL)	{
		err = 1;
		goto GC2_Load_File_Err;
	}

	/* Header Read */
	if(fread(header,GC2_FILE_HEADER_SIZE,1,fr)!=1)	{
		err = 2;
		hiyoGL_PutLog("    [GC2] ERROR! - Can't read file header.");
		goto GC2_Load_File_Err;
	}
	if(GC2_Load_Header(gc2,header))	{
		err = 3;
		goto GC2_Load_File_Err;
	}

	/* Load Picture Data */
	if((rdata=(unsigned char *)malloc(GC2Info_RawSize(gc2)))==NULL)	{
		err = 4;
		hiyoGL_PutLog("    [GC2] ERROR! - Can't alloc file read area.");
		goto GC2_Load_File_Err;
	}
	if(fread(rdata, GC2Info_RawSize(gc2), 1, fr)!=1)	{
		err = 5;
		hiyoGL_PutLog("    [GC2] ERROR! - Can't read file data.");
		goto GC2_Load_File_Err;
	}
	GC2_Load_RawData(gc2, rdata);
	free(rdata);

	snprintf(tmpstr, sizeof(tmpstr), "%lux%lu:%u",
				GC2Info_X(gc2), GC2Info_Y(gc2), GC2Info_Colors(gc2) );

GC2_Load_File_Err:
	hiyoGL_PutLogString("    [GC2] File Load ", fname);
	return err;
}

int GC2_Load_Binary(GC2_INFO *gc2, unsigned char *bdata)
{
	int				err = 0;
	unsigned char	*bptr;
	char			tmpstr[128];

	tmpstr[0]	= '-';
	tmpstr[1]	= '\0';

	if((gc2==NULL)||(bdata==NULL))	{
		err = 1;
		hiyoGL_PutLog("    [GC2] ERROR! - NULL ptr. operand");
		goto GC2_Load_Binary_Err1;
	}
	bptr = bdata;

	/* Header Read */
	if(GC2_Load_Header(gc2, bptr))	{
		err = 2;
		hiyoGL_PutLog("    [GC2] ERROR! - Can't load GC2 binary header.");
		goto GC2_Load_Binary_Err1;
	}
	bptr += GC2_FILE_HEADER_SIZE;

	/* Load Picture Data */
	if(GC2_Load_RawData(gc2, bptr))	{
		err = 3;
		hiyoGL_PutLog("    [GC2] ERROR! - Can't load GC2 binary data.");
		goto GC2_Load_Binary_Err1;
	}
	bptr += GC2Info_RawSize(gc2);

	/* Load Extension 1 Data */
	if(GC2Info_Ext1Size(gc2)>0)
		{ GC2_Load_Ext1Data(gc2, bptr); }

	snprintf(tmpstr, sizeof(tmpstr), "%lux%lu:%u",
				GC2Info_X(gc2), GC2Info_Y(gc2), GC2Info_Colors(gc2) );

GC2_Load_Binary_Err1:
	hiyoGL_PutLogString("    [GC2] Loaded GC2 binary", tmpstr);
	return err;
}

/* public - GC2 Free InfoHeader ----------------------*/
void GC2_FreeInfo(GC2_INFO *infop)
{
	if(infop==NULL)		{
		hiyoGL_PutLog("    [GC2] ERROR! - NULL ptr. operand");
		return;
	}

	if(GC2Info_RawData(infop) != NULL)
		{ free(GC2Info_RawData(infop)); }
	GC2_FreeExt1(infop);
	free(infop);
	return;
}



/* Load GLD file Section ========================*/
/* Get Header Data Number -----------------------*/
static unsigned long GetDataNumber(FILE *fr)
{
	unsigned long	num;
	unsigned char	tmpdata[4];

	if(fread(tmpdata, 4, 1, fr)!=1)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't read data number.");
		return 0;
	}
	num = GetUDWord(tmpdata);
	return num;
}

/* Load Vertex ----------------------------------*/
static GLfloat *LibGLD_LoadVertex(FILE *fr, unsigned long *vnum)
{
	GLfloat			*vdata = NULL;
	GLfloat			*vptr;
	unsigned long	cnt;
	unsigned long	num = 0;
	unsigned char	tmpdata[12];

	if((num=GetDataNumber(fr))==0)	{
		hiyoGL_PutLog("  [GLD] No Vertex data.");
		goto LibGLD_LoadVertex_Err1;
	}

	if((vdata=(GLfloat *)malloc(sizeof(GLfloat) * num * 3))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Vertex area.");
		goto LibGLD_LoadVertex_Err1;
	}
	vptr = vdata;

	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, 12, 1, fr)!=1)	{
			free(vdata);
			vdata = NULL;
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Vertex data.");
			goto LibGLD_LoadVertex_Err1;
		}
		*vptr++ = GetFloat(tmpdata +  0);
		*vptr++ = GetFloat(tmpdata +  4);
		*vptr++ = GetFloat(tmpdata +  8);
	}

LibGLD_LoadVertex_Err1:
	*vnum = num;
	return vdata;
}

/* Load Vertec Normal Vector --------------------*/
static GLfloat *LibGLD_LoadVtxNormal(FILE *fr, unsigned long *vnum)
{
	GLfloat			*vdata = NULL;
	GLfloat			*vptr;
	unsigned long	cnt;
	unsigned long	num = 0;
	unsigned char	tmpdata[12];

	if((num=GetDataNumber(fr))==0)	{
		hiyoGL_PutLog("  [GLD] No Normal Vertex data.");
		goto LibGLD_LoadVtxNormal_NoNormal;
	}

	if((vdata=(GLfloat *)malloc(sizeof(GLfloat) * num * 3))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Normal Vertex area.");
		goto LibGLD_LoadVtxNormal_Err1;
	}
	vptr = vdata;

	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, 12, 1, fr)!=1)	{
			free(vdata);
			vdata = NULL;
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Normal Vertex data.");
			goto LibGLD_LoadVtxNormal_Err1;
		}
		*vptr++ = GetFloat(tmpdata +  0);
		*vptr++ = GetFloat(tmpdata +  4);
		*vptr++ = GetFloat(tmpdata +  8);
	}

LibGLD_LoadVtxNormal_NoNormal:
LibGLD_LoadVtxNormal_Err1:
	*vnum = num;
	return vdata;
}

/* Load Material --------------------------------*/
static SRC_MATERIAL *LibGLD_LoadMaterial(FILE *fr, unsigned long *dnum)
{
	SRC_MATERIAL		*sdata = NULL;
	SRC_MATERIAL		*sptr;
	unsigned char		tmpdata[SRC_MATERIAL_SIZE];
	unsigned long		num = 0;
	unsigned long		cnt;

	if((num=GetDataNumber(fr))==0)	{
		hiyoGL_PutLog("  [GLD] No Material data.");
		goto LibGLD_LoadMaterial_Err1;
	}

	if((sdata=(SRC_MATERIAL *)malloc(sizeof(SRC_MATERIAL) * num))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Material area.");
		goto LibGLD_LoadMaterial_Err1;
	}
	sptr = sdata;

	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, SRC_MATERIAL_SIZE, 1, fr)!=1)		{
			free(sdata);
			sdata = NULL;
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Material data.");
			goto LibGLD_LoadMaterial_Err1;
		}

		SrcDiffuse(sptr)[0] 	= GetFloat(tmpdata +  0);
		SrcDiffuse(sptr)[1] 	= GetFloat(tmpdata +  4);
		SrcDiffuse(sptr)[2] 	= GetFloat(tmpdata +  8);
		SrcDiffuse(sptr)[3] 	= GetFloat(tmpdata +  12);

		SrcSpecular(sptr)[0] 	= GetFloat(tmpdata +  16);
		SrcSpecular(sptr)[1] 	= GetFloat(tmpdata +  20);
		SrcSpecular(sptr)[2] 	= GetFloat(tmpdata +  24);
		SrcSpecular(sptr)[3] 	= GetFloat(tmpdata +  28);

		SrcAmbient(sptr)[0] 	= GetFloat(tmpdata +  32);
		SrcAmbient(sptr)[1] 	= GetFloat(tmpdata +  36);
		SrcAmbient(sptr)[2] 	= GetFloat(tmpdata +  40);
		SrcAmbient(sptr)[3] 	= GetFloat(tmpdata +  44);

		SrcEmission(sptr)[0] 	= GetFloat(tmpdata +  48);
		SrcEmission(sptr)[1] 	= GetFloat(tmpdata +  52);
		SrcEmission(sptr)[2] 	= GetFloat(tmpdata +  56);
		SrcEmission(sptr)[3] 	= GetFloat(tmpdata +  60);

		SrcShininess(sptr)	 	= GetFloat(tmpdata +  64);
		sptr++;
	}

LibGLD_LoadMaterial_Err1:
	*dnum = num;
	return sdata;
}

/* Load Polygon ---------------------------------*/
static SRC_POLYGON *LibGLD_LoadPolygon(FILE *fr, unsigned long *bnum, Src_Model *smodel)
{
	SRC_POLYGON		*bdata = NULL;
	SRC_POLYGON		*bptr;
	GLfloat			*svtx = ModelVertex(smodel);
	GLfloat			*svtn = ModelVNormal(smodel);
	SRC_TEXTURE		*stex = ModelTexture(smodel);
	SRC_MATERIAL	*smat = ModelMaterial(smodel);
	SRC_MATERIAL	*oldmat	= NULL;
	unsigned char	tmpdata[SRC_POLYGON_MAX_SIZE+5];
	unsigned char	*tmpptr;
	unsigned char	pflag;
	unsigned long	num = 0;
	unsigned long	cnt;
	long			tmpid4;
	short			tmpid2;
	int				basesize;
	int				rdsize;
	unsigned long	vtxnum = ModelVtxNum(smodel);
	unsigned long	matnum = ModelMatNum(smodel);

	if((num=GetDataNumber(fr))==0)	{
		hiyoGL_PutLog("  [GLD] No Polygon data.");
		goto LibGLD_LoadPolygon_Err1;
	}

	if((bdata=(SRC_POLYGON *)malloc(sizeof(SRC_POLYGON) * num))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Polygon area.");
		goto LibGLD_LoadPolygon_Err1;
	}
	bptr = bdata;

	basesize = 7;
	if(vtxnum>=32768)	{ basesize += 6; }
	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, basesize, 1, fr)!=1)	{
			free(bdata);
			bdata = NULL;
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon data 1.");
			goto LibGLD_LoadPolygon_Err1;
		}
		tmpptr = tmpdata;

		if(vtxnum<32768)	{
			SrcPolyVtx(bptr,0)			= GetShort(tmpptr);	tmpptr += 2;
			SrcPolyVtx(bptr,1)			= GetShort(tmpptr);	tmpptr += 2;
			SrcPolyVtx(bptr,2)			= GetShort(tmpptr);	tmpptr += 2;
		}
		else	{
			SrcPolyVtx(bptr,0)			= GetUDWord(tmpptr);	tmpptr += 4;
			SrcPolyVtx(bptr,1)			= GetUDWord(tmpptr);	tmpptr += 4;
			SrcPolyVtx(bptr,2)			= GetUDWord(tmpptr);	tmpptr += 4;
		}
		SrcPolyVtxPtr(bptr,0)		= svtx + (SrcPolyVtx(bptr,0) * 3);
		SrcPolyVtxPtr(bptr,1)		= svtx + (SrcPolyVtx(bptr,1) * 3);
		SrcPolyVtxPtr(bptr,2)		= svtx + (SrcPolyVtx(bptr,2) * 3);
		SrcPolyVtxNormPtr(bptr,0)	= svtn + (SrcPolyVtx(bptr,0) * 3);
		SrcPolyVtxNormPtr(bptr,1)	= svtn + (SrcPolyVtx(bptr,1) * 3);
		SrcPolyVtxNormPtr(bptr,2)	= svtn + (SrcPolyVtx(bptr,2) * 3);

		pflag = GetByte(tmpptr);	tmpptr += 1;
		SrcPolyTexNum(bptr)		= (pflag & 0x07);
		SrcPolyBlend(bptr)		= (((pflag&SrcPoly_Blend)==SrcPoly_Blend) ? 1 : 0);
		SrcPolyCulling(bptr)	= (((pflag&SrcPoly_Culling)==SrcPoly_Culling) ? 1 : 0);
		SrcPolySmooth(bptr)		= (((pflag&SrcPoly_Smooth)==SrcPoly_Smooth) ? 3 : 0);

		if((pflag&SrcPoly_MaterialFore)==SrcPoly_MaterialFore)
			{ SrcPolyMaterial(bptr)		= oldmat; }
		else	{
			if(matnum<32768)	{ rdsize = 2; }
			else				{ rdsize = 4; }

			if(fread(tmpdata, rdsize, 1, fr)!=1)	{
				free(bdata);
				bdata = NULL;
				hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon Material data.");
				goto LibGLD_LoadPolygon_Err1;
			}
			if(matnum<32768)	{
				tmpid2 = GetShort(tmpdata);
				SrcPolyMaterial(bptr)		= ((tmpid2<0) ? NULL : (smat + tmpid2));
			}
			else	{
				tmpid4 = GetDWord(tmpdata);
				SrcPolyMaterial(bptr)		= ((tmpid4<0) ? NULL : (smat + tmpid4));
			}
			oldmat = SrcPolyMaterial(bptr);
		}

		if((pflag&SrcPoly_PrimNext)==SrcPoly_PrimNext)
			{ SrcPolyNext(bptr)			= (bdata + cnt + 1); }
		else	{
			if(num<32768)		{ rdsize = 2; }
			else				{ rdsize = 4; }

			if(fread(tmpdata, rdsize, 1, fr)!=1)	{
				free(bdata);
				bdata = NULL;
				hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon Primitive data.");
				goto LibGLD_LoadPolygon_Err1;
			}
			if(num<32768)	{
				tmpid2 = GetShort(tmpdata);
				SrcPolyNext(bptr)			= ((tmpid2<0) ? NULL : (bdata + tmpid2));
			}
			else	{
				tmpid4 = GetDWord(tmpdata);
				SrcPolyNext(bptr)			= ((tmpid4<0) ? NULL : (bdata + tmpid4));
			}
		}

		if(SrcPolySmooth(bptr)==0)	{
			if(fread(tmpdata, 12, 1, fr)!=1)	{
				free(bdata);
				bdata = NULL;
				hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon Smooth data.");
				goto LibGLD_LoadPolygon_Err1;
			}
			tmpptr = tmpdata;
			SrcPolyNormal(bptr,0)		= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyNormal(bptr,1)		= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyNormal(bptr,2)		= GetFloat(tmpptr);		tmpptr += 4;
		}

		SrcPolyTexture(bptr,0) = NULL;
		SrcPolyTexture(bptr,1) = NULL;
		if(SrcPolyTexNum(bptr)>0)	{
			if(fread(tmpdata, 28, 1, fr)!=1)	{
				free(bdata);
				bdata = NULL;
				hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon Texture data 1.");
				goto LibGLD_LoadPolygon_Err1;
			}
			tmpptr = tmpdata;
			tmpid4 = GetDWord(tmpptr);	tmpptr += 4;
			SrcPolyTexture(bptr,0)		= ((tmpid4<0) ? NULL : (stex + tmpid4));

			SrcPolyTexPntX(bptr,0,0)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,0,0)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntX(bptr,0,1)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,0,1)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntX(bptr,0,2)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,0,2)= GetFloat(tmpptr);		tmpptr += 4;
		}

		if(SrcPolyTexNum(bptr)>1)	{
			if(fread(tmpdata, 28, 1, fr)!=1)	{
				free(bdata);
				bdata = NULL;
				hiyoGL_PutLog("  [GLD] ERROR! - Can't read Polygon Texture data 2.");
				goto LibGLD_LoadPolygon_Err1;
			}
			tmpptr = tmpdata;
			tmpid4 = GetDWord(tmpptr);		tmpptr += 4;
			SrcPolyTexture(bptr,1)	= ((tmpid4<0) ? NULL : (stex + tmpid4));

			SrcPolyTexPntX(bptr,1,0)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,1,0)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntX(bptr,1,1)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,1,1)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntX(bptr,1,2)= GetFloat(tmpptr);		tmpptr += 4;
			SrcPolyTexPntY(bptr,1,2)= GetFloat(tmpptr);		tmpptr += 4;
		}
		bptr++;
	}

LibGLD_LoadPolygon_Err1:
	*bnum = num;
	return bdata;
}


/* Load Primitive -------------------------------*/
static SRC_PRIMITIVE *LibGLD_LoadPrimitive(FILE *fr, unsigned long *vnum, SRC_POLYGON *polyp)
{
	SRC_PRIMITIVE	*vdata = NULL;
	SRC_PRIMITIVE	*vptr;
	unsigned char	tmpdata[SRC_PRIMITIVE_SIZE];
	unsigned long	num = 0;
	unsigned long	cnt;
	long			tmpid;

	if((num=GetDataNumber(fr))==0)	{
		hiyoGL_PutLog("  [GLD] No Primitive data.");
		goto LibGLD_LoadPrimitive_Err1;
	}

	if((vdata=(SRC_PRIMITIVE *)malloc(sizeof(SRC_PRIMITIVE) * num))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Primitive area.");
		goto LibGLD_LoadPrimitive_Err1;
	}
	vptr = vdata;

	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, SRC_PRIMITIVE_SIZE, 1, fr)!=1)	{
			free(vdata);
			vdata = NULL;
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Primitive data.");
			goto LibGLD_LoadPrimitive_Err1;
		}
		SrcPrimPolyNum(vptr)	= GetUDWord(tmpdata +  0);
		tmpid 					= GetUDWord(tmpdata +  4);
		SrcPrimPoly(vptr)	 	= ((tmpid<0) ? NULL : (polyp + tmpid));
		vptr++;
	}

LibGLD_LoadPrimitive_Err1:
	*vnum = num;
	return vdata;
}

/* Load Texture ---------------------------------*/
static SRC_TEXTURE *LibGLD_LoadTexture(FILE *fr, unsigned long *vnum)
{
	SRC_TEXTURE		*vdata = NULL;
	SRC_TEXTURE		*vptr;
	GC2_INFO		*gc2;
	unsigned char	tmpdata[SRC_TEXTURE_SIZE];
	unsigned long	cnt;
	unsigned long	num = 0;
	unsigned char	*pdata;

	if((num=GetDataNumber(fr))==0)	{
		vdata = (SRC_TEXTURE *)vnum;
		hiyoGL_PutLog("  [GLD] No Texture data.");
		goto LibGLD_LoadTexture_Post;
	}

	if((vdata=(SRC_TEXTURE *)malloc(sizeof(SRC_TEXTURE) * num))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Texture area.");
		goto LibGLD_LoadTexture_Post;
	}
	vptr = vdata;

	for(cnt=0; cnt<num; cnt++)	{
		if(fread(tmpdata, SRC_TEXTURE_SIZE, 1, fr)!=1)	{
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Texture meta data.");
			goto LibGLD_LoadTexture_Err;
		}
		SrcTexFlag(vptr) 	= GetUDWord(tmpdata +  0);
		SrcTexX(vptr) 		= GetUDWord(tmpdata +  4);
		SrcTexY(vptr)		= GetUDWord(tmpdata +  8);
		SrcTexSize(vptr) 	= GetUDWord(tmpdata + 12);

		if((pdata=(unsigned char *)malloc(SrcTexSize(vptr)))==NULL)	{
			hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc Texture data area.");
			goto LibGLD_LoadTexture_Err;
		}
		if(fread(pdata, SrcTexSize(vptr), 1, fr)!=1)	{
			free(pdata);
			hiyoGL_PutLog("  [GLD] ERROR! - Can't read Texture data.");
			goto LibGLD_LoadTexture_Err;
		}

		gc2 = GC2_InitInfo();
		GC2_Load_Binary(gc2,pdata);
		SrcTexData(vptr) = GC2_CopyPictureData(GC2Info_RawSize(gc2),GC2Info_RawData(gc2));
		free(pdata);
		GC2_FreeInfo(gc2);
		vptr++;
	}
	goto LibGLD_LoadTexture_Post;

LibGLD_LoadTexture_Err:
	free(vdata);
	vdata = NULL;
LibGLD_LoadTexture_Post:
	*vnum = num;
	return vdata;
}

static int LibGLD_FreeTextureData(Src_Model *smodel)
{	
	unsigned long	cnt;
	SRC_TEXTURE 	*stex;

	stex = ModelTexture(smodel);
	for(cnt=0; cnt<ModelTexNum(smodel); cnt++)	{
		if(SrcTexData(stex)!=NULL)	{
			free(SrcTexData(stex));
			SrcTexData(stex) = NULL;
		}
	}
	return 0;
}

static int LibGLD_InitSrcModel(Src_Model *smodel)
{
	ModelVtxNum(smodel)		= 0;
	ModelVertex(smodel)		= NULL;
	ModelVNormNum(smodel)	= 0;
	ModelVNormal(smodel)	= NULL;
	ModelMatNum(smodel)		= 0;
	ModelMaterial(smodel)	= NULL;
	ModelPolyNum(smodel)	= 0;
	ModelPolygon(smodel)	= NULL;
	ModelPrimNum(smodel)	= 0;
	ModelPrimitive(smodel)	= NULL;
	ModelTexNum(smodel) 	= 0;
	ModelTexture(smodel)	= NULL;
	ModelGLID(smodel)		= 0;

	return 0;
}

static int LibGLD_FreeSrcModel(Src_Model *smodel,int level)
{
	if((level>=6)&&(ModelPrimNum(smodel)>0))	{
		free(ModelPrimitive(smodel));
		ModelPrimitive(smodel) = NULL;
		ModelPrimNum(smodel) = 0;
	}
	if((level>=5)&&(ModelPolyNum(smodel)>0))	{
		free(ModelPolygon(smodel));
		ModelPolygon(smodel) = NULL;
		ModelPolyNum(smodel) /= 100;
	}
	if((level>=4)&&(ModelMatNum(smodel)>0))		{
		free(ModelMaterial(smodel));
		ModelMaterial(smodel) = NULL;
		ModelMatNum(smodel) = 0;
	}
	if((level>=3)&&(ModelTexNum(smodel)>0))		{
		LibGLD_FreeTextureData(smodel);
		free(ModelTexture(smodel));
		ModelTexture(smodel) = NULL;
		ModelTexNum(smodel) = 0;
	}
	if((level>=2)&&(ModelVNormNum(smodel)>0))	{
		free(ModelVNormal(smodel));
		ModelVNormal(smodel) = NULL;
		ModelVNormNum(smodel) = 0;
	}
	if((level>=1)&&(ModelVtxNum(smodel)>0))		{
		free(ModelVertex(smodel));
		ModelVertex(smodel) = NULL;
		ModelVtxNum(smodel) = 0;
	}

	return 0;
}

/* Compare Material Pointer for qsort ------------*/
static int LibGLD_CompPolyMaterial(const void *vpolya, const void *vpolyb)
{
	SRC_POLYGON		*polya;
	SRC_POLYGON		*polyb;

	polya	= *(SRC_POLYGON **)vpolya;
	polyb	= *(SRC_POLYGON **)vpolyb;

	if(SrcPolyMaterial(polya) > SrcPolyMaterial(polyb))			{ return 1; }
	else if(SrcPolyMaterial(polya) < SrcPolyMaterial(polyb))	{ return -1; }
	return 0;
}

/* Optimize Model --------------------------------*/
static int LibGLD_OptimizeModel(Src_Model *smodel)
{
	unsigned long	cnt;
	unsigned long	polynum;
	SRC_POLYGON		**polylist;
	SRC_POLYGON		**pcur;
	SRC_POLYGON		*pnow;
	SRC_PRIMITIVE	*prim;

	polynum	= ModelPolyNum(smodel);
	prim = ModelPrimitive(smodel);

	polylist = (SRC_POLYGON **)malloc(sizeof(SRC_POLYGON *) * polynum);
	if(polylist == NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't alloc temporary Polygon lists.");
		return -1;
	}
	memset( polylist, 0x00, sizeof(SRC_POLYGON *) * polynum);

	pnow = (SRC_POLYGON *)SrcPrimPoly(prim);
	pcur = polylist;
	do { *pcur++ = pnow; } while((pnow=SrcPolyNext(pnow))!=NULL);

	qsort(polylist, (size_t)polynum, sizeof(SRC_POLYGON *), LibGLD_CompPolyMaterial);

	pcur = polylist;
	SrcPrimPoly(prim)	= (void *)*pcur;
	for(cnt=0 ; cnt<(polynum-1); cnt++, pcur++)
		{ SrcPolyNext(*pcur) = (void *)*(pcur + 1); }
	SrcPolyNext(*pcur)	= (void *)NULL;

	free(polylist);

	return 0;
}

/* Load GLD File --------------------------------*/
static int LibGLD_LoadGLDFile(Src_Model *smodel,char *fname)
{
	int				err = 0;
	unsigned long	tnum;
	FILE			*fr;

	hiyoGL_PutLogString("  [GLD] Load GLD File", fname);

	if((fr=fopen(fname, "rb"))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - GLD File Open Error");
		err = -1;	goto LibGLD_LoadGLDFile_Post;
	}

	if(GetDataNumber(fr)!=LIBGLD_VERSION)	{
		hiyoGL_PutLog("  [GLD] ERROR! - GLD File Version");
		err = -2;	goto LibGLD_LoadGLDFile_Err2;
	}

	if((ModelVertex(smodel)=LibGLD_LoadVertex(fr,&tnum))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Vertex Load");
		err = 1;	goto LibGLD_LoadGLDFile_Err3;
	}
	ModelVtxNum(smodel) = tnum;

	if((ModelVNormal(smodel)=LibGLD_LoadVtxNormal(fr,&tnum))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - VertexNormal Load");
		err = 2;	goto LibGLD_LoadGLDFile_Err3;
	}
	ModelVNormNum(smodel) = tnum;

	if((ModelTexture(smodel)=LibGLD_LoadTexture(fr,&tnum))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Texture Load");
		err = 3;	goto LibGLD_LoadGLDFile_Err3;
	}
	if((ModelTexNum(smodel)=tnum)==0)
		{ ModelTexture(smodel) = NULL; }

	if((ModelMaterial(smodel)=LibGLD_LoadMaterial(fr,&tnum))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Material Load");
		err = 4;	goto LibGLD_LoadGLDFile_Err3;
	}
	ModelMatNum(smodel) = tnum;

	if((ModelPolygon(smodel)=LibGLD_LoadPolygon(fr,&tnum,smodel))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Polygon Load");
		err = 5;	goto LibGLD_LoadGLDFile_Err3;
	}
	ModelPolyNum(smodel) = tnum;

	if((ModelPrimitive(smodel)=LibGLD_LoadPrimitive(fr,&tnum, ModelPolygon(smodel)))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Primitive Load");
		err = 6;	goto LibGLD_LoadGLDFile_Err3;
	}
	ModelPrimNum(smodel) = tnum;

	if((err = LibGLD_OptimizeModel(smodel))!=0)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Optimize GLD model.");
		err = 7;	goto LibGLD_LoadGLDFile_Err3;
	}

	goto	LibGLD_LoadGLDFile_Post;

LibGLD_LoadGLDFile_Err3: 
	LibGLD_FreeSrcModel(smodel, err);
LibGLD_LoadGLDFile_Err2:
	fclose(fr);
LibGLD_LoadGLDFile_Post:
	hiyoGL_PutLogInt("  [GLD] Loaded GLD File  err", err);
	return err;
}

/* Model Draw Section ===========================*/
static int LibGLD_SetArrayGLD(Src_Model *smodel)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, ModelVertex(smodel));

	return 0;
}

static void LibGLD_SetMaterial_Optimize(SRC_MATERIAL *smat)
{
	if(SrcStatMaterial(sstat)==smat)	{ return; }

	glMaterialfv(GL_FRONT,GL_DIFFUSE, SrcDiffuse(smat));
	glMaterialfv(GL_FRONT,GL_SPECULAR, SrcSpecular(smat));
	glMaterialfv(GL_FRONT,GL_AMBIENT, SrcAmbient(smat));
	glMaterialfv(GL_FRONT,GL_EMISSION, SrcEmission(smat));
	glMaterialf(GL_FRONT,GL_SHININESS, SrcShininess(smat));

	SrcStatMaterial(sstat) = smat;

	return;
}

static void LibGLD_InitDrawStatus(void)
{
	SrcStatMaterial(sstat) = NULL;
	SrcStatBlend(sstat) = 0;
	SrcStatCulling(sstat) = 0;
	SrcStatSmooth(sstat) = 0;
	glShadeModel(GL_FLAT);
	SrcStatTexture(sstat) = 0x00;
	return;
}

static void LibGLD_SetPolygonStatus(SRC_POLYGON *polyp)
{
	if(SrcPolyBlend(polyp)^SrcStatBlend(sstat))	{
		if(SrcPolyBlend(polyp))		{ glEnable(GL_BLEND); }
		else						{ glDisable(GL_BLEND); }
		SrcStatBlend(sstat) = SrcPolyBlend(polyp);
	}
	if(SrcPolyCulling(polyp)^SrcStatCulling(sstat))	{
		if(SrcPolyCulling(polyp))	{ glEnable(GL_CULL_FACE); }
		else						{ glDisable(GL_CULL_FACE); }
		SrcStatCulling(sstat) = SrcPolyCulling(polyp);
	}
	return;
}

static int LibGLD_SetupTexture(Src_Model *smodel)
{
	unsigned long	cnt;
	SRC_TEXTURE		*stex;
	SRC_POLYGON		*spoly;
	SRC_POLYGON		*opoly[2];

	spoly = ModelPolygon(smodel);
	opoly[0] = NULL;
	opoly[1] = NULL;
	for(cnt=0; cnt<ModelPolyNum(smodel); cnt++)	{
		if((stex=SrcPolyTexture(spoly,1))!=NULL)	{
			SrcPolyTexID(spoly,1) = ((stex!=NULL) ? SrcTexGLID(stex) : 0);
			opoly[1] = spoly;
		}
		if((stex=SrcPolyTexture(spoly,0))!=NULL)	{
			SrcPolyTexID(spoly,0) = ((stex!=NULL) ? SrcTexGLID(stex) : 0);
			opoly[0] = spoly;
		}
		spoly++;
	}
	if(opoly[0]!=NULL)	{
		glBindTexture(GL_TEXTURE_2D, SrcPolyTexID(opoly[0],0));
		SrcStatTexID(sstat,0) = SrcPolyTexID(opoly[0],0);
	}

	return 0;
}

static int LibGLD_BuildGLTexture(Src_Model *smodel)
{
	unsigned long	cnt;
	SRC_TEXTURE		*stex;
	GLint			comp;
	GLenum			tmode;
	GLuint			glid;

	stex = ModelTexture(smodel);

	for(cnt=0; cnt<ModelTexNum(smodel); cnt++)	{
		if((isSrcTexColor(stex))&&(isSrcTexAlpha(stex)))	{
			tmode = ((isSrcTexMono(stex)) ? GL_LUMINANCE_ALPHA : GL_RGBA);
			comp = ((isSrcTexMono(stex)) ? 2 : 4);
		}
		else if((isSrcTexColor(stex))&&(!isSrcTexAlpha(stex)))	{
			tmode = ((isSrcTexMono(stex)) ? GL_LUMINANCE : GL_RGB);
			comp = ((isSrcTexMono(stex)) ? 1 : 3);
		}
		else if((!isSrcTexColor(stex))&&(isSrcTexAlpha(stex)))	{
			tmode = GL_ALPHA;
			comp = GL_ALPHA8;
		}
		glGenTextures(1,&glid);

		glBindTexture(GL_TEXTURE_2D, glid);
		glTexImage2D(GL_TEXTURE_2D, 0,comp, SrcTexX(stex), SrcTexY(stex),
						0, tmode, GL_UNSIGNED_BYTE,SrcTexData(stex));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		SrcTexGLID(stex) = glid;

		stex++;
	}

	return 0;
}

static void LibGLD_SwitchTexture_Off2Multi(void)
{
	glEnable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x02;
	return;
}

static void LibGLD_SwitchTexture_Multi2Off(void)
{
	glDisable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x00;
	return;
}

static void LibGLD_SwitchTexture_Tex2Multi(void)
{
	glEnable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x02;
	return;
}

static void LibGLD_SwitchTexture_Multi2Tex(void)
{
	glEnable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x01;
	return;
}

static void LibGLD_SwitchTexture_Off2Tex(void)
{
	glEnable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x01;
	return;
}

static void LibGLD_SwitchTexture_Tex2Off(void)
{
	glDisable(GL_TEXTURE_2D);
	SrcStatTexture(sstat) = 0x00;
	return;
}

static void LibGLD_SwitchTexture_Error(void)
{
	hiyoGL_PutLog("  [GLD] ERROR! - libgld Texture Logic Error");
	return;
}

static void LibGLD_SwitchTexture_None(void)
{
	/* Dummy Func. */
	return;
}

static void LibGLD_BindTexture_Mutil(SRC_POLYGON *polyp)
{
	if(SrcStatTexID(sstat,0)!=SrcPolyTexID(polyp,0))	{
		glBindTexture(GL_TEXTURE_2D, SrcPolyTexID(polyp,0));
		SrcStatTexID(sstat,0) = SrcPolyTexID(polyp,0);
	}
	return;
}

static void LibGLD_BindTexture_Tex(SRC_POLYGON *polyp)
{
	if(SrcStatTexID(sstat,0)!=SrcPolyTexID(polyp,0))	{
		glBindTexture(GL_TEXTURE_2D, SrcPolyTexID(polyp,0));
		SrcStatTexID(sstat,0) = SrcPolyTexID(polyp,0);
	}
	return;
}

static void LibGLD_BindTexture_None(SRC_POLYGON *spoly)
{
	return;
}

static void LibGLD_Draw_Array_Smooth_TexOn(SRC_POLYGON *polyp)
{
	if(SrcPolySmooth(polyp)^SrcStatSmooth(sstat))	{
		SrcStatSmooth(sstat) = SrcPolySmooth(polyp);
		glShadeModel(GL_SMOOTH);
	}
	LibGLD_BindTexture[SrcPolyTexNum(polyp)](polyp);

	glBegin(GL_TRIANGLES);
		glTexCoord2fv(SrcPolyTexPnt(polyp,0,0));
		glNormal3fv(SrcPolyVtxNormPtr(polyp,0));
		glArrayElement(SrcPolyVtx(polyp,0));

		glTexCoord2fv(SrcPolyTexPnt(polyp,0,1));
		glNormal3fv(SrcPolyVtxNormPtr(polyp,1));
		glArrayElement(SrcPolyVtx(polyp,1));

		glTexCoord2fv(SrcPolyTexPnt(polyp,0,2));
		glNormal3fv(SrcPolyVtxNormPtr(polyp,2));
		glArrayElement(SrcPolyVtx(polyp,2));
	glEnd();
	return;
}

static void LibGLD_Draw_Array_Smooth_TexOff(SRC_POLYGON *polyp)
{
	if(SrcPolySmooth(polyp)^SrcStatSmooth(sstat))	{
		SrcStatSmooth(sstat) = SrcPolySmooth(polyp);
		glShadeModel(GL_SMOOTH);
	}

	glBegin(GL_TRIANGLES);
		glNormal3fv(SrcPolyVtxNormPtr(polyp,0));
		glArrayElement(SrcPolyVtx(polyp,0));

		glNormal3fv(SrcPolyVtxNormPtr(polyp,1));
		glArrayElement(SrcPolyVtx(polyp,1));

		glNormal3fv(SrcPolyVtxNormPtr(polyp,2));
		glArrayElement(SrcPolyVtx(polyp,2));
	glEnd();
	return;
}

static void LibGLD_Draw_Array_Flat_TexOn(SRC_POLYGON *polyp)
{
	if(SrcPolySmooth(polyp)^SrcStatSmooth(sstat))	{
		SrcStatSmooth(sstat) = SrcPolySmooth(polyp);
		glShadeModel(GL_FLAT);
	}
	LibGLD_BindTexture[SrcPolyTexNum(polyp)](polyp);

	glBegin(GL_TRIANGLES);
		glNormal3fv(SrcPolyNormalPtr(polyp));

		glTexCoord2fv(SrcPolyTexPnt(polyp,0,0));
		glArrayElement(SrcPolyVtx(polyp,0));

		glTexCoord2fv(SrcPolyTexPnt(polyp,0,1));
		glArrayElement(SrcPolyVtx(polyp,1));

		glTexCoord2fv(SrcPolyTexPnt(polyp,0,2));
		glArrayElement(SrcPolyVtx(polyp,2));
	glEnd();
	return;
}

static void LibGLD_Draw_Array_Flat_TexOff(SRC_POLYGON *polyp)
{
	if(SrcPolySmooth(polyp)^SrcStatSmooth(sstat))	{
		SrcStatSmooth(sstat) = SrcPolySmooth(polyp);
		glShadeModel(GL_FLAT);
	}

	glBegin(GL_TRIANGLES);
		glNormal3fv(SrcPolyNormalPtr(polyp));

		glArrayElement(SrcPolyVtx(polyp,0));
		glArrayElement(SrcPolyVtx(polyp,1));
		glArrayElement(SrcPolyVtx(polyp,2));
	glEnd();
	return;
}

static void LibGLD_Draw_Array_Wireframe(SRC_POLYGON *polyp)
{
	// Change Draw function from glDrawElements to glArrayElement
	//   because of invalid output with the 64bit environment.
	// glDrawElements(GL_LINE_STRIP, 3, GL_UNSIGNED_INT, SrcPolyVtxIndex(polyp));
	glBegin(GL_LINE_STRIP);
		glArrayElement(SrcPolyVtx(polyp,0));
		glArrayElement(SrcPolyVtx(polyp,1));
		glArrayElement(SrcPolyVtx(polyp,2));
	glEnd();
	return;
}

/* Model Regist Section =========================*/
static int LibGLD_RegistWireframeGLD(Src_Model *smodel,unsigned short flag)
{
	unsigned long	cnt;
	GLuint			gid;
	SRC_PRIMITIVE	*primp;
	SRC_POLYGON		*polyp;
	GLfloat			*colarray;

	LibGLD_InitDrawStatus();

	if((colarray=(GLfloat *)malloc(ModelVtxNum(smodel) * 3 * sizeof(GLfloat)))==NULL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Can't Alloc ColorArray");
		goto LibGLD_RegistWireframeGLD_NonColor;
	}

	for(cnt=0; cnt<(ModelVtxNum(smodel)*3); cnt++)
		{ *(colarray + cnt) = (GLfloat)rand() / (GLfloat)RAND_MAX; }

	glColorPointer(3, GL_FLOAT, 0, colarray);
	glEnableClientState(GL_COLOR_ARRAY);

LibGLD_RegistWireframeGLD_NonColor:
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, ModelVertex(smodel));

	/* Display List Regist */
	primp = ModelPrimitive(smodel);
	gid = glGenLists(1);
	glNewList(gid,GL_COMPILE);
		for(cnt=0; cnt<ModelPrimNum(smodel); cnt++)	{
			polyp = SrcPrimPoly(primp);
			do	{
				LibGLD_Draw_Array_Wireframe(polyp);
			} while((polyp=SrcPolyNext(polyp))!=NULL);
			primp++;
		}
	glEndList();

	free(colarray);
	glDisableClientState(GL_COLOR_ARRAY);
	ModelGLWire(smodel) = gid;

	hiyoGL_PutLog("  [GLD] Registed Wireframe Array");
	return 0;
}

static int LibGLD_SetCommandGLD(SRC_PRIMITIVE *primp)
{
	SRC_POLYGON		*polyp;
	unsigned int	tflag;

	polyp = SrcPrimPoly(primp);

	do	{
		LibGLD_SetPolygonStatus(polyp);
		LibGLD_SetMaterial_Optimize(SrcPolyMaterial(polyp));

		tflag = ((SrcStatTexture(sstat)^SrcPolyTexNum(polyp)) * 3)
					+ SrcStatTexture(sstat);
		LibGLD_SwitchTexture[tflag]();
		tflag = SrcPolySmooth(polyp) + SrcPolyTexNum(polyp);
		LibGLD_DrawPolygon[tflag](polyp);
	} while((polyp=SrcPolyNext(polyp))!=NULL);

	return 0;
}

/* Model Regist Section =========================*/
static int LibGLD_RegistGLD(Src_Model *smodel,unsigned short flag)
{
	unsigned long	cnt;
	GLuint			gid;
	SRC_PRIMITIVE	*primp;

	LibGLD_InitDrawStatus();

	/* Texture Regist */
	LibGLD_BuildGLTexture(smodel);
	/* Data Array */
	if(isFlagArray(flag))
		{ LibGLD_SetArrayGLD(smodel); }

	/* Display List Regist */
	primp = ModelPrimitive(smodel);
	gid = glGenLists(1);
	glNewList(gid,GL_COMPILE);
		LibGLD_SetupTexture(smodel);
		for(cnt=0; cnt<ModelPrimNum(smodel); cnt++)
			{ LibGLD_SetCommandGLD(primp++); }
	glEndList();
	ModelGLID(smodel) = gid;

	hiyoGL_PutLog("  [GLD] Registed Model");
	return 0;
}

static void LibGLD_InitFunctions(unsigned short flag)
{
	LibGLD_SwitchTexture[ 0] = LibGLD_SwitchTexture_None;
	LibGLD_SwitchTexture[ 1] = LibGLD_SwitchTexture_None;
	LibGLD_SwitchTexture[ 2] = LibGLD_SwitchTexture_None;
	LibGLD_SwitchTexture[ 3] = LibGLD_SwitchTexture_Off2Tex;
	LibGLD_SwitchTexture[ 4] = LibGLD_SwitchTexture_Tex2Off;
	LibGLD_SwitchTexture[ 5] = LibGLD_SwitchTexture_Error;
	LibGLD_SwitchTexture[ 6] = LibGLD_SwitchTexture_Off2Multi;
	LibGLD_SwitchTexture[ 7] = LibGLD_SwitchTexture_Error;
	LibGLD_SwitchTexture[ 8] = LibGLD_SwitchTexture_Multi2Off;
	LibGLD_SwitchTexture[ 9] = LibGLD_SwitchTexture_Error;
	LibGLD_SwitchTexture[10] = LibGLD_SwitchTexture_Tex2Multi;
	LibGLD_SwitchTexture[11] = LibGLD_SwitchTexture_Multi2Tex;

	LibGLD_BindTexture[0] = LibGLD_BindTexture_None;
	LibGLD_BindTexture[1] = LibGLD_BindTexture_Tex;
	LibGLD_BindTexture[2] = LibGLD_BindTexture_Mutil;

	LibGLD_DrawPolygon[0] = LibGLD_Draw_Array_Flat_TexOff;
	LibGLD_DrawPolygon[1] = LibGLD_Draw_Array_Flat_TexOn;
	LibGLD_DrawPolygon[2] = LibGLD_Draw_Array_Flat_TexOn;
	LibGLD_DrawPolygon[3] = LibGLD_Draw_Array_Smooth_TexOff;
	LibGLD_DrawPolygon[4] = LibGLD_Draw_Array_Smooth_TexOn;
	LibGLD_DrawPolygon[5] = LibGLD_Draw_Array_Smooth_TexOn;

	return;
}

/* GLD API functions ============================*/
int LibGLD_Init(Src_Model *smodel,char *fname,unsigned short flag,unsigned long basever)
{
	int		err	= 0;
	int		optlevel = 1;

	/* libgld Src Version Check */
	if(LIBGLDC_SERIAL!=LIBGLDH_SERIAL)	{
		hiyoGL_PutLog("  [GLD] BUG!! - Not match internal serial.");
		err = 1;
		goto	goto_LibGLD_Init_post;
	}
	/* libgld - base.c src version check */
	if(basever!=LIBGLDC_SERIAL)	{
		hiyoGL_PutLog("  [GLD] ERROR! - Not match GLD file serial.");
		err = 2;
		goto	goto_LibGLD_Init_post;
	}

	LibGLD_InitFunctions(flag);
	LibGLD_InitSrcModel(smodel);
	err = LibGLD_LoadGLDFile(smodel,fname);
	if(0 != err)	{
		hiyoGL_PutLogInt("  [GLD] ERROR! - Failure load GLD file. err", err);
		goto	goto_LibGLD_Init_post;
	}
	LibGLD_RegistGLD(smodel, flag);

	if(isFlagWireFrame(flag))
		{ LibGLD_RegistWireframeGLD(smodel,flag); }

	if((isFlagArray(flag))&&(isFlagOptMem1(flag)))	{
		if(isFlagOptMem2(flag))	{ optlevel = 5; }
		LibGLD_FreeSrcModel(smodel,optlevel);
	}

goto_LibGLD_Init_post:
	hiyoGL_PutLogString("  [GLD] Initalized Model", fname);
	return err;
}

unsigned long long LibGLD_Draw(Src_Model *smodel)
{
	glCallList(ModelGLID(smodel));
	return (unsigned long long)ModelPolyNum(smodel);
}

unsigned long long LibGLD_DrawWireFrame(Src_Model *smodel)
{
	glCallList(ModelGLWire(smodel));
	return (unsigned long long)ModelPolyNum(smodel);
}

int LibGLD_Free(Src_Model *smodel)
{
	LibGLD_FreeSrcModel(smodel, 10);
	return 0;
}

/* EOF of libgld.c */

