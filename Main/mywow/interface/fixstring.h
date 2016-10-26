#pragma once

#include <string.h>
#include "function.h"

template <u32 MaxSize>
class string
{
public:
	string(const c8* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string(const c8* c, u32 len)
	{
		if (len >= MaxSize)
			len = MaxSize-1;
		Q_strncpy(data, MaxSize, c, len);
        data[len] = '\0';
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	string(const string& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string& operator=(const c8* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());

		return *this;
	}

	string& operator=(const string& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	bool operator==(const string& other) const
	{
		return 0 == Q_stricmp(data, other.data);
	}

	bool operator!=(const string& other) const
	{
		return 0 != Q_stricmp(data, other.data);
	}

	bool operator==(const c8* c) const
	{
		return 0 == Q_stricmp(data, c);
	}

	bool operator!=(const c8* c) const
	{
		return 0 != Q_stricmp(data, c);
	}

	const bool operator<(const string& right) const
	{
		return Q_stricmp(data, right.data) < 0;
	}

	bool empty() const
	{
		return used==0;
	}

	void cpptext_to_utf8(const c8* cpptext)
	{
		cpptext_to_utf8(cpptext, data, MaxSize);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	void clear()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	const c8* c_str() const
	{
		return data;
	}

	u32 length() const
	{
		return used;
	}

	char operator [] (int n) const { assert(n >= 0 && n <= (int)used); return data[n]; }
	char& operator [] (int n) { assert(n >= 0 && n <= (int)used); return data[n]; }

	s32 findNext(c8 c, u32 startPos) const
	{
		if (startPos >= MaxSize)
			return -1;
		u32 endPos = min_(length(), MaxSize);
		for (u32 i=startPos; i<endPos; ++i)
		{
			if(data[i] == c)
				return i;
		}
		return -1;
	}

	s32 findLast(c8 c, s32 start = -1) const
	{
		start = clamp_(start < 0 ? (s32)length()-1 : start, 0, (s32)length()-1);
		for (s32 i=(s32)length()-1; i>=start; --i)
		{
			if (data[i] == c)
				return i;
		}

		return -1;
	}

	void subString(u32 begin, u32 len, string& outString)
	{
		u32 l = length();

		if (begin >= l)
			outString = "";
		if ((begin + len) > l)
			len = l - begin;
		Q_strncpy(outString.data, MaxSize, &data[begin], len);
        outString.data[len] = '\0';
		outString.used = (u32)strlen(outString.data);
		outString.data[used] = '\0';

		ASSERT(outString.isValid());
	}

	bool endWith(const c8* end)
	{
		u32 l = length();

		u32 len = (u32)strlen(end);

		if (l < len)
			return false;

		return  Q_stricmp(&data[l-len], end) == 0;
	}

	bool beginWith(const c8* begin)
	{
		u32 l = length();

		u32 len = (u32)strlen(begin);

		if (l < len)
			return false;

		return  Q_strnicmp(&data[0], begin, len) == 0;
	}

	void append(c8 c)
	{
		++used;
		data[used-1] = c;
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const c8* c)
	{
		Q_strcat(data, MaxSize, c);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const string& other)
	{
		append(other.c_str());
	}

    void make_lower()
	{
		Q_strlwr(data);
	}

    void make_upper()
	{
		Q_strupr(data, MaxSize);
	}
    
    s32 findString(const c8* subStr)
    {
        if(empty()) return -1;
        
        c8* c = strstr(data, subStr);
        if(!c)
            return -1;
        return (s32)(ptrdiff_t)(c - data);
    }

	void format(const c8* fmt, ...)
	{
		va_list va;
		va_start( va, fmt );
		Q_vsprintf( data, MaxSize, fmt, va );
		va_end( va );

		used = (u32)strlen(data);
		data[used] = '\0';
	}

	void normalize()
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}
	}

	void normalizeDir()
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}

		if (len > 0)
		{
			c8 last = data[len -1];
			if (last != '/' && last != '\\' )
			{
				append('/');
			}
		}
	}

	bool isNormalized() const
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				return false;
		}
		return true;
	}

	bool changeExt(const c8* ext, const c8* newExt)
	{
		if (endWith(ext))
		{
			u32 oldlen = length() - (u32)strlen(ext);
			data[oldlen] = '\0';
			append(newExt);
			return true;
		}
		return false;
	}

	void makeHDFileName(const c8* textureName)
	{
		const c8* p = strrchr(textureName, '.' );
		if (p)
		{
			Q_strncpy(data, MaxSize, textureName, p-textureName);
			Q_strcat(data, MaxSize, "_HD");
			Q_strcat(data, MaxSize, p);
		}
		else
		{
			Q_strcat(data, MaxSize, "_HD");
		}
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	struct string_hash
	{
		size_t operator()(const string& _Keyval) const
		{
			string str(_Keyval);
			str.make_lower();

			unsigned long __h = 0;
			for (unsigned int i = 0; i < str.used; ++i)
				__h = 5 * __h + str.data[i];
			return size_t(__h);
		}
	};

private:
	c8 data[MaxSize];
	u32 used;

	bool isValid() const
	{
		return used<MaxSize && data[used]=='\0';
	}
};

