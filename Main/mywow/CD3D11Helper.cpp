#include "stdafx.h"
#include "CD3D11Helper.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"

bool CD3D11Helper::init()
{
	return true;
}

#endif