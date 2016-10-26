#include "stdafx.h"
#include "uiFrameXmlDirectory.h"
#include "mywowui.h"
#include "uiInheritableObject.h"

uiFrameXmlDirectory::~uiFrameXmlDirectory()
{
	for (T_InheritableObjectMap::iterator itr = InheritableObjectMap.begin(); itr != InheritableObjectMap.end(); ++itr)
	{
		uiInheritableObject* obj = itr->second;
		delete obj;
	}
	InheritableObjectMap.clear();
}

void uiFrameXmlDirectory::registerObject( uiInheritableObject* obj, const c8* oldname )
{
	_ASSERT(obj && strlen(obj->getName()) > 0);
	_ASSERT(obj->isVirtual());

	if (oldname && strlen(oldname))
	{
		T_InheritableObjectMap::iterator itr = InheritableObjectMap.find(oldname);
		if ( itr != InheritableObjectMap.end())
		{
			uiInheritableObject* obj = itr->second;
			delete obj;
			InheritableObjectMap.erase(itr);
		}
	}

	InheritableObjectMap[obj->getName()] = obj;
}

void uiFrameXmlDirectory::registerObject( uiInheritableObject* obj )
{
	_ASSERT(obj && strlen(obj->getName()) > 0);
	_ASSERT(obj->isVirtual());
	_ASSERT(InheritableObjectMap.find(obj->getName()) == InheritableObjectMap.end());

	InheritableObjectMap[obj->getName()] = obj;
}

void uiFrameXmlDirectory::unregisterObject( uiInheritableObject* obj )
{
	_ASSERT(obj && strlen(obj->getName()) > 0);
	_ASSERT(obj->isVirtual());

	T_InheritableObjectMap::iterator itr = InheritableObjectMap.find(obj->getName());
	if ( itr != InheritableObjectMap.end())
	{
		uiInheritableObject* obj = itr->second;
		delete obj;
		InheritableObjectMap.erase(itr);
	}
}

bool uiFrameXmlDirectory::isRegistered( const c8* name )
{
	return InheritableObjectMap.find(name) != InheritableObjectMap.end();
}

uiInheritableObject* uiFrameXmlDirectory::resolve( const c8* name )
{
	T_InheritableObjectMap::iterator itr = InheritableObjectMap.find(name);
	if (itr != InheritableObjectMap.end())
		return itr->second;

	return NULL;
}
