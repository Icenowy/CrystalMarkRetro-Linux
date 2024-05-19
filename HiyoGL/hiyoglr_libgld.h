/*---------------------------------------------------------------------------*/
//       Author : koinec, hiyohiyo
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#ifndef	LIBGLD_HEADER	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#define	LIBGLD_HEADER

#include<GL/gl.h>

#define	LIBGLD_VERSION		0x100
#define	LIBGLDH_SERIAL		2004020101


/*=========================================================*/
/* GC2 Inner Format Defines == ============================*/

typedef	struct{
	unsigned	short	version;
	unsigned	short	colors;
	unsigned	long	x;
	unsigned	long	y;
	unsigned	short	flag;
	unsigned	char	author[64];
	unsigned	char	*rawdata;
	unsigned	long	rawsize;
	char				fheader[17];
	unsigned	long	ext1_size;
	unsigned	char	*ext1;
	unsigned	char	ext1header[17];
} GC2_INFO;

#define	GC2Info_Ver(n)			((n)->version)
#define GC2Info_Colors(n)		((n)->colors)
#define	GC2Info_X(n)			((n)->x)
#define	GC2Info_Y(n)			((n)->y)
#define	GC2Info_Flag(n)			((n)->flag)
#define	GC2Info_Author(n)		((n)->author)
#define	GC2Info_ColorData(n)	((n)->coldata)
#define	GC2Info_ColorSize(n)	((n)->colsize)
#define	GC2Info_AlphaData(n)	((n)->alphadata)
#define	GC2Info_AlphaSize(n)	((n)->alphasize)
#define	GC2Info_RawData(n)		((n)->rawdata)
#define	GC2Info_RawSize(n)		((n)->rawsize)
#define	GC2Info_FileHeader(n)	((n)->fheader)
#define	GC2Info_Ext1Size(n)		((n)->ext1_size)
#define	GC2Info_Ext1Data(n)		((n)->ext1)
#define	GC2Info_Ext1Header(n)	((n)->ext1header)

#define	isGC2_Alpha(n)			(((n)->flag)&GC2_ALPHA)

#ifdef SRC_LIBGLD	/*---------------------------*/
#define	GC2_VERSION_100		0x0100
#define	GC2_VERSION_200		0x0200

#define	GC2_VER1			GC2_VERSION_100
#define	GC2_VER2			GC2_VERSION_200

#define	GC2_NODATA			0
#define	GC2_MONO			1
#define	GC2_MONO_ALPHA		2
#define	GC2_COLOR			3
#define	GC2_COLOR_ALPHA		4

#define	GC2_NOFLAG			0x00
#define	GC2_ALPHA			0x01

#define	GC2_DEF_FILEHEADER_STRING	"Koinec GC2      "

typedef	unsigned char *		BinData;

typedef struct{
	unsigned char	*bdata;
	unsigned char	*bptr;
	unsigned char	bloc;
} GC2_BIT_DATA;

/* File Header - 352B*/
typedef struct{
	unsigned	char	gc2_headstring[16];	/*  16B - "Koinec GC2      " */
	unsigned	short	version;			/*   2B */
	unsigned	short	colors;				/*   2B */
	unsigned	long	x;					/*   4B */
	unsigned	long	y;					/*   4B */
	unsigned	long	size;				/*   4B */
	unsigned	char	author[64];			/*  64B */
	unsigned	char	flag;				/*   2B */
	unsigned	long	ext1_size;			/*   4B */
	unsigned	char	extension[256];		/* 236B */
} GC2_FILE_HEADER;

#define	GC2_FILE_HEADER_SIZE		358
#define	GC2_DEF_FILEHEADER_STRING	"Koinec GC2      "

#define	GC2_GetByte(m)		(unsigned char)(*(m))
#define	GC2_GetWord(n)		(((*(n))<<8)|(*((n)+1)))
#define	GC2_GetDWord(n)		(((*(n))<<24)|((*((n)+1))<<16)|((*((n)+2))<<8)|(*((n)+3)))

#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of GC2 Inner Format Defines == =====================*/
/*=========================================================*/


/*=========================================================*/
/* FileLoad Macros & Functions ============================*/
#ifdef SRC_LIBGLD	/*---------------------------*/

#define PB(m,n)					(unsigned long)(*((m)+(n)))
#define	GetByte(m)				(unsigned short)(*(m))
#define GetUShort(m)			(unsigned short)((PB((m),0)<<8)|(PB((m),1)))
#define GetShort(m)				(short)((PB((m),0)<<8)|(PB((m),1)))
#define GetDWord(m)				(long)((PB((m),0)<<24)|(PB((m),1)<<16)|(PB((m),2)<<8)|(PB((m),3)))
#define GetUDWord(m)			(unsigned long)((PB((m),0)<<24)|(PB((m),1)<<16)|(PB((m),2)<<8)| (PB((m),3)))
#define GetString(m,n,o)		*((m)+0)='\0';strncat((m),(n),(o))

typedef union{
	float			data;
	unsigned long	bindat;
} BIN_FLOAT;

