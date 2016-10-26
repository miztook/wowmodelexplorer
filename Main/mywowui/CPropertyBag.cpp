#include "stdafx.h"
#include "CPropertyBag.h"

CPropertyBag::CPropertyBag( uiInheritableObject* obj )
	:  ReferenceCounter(1)
{
	InheritedPropertyBag = NULL;
	InheritanceEnabled = false;
}

CPropertyBag::~CPropertyBag()
{
	if(InheritedPropertyBag)
		InheritedPropertyBag->drop();
}

void CPropertyBag::setInheritedPropertyBag(CPropertyBag* propbag)
{
	if (InheritedPropertyBag)
		InheritedPropertyBag->drop();

	InheritedPropertyBag = propbag;

	if (InheritedPropertyBag)
		InheritedPropertyBag->grab();
}

bool CPropertyBag::drop()
{
	_ASSERT( ReferenceCounter>0 );
	--ReferenceCounter;

	if (ReferenceCounter == 0)
	{
		delete this;
		return true;
	}

	return false;
}
