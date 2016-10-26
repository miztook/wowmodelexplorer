#pragma once

#include "CSysSync.h"

class CLock
{
public:
	explicit CLock(lock_type* cs) : CS(cs)
	{
		BEGIN_LOCK(CS);
	}

	~CLock()
	{
		END_LOCK(CS);
	}

private:
	lock_type* CS;
};
