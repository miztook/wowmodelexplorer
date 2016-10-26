#pragma once

#include "fixstring.h"
#include <vector>

inline bool isComment( const c8* p )
{
	u32 len = (u32)strlen(p);
	for (u32 i=0; i<len; ++i)
	{
		if (p[i] == '\t' || p[i] == '\r' || p[i] == ' ')
			continue;

		if (p[i] == '/' && i+1 < len && p[i+1] == '/')
			return true;
		else 
			return false;
	}
	return false;
}

//make multiple #define MACRO, split by #
inline void makeMacroString(string1024& macroString, const c8* strMacro)
{
	macroString.clear();
	u32 len = (u32)strlen(strMacro);
	if (len == 0)
		return;

	u32 p = 0;
	for (u32 i=0; i<len; ++i)
	{
		if (i>0 && strMacro[i] == '#')	//split
		{
			if(i - p > 0)
			{
				string256 str(&strMacro[p], i-p);
				p = i+1;
				macroString.append("#define ");
				macroString.append(str.c_str());
				macroString.append("\n");
			}
		}
	}

	//last
	if (len - p > 0)
	{
		string256 str(&strMacro[p], len-p);
		macroString.append("#define ");
		macroString.append(str.c_str());
		macroString.append("\n");
	}
}

inline void makeMacroStringList(std::vector<string128>& macroStrings, const c8* strMacro)
{
	macroStrings.clear();
	u32 len = (u32)strlen(strMacro);
	if (len == 0)
		return;

	u32 p = 0;
	for (u32 i=0; i<len; ++i)
	{
		if (i>0 && strMacro[i] == '#')	//split
		{
			if(i - p > 0)
			{
				string256 str(&strMacro[p], i-p);
				p = i+1;
				macroStrings.push_back(str.c_str());
			}
		}
	}

	//last
	if (len - p > 0)
	{
		string256 str(&strMacro[p], len-p);
		macroStrings.push_back(str.c_str());
	}
}