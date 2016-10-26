#include "stdafx.h"
#include "uiInheritableObject.h"
#include "mywowui.h"
#include "CPropertyBag.h"

uiInheritableObject::uiInheritableObject()
{
	Properties = new CPropertyBag(this);
	InheritedObject = NULL;
	Virtual = false;
}

uiInheritableObject::~uiInheritableObject()
{
	if(Properties)
		Properties->drop();;
}

void uiInheritableObject::setName( const c8* name )
{
	if (Name != name)
	{
		string64 oldName = Name;
		Name = name;

		if (isVirtual())
		{
			g_UISystem->getUiFrameXmlDirectory()->registerObject(this, oldName.c_str());
		}
	}
}

void uiInheritableObject::setInherits( const c8* inherits )
{
	if (Inherits != inherits)
	{
		Inherits = inherits;

		if (strlen(inherits) >0 && g_UISystem->getUiFrameXmlDirectory()->isRegistered(inherits))
		{
			InheritedObject = g_UISystem->getUiFrameXmlDirectory()->resolve(inherits);
			Properties->setInheritedPropertyBag(InheritedObject->getProperties());
		}
		else
		{
			InheritedObject = NULL;
			Properties->setInheritedPropertyBag(NULL);
		}
	}
}

void uiInheritableObject::setVirtual( bool virt )
{
	if (Virtual != virt)
	{
		Virtual = virt;
		if (!Name.empty())
		{
			if (Virtual)
				g_UISystem->getUiFrameXmlDirectory()->registerObject(this);
			else
				g_UISystem->getUiFrameXmlDirectory()->unregisterObject(this);
		}
	}
}

bool uiParser_InheritableObject::loadXml( const pugi::xml_node& node, uiInheritableObject& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

bool uiParser_InheritableObject::loadProperties( const pugi::xml_node& node, uiInheritableObject& control )
{
	control.setInherits(node.attribute("inherits").as_string());
	control.setVirtual(node.attribute("virtual").as_bool());
	control.setName(node.attribute("name").as_string());

	return true;
}
