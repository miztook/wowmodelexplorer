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
	virtual void copyToScaling( void* target, uint32_t width, uint32_t height, ECOLOR_FORMAT format=ECF_A8R8G8B8, uint32_t pitch=0 );
	virtual void copyToScaling( IImage* target );
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) );
	virtual bool checkHasAlpha() const;

private:
	bool		DeleteData;
};

