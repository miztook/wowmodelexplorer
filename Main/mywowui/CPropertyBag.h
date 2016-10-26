#pragma once

#include "core.h"
#include <map>

class uiInheritableObject;

class CPropertyBag
{
public:
	explicit CPropertyBag(uiInheritableObject* obj);

	virtual ~CPropertyBag();

public:
	void setInheritedPropertyBag(CPropertyBag* propbag);
	CPropertyBag* getInheritedPropertyBag() const { return InheritedPropertyBag; }

	s32 getReferenceCount() const { return ReferenceCounter; }
	void grab() { ++ReferenceCounter; }
	bool drop();

private:	
	s32 ReferenceCounter;
	CPropertyBag*		InheritedPropertyBag;
	bool		InheritanceEnabled;
	u8	Padding[3];
};