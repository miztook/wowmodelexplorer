#pragma once

#include "core.h"
#include <unordered_map>

class uiInheritableObject;

//virtual
class uiFrameXmlDirectory
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiFrameXmlDirectory);

public:
	uiFrameXmlDirectory() {}
	~uiFrameXmlDirectory();

public:
	u32 getObjectCount() { return (u32)InheritableObjectMap.size(); }

	void registerObject(uiInheritableObject* obj, const c8* oldname);
	void registerObject(uiInheritableObject* obj);
	void unregisterObject(uiInheritableObject* obj);

	bool isRegistered(const c8* name);
	uiInheritableObject* resolve(const c8* name);

private:
	typedef std::unordered_map<string64, uiInheritableObject*, string64::string_hash> T_InheritableObjectMap;
	T_InheritableObjectMap	InheritableObjectMap;
};