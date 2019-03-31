#pragma once

#include <string.h>
#include "function.h"

template <uint32_t MaxSize>
class string
{
public:
	string(const char* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string(const char* c, uint32_t len)
	{
		if (len >= MaxSize)
			len = MaxSize-1;
		Q_strncpy(data, MaxSize, c, len);
        data[len] = '\0';
		used = (uint32_t)strlen(data);
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
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string& operator=(const char* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());

		return *this;
	}

	string& operator=(const string& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (uint32_t)strlen(data);
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

	bool operator==(const char* c) const
	{
		return 0 == Q_stricmp(data, c);
	}

	bool operator!=(const char* c) const
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

	void cpptext_to_utf8(const char* cpptext)
	{
		cpptext_to_utf8(cpptext, data, MaxSize);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	void clear()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	const char* c_str() const
	{
		return data;
	}

	uint32_t length() const
	{
		return used;
	}

	char operator [] (int n) const { assert(n >= 0 && n <= (int)used); return data[n]; }
	char& operator [] (int n) { assert(n >= 0 && n <= (int)used); return data[n]; }

	int32_t findNext(char c, uint32_t startPos) const
	{
		if (startPos >= MaxSize)
			return -1;
		uint32_t endPos = min_(length(), MaxSize);
		for (uint32_t i=startPos; i<endPos; ++i)
		{
			if(data[i] == c)
				return i;
		}
		return -1;
	}

	int32_t findLast(char c, int32_t start = -1) const
	{
		start = clamp_(start < 0 ? (int32_t)length()-1 : start, 0, (int32_t)length()-1);
		for (int32_t i=(int32_t)length()-1; i>=start; --i)
		{
			if (data[i] == c)
				return i;
		}

		return -1;
	}

	void subString(uint32_t begin, uint32_t len, string& outString)
	{
		uint32_t l = length();

		if (begin >= l)
			outString = "";
		if ((begin + len) > l)
			len = l - begin;
		Q_strncpy(outString.data, MaxSize, &data[begin], len);
        outString.data[len] = '\0';
		outString.used = (uint32_t)strlen(outString.data);
		outString.data[used] = '\0';

		ASSERT(outString.isValid());
	}

	bool endWith(const char* end)
	{
		uint32_t l = length();

		uint32_t len = (uint32_t)strlen(end);

		if (l < len)
			return false;

		return  Q_stricmp(&data[l-len], end) == 0;
	}

	bool beginWith(const char* begin)
	{
		uint32_t l = length();

		uint32_t len = (uint32_t)strlen(begin);

		if (l < len)
			return false;

		return  Q_strnicmp(&data[0], begin, len) == 0;
	}

	void append(char c)
	{
		++used;
		data[used-1] = c;
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const char* c)
	{
		Q_strcat(data, MaxSize, c);
		used = (uint32_t)strlen(data);
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
    
    int32_t findString(const char* subStr)
    {
        if(empty()) return -1;
        
        char* c = strstr(data, subStr);
        if(!c)
            return -1;
        return (int32_t)(ptrdiff_t)(c - data);
    }

	void format(const char* fmt, ...)
	{
		va_list va;
		va_start( va, fmt );
		Q_vsprintf( data, MaxSize, fmt, va );
		va_end( va );

		used = (uint32_t)strlen(data);
		data[used] = '\0';
	}

	void normalize()
	{
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}
	}

	void normalizeDir()
	{
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}

		if (len > 0)
		{
			char last = data[len -1];
			if (last != '/' && last != '\\' )
			{
				append('/');
			}
		}
	}

	bool isNormalized() const
	{
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				return false;
		}
		return true;
	}

	bool changeExt(const char* ext, const char* newExt)
	{
		if (endWith(ext))
		{
			uint32_t oldlen = length() - (uint32_t)strlen(ext);
			data[oldlen] = '\0';
			append(newExt);
			return true;
		}
		return false;
	}

	void makeHDFileName(const char* textureName)
	{
		const char* p = strrchr(textureName, '.' );
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
		used = (uint32_t)strlen(data);
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
	char data[MaxSize];
	uint32_t used;

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

template <uint32_t MaxSize>
class string_cs
{
public:
	string_cs(const char* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string_cs(const char* c, uint32_t len)
	{
		if (len > MaxSize)
			len = MaxSize;
		Q_strncpy(data, MaxSize, c, len);
        data[len] = '\0';
		used = (uint32_t)strlen(data);
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
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
	}

	string_cs& operator=(const char* c)
	{
		Q_strcpy(data, MaxSize, c);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());

		return *this;
	}

	string_cs& operator=(const string_cs& other)
	{
		Q_strcpy(data, MaxSize, other.data);
		used = (uint32_t)strlen(data);
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

	bool operator==(const char* c) const
	{
		return 0 == strcmp(data, c);
	}

	bool operator!=(const char* c) const
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

	void cpptext_to_utf8(const char* cpptext)
	{
		cpptext_to_utf8(cpptext, data, MaxSize);
		used = (uint32_t)strlen(data);
		data[used] = '\0';

		ASSERT(isValid());
		return *this;
	}

	void clear()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	const char* c_str() const
	{
		return data;
	}

	uint32_t length() const
	{
		return used;
	}

	int32_t findNext(char c, uint32_t startPos) const
	{
		if (startPos >= MaxSize)
			return -1;
		uint32_t endPos = min_(length(), MaxSize);
		for (uint32_t i=startPos; i<endPos; ++i)
		{
			if(data[i] == c)
				return i;
		}
		return -1;
	}

	int32_t findLast(char c, int32_t start = -1) const
	{
		start = clamp_(start < 0 ? (int32_t)length()-1 : start, 0, (int32_t)length()-1);
		for (int32_t i=start; i>=0; --i)
		{
			if (data[i] == c)
				return i;
		}

		return -1;
	}

	void subString(uint32_t begin, uint32_t len, string_cs& outString) const
	{
		uint32_t l = length();

		if (begin >= l)
			outString = "";
		if ((begin + len) > l)
			len = l - begin;
		Q_strncpy(outString.data, MaxSize, &data[begin], len);
        outString.data[len] = '\0';
		outString.used = (uint32_t)strlen(outString.data);
		outString.data[used] = '\0';

		ASSERT(outString.isValid());
	}

	bool endWith(const char* end)
	{
		uint32_t l = length();

		uint32_t len = strlen(end);

		if (l < len)
			return false;

		return  strcmp(&data[l-len], end) == 0;
	}

	bool beginWith(const char* begin)
	{
		uint32_t l = length();

		uint32_t len = strlen(begin);

		if (l < len)
			return false;

		return  strncmp(&data[0], begin, len) == 0;
	}

	void append(char c)
	{
		++used;
		data[used-1] = c;
		data[used] = '\0';

		ASSERT(isValid());
	}

	void append(const char* c)
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
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}
	}

	void normalizeDir()
	{
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				data[i] = '/';
		}

		if (len > 0)
		{
			char last = data[len -1];
			if (last != '/' && last != '\\' )
			{
				append('/');
			}
		}
	}

	bool isNormalized() const
	{
		uint32_t len = length();
		for (uint32_t i=0; i<len; ++i)
		{
			if(data[i] == '\\')
				return false;
		}
		return true;
	}

	bool changeExt(const char* ext, const char* newExt)
	{
		if (endWith(ext))
		{
			uint32_t oldlen = length() - strlen(ext);
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
	char data[MaxSize];
	uint32_t used;

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