#pragma once

#include "core.h"
#include "IPVRImage.h"

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

#define PVR_GEN_PIXEL_ID4(C1Name, C2Name, C3Name, C4Name, C1Bits, C2Bits, C3Bits, C4Bits) \
	( ( (uint64_t)C1Name) + ( (uint64_t)C2Name<<8) + ( (uint64_t)C3Name<<16) + ( (uint64_t)C4Name<<24) + \
	( (uint64_t)C1Bits<<32) + ( (uint64_t)C2Bits<<40) + ( (uint64_t)C3Bits<<48) + ( (uint64_t)C4Bits<<56) )

class CPVRImage : public IPVRImage
{
private:
	DISALLOW_COPY_AND_ASSIGN(CPVRImage);

public:
	CPVRImage();
	~CPVRImage();

public:
	virtual bool loadFile(IMemFile* file);
	virtual bool fromImageData(const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap);
	virtual const void* getMipmapData(uint32_t level) const;  
	virtual bool copyMipmapData(uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height);

private:
	void copy32BitMipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt) const;

private:
	uint8_t*			FileData;
	uint32_t			MipmapOffset[16];

public:

//values for each meta data type that we know about. Texture arrays hinge on each surface being identical in all but content, including meta data. 
//If the meta data varies even slightly then a new texture should be used. It is possible to write your own extension to get around this however.
enum EPVRTMetaData
{
	ePVRTMetaDataTextureAtlasCoords=0,
	ePVRTMetaDataBumpData,
	ePVRTMetaDataCubeMapOrder,
	ePVRTMetaDataTextureOrientation,
	ePVRTMetaDataBorderData,
	ePVRTMetaDataNumMetaDataTypes
};

enum EPVRTAxis
{
	ePVRTAxisX = 0,
	ePVRTAxisY = 1,
	ePVRTAxisZ = 2
};

enum EPVRTOrientation
{
	ePVRTOrientLeft	= 1<<ePVRTAxisX,
	ePVRTOrientRight= 0,
	ePVRTOrientUp	= 1<<ePVRTAxisY,
	ePVRTOrientDown	= 0,
	ePVRTOrientOut	= 1<<ePVRTAxisZ,
	ePVRTOrientIn	= 0
};

enum EPVRTColourSpace
{
	ePVRTCSpacelRGB,
	ePVRTCSpacesRGB
};

//Compressed pixel formats
enum EPVRTPixelFormat
{
	ePVRTPF_PVRTCI_2bpp_RGB,
	ePVRTPF_PVRTCI_2bpp_RGBA,
	ePVRTPF_PVRTCI_4bpp_RGB,
	ePVRTPF_PVRTCI_4bpp_RGBA,
	ePVRTPF_PVRTCII_2bpp,
	ePVRTPF_PVRTCII_4bpp,
	ePVRTPF_ETC1,
	ePVRTPF_DXT1,
	ePVRTPF_DXT2,
	ePVRTPF_DXT3,
	ePVRTPF_DXT4,
	ePVRTPF_DXT5,

	//These formats are identical to some DXT formats.
	ePVRTPF_BC1 = ePVRTPF_DXT1,
	ePVRTPF_BC2 = ePVRTPF_DXT3,
	ePVRTPF_BC3 = ePVRTPF_DXT5,

	//These are currently unsupported:
	ePVRTPF_BC4,
	ePVRTPF_BC5,
	ePVRTPF_BC6,
	ePVRTPF_BC7,
	ePVRTPF_UYVY,
	ePVRTPF_YUY2,
	ePVRTPF_BW1bpp,
	ePVRTPF_SharedExponentR9G9B9E5,
	ePVRTPF_RGBG8888,
	ePVRTPF_GRGB8888,
	ePVRTPF_ETC2_RGB,
	ePVRTPF_ETC2_RGBA,
	ePVRTPF_ETC2_RGB_A1,
	ePVRTPF_EAC_R11_Unsigned,
	ePVRTPF_EAC_R11_Signed,
	ePVRTPF_EAC_RG11_Unsigned,
	ePVRTPF_EAC_RG11_Signed,
	ePVRTPF_NumCompressedPFs
};

//Variable Type Names
enum EPVRTVariableType
{
	ePVRTVarTypeUnsignedByteNorm,
	ePVRTVarTypeSignedByteNorm,
	ePVRTVarTypeUnsignedByte,
	ePVRTVarTypeSignedByte,
	ePVRTVarTypeUnsignedShortNorm,
	ePVRTVarTypeSignedShortNorm,
	ePVRTVarTypeUnsignedShort,
	ePVRTVarTypeSignedShort,
	ePVRTVarTypeUnsignedIntegerNorm,
	ePVRTVarTypeSignedIntegerNorm,
	ePVRTVarTypeUnsignedInteger,
	ePVRTVarTypeSignedInteger,
	ePVRTVarTypeFloat,
	ePVRTVarTypeNumVarTypes
};

