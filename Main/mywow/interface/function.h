#pragma once

#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cassert>
#include <cerrno>
#include "base.h"

#ifdef MW_PLATFORM_WINDOWS
#include <share.h>
#include <direct.h>
#include <io.h>
#include <malloc.h>
#include <WinNls.h>
#else
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

//
//		math
//

#define IR(x)                           ((u32&)(x))
#define FR(x)                           ((f32&)(x))

const s32 ROUNDING_ERROR_S32 = 0;
const f32 ROUNDING_ERROR_f32 =  0.000001f;

const f32 PI = 3.14159265358979324f;

const f32 DEGTORAD = 0.01745329251994329577f;
const f32 RADTODEG = 57.2957795130823208768f;

inline f32 radToDeg( f32 radians ){ return RADTODEG*radians; }
inline f32 degToRad( f32 degrees ){ return DEGTORAD*degrees; }

template<class T>
inline const T& min_(const T& a, const T& b)
{
	return a < b ? a : b;
}
template<class T>
inline const T& max_(const T& a, const T& b)
{
	return a < b ? b : a;
}
template<class T>
inline T abs_(const T& a)
{
	return a < (T)0.f ? -a : a;
}
template<class T>
inline T lerp_(const T& a, const T& b, const f32 t)
{
	return (T)(a*(1.f-t)) + (b*t);
}
template <class T>
inline const T clamp_(const T& value, const T& low, const T& high)
{
	return min_ (max_(value,low), high);
}
template <class T>
inline void swap_(T& a, T& b)
{
	T c(a);
	a = b;
	b = c;
}
inline bool equals_(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const s32 a, const s32 b, const s32 tolerance = ROUNDING_ERROR_S32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const u32 a, const u32 b, const s32 tolerance = ROUNDING_ERROR_S32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool iszero_(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return fabsf(a) <= tolerance;
}
inline f32 squareroot_(const f32 f)
{
	return sqrtf(f);
}

inline f32 reciprocal_squareroot_(const f32 f)
{
	if (equals_(f, 0.0f))
	{
		ASSERT(false);
		return FLT_MAX;
	}
	else
		return 1.f / sqrtf(f);
}

inline f32 reciprocal_( const f32 f )
{
	if (equals_(f, 0.0f))
	{
		ASSERT(false);
		return FLT_MAX;
	}
	else
		return 1.f / f;
}
inline s32 floor32_(f32 x)
{
	return (s32) floorf ( x );
}
inline s32 ceil32_( f32 x )
{
	return (s32) ceilf ( x );
}
inline f32 round_( f32 x )
{
	return floorf( x + 0.5f );
}
inline s32 round32_(f32 x)
{
	return (s32)round_(x);
}
inline  f32 fract_ ( f32 x )
{
	return x - floorf ( x );
}

static int randSeed = 0;

inline void	srand_( unsigned seed ) {
	randSeed = seed;
}

inline  int		rand_( void ) {
	randSeed = (69069 * randSeed + 1);
	return randSeed & 0x7fff;
}

inline float random_0_to_1() {
	union {
		u32 d;
		float f;
	} u;
	u.d = (((u32)rand_() & 0x7fff) << 8) | 0x3f800000;
	return u.f - 1.0f;
}

inline float random_minus1_to_1() {
	union {
		u32 d;
		float f;
	} u;
	u.d = (((u32)rand_() & 0x7fff) << 8) | 0x40000000;
	return u.f - 3.0f;
}

inline  int	randint( int lower, int upper)
{
	return lower + (int)((upper + 1 - lower) * random_0_to_1());
}

inline bool isPOT(u32 x)
{
	return (x > 0) && ((x & (x-1)) == 0);
}

inline void flipcc(char fcc[4])
{
	char t;
	t=fcc[0];
	fcc[0]=fcc[3];
	fcc[3]=t;
	t=fcc[1];
	fcc[1]=fcc[2];
	fcc[2]=t;
}

inline void* Q_malloc(size_t size, u32 align)
{
	return malloc(ROUND_N_BYTES(size, align));
}

inline void Q_free(void* ptr)
{
	free(ptr);
}

inline void Q_memcpy( void * dst,
	size_t bytes,
	const void * src,
	size_t count)
{
#ifdef MW_PLATFORM_WINDOWS
	memcpy_s(dst, bytes, src, count);
#else
	memcpy(dst, src, count);
#endif
}

inline void Q_memset( void *dst,
	int val,
	size_t count)
{
	memset(dst, val, count);
}

inline int Q_memcmp( const void * buf1,
	const void * buf2,
	size_t count)
{
	return memcmp(buf1, buf2, count);
}

#ifdef MW_PLATFORM_WINDOWS
#define Q_sscanf		sscanf_s
#else
#define Q_sscanf		sscanf
#endif

// #ifdef MW_PLATFORM_WINDOWS
// #define Q_swprintf swprintf_s
// #else
// #define Q_swprintf swprintf
// #endif

#ifdef MW_PLATFORM_WINDOWS
#define Q_vsprintf vsprintf_s
#else
#define Q_vsprintf vsnprintf
#endif

inline void Q_strcpy(char * dst, size_t count, const char * src)
{
#ifdef MW_PLATFORM_WINDOWS
	strcpy_s(dst, count, src);
#else
	strncpy(dst, src, count-1);
	dst[count-1] = '\0';
#endif
}

inline void Q_strlwr(char * dst)
{
#ifdef MW_PLATFORM_WINDOWS
	_strlwr_s(dst, strlen(dst)+1);
#else
	char* p = dst;
	for ( ; *p != 0; p++)
		*p = tolower(*p);
#endif
}

inline void Q_strupr(char * dst, size_t count)
{
#ifdef MW_PLATFORM_WINDOWS
	_strupr_s(dst, count);
#else
	char* p = dst;
	for ( ; *p != 0; p++)
		*p = toupper(*p);
#endif
}

// inline void Q_utf16cpy(c16* dst, size_t count, const c16* src)
// {
// #ifdef MW_PLATFORM_WINDOWS
// 	wcscpy_s((wchar_t*)dst, count, (const wchar_t*)src);
// #else
// 	const c16* cs = src;
// 	c16* ct = dst;
// 	size_t c = 0;
// 	while (*cs && c + 1< count)
// 	{
// 		*ct = *cs;
// 		++c;
// 		++cs;
// 		++ct;
// 	}
// 	*ct = 0;
// #endif
// }

inline void Q_strncpy( char * dest,
	size_t bytes,
	const char * source,
	size_t count)
{
	assert(bytes >= count + 1);
#ifdef MW_PLATFORM_WINDOWS
	strncpy_s(dest, bytes, source, count);
#else
	strncpy(dest, source, count);
#endif
}

inline void Q_strcat( char * dst, size_t count, const char * src)
{
#ifdef MW_PLATFORM_WINDOWS
	strcat_s(dst, count, src);
#else
	strcat(dst, src);
#endif
}

inline int Q_strcmp( const char * src,
	const char * dst)
{
	return strcmp(src, dst);
}

inline int Q_stricmp(const char * src,
	const char * dst)
{
#ifdef MW_PLATFORM_WINDOWS
	return _stricmp(src, dst);
#else
	return strcasecmp(src, dst);
#endif
}

inline int Q_strnicmp(const char * src, const char * dst, size_t len)
{
#ifdef MW_PLATFORM_WINDOWS
	return _strnicmp(src, dst, len);
#else
	return strncasecmp(src, dst, len);
#endif
}

// inline int Q_wcsicmp(const c16 * src,
// 	const c16 * dst)
// {
// #ifdef MW_PLATFORM_WINDOWS
// 	return _wcsicmp((const wchar_t*)src, (const wchar_t*)dst);
// #else
// 	const c16* cs = src;
// 	const c16* ct = dst;
// 	
// 	while (towlower((wchar_t)*cs) == towlower((wchar_t)*ct))
// 	{
// 		if (*cs == 0)
// 			return 0;
// 		cs++;
// 		ct++;
// 	}
// 	return towlower((wchar_t)*cs) - towlower((wchar_t)*ct);
// #endif
// }

inline size_t Q_strlen(const char * src)
{
	return strlen(src);
}

inline int Q_sprintf( char *string, size_t sizeInBytes, const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);

	int ret = Q_vsprintf(string, sizeInBytes, format, arglist);

	va_end( arglist );

	return ret;
}

