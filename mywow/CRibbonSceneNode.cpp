#include "stdafx.h"
#include "CRibbonSceneNode.h"
#include "mywow.h"

CRibbonSceneNode::CRibbonSceneNode( RibbonEmitter* re, IM2SceneNode* parent )
	: IRibbonSceneNode(parent), Re(re)
{
	SceneManager = g_Engine->getSceneManager();
	RibbonEmitterServices = g_Engine->getRibbonEmitterServices();

	Character = parent->getM2Instance();

	Material.Lighting = false;
	Material.ZWriteEnable = false;
	Material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;

	Texture = Re->texture;

	tpos = Re->pos;
	tabove = tbelow = 0;
}

CRibbonSceneNode::~CRibbonSceneNode()
{
	for (T_LiveSegments::iterator itr = LiveSegments.begin(); itr != LiveSegments.end(); ++itr)
	{
		RibbonEmitterServices->putSegment((*itr));
	}
	LiveSegments.clear();
}

void CRibbonSceneNode::registerSceneNode(bool frustumcheck)
{
	update(false);

	if (!Visible)
		return;

	SceneManager->registerNodeForRendering(this);

	IRibbonSceneNode::registerSceneNode(frustumcheck);
}

void CRibbonSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	u32 anim = 0;
	u32 time = timeSinceStart;

	matrix4 mat;			//骨骼矩阵
	mat = AbsoluteTransformation * Character->DynBones[Re->boneIndex].mat;
	vector3df ntpos = Re->pos;
	mat.transformVect(ntpos);
 	vector3df ntup = Re->pos + vector3df(0,1,0);
 	mat.transformVect(ntup);
 	ntup -= ntpos;
	float dlen = (ntpos - tpos).getLength();

	if (LiveSegments.empty())
	{	
		RibbonSegment* s = RibbonEmitterServices->getSegment();
		if (!s)
			return;
		s->pos = ntpos;
		s->len = 0;
		LiveSegments.push_front(s);
	}

	RibbonSegment* first = *LiveSegments.begin();;

	if (first->len > Re->seglen)		//add new segment
	{
		first->back = (tpos - ntpos).normalize();
		first->len0 = first->len;

		RibbonSegment* s = RibbonEmitterServices->getSegment();
		if (s)
		{
			s->pos = ntpos;
			s->up = ntup;
			s->len = dlen;
			s->time = 2000;
			LiveSegments.push_front(s);
		}
	}
	else
	{
		first->up = ntup;
		first->pos = ntpos;
		first->len += dlen;
	}

	//去掉超过长度的segment
	float l = 0;
	for (T_LiveSegments::iterator itr = LiveSegments.begin(); itr != LiveSegments.end();)
	{
		(*itr)->time -= timeSinceLastFrame;
		if ((*itr)->time > 0)
		{
			l += (*itr)->len;
			++itr;
		}
		else
		{
			RibbonEmitterServices->putSegment((*itr));
			itr = LiveSegments.erase(itr);	
		}
	}

	tpos = ntpos;

	vector3df v;
	float alpha = 1.0f;
	//color
	if( -1 != Re->color.getValue(0, time, v) )
		tcolor.set(v.X, v.Y, v.Z);

	//alpha
	if ( -1 != Re->opacity.getValue(anim, time, alpha))
		tcolor.a = alpha;

	Re->above.getValue(anim, time, tabove);
	Re->below.getValue(anim, time, tbelow);
}

void CRibbonSceneNode::render()
{
	if (LiveSegments.size() < 1)
		return;

	SRenderUnit unit = {0};

	unit.bufferParam= RibbonEmitterServices->BufferParam;
	unit.primType = EPT_TRIANGLE_STRIP;
	unit.sceneNode = this;
	unit.material = Material;
	unit.textures[0] = Texture;
	unit.matWorld = NULL;			//unit

	g_Engine->getSceneRenderServices()->addRenderUnit(&unit, ERT_EFFECT);
}

void CRibbonSceneNode::onPreRender()
{
	SGVertex_PC*	gVertices;
	STVertex_1T*	tVertices;
	if (RibbonEmitterServices->CurrentOffset + 2 * (1+LiveSegments.size()) > RibbonEmitterServices->BufferParam.vbuffer0->Size)
	{
		gVertices = (SGVertex_PC*)RibbonEmitterServices->GVertices;
		tVertices = (STVertex_1T*)RibbonEmitterServices->TVertices;
	}
	else
	{
		gVertices = (SGVertex_PC*)&RibbonEmitterServices->GVertices[RibbonEmitterServices->CurrentOffset];
		tVertices = (STVertex_1T*)&RibbonEmitterServices->TVertices[RibbonEmitterServices->CurrentOffset];
	}

	u32 vCount = 0;
	float len = 0;
	u32 count = 0;

	float above = tabove;
	float below = tbelow;
	for ( T_LiveSegments::iterator i = LiveSegments.begin(); i != LiveSegments.end(); ++i )
	{
		RibbonSegment* s = (*i);

		float u = count / (float)LiveSegments.size();
		gVertices[vCount].Color = tcolor.toSColor();
		gVertices[vCount].Pos.set(s->pos + s->up * above);
		tVertices[vCount].TCoords.set(u, 0);

		gVertices[vCount+1].Color = tcolor.toSColor();
		gVertices[vCount+1].Pos.set(s->pos - s->up * below);
		tVertices[vCount+1].TCoords.set(u, 1);

		vCount += 2;

		len += s->len;

		++count;
		//last segment
		if (count == LiveSegments.size())
		{
			gVertices[vCount].Color = tcolor.toSColor();
			gVertices[vCount].Pos.set(gVertices[vCount-2].Pos +  s->back * (s->len / s->len0));
			tVertices[vCount].TCoords.set(1, 0);

			gVertices[vCount+1].Color = tcolor.toSColor();
			gVertices[vCount+1].Pos.set(gVertices[vCount-1].Pos + s->back * (s->len / s->len0));
			tVertices[vCount+1].TCoords.set(1, 1);

			vCount += 2;
		}
	}

	SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	unit->drawParam.baseVertIndex = RibbonEmitterServices->CurrentOffset;
	unit->drawParam.numVertices = vCount;
	unit->primCount = vCount / 4 * 2;

	//update
	RibbonEmitterServices->updateVertices( vCount, unit->drawParam.baseVertIndex );
}

