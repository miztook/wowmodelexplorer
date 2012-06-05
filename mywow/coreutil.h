#pragma once

#include <string.h>
#include <windows.h>
#include <float.h>
#include <intsafe.h>

inline void getFileDirA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* lastSlash =strrchr( filename, ('/') );
	const c8* lastBackSlash = strrchr( filename, ('\\') );
	if ( !lastSlash || lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;

	if (!lastSlash || lastSlash-filename<=0)
		strcpy_s(outfilename, size, ("."));
	else
	{
		strncpy_s( outfilename, size, filename, lastSlash-filename );
		outfilename[lastSlash-filename] = '\0';
	}
}

inline void getFileNameA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* lastSlash = strrchr( filename, '/' );
	const c8* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;

	if (!lastSlash || *(lastSlash+1) == '\0')
		strcpy_s(outfilename, size, filename);
	else
		strcpy_s( outfilename, size, lastSlash+1 );
}

inline void getFileExtensionA(const c8* filename, c8* outfilename, u32 size )
{
	const c8* p = strrchr(filename, '.' );
	if (!p || *(p+1)=='\0')
		strcpy_s(outfilename, size, "");
	else
		strcpy_s( outfilename, size, p+1 );
}

inline void getFileNameNoExtensionA(const c8* filename, c8* outfilename, u32 size)
{
	const c8* p = strrchr( filename, '.' );
	if (!p || *(p+1)=='\0')
		strcpy_s(outfilename, size, filename);
	else
		strncpy_s(outfilename, size, filename, p-filename);
}

inline bool hasFileExtensionA( const c8* filename, const c8* ext )
{
	c8 extension[5];
	getFileExtensionA(filename, extension, 5);

	if (  _stricmp(extension, ext) != 0 )
		return false;
	return true;
}

inline void str8to16( const c8* str8, c16* str16, u32 size )
{
	MultiByteToWideChar( CP_ACP, 0, str8, -1, str16, size );
}

inline void str16to8( const c16* str16, c8* str8, u32 size )
{
	WideCharToMultiByte( CP_ACP, 0, str16, -1,
		str8, size, NULL, NULL );
}

inline void utf8to16( const c8* utf8, c16* utf16, u32 size)
{
	MultiByteToWideChar( CP_UTF8, 0, utf8, -1, utf16, size);
}

inline void utf16to8( const c16* utf16, c8* utf8, u32 size )
{
	WideCharToMultiByte( CP_UTF8, 0, utf16, -1, utf8, size, NULL, NULL);
}

inline c16 c8to16( c8 ch )
{
	c16 cRet;
	str8to16(&ch, &cRet, 1);
	return cRet;
}

inline c8 c16to8( c16 ch )
{
	c8 cRet;
	str16to8(&ch, &cRet, 1);
	return cRet;
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
