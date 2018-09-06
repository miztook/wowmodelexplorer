#pragma once

#include "base.h"
#include "wow_def.h"
#include "wow_dbc70.h"

class wowEnvironment;

class wowDatabase70
{
public:
	explicit wowDatabase70(wowEnvironment* env);

	bool init(const c8* xmlfile);

	~wowDatabase70();

private:
	wowEnvironment*		Environment;

};