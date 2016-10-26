#pragma once

#include "CSysSync.h"

struct SGlobal
{
	lock_type refCS;
	lock_type allocatorCS;
	lock_type tempCS;

// 	lock_type	textureCS;
// 	lock_type	hwbufferCS;
// 	lock_type	vaoCS;
// 	lock_type	adtCS;
// 	lock_type	m2CS;
// 	lock_type	wmoCS;
	lock_type	logCS;
};

extern SGlobal g_Globals;