inline size_t utf16len(const c16* utf16)
{
	const c16* p = utf16;
	if (p == 0)
		return 0;
	while(*p) ++p;
	return (size_t)(p - utf16);
}

struct UTF8_EncodedChar
{
	UTF8_EncodedChar()
	{
		memset(bytes, 0, 8);
	}
	int GetByteCount()
	{
		return len > 6 ? 6 : len;
	}
	union
	{
		char bytes[8];
		struct
		{
			char byte0;
			char byte1;
			char byte2;
			char byte3;
			char byte4;
			char byte5;
			char byte6; // always null
			u8 len;
		};
	};
};

inline UTF8_EncodedChar Q_EncodeUTF8(int ch)
{
	UTF8_EncodedChar ch8;
	if(ch < 0x80)
	{ // 1-byte
		ch8.len = 1;
		ch8.byte0 = (char)ch;
	}
	else if (ch < 0x800)
	{ // 2-byte
		ch8.len = 2;
		ch8.byte0 = 0xC0 | ((ch & 0x7C0) >> 6);
		ch8.byte1 = 0x80 | (ch & 0x3F);
	}
	else if(ch < 0x10000)
	{ // 3-byte
		ch8.len = 3;
		ch8.byte0 = 0xE0 | ((ch & 0xF000) >> 12);
		ch8.byte1 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte2 = 0x80 | (ch & 0x3F);
	}
	else if(ch < 0x200000)
	{ // 4-byte
		ch8.len = 4;
		ch8.byte0 = 0xF0 | ((ch & 0x1C0000) >> 18);
		ch8.byte1 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte2 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte3 = 0x80 | (ch & 0x3F);
	}
	else if(ch < 0x4000000)
	{ // 5-byte
		ch8.len = 5;
		ch8.byte0 = 0xF8 | ((ch & 0x3000000) >> 24);
		ch8.byte1 = 0x80 | ((ch & 0x0FC0000) >> 18);
		ch8.byte2 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte3 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte4 = 0x80 | (ch & 0x3F);
	}
	else if(ch > 0)
	{ // 6-byte
		ch8.len = 6;
		ch8.byte0 = 0xFC | ((ch & 0x40000000) >> 30);
		ch8.byte1 = 0x80 | ((ch & 0x3F000000) >> 24);
		ch8.byte2 = 0x80 | ((ch & 0x0FC0000) >> 18);
		ch8.byte3 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte4 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte5 = 0x80 | (ch & 0x3F);
	}
	else
	{ // error.

	}
	return ch8;
}