typedef string<8>		string8;
typedef string<16>		string16;
typedef string<32>		string32;
typedef string<64>		string64;
typedef string<128>	string128;
typedef string<256>	string256;
typedef string<512>	string512;
typedef string<1024>	string1024;

typedef string<QMAX_PATH>		string_path;

template <u32 MaxSize>
class string_cs
{
public:
	string_cs(const c8* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string_cs(const c8* c, u32 len)
	{
		if (len > MaxSize)
			len = MaxSize;
		Q_strncpy(data, MaxSize, c, len);
        data[len] = '\0';
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string_cs()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	string_cs(const string_cs& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string_cs& operator=(const c8* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());

		return *this;
	}

	string_cs& operator=(const string_cs& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	bool operator==(const string_cs& other) const
	{
		return 0 == strcmp(data, other.data);
	}

	bool operator!=(const string_cs& other) const
	{
		return 0 != strcmp(data, other.data);
	}

	bool operator==(const c8* c) const
	{
		return 0 == strcmp(data, c);
	}

	bool operator!=(const c8* c) const
	{
		return 0 != strcmp(data, c);
	}

	const bool operator<(const string_cs& right) const
	{
		return strcmp(data, right.data) < 0;
	}

	bool empty() const
	{
		return used==0;
	}

	void cpptext_to_utf8(const c8* cpptext)
	{
		cpptext_to_utf8(cpptext, data, MaxSize);
		used = (u32)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	void clear()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	const c8* c_str() const
	{
		return data;
	}

	u32 length() const
	{
		return used;
	}

	s32 findNext(c8 c, u32 startPos) const
	{
		if (startPos >= MaxSize)
			return -1;
		u32 endPos = min_(length(), MaxSize);
		for (u32 i=startPos; i<endPos; ++i)
		{
			if(data[i] == c)
				return i;
		}
		return -1;
	}

	s32 findLast(c8 c, s32 start = -1) const
	{
		start = clamp_(start < 0 ? (s32)length()-1 : start, 0, (s32)length()-1);
		for (s32 i=start; i>=0; --i)
		{
			if (data[i] == c)
				return i;
		}

		return -1;
	}

	void subString(u32 begin, u32 len, string_cs& outString) const
	{
		u32 l = length();

		if (begin >= l)
			outString = "";
		if ((begin + len) > l)
			len = l - begin;
		Q_strncpy(outString.data, MaxSize, &data[begin], len);
        outString.data[len] = '\0';
		outString.used = (u32)strlen(outString.data);
		outString.data[used] = '\0';

		ASSERT(outString.isValid());
	}

	bool endWith(const c8* end)
	{
		u32 l = length();

		u32 len = strlen(end);

		if (l < len)
			return false;

		return  strcmp(&data[l-len], end) == 0;
	}

	bool beginWith(const c8* begin)
	{
		u32 l = length();

		u32 len = strlen(begin);

		if (l < len)
			return false;

		return  strncmp(&data[0], begin, len) == 0;
	}

	void append(c8 c)
	{
		++used;
		data[used-1] = c;
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const c8* c)
	{
		Q_strcat(data, MaxSize, c);
		used = strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const string_cs& other)
	{
		append(other.c_str());
	}

	void make_lower()
	{
		_strlwr_s(data, MaxSize);
	}

	void make_upper()
	{
		_strupr_s(data, MaxSize);
	}

	void normalize()
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}
	}

	void normalizeDir()
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}

		if (len > 0)
		{
			c8 last = data[len -1];
			if (last != '/' && last != '\\' )
			{
				append('/');
			}
		}
	}

	bool isNormalized() const
	{
		u32 len = length();
		for (u32 i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				return false;
		}
		return true;
	}

	bool changeExt(const c8* ext, const c8* newExt)
	{
		if (endWith(ext))
		{
			u32 oldlen = length() - strlen(ext);
			data[oldlen] = '\0';
			append(newExt);
			return true;
		}
		return false;
	}

	struct string_cs_hash
	{
		size_t operator()(const string_cs& _Keyval) const
		{
			unsigned long __h = 0;
			for (unsigned int i = 0; i < _Keyval.used; ++i)
				__h = 5 * __h + _Keyval.data[i];
			return size_t(__h);
		}
	};

private:
	c8 data[MaxSize];
	u32 used;

	bool isValid() const
	{
		return used<MaxSize && data[used]=='\0';
	}
};

typedef string_cs<8>		string_cs8;
typedef string_cs<16>		string_cs16;
typedef string_cs<32>		string_cs32;
typedef string_cs<64>		string_cs64;
typedef string_cs<128>	string_cs128;
typedef string_cs<256>	string_cs256;
typedef string_cs<512>	string_cs512;
typedef string_cs<1024>	string_cs1024;