static float GetFloat(unsigned char *datptr)
{
	BIN_FLOAT			bfdat;
	bfdat.bindat = GetDWord(datptr);
	return bfdat.data;
}
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of FileLoad Macros & Functions =====================*/


/*=========================================================*/
/* SrcModel Section =======================================*/
typedef struct {
	unsigned long	vtxnum;
	GLfloat			*vertex;	/* opt */
	unsigned long	vnormnum;
	GLfloat			*vnormal;	/* (opt) */
	unsigned long	matnum;
	void			*material;
	unsigned long	polynum;
	void			*polygon;
	unsigned long	primnum;
	void			*primitive;
	unsigned long	texnum;
	void			*texture;	/* opt */
	GLuint			glid;
	GLuint			glwire;
} Src_Model;

#ifdef SRC_LIBGLD	/*---------------------------*/
#define	ModelVtxNum(n)		((n)->vtxnum)
#define	ModelVertex(n)		((n)->vertex)
#define	ModelVNormNum(n)	((n)->vnormnum)
#define	ModelVNormal(n)		((n)->vnormal)
#define	ModelMatNum(n)		((n)->matnum)
#define	ModelMaterial(n)	((n)->material)
#define	ModelPolyNum(n)		((n)->polynum)
#define	ModelPolygon(n)		((n)->polygon)
#define	ModelPrimNum(n)		((n)->primnum)
#define	ModelPrimitive(n)	((n)->primitive)
#define	ModelTexNum(n)		((n)->texnum)
#define	ModelTexture(n)		((n)->texture)
#define	ModelGLID(n)		((n)->glid)
#define	ModelGLWire(n)		((n)->glwire)
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of SrcModel Section ================================*/


/*=========================================================*/
/* SrcStatus Section ======================================*/
#ifdef SRC_LIBGLD	/*---------------------------*/
typedef struct {
	void			*material;
	char			blend;
	char			culling;
	char			smooth;
	unsigned char	texture;
	GLuint			texid[2];
} SRC_STATUS;

#define	SrcStatMaterial(n)	((n).material)
#define	SrcStatBlend(n)		((n).blend)
#define	SrcStatCulling(n)	((n).culling)
#define	SrcStatSmooth(n)	((n).smooth)
#define	SrcStatTexture(n)	((n).texture)
#define	SrcStatTexID(m,n)	((m).texid[(n)])

#define	isFlagArray(n)		((n)&SRCFLAG_ARRAY)
#define	isFlagOptMem1(n)	((n)&SRCFLAG_OPTMEMORY1)
#define	isFlagOptMem2(n)	((n)&SRCFLAG_OPTMEMORY2)
#define	isFlagWireFrame(n)	((n)&SRCFLAG_WIREFRAME)
#endif	/* SRC_LIBGLD ---------------------------*/

#define	SRCFLAG_NONE		0x00
#define	SRCFLAG_ARRAY		0x01
#define	SRCFLAG_OPTMEMORY1	0x02
#define	SRCFLAG_OPTMEMORY2	0x04
#define	SRCFLAG_WIREFRAME	0x08
/* EOL of SrcStatus Section ===============================*/


/*=========================================================*/
/* Src Vertex Section =====================================*/
#define	SRC_VERTEX_SIZE		24
/* EOL of Src Vertex Section ==============================*/


/*=========================================================*/
/* Src Material Section ===================================*/ 
#define	SRC_MATERIAL_SIZE	68

#ifdef SRC_LIBGLD	/*---------------------------*/
typedef struct{
	GLfloat			diffuse[4];		/*	16 */
	GLfloat			specular[4];	/*  16 */
	GLfloat			ambient[4];		/*  16 */
	GLfloat			emission[4];	/*  16 */
	GLfloat			shininess;		/*   4 */
} SRC_MATERIAL;

#define	SrcDiffuse(n)	((n)->diffuse)
#define	SrcSpecular(n)	((n)->specular)
#define	SrcAmbient(n)	((n)->ambient)
#define	SrcEmission(n)	((n)->emission)
#define	SrcShininess(n)	((n)->shininess)
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of Src Material Section ============================*/


/*=========================================================*/
/* Src Polygon Section ====================================*/
/*#define	SRC_POLYGON_SIZE	93*/
#define	SRC_POLYGON_MAX_SIZE	89
#define	SRC_POLYGON_BASE_SIZE	21

/*#define	SrcPoly_Normal			0x00
#define	SrcPoly_Culling			0x01
#define	SrcPoly_Blend			0x02*/
#define	SrcPoly_Normal			0x00
#define	SrcPoly_Culling			0x10
#define	SrcPoly_Blend			0x20
#define	SrcPoly_Smooth			0x40
#define	SrcPoly_PrimNext		0x80
#define	SrcPoly_MaterialFore	0x08