#	pragma pack (1)

struct PVRTextureHeaderV3
{
	uint32_t	u32Version;			//Version of the file header, used to identify it.
	uint32_t	u32Flags;			//Various format flags.
	uint64_t	u64PixelFormat;		//The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
	uint32_t	u32ColourSpace;		//The Colour Space of the texture, currently either linear RGB or sRGB.
	uint32_t	u32ChannelType;		//Variable type that the channel is stored in. Supports signed/unsigned int/short/byte or float for now.
	uint32_t	u32Height;			//Height of the texture.
	uint32_t	u32Width;			//Width of the texture.
	uint32_t	u32Depth;			//Depth of the texture. (Z-slices)
	uint32_t	u32NumSurfaces;		//Number of members in a Texture Array.
	uint32_t	u32NumFaces;		//Number of faces in a Cube Map. Maybe be a value other than 6.
	uint32_t	u32MIPMapCount;		//Number of MIP Maps in the texture - NB: Includes top level.
	uint32_t	u32MetaDataSize;	//Size of the accompanying meta data.
};

/*!***************************************************************************
Describes the Version 2 header of a PVR texture header.
*****************************************************************************/
struct PVR_Texture_Header
{
	uint32_t dwHeaderSize;		/*!< size of the structure */
	uint32_t dwHeight;			/*!< height of surface to be created */
	uint32_t dwWidth;				/*!< width of input surface */
	uint32_t dwMipMapCount;		/*!< number of mip-map levels requested */
	uint32_t dwpfFlags;			/*!< pixel format flags */
	uint32_t dwTextureDataSize;	/*!< Total size in bytes */
	uint32_t dwBitCount;			/*!< number of bits per pixel  */
	uint32_t dwRBitMask;			/*!< mask for red bit */
	uint32_t dwGBitMask;			/*!< mask for green bits */
	uint32_t dwBBitMask;			/*!< mask for blue bits */
	uint32_t dwAlphaBitMask;		/*!< mask for alpha channel */
	uint32_t dwPVR;				/*!< magic number identifying pvr file */
	uint32_t dwNumSurfs;			/*!< the number of surfaces present in the pvr */
} ;

#	pragma pack ()

/*****************************************************************************
* Legacy (V2 and V1) ENUMS
*****************************************************************************/

enum PVRTPixelType
{
	MGLPT_ARGB_4444 = 0x00,
	MGLPT_ARGB_1555,
	MGLPT_RGB_565,
	MGLPT_RGB_555,
	MGLPT_RGB_888,
	MGLPT_ARGB_8888,
	MGLPT_ARGB_8332,
	MGLPT_I_8,
	MGLPT_AI_88,
	MGLPT_1_BPP,
	MGLPT_VY1UY0,
	MGLPT_Y1VY0U,
	MGLPT_PVRTC2,
	MGLPT_PVRTC4,

	// OpenGL version of pixel types
	OGL_RGBA_4444= 0x10,
	OGL_RGBA_5551,
	OGL_RGBA_8888,
	OGL_RGB_565,
	OGL_RGB_555,
	OGL_RGB_888,
	OGL_I_8,
	OGL_AI_88,
	OGL_PVRTC2,
	OGL_PVRTC4,
	OGL_BGRA_8888,
	OGL_A_8,
	OGL_PVRTCII4,	//Not in use
	OGL_PVRTCII2,	//Not in use

	// S3TC Encoding
	D3D_DXT1 = 0x20,
	D3D_DXT2,
	D3D_DXT3,
	D3D_DXT4,
	D3D_DXT5,

	//RGB Formats
	D3D_RGB_332,
	D3D_AL_44,
	D3D_LVU_655,
	D3D_XLVU_8888,
	D3D_QWVU_8888,

	//10 bit integer - 2 bit alpha
	D3D_ABGR_2101010,
	D3D_ARGB_2101010,
	D3D_AWVU_2101010,

	//16 bit integers
	D3D_GR_1616,
	D3D_VU_1616,
	D3D_ABGR_16161616,

	//Float Formats
	D3D_R16F,
	D3D_GR_1616F,
	D3D_ABGR_16161616F,

	//32 bits per channel
	D3D_R32F,
	D3D_GR_3232F,
	D3D_ABGR_32323232F,

	// Ericsson
	ETC_RGB_4BPP,
	ETC_RGBA_EXPLICIT,				// unimplemented
	ETC_RGBA_INTERPOLATED,			// unimplemented

	D3D_A8 = 0x40,
	D3D_V8U8,
	D3D_L16,

	D3D_L8,
	D3D_AL_88,

	//Y'UV Colourspace
	D3D_UYVY,
	D3D_YUY2,

