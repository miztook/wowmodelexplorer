#pragma once

#include "core.h"
#include "pugixml.hpp"

class CPropertyBag;

class uiInheritableObject
{
public:
	uiInheritableObject();
	virtual ~uiInheritableObject();

public:
	void setName(const c8* name);
	const c8* getName() const { return Name.c_str(); }

	void setInherits(const c8* inherits);
	const c8* getInherits() const { return Inherits.c_str(); }

	void setVirtual(bool virt);
	bool isVirtual() const { return Virtual; }

	CPropertyBag* getProperties() const { return Properties; }
	uiInheritableObject* getInheritedObject() const { return InheritedObject; }

private:
	CPropertyBag*		Properties;
	uiInheritableObject*		InheritedObject;

	string64		Name;
	string64		Inherits;
	bool	Virtual;

	u8	Padding[3];
};

//parser
class uiParser_InheritableObject
{
	DECLARE_INSTANCE(uiParser_InheritableObject)

public:
	bool loadXml(const pugi::xml_node& node, uiInheritableObject& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiInheritableObject& control);

};