inline int Q_ParseUnicodeFromUTF8Str(const char* szUTF8, int* pnAdvancedInUtf8Str = 0, u32 nUtf8StrMaxLen = A_MAX_UINT32)
{
	u32 nAdv = 0;
	int nResult = 0;
	if(pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = 0;

	// Check
	if(szUTF8 == 0)
		return 0;
	if(*szUTF8 == 0)
		return 0;
	if(nUtf8StrMaxLen == 0)
		return 0;

	// Byte count to combine.
	char ch = *szUTF8;
	if((ch & 0x80) == 0)
		nAdv = 1;
	else
	{
		for(int i = 0; i < 8; ++i)
		{
			if(ch & (0x80 >> i))
				++nAdv;
			else
				break;
		}
		if(nAdv == 1)
		{ // start with 10XX XXXX ? error
			if(pnAdvancedInUtf8Str != 0)
				*pnAdvancedInUtf8Str = 1;
			return -1;
		}
	}
	if(nAdv > nUtf8StrMaxLen)
	{
		// the str is not complete.
		return 0;
	}
	if(nAdv > 6)
	{ // start with 1111 111X ? error
		if(pnAdvancedInUtf8Str != 0)
			*pnAdvancedInUtf8Str = 1;
		return -1;
	}
	if(pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = nAdv;

	// Parse
	if(nAdv == 1)
	{
		nResult = ch & 0x7F;
	}
	else
	{
		nResult = ch & (0xFF >> (nAdv + 1));
		for(u32 i = 1; i < nAdv; ++i)
		{
			ch = szUTF8[i];
			if((ch & 0xC0) != 0x80)
			{ // not 10XX XXXX
				return -1;
			}
			nResult = nResult << 6;
			nResult |= (ch & 0x3F);
		}
	}
	return nResult;
}

inline size_t Q_UTF16Len(const c16* utf16)
{
	const c16* p = utf16;
	if (p == 0)
		return 0;
	while(*p) ++p;
	return (size_t)(p - utf16);
}

inline void utf8to16( const c8* utf8, c16* utf16, u32 size)
{
	/*
	std::wstring_convert<std::codecvt_utf8<c16>, c16> conv;
	auto ret = conv.from_bytes( utf8 );
	u32 len = (u32)ret.length();
	//ASSERT(size > len);
	u32 count = min_(size-1, len);
	ret.copy(utf16, count, 0);		//clip
	utf16[count] = (c16)'\0';
	*/

	if(utf8 == 0)
	{
		*utf16 = 0;
		return;
	}
	u32 count = 0;
	while (*utf8 != 0)
	{
		int nAdv;
		int value = Q_ParseUnicodeFromUTF8Str(utf8, &nAdv);
		if(nAdv == 0)
			break;
		utf8 += nAdv;
		if(value < 0) continue;
		++count;
		if(value >= 0x10000)
			++count;

		if (count+1 > size)			//exceed
			break;

		if(utf16 != 0)
		{
			if(value < 0x10000)
				*(utf16++) = value;
			else
			{
				u32 ch322 = value - 0x10000;
				*(utf16++) = ((ch322 & 0xFFC00) >> 10) | 0xD800;
				*(utf16++) = (ch322 & 0x3FF) | 0xDC00;
			}
		}
	}
	if(utf16 != 0)
		*utf16 = 0;
}

inline void utf16to8( const c16* utf16, c8* utf8, u32 size )
{
	/*
	std::wstring_convert<std::codecvt_utf8<c16>, c16> conv;
	auto ret = conv.to_bytes( utf16 );
	u32 len = (u32)ret.length();
	//ASSERT(size > len);
	u32 count = min_(size-1, len);
	ret.copy(utf8, count, 0);
	utf8[count] = (c8)'\0';
	*/
	if(utf16 == 0)
	{
		*utf8 = 0;
		return;
	}
	u32 count = 0;
	// 	if(*sz16 == (auint16)0xFEFF)
	// 	{
	// 		++sz16;
	// 	}
	while (*utf16 != 0)
	{
		int nAdv;
		int ch = *(utf16++);
		int ch32 = ch;
		if(ch >= 0xD800 && ch <= 0xDBFF)
		{
			c16 ch2 = *(utf16++);
			if(ch2 >= 0xDC00 && ch2 <= 0xDFFF)
			{
				ch32 = ch & 0xD800;
				ch32 <<= 10;
				ch32 |= ch2 & 0xDC00;
			}
		}
		UTF8_EncodedChar value = Q_EncodeUTF8(ch32);
		nAdv = value.GetByteCount();
		count += nAdv;

		if (count+1 > size)  //exceed
			break;

		if(utf8 != 0)
		{
			for(int i = 0; i < nAdv; ++i)
			{
				*(utf8++) = value.bytes[i];
			}
		}
	}
	if(utf8 != 0)
		*utf8 = 0;
}

inline void utf16_to_gbk(const c16* utf16, c8* gbk, u32 size)
{
#ifdef MW_PLATFORM_WINDOWS
	if (utf16)
	{
		WideCharToMultiByte( 936, 0, (LPCWSTR)utf16, -1, gbk, size, NULL, NULL);

		gbk[size - 1] = '\0';
	}
	else
		gbk[0] = '\0';
#else
	if (utf16)
	{
		iconv_t cd = iconv_open("GB2312", "UCS-2-INTERNAL");
		if (cd == reinterpret_cast<iconv_t>(-1))
		{
			assert(false);
			gbk[0] = '\0';
			return;
		}

		size_t	sourceLength	= utf16len(utf16) * sizeof(c16);
		size_t	destLength		= size - 1;
		char*	pSource 		= const_cast<char*>(reinterpret_cast<const char*>(utf16));
		char*	pDest			= gbk;

		iconv(cd, &pSource, &sourceLength, &pDest, &destLength);

		iconv_close(cd);
		gbk[size - 1] = '\0';
	}
	else
		gbk[0] = '\0';
#endif
}

inline void gbk_to_utf16(const c8* gbk, c16* utf16, u32 size)
{
#ifdef MW_PLATFORM_WINDOWS				//windows下gbk编码
	if (gbk)
	{
		//gbk - unicode
		MultiByteToWideChar(936, 0, gbk, -1, (LPWSTR)utf16, size);
		utf16[size - 1] = (c16)'\0';
	}
	else
	{
		utf16[0] = (c16)'\0';
	}
#else							//android 和 ios下utf8编码
	if(gbk)
	{
		iconv_t cd = iconv_open("UCS-2-INTERNAL", "GB2312");
		if (cd == reinterpret_cast<iconv_t>(-1))
		{
			assert(false);
			utf16[0] = (c16)'\0';
			return;
		}

		size_t	sourceLength	= strlen(gbk);
		size_t	destLength		= (size - 1) * sizeof(c16);
		char*	pSource 		= const_cast<char*>(gbk);
		char*	pDest			= reinterpret_cast<char*>(utf16);

		iconv(cd, &pSource, &sourceLength, &pDest, &destLength);

		iconv_close(cd);

		utf16[size - 1] = 0;
	}
	else
		utf16[0] = 0;
#endif
}

inline FILE*  Q_fopen(const char * filename, const char * mode)
{
#ifdef MW_PLATFORM_WINDOWS
	return _fsopen(filename, mode, _SH_DENYWR);
#else
	return fopen(filename, mode);
#endif
}

inline int Q_access(const char * filename, int mode)
{
#ifdef MW_PLATFORM_WINDOWS
	return (int)_access_s(filename, mode);
#else
	return access(filename, mode);
#endif
}

inline void Q_getLocalTime(c8* timebuf, size_t size)
{
#ifdef MW_PLATFORM_WINDOWS
	SYSTEMTIME time;
	GetLocalTime(&time);
	Q_sprintf(timebuf, size, "%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%0.3d\t", time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
#else
	::tm t;
	::time_t m_time;
	::timeval timeofday;
	::localtime_r(&m_time, &t);
	::gettimeofday(&timeofday, NULL_PTR);
	Q_sprintf(
		timebuf, 
		size, 
		"%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%0.3d\t", 
		t.tm_mon + 1, 
		t.tm_mday, 
		t.tm_hour, 
		t.tm_min, 
		t.tm_sec, 
		timeofday.tv_usec / 1000);
#endif
}

inline void Q_fullpath(const char* filename, char* outfilename, size_t size)
{
#ifdef MW_PLATFORM_WINDOWS
	if (!_fullpath(outfilename, filename, size))	
		ASSERT(false);
#else
	if(!realpath(filename, outfilename))
        strcpy(outfilename, filename);
#endif
}

inline int Q_mkdir(const char* path)
{
#ifdef MW_PLATFORM_WINDOWS
	return _mkdir(path);
#else
	return mkdir(path, 0777);
#endif
}

inline int Q_rmdir(const char* path)
{
#ifdef MW_PLATFORM_WINDOWS
	return _rmdir(path);
#else
	return rmdir(path);
#endif
}

inline char* Q_getcwd(char* dstbuf, int sizebytes)
{
#ifdef  MW_PLATFORM_WINDOWS
	return _getcwd(dstbuf, sizebytes);
#else
	return getcwd(dstbuf, sizebytes);
#endif
}

inline bool isAbsoluteFileName(const c8* filename)
{
	u32 len = (u32)strlen(filename);
#ifdef  MW_PLATFORM_WINDOWS
	if (len >= 2)
		return filename[1] == ':';
#else
	if (len >= 1)
		return filename[0] == '/';
#endif

	return false;
}

inline void normalizeFileName(const c8* filename, c8* outfilename, u32 size)
{
	ASSERT(size >= strlen(filename)+1);
	u32 len = (u32)strlen(filename);
	for (u32 i=0; i<len; ++i)
	{
		if(filename[i] == '\\')
			outfilename[i] = '/';
		else
			outfilename[i] = filename[i];
	}
	outfilename[len] = '\0';
}

inline void normalizeDirName(c8* dirName)
{
	u32 len = (u32)strlen(dirName);
	for (u32 i=0; i<len; ++i)
	{
		if(dirName[i] == '\\')
			dirName[i] = '/';
	}

	if (len > 0)
	{
		c8 last = dirName[len -1];
		if (last != '/' && last != '\\' )
			Q_strcat(dirName, len + 2, "/");
	}
}

inline void normalizeFileName(c8* filename)
{
	u32 len = (u32)strlen(filename);
	for (u32 i=0; i<len; ++i)
	{
		if(filename[i] == '\\')
			filename[i] = '/';
	}
}

inline bool isNormalized(const c8* filename)
{
	u32 len = (u32)strlen(filename);
	for (u32 i=0; i<len; ++i)
	{
		if(filename[i] == '\\')
			return false;	
	}
	return true;
}

inline bool isLowerFileName(const c8* filename)
{
	u32 len = (u32)strlen(filename);
	for (u32 i=0; i<len; ++i)
	{
		if(isupper((unsigned char)filename[i]))
			return false;	
	}
	return true;
}

typedef void (*ITERATEFILECALLBACK)(const c8* filename, void* args);

inline bool Q_iterateFiles(const c8* dirname, const c8* ext, ITERATEFILECALLBACK callback, void* args, const c8* initdir = "")
{	
	c8 path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, dirname);
	normalizeDirName(path);
	Q_strcat(path, QMAX_PATH, ext);


	c8 initdirname[QMAX_PATH];
	Q_strcpy(initdirname, QMAX_PATH, initdir);
	normalizeDirName(initdirname);

#ifdef MW_PLATFORM_WINDOWS
	_finddata_t finddata;
	intptr_t hfile = _findfirst(path, &finddata);
	if (hfile == -1)
		return false;

	do
	{
		if (strcmp(finddata.name, "..") == 0 || strcmp(finddata.name, ".") == 0)
			continue;

		if (finddata.attrib & (_A_HIDDEN | _A_SYSTEM))
			continue;

		c8 subpath[QMAX_PATH];
		Q_strcpy(subpath, QMAX_PATH, dirname);
		normalizeDirName(subpath);
		Q_strcat(subpath, QMAX_PATH, finddata.name);

		if (finddata.attrib & _A_SUBDIR)
		{	
			Q_iterateFiles(subpath, ext, callback, args, initdir);
		}
		else
		{
			if (strlen(initdirname) == 0)
			{
				callback(subpath, args);
			}
			else
			{
				c8 shortpath[QMAX_PATH];
				const c8* start = strstr(subpath, initdirname);
				u32 len = (u32)strlen(initdir);
				if (initdirname[len-1] != '\\' && initdirname[len -1] != '/')
					++len;
				Q_strcpy(shortpath, QMAX_PATH, start ? start + len : subpath);
				callback(shortpath, args);
			}
		}
	} while (_findnext(hfile, &finddata) != -1);

	if (errno != ENOENT)
	{
		_findclose(hfile);
		return false;
	}

	_findclose(hfile);
	return true;

#else
	DIR* handle = opendir(path);
	if (NULL_PTR == handle)
	{
		perror("directory::open");
		return false;
	}

	while (dirent* pdata = readdir(handle))
	{
		if (strcmp(pdata->d_name, "..") == 0 || strcmp(pdata->d_name, ".") == 0)
			continue;

		if (pdata->d_name[0] == 0)
			continue;

		c8 subpath[QMAX_PATH];
		Q_strcpy(subpath, QMAX_PATH, dirname);
		normalizeDirName(subpath);
		Q_strcat(subpath, QMAX_PATH, pdata->d_name);

		//获取file state
		struct stat buf;
		if (0 != stat(subpath, &buf))
		{
			perror("stat() failed");
			closedir(handle);
			return false;
		}

		if (S_ISDIR(buf.st_mode))
		{
			Q_iterateFiles(subpath, ext, callback, args, initdir);
		}
		else
		{
			if (strlen(initdirname) == 0)
			{
				callback(subpath, args);
			}
			else
			{
				c8 shortpath[QMAX_PATH];
				const c8* start = strstr(subpath, initdirname);
				u32 len = (u32)strlen(initdir);
				if (initdirname[len-1] != '\\' && initdirname[len -1] != '/')
					++len;
				Q_strcpy(shortpath, QMAX_PATH, start ? start + len : subpath);
				callback(shortpath, args);
			}
		}
	}

	closedir(handle);
	return true;

#endif
}

inline void getFileDirA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* lastSlash =strrchr( filename, ('/') );
	const c8* lastBackSlash = strrchr( filename, ('\\') );
	if ( !lastSlash || lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;

	if (!lastSlash || lastSlash-filename<=0)
		Q_strcpy(outfilename, size, ("."));
	else
	{
		Q_strncpy( outfilename, size, filename, lastSlash-filename );
		outfilename[lastSlash-filename] = '\0';
	}
}

inline void getFileNameA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* lastSlash = strrchr( filename, '/' );
	const c8* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;

	bool noPrefix = (!lastSlash || *(lastSlash+1) == '\0');
	if (noPrefix)
		Q_strcpy(outfilename, size, filename);
	else
		Q_strcpy( outfilename, size, lastSlash+1 );
}

inline void getFileNameNoExtensionA(const c8* filename, c8* outfilename, u32 size)
{
	const c8* lastSlash = strrchr( filename, '/' );
	const c8* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;
	const c8* p = strrchr(filename, '.' );
	if (p < lastSlash)
	{
		Q_strcpy(outfilename, size, filename);
		return;
	}

	bool noPrefix = (!lastSlash || *(lastSlash+1) == '\0');
	bool noSuffix = (!p || *(p+1)=='\0');
	if (noPrefix && noSuffix)
		Q_strcpy(outfilename, size, filename);
	else if(noPrefix)			//suffix
	{
        Q_strncpy(outfilename, size, filename, p-filename);
        outfilename[p-filename] = '\0';
    }
	else if(noSuffix)			//prefix
		Q_strcpy( outfilename, size, lastSlash+1 );
	else
    {
		Q_strncpy(outfilename, size, lastSlash+1, p-(lastSlash+1));
        outfilename[p-(lastSlash+1)] = '\0';
    }
}

inline void getFileExtensionA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* lastSlash = strrchr( filename, '/' );
	const c8* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;
	
	const c8* p = strrchr(filename, '.' );
	if (p < lastSlash)
	{
		Q_strcpy(outfilename, size, "");
		return;
	}

	if (!p || *(p+1)=='\0')
		Q_strcpy(outfilename, size, "");
	else
		Q_strcpy( outfilename, size, p+1 );
}

