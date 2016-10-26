#pragma once

enum E_UniMode 
{
	uni8Bit=0,		//ansi
	uniUTF8=1, 
	uni16BE=2, 
	uni16LE=3, 
	uniCookie=4,		//utf8 no bom
	uni7Bit=5,			//ansi7
	uniEnd,
};

enum u78
{
	utf8NoBOM=0, 
	ascii7bits=1, 
	ascii8bits=2
};

static const c8* g_uniModeTextString[uniEnd] = 
{
	("ANSI"),
	("UTF-8"),
	("UCS-2 Big Endian"),
	("UCS-2 Little Endian"),
	("UTF-8 w/o BOM"),
	("ANSI7"),
};

inline const c8* getEncodingString(E_UniMode mode)
{
	if (mode < uniEnd)
		return g_uniModeTextString[mode];
	return "Unknown";
}

static const u8 k_Boms[uniEnd][3] =
{
	{0x00, 0x00, 0x00},  // Unknown
	{0xEF, 0xBB, 0xBF},  // UTF8
	{0xFE, 0xFF, 0x00},  // Big endian
	{0xFF, 0xFE, 0x00},  // Little endian
};

inline u78 utf8_7bits_8bits(const u8* buf, u32 bufLen)
{
	int rv = 1;
	int ASCII7only = 1;
	u8* sx	= (u8*)buf;
	u8* endx	= sx + bufLen;

	while (sx<endx)
	{
		if (!*sx)
		{											// For detection, we'll say that NUL means not UTF8
			ASCII7only = 0;
			rv = 0;
			break;
		} 
		else if (*sx < 0x80)
		{			// 0nnnnnnn If the byte's first hex code begins with 0-7, it is an ASCII character.
			++sx;
		} 
		else if (*sx < (0x80 + 0x40)) 
		{											  // 10nnnnnn 8 through B cannot be first hex codes
			ASCII7only=0;
			rv=0;
			break;
		} 
		else if (*sx < (0x80 + 0x40 + 0x20))
		{					  // 110xxxvv 10nnnnnn  If it begins with C or D, it is an 11 bit character
			ASCII7only=0;
			if (sx>=endx-1) 
				break;
			if ((*sx & 0xC0) != 0xC0 || (sx[1]&(0x80+0x40)) != 0x80) {
				rv=0; break;
			}
			sx+=2;
		} 
		else if (*sx < (0x80 + 0x40 + 0x20 + 0x10))
		{								// 1110qqqq 10xxxxvv 10nnnnnn If it begins with E, it is 16 bit
			ASCII7only=0;
			if (sx>=endx-2) 
				break;
			if ((*sx & 0xE0) != 0xE0 || (sx[1]&(0x80+0x40)) != 0x80 || (sx[2]&(0x80+0x40)) != 0x80) {
				rv=0; break;
			}
			sx+=3;
		} 
		else 
		{													  // more than 16 bits are not allowed here
			ASCII7only=0;
			rv=0;
			break;
		}
	}
	if (ASCII7only) 
		return ascii7bits;
	if (rv)
		return utf8NoBOM;
	return ascii8bits;
}

inline E_UniMode determineEncoding(const u8* buf, u32 bufLen)
{
	// detect UTF-16 big-endian with BOM
	if (bufLen > 1 && buf[0] == k_Boms[uni16BE][0] && buf[1] == k_Boms[uni16BE][1])
	{
		return uni16BE;
	}
	// detect UTF-16 little-endian with BOM
	else if (bufLen > 1 && buf[0] == k_Boms[uni16LE][0] && buf[1] == k_Boms[uni16LE][1])
	{
		return uni16LE;
	}
	// detect UTF-8 with BOM
	else if (bufLen > 2 && buf[0] == k_Boms[uniUTF8][0] && buf[1] == k_Boms[uniUTF8][1] && buf[2] == k_Boms[uniUTF8][2])
	{
		return uniUTF8;
	}
	// try to detect UTF-16 little-endian without BOM
// 	else if (m_nLen > 1 && m_pBuf[0] != NULL_PTR && m_pBuf[1] == NULL_PTR && IsTextUnicode(m_pBuf, m_nLen, &uniTest))
// 	{
// 		return uni16LE_NoBOM;
// 	}
	else
	{
		u78 detectedEncoding = utf8_7bits_8bits(buf, bufLen);
		if (detectedEncoding == utf8NoBOM)
			return uniCookie;
		else if (detectedEncoding == ascii7bits)
			return uni7Bit;
		else //(detectedEncoding == ascii8bits)
			return uni8Bit;
	}

	return uni8Bit;
}


