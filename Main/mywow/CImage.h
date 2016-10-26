#pragma once

#include "core.h"
#include "IImage.h"

class CImage : public IImage
{
public:
	CImage( ECOLOR_FORMAT format, const dimension2du& size, const void* data, bool deletaData );
	CImage( ECOLOR_FORMAT format, const dimension2du& size );

private:
	~CImage();

public:
	//scale”√À´œﬂ–‘
	virtual void copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0 );
	virtual void copyToScaling( IImage* target );
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) );
	virtual bool checkHasAlpha() const;

private:
	bool		DeleteData;
};