inline void getFullFileNameNoExtensionA(const c8* filename, c8* outfilename, u32 size)
{
	const c8* lastSlash = strrchr( filename, '/' );
	const c8* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;

	const c8* p = strrchr(filename, '.' );
	if (p && p < lastSlash)
	{
		Q_strcpy(outfilename, size, "");
		return;
	}

	if (!p || *(p+1)=='\0')
		Q_strcpy(outfilename, size, filename);
	else
    {
		Q_strncpy(outfilename, size, filename, p-filename);
        outfilename[p-filename] = '\0';
    }
}

inline bool hasFileExtensionA( const c8* filename, const c8* ext )
{
	if (*ext == '*')
		return true;

	c8 extension[32];
	getFileExtensionA(filename, extension, 32);

	if (  Q_stricmp(extension, ext) != 0 )
		return false;
	return true;
}

inline void makeHDFileName(c8* name, u32 size, const c8* filename)
{
	const c8* p = strrchr(filename, '.' );
	if (p)
	{
		Q_strncpy(name, size, filename, p-filename);
		Q_strcat(name, size, "_HD");
		Q_strcat(name, size, p);
	}
	else
	{
		Q_strcat(name, size, "_HD");
	}
}

inline bool Q_MakeDirForFileName(const c8* filename)
{
	//create directory if not exist
	c8 dir[QMAX_PATH];
	getFileDirA(filename, dir, QMAX_PATH);
	normalizeFileName(dir);
	Q_strcat(dir, QMAX_PATH, "/");

	const c8* p = strchr(dir, '/');

	while(p)
	{
		c8 tmp[QMAX_PATH];
		Q_strncpy(tmp, QMAX_PATH, dir, p-dir+1);
        tmp[p-dir+1] = '\0';
		if (Q_access(tmp, 0) != 0)
		{
			int ret = Q_mkdir(tmp);
			if (ret != 0 && ret != EEXIST)
				return false;
		}
		p = strchr(p+1, '/');
	}
	return true;
}