#ifdef SRC_LIBGLD	/*---------------------------*/
typedef struct{
	unsigned long	vertex[3];			/*  12 - Vertex Ptr. */
	GLfloat			*vtxp[3];			/* NOT - RunTime Data */
	GLfloat			*vtnp[3];			/* NOT - RunTime Data */
	GLfloat			rnorm[3];			/*  12 - Polygon Normal Data */
	void			*material;			/*   4 - Material Ptr. */
	unsigned char	blend;				/*   1 - Blend Flag */
	unsigned char	cull;				/*   1 - Culling Flag */
	unsigned char	smooth;				/*   1 - Shader Flag */
	void			*primnext;			/*   4 - Next Polygon */
	unsigned short	texnum;				/*   2 - Textures Number */
	GLuint			texid[2];			/* NOT - RumTime Data */
	void			*texture[2];		/*   8 - Texture Ptr. */
	GLfloat			texpnt[2][3][2];	/*  48 - Texture Coord. */
} SRC_POLYGON;

#define	SrcPolyVtx(m,n)			((m)->vertex[(n)])
#define	SrcPolyVtxIndex(m)		((m)->vertex)
#define	SrcPolyVtxPtr(m,n)		((m)->vtxp[(n)])
#define	SrcPolyVtxNormPtr(m,n)	((m)->vtnp[(n)])
#define	SrcPolyNormal(m,n)		((m)->rnorm[(n)])
#define	SrcPolyNormalPtr(n)		((n)->rnorm)
#define	SrcPolyMaterial(n)		((n)->material)
#define	SrcPolyFlag(n)			((n)->flag)
#define	SrcPolyBlend(n)			((n)->blend)
#define	SrcPolyCulling(n)		((n)->cull)
#define	SrcPolySmooth(n)		((n)->smooth)
#define	SrcPolyNext(n)			((n)->primnext)
#define	SrcPolyTexNum(n)		((n)->texnum)
#define	SrcPolyTexture(m,n)		((m)->texture[(n)])
#define	SrcPolyTexPntX(m,n,o)	((m)->texpnt[(n)][(o)][0])
#define	SrcPolyTexPntY(m,n,o)	((m)->texpnt[(n)][(o)][1])
#define	SrcPolyTexPnt(m,n,o)	((m)->texpnt[(n)][(o)])
#define	SrcPolyTexID(m,n)		((m)->texid[(n)])

#define	isSrcPolyCulling(n)		((SrcPolyFlag(n))&SrcPoly_Culling)
#define	isSrcPolyBlend(n)		((SrcPolyFlag(n))&SrcPoly_Blend)
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of Src Polygon Section =============================*/


/*=========================================================*/
/* Src Primitive Section ==================================*/
#define	SRC_PRIMITIVE_SIZE	8

#ifdef SRC_LIBGLD	/*---------------------------*/
typedef struct{
	unsigned long	polynum;			/*  4 */
	void			*polyroot;
	GLuint			glid;				/* NOT */
} SRC_PRIMITIVE;

#define	SrcPrimPolyNum(n)			((n)->polynum)
#define	SrcPrimPoly(n)				((n)->polyroot)
#define	SrcPrimGLID(n)				((n)->glid)
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of Src Primitive Section ===========================*/


/*=========================================================*/
/* Src Texture Section ====================================*/
#define	SRC_TEXTURE_SIZE	16

#define	SrcTex_Normal	0x00
#define	SrcTex_Color	0x01
#define	SrcTex_Alpha	0x02
#define	SrcTex_Mono		0x04

#ifdef SRC_LIBGLD	/*---------------------------*/
typedef struct{
	unsigned long	flag;				/*   4 */
	unsigned long	x;					/*   4 */
	unsigned long	y;					/*   4 */
	unsigned long	size;				/*   4 */
	unsigned char	*data;				/* NOT */
	int				texid; 				/* NOT */
} SRC_TEXTURE;

#define	SrcTexFlag(n)	((n)->flag)
#define	SrcTexX(n)		((n)->x)
#define	SrcTexY(n)		((n)->y)
#define	SrcTexSize(n)	((n)->size)
#define	SrcTexData(n)	((n)->data)
#define	SrcTexGLID(n)	((n)->texid)

#define	isSrcTexColor(n)	((SrcTexFlag(n))&SrcTex_Color)
#define	isSrcTexAlpha(n)	((SrcTexFlag(n))&SrcTex_Alpha)
#define	isSrcTexMono(n)		((SrcTexFlag(n))&SrcTex_Mono)
#endif	/* SRC_LIBGLD ---------------------------*/
/* EOL of Src Texture Section =============================*/


#ifdef SRC_LIBGLD
	#define	LibGLD_API
#else
	#define	LibGLD_API	extern
#endif
LibGLD_API int LibGLD_Init(Src_Model *,char *,unsigned short,unsigned long);
LibGLD_API unsigned long long LibGLD_Draw(Src_Model *);
LibGLD_API unsigned long long LibGLD_DrawWireFrame(Src_Model *);
LibGLD_API int LibGLD_Free(Src_Model *);
LibGLD_API GC2_INFO *GC2_InitInfo(void);
LibGLD_API int GC2_Load_File(GC2_INFO *gc2, char *fname);
LibGLD_API void GC2_FreeInfo(GC2_INFO *infop);
LibGLD_API unsigned char *GC2_CopyPictureData(unsigned long,unsigned char *);

#endif	/* LIBGLD_HEADER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* EOF of libgld.h ========================================*/

