#pragma once

#include <string.h>

template <u32 MaxSize>
class string
{
public:
	string(const c8* c)
	{
		strcpy_s(data, MaxSize, c);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());
	}

	string(const c8* c, u32 len)
	{
		if (len > MaxSize)
			len = MaxSize;
		strncpy_s(data, MaxSize, c, len);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());
	}

	string()
	{
		memset(data, 0, MaxSize);
		used = 0;
	}

	string(const string& other)
	{
		strcpy_s(data, MaxSize, other.data);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());
	}

	string& operator=(const c8* c)
	{
		strcpy_s(data, MaxSize, c);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());

		return *this;
	}

	string& operator=(const string& other)
	{
		strcpy_s(data, MaxSize, other.data);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());
		return *this;
	}

	bool operator==(const string& other) const
	{
		return 0 == _stricmp(data, other.data);
	}

	bool operator==(const c8* c) const
	{
		return 0 == _stricmp(data, c);
	}

	const bool operator<(const string& right) const
	{
		return _stricmp(data, right.data) < 0;
	}

	bool empty() const
	{
		return used==0;
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
		for (u32 i=startPos; i<length(); ++i)
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

	void subString(u32 begin, u32 len, string& outString)
	{
		u32 l = length();

		if (begin >= l)
			outString = "";
		if ((begin + len) > l)
			len = l - begin;
		strncpy_s(outString.data, MaxSize, &data[begin], len);
		outString.used = strlen(outString.data);
		outString.data[used] = '\0';

		_ASSERT(outString.isValid());
	}

	bool endWith(const c8* end)
	{
		u32 l = length();

		u32 len = strlen(end);

		if (l < len)
			return false;

		return _stricmp(&data[l-len], end) == 0;
	}

	void append(c8 c)
	{
		++used;
		data[used-1] = c;
		data[used] = '\0';

		_ASSERT(isValid());
	}

	void append(const c8* c)
	{
		strcat_s(data, MaxSize, c);
		used = strlen(data);
		data[used] = '\0';

		_ASSERT(isValid());
	}

	void append(const string& other)
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