inline bool getBitH2L(u8* data, u32 bit)
{
	u8	byte = data[bit/8];
	u8	pos = bit % 8;
	u8	mask = 0x80;
	mask >>= pos;
	return (byte & mask) == mask;
}

inline bool getBitL2H(u8* data, u32 bit)
{
	u8	byte = data[bit/8];
	u8	pos = bit % 8;
	u8	mask = 0x1;
	mask <<= pos;

	return (byte & mask) == mask;
}

inline void trim(const c8* str, c8* outstring, u32 size)
{
	u32 len = (u32)strlen(str);
	if (len == 0)
	{
		Q_strcpy(outstring, size, "");
		return;
	}
	u32 left = 0;
	u32 right = len;
	for (u32 i=0; i<len; ++i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			++left;
		else
			break;
	}

	for (s32 i=(s32)len-1; i>=0; --i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			--right;
		else
			break;
	}

	if (left >= right)
	{
		Q_strcpy(outstring, size, "");
	}
	else
	{
        u32 copysize = min_(size-1, right- left);
		Q_strncpy(outstring, size, &str[left], copysize);
        outstring[copysize] = '\0';
	}
}

inline void trim(const c8* str, u32 count, c8* outstring, u32 size)
{
	u32 len = count;
	if (len == 0)
	{
		Q_strcpy(outstring, size, "");
		return;
	}
	u32 left = 0;
	u32 right = len;
	for (u32 i=0; i<len; ++i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			++left;
		else
			break;
	}

	for (s32 i=(s32)len-1; i>=0; --i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			--right;
		else
			break;
	}

	if (left >= right)
	{
		Q_strcpy(outstring, size, "");
	}
	else
	{
		u32 copysize = min_(size-1, right- left);
		Q_strncpy(outstring, size, &str[left], copysize);
        outstring[copysize] = '\0';
	}
}

