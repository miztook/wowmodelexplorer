#pragma once

#include "core.h"
#include <unordered_map>

class uiUi;

class uiSceneManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiSceneManager);

public:
	uiSceneManager();
	virtual ~uiSceneManager();

public:
	uiUi*		addUi(const c8* filename);
	bool		removeUi(uiUi* ui);
	void		removeAllUi();

	uiUi*		getUi(const c8* filename);

protected:
	typedef std::unordered_map<string256, uiUi*, string256::string_hash> T_UiMap;
	T_UiMap	UiMap;
};