	// DX10
	DX10_R32G32B32A32_FLOAT= 0x50,
	DX10_R32G32B32A32_UINT , 
	DX10_R32G32B32A32_SINT,

	DX10_R32G32B32_FLOAT,
	DX10_R32G32B32_UINT,
	DX10_R32G32B32_SINT,

	DX10_R16G16B16A16_FLOAT ,
	DX10_R16G16B16A16_UNORM,
	DX10_R16G16B16A16_UINT ,
	DX10_R16G16B16A16_SNORM ,
	DX10_R16G16B16A16_SINT ,

	DX10_R32G32_FLOAT ,
	DX10_R32G32_UINT ,
	DX10_R32G32_SINT ,

	DX10_R10G10B10A2_UNORM ,
	DX10_R10G10B10A2_UINT ,

	DX10_R11G11B10_FLOAT ,				// unimplemented

	DX10_R8G8B8A8_UNORM , 
	DX10_R8G8B8A8_UNORM_SRGB ,
	DX10_R8G8B8A8_UINT ,
	DX10_R8G8B8A8_SNORM ,
	DX10_R8G8B8A8_SINT ,

	DX10_R16G16_FLOAT , 
	DX10_R16G16_UNORM , 
	DX10_R16G16_UINT , 
	DX10_R16G16_SNORM ,
	DX10_R16G16_SINT ,

	DX10_R32_FLOAT ,
	DX10_R32_UINT ,
	DX10_R32_SINT ,

	DX10_R8G8_UNORM ,
	DX10_R8G8_UINT ,
	DX10_R8G8_SNORM , 
	DX10_R8G8_SINT ,

	DX10_R16_FLOAT ,
	DX10_R16_UNORM ,
	DX10_R16_UINT ,
	DX10_R16_SNORM ,
	DX10_R16_SINT ,

	DX10_R8_UNORM, 
	DX10_R8_UINT,
	DX10_R8_SNORM,
	DX10_R8_SINT,

	DX10_A8_UNORM, 
	DX10_R1_UNORM, 
	DX10_R9G9B9E5_SHAREDEXP,	// unimplemented
	DX10_R8G8_B8G8_UNORM,		// unimplemented
	DX10_G8R8_G8B8_UNORM,		// unimplemented

	DX10_BC1_UNORM,	
	DX10_BC1_UNORM_SRGB,

	DX10_BC2_UNORM,	
	DX10_BC2_UNORM_SRGB,

	DX10_BC3_UNORM,	
	DX10_BC3_UNORM_SRGB,

	DX10_BC4_UNORM,				// unimplemented
	DX10_BC4_SNORM,				// unimplemented

	DX10_BC5_UNORM,				// unimplemented
	DX10_BC5_SNORM,				// unimplemented

	// OpenVG

	/* RGB{A,X} channel ordering */
	ePT_VG_sRGBX_8888  = 0x90,
	ePT_VG_sRGBA_8888,
	ePT_VG_sRGBA_8888_PRE,
	ePT_VG_sRGB_565,
	ePT_VG_sRGBA_5551,
	ePT_VG_sRGBA_4444,
	ePT_VG_sL_8,
	ePT_VG_lRGBX_8888,
	ePT_VG_lRGBA_8888,
	ePT_VG_lRGBA_8888_PRE,
	ePT_VG_lL_8,
	ePT_VG_A_8,
	ePT_VG_BW_1,

	/* {A,X}RGB channel ordering */
	ePT_VG_sXRGB_8888,
	ePT_VG_sARGB_8888,
	ePT_VG_sARGB_8888_PRE,
	ePT_VG_sARGB_1555,
	ePT_VG_sARGB_4444,
	ePT_VG_lXRGB_8888,
	ePT_VG_lARGB_8888,
	ePT_VG_lARGB_8888_PRE,

	/* BGR{A,X} channel ordering */
	ePT_VG_sBGRX_8888,
	ePT_VG_sBGRA_8888,
	ePT_VG_sBGRA_8888_PRE,
	ePT_VG_sBGR_565,
	ePT_VG_sBGRA_5551,
	ePT_VG_sBGRA_4444,
	ePT_VG_lBGRX_8888,
	ePT_VG_lBGRA_8888,
	ePT_VG_lBGRA_8888_PRE,

	/* {A,X}BGR channel ordering */
	ePT_VG_sXBGR_8888,
	ePT_VG_sABGR_8888 ,
	ePT_VG_sABGR_8888_PRE,
	ePT_VG_sABGR_1555,
	ePT_VG_sABGR_4444,
	ePT_VG_lXBGR_8888,
	ePT_VG_lABGR_8888,
	ePT_VG_lABGR_8888_PRE,

	// max cap for iterating
	END_OF_PIXEL_TYPES,

	MGLPT_NOTYPE = 0xffffffff
};

};