// we write [17] here instead of [] to work around a swig bug
const float fast_atof_table[17] = {
	0.f,
	0.1f,
	0.01f,
	0.001f,
	0.0001f,
	0.00001f,
	0.000001f,
	0.0000001f,
	0.00000001f,
	0.000000001f,
	0.0000000001f,
	0.00000000001f,
	0.000000000001f,
	0.0000000000001f,
	0.00000000000001f,
	0.000000000000001f,
	0.0000000000000001f
};

//! Convert a simple string of base 10 digits into a signed 32 bit integer.
//! \param[in] in: The string of digits to convert. Only a leading - or + followed 
//!					by digits 0 to 9 will be considered.  Parsing stops at the
//!					first non-digit.
//! \param[out] out: (optional) If provided, it will be set to point at the first
//!					 character not used in the calculation.
//! \return The signed integer value of the digits. If the string specifies too many
//!			digits to encode in an s32 then +INT_MAX or -INT_MAX will be returned.
inline s32 strtol10(const char* in, const char** out=0)
{
	if(!in)
		return 0;

	bool negative = false;
	if('-' == *in)
	{
		negative = true;
		++in;
	}
	else if('+' == *in)
		++in;

	u32 unsignedValue = 0;

	while ( ( *in >= '0') && ( *in <= '9' ))
	{
		unsignedValue = ( unsignedValue * 10 ) + ( *in - '0' );
		++in;

		if(unsignedValue > (u32)INT_MAX)
		{
			unsignedValue = (u32)INT_MAX;
			break;
		}
	}
	if (out)
		*out = in;

	if(negative)
		return -((s32)unsignedValue);
	else
		return (s32)unsignedValue;
}

//! Converts a sequence of digits into a whole positive floating point value.
//! Only digits 0 to 9 are parsed.  Parsing stops at any other character, 
//! including sign characters or a decimal point.
//! \param in: the sequence of digits to convert.
//! \param out: (optional) will be set to point at the first non-converted character.
//! \return The whole positive floating point representation of the digit sequence.
inline f32 strtof10(const char* in, const char * * out = 0)
{
	if(out)
		*out = in;

	if(!in)
		return 0.f;

	static const u32 MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
	f32 floatValue = 0.f;
	u32 intValue = 0;

	// Use integer arithmetic for as long as possible, for speed
	// and precision.
	while ( ( *in >= '0') && ( *in <= '9' ) )
	{
		// If it looks like we're going to overflow, bail out
		// now and start using floating point.
		if(intValue >= MAX_SAFE_U32_VALUE)
			break;

		intValue = ( intValue * 10) + ( *in - '0' );
		++in;
	}

	floatValue = (f32)intValue;

	// If there are any digits left to parse, then we need to use 
	// floating point arithmetic from here.
	while ( ( *in >= '0') && ( *in <= '9' ) )
	{
		floatValue = ( floatValue * 10.f ) + (f32)( *in - '0' );
		++in;
		if(floatValue > FLT_MAX) // Just give up.
			break;
	}

	if(out)
		*out = in;

	return floatValue;
}

//! Provides a fast function for converting a string into a float.
//! This is not guaranteed to be as accurate as atof(), but is 
//! approximately 6 to 8 times as fast.
//! \param[in] in: The string to convert. 
//! \param[out] out: The resultant float will be written here.
//! \return A pointer to the first character in the string that wasn't
//!         use to create the float value.
inline const char* fast_atof_move( const char * in, f32 & out)
{

	out = 0.f;
	if(!in)
		return 0;

	bool negative = false;
	if(*in == '-')
	{
		negative = true;
		++in;
	}

	f32 value = strtof10 ( in, &in );

	if (*in == '.')
	{
		++in;

		const char * afterDecimal = in;
		f32 decimal = strtof10 ( in, &afterDecimal );
		decimal *= fast_atof_table[afterDecimal - in];

		value += decimal;

		in = afterDecimal;
	}

	if ('e' == *in || 'E' == *in)
	{
		++in;
		// Assume that the exponent is a whole number.
		// strtol10() will deal with both + and - signs,
		// but cast to (f32) to prevent overflow at FLT_MAX
		value *= (f32)pow(10.0f, (f32)strtol10(in, &in));
	}

	if(negative)
		out = -value;
	else
		out = value;

	return in;
}

//! Convert a string to a floating point number
//! \param floatAsString: The string to convert. 
inline float fast_atof(const char* floatAsString)
{
	float ret;
	fast_atof_move(floatAsString, ret);
	return ret;
}

// random noise [-1;1]
struct Noiser
{
	static f32 get ()
	{
		static u32 RandomSeed = 0x69666966;
		RandomSeed = (RandomSeed * 3631 + 1);

		f32 value = ( (f32) (RandomSeed & 0x7FFF ) * (1.0f / (f32)(0x7FFF >> 1) ) ) - 1.f;
		return value;
	}
};

//hash functions
#define CRC32_POLY	0x04c11db7
#define CRC32_INIT_VALUE	0xffffffffL
#define CRC32_XOR_VALUE		0xffffffffL

static const u32 l_aCRC32Table[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

inline void a_CRC32_InitChecksum(u32& uCrcvalue)
{
	uCrcvalue = CRC32_INIT_VALUE;
}

inline void a_CRC32_Update(u32& uCrcvalue, u8 data)
{
	uCrcvalue = l_aCRC32Table[(uCrcvalue ^ data) & 0xff] ^ (uCrcvalue >> 8);
}

inline void a_CRC32_UpdateChecksum(u32 &uCrcvalue, const void* pData, size_t uLength)
{
	u32 uCrc;
	const u8 *pBuf = (const u8*)pData;

	uCrc = uCrcvalue;
	while (uLength--)
	{
		uCrc = l_aCRC32Table[(uCrc ^ (*pBuf++)) & 0xff] ^ (uCrc >> 8);
	}
	uCrcvalue = uCrc;
}

inline void a_CRC32_FinishChecksum(u32& uCrcvalue)
{
	uCrcvalue ^= CRC32_XOR_VALUE;
}

inline u32 CRC32_BlockChecksum(const void* pData, size_t length)
{
	u32 uCrc;
	a_CRC32_InitChecksum(uCrc);
	a_CRC32_UpdateChecksum(uCrc, pData, length);
	a_CRC32_FinishChecksum(uCrc);
	return uCrc;
}

template<class T>
void Hash(u32& uHash, const T& val)
{
	a_CRC32_UpdateChecksum(uHash, &val, sizeof(T));
}

inline void HashString(u32& uHash, const char* str)
{
	a_CRC32_UpdateChecksum(uHash, (const char*)str, strlen(str));
}

