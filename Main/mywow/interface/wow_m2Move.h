#pragma once

#include "core.h"

class IM2SceneNode;

class wow_m2Move
{
public:
	explicit wow_m2Move(IM2SceneNode* m2Node);
	~wow_m2Move();

public:
	vector3df getDir() const;
	void setDir( const vector3df& dir );
	vector3df getPos() const;
	void setPos( const vector3df& pos );
	vector3df getUp() const;
	void setUp( const vector3df& up );
	vector3df getScale() const;
	void setScale( const vector3df& scale );

	void rotateAxisY(float radians);
	void move( const vector3df& offset );

	float getDestHeight() const { return Height; }				//目标高度
	void setDestHeight(float height) { Height = height; }

private:
	void rotate(const quaternion& q);

private:
	IM2SceneNode*			M2SceneNode;
	float		Height;
};