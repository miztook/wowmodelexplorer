#include "stdafx.h"
#include "CRibbonSceneNode.h"
#include "mywow.h"
#include "wow_m2instance.h"
#include "CSceneRenderServices.h"
#include "CRibbonEmitterServices.h"

CRibbonSceneNode::CRibbonSceneNode( RibbonEmitter* re, IM2SceneNode* parent )
	: IRibbonSceneNode(parent), Re(re)
{
	RibbonEmitterServices = static_cast<CRibbonEmitterServices*>(g_Engine->getRibbonEmitterServices());

	Character = parent->getM2Instance();

	Material.Lighting = false;
	Material.ZWriteEnable = false;
	Material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;

	Texture = Re->texture;

	tpos = Re->pos;
	tabove = tbelow = 0;

	InitializeListHead(&LiveSegmentList);
	LiveSegmentCount = 0;
}

CRibbonSceneNode::~CRibbonSceneNode()
{
	for (PLENTRY e = LiveSegmentList.Flink; e != &LiveSegmentList;)
	{
		RibbonSegment* s = reinterpret_cast<RibbonSegment*>CONTAINING_RECORD(e, RibbonSegment, Link);
		e = e->Flink;

		RibbonEmitterServices->putSegment(s);
	}
	::InitializeListHead(&LiveSegmentList);
	LiveSegmentCount = 0;
}

void CRibbonSceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update(false);

	if (!Visible)
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	IRibbonSceneNode::registerSceneNode(frustumcheck, sequence);
}

void CRibbonSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	u32 anim = 0;
	u32 time = timeSinceStart;

	matrix4 mat;			//骨骼矩阵
	mat = Character->DynBones[Re->boneIndex].mat * AbsoluteTransformation;
	vector3df ntpos = Re->pos;
	mat.transformVect(ntpos);
 	vector3df ntup = Re->pos + vector3df(0,1,0);
 	mat.transformVect(ntup);
 	ntup -= ntpos;
	float dlen = (ntpos - tpos).getLength();

	if (LiveSegmentCount == 0)
	{	
		RibbonSegment* s = RibbonEmitterServices->getSegment();
		if (!s)
			return;
		s->pos = ntpos;
		s->len = 0;
		InsertHeadList(&LiveSegmentList, &s->Link);
		++LiveSegmentCount;
	}

	RibbonSegment* first = reinterpret_cast<RibbonSegment*>CONTAINING_RECORD(LiveSegmentList.Flink, RibbonSegment, Link);

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

			InsertHeadList(&LiveSegmentList, &s->Link);
			++LiveSegmentCount;
		}
	}
	else
	{
		first->up = ntup;
		first->pos = ntpos;
		first->len += dlen;
	}

	//去掉超过长度的segment
	for (PLENTRY e = LiveSegmentList.Flink; e != &LiveSegmentList;)
	{
		RibbonSegment* s = reinterpret_cast<RibbonSegment*>CONTAINING_RECORD(e, RibbonSegment, Link);
		e = e->Flink;

		s->time -= timeSinceLastFrame;
		if (s->time <= 0)
		{
			RemoveEntryList(&s->Link);
			--LiveSegmentCount;
			RibbonEmitterServices->putSegment(s);
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

void CRibbonSceneNode::render() const
{
	if (LiveSegmentCount == 0)
		return;

	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	SRenderUnit unit = {0};

	unit.drawParam.numVertices = 2 * (1+LiveSegmentCount);
	unit.bufferParam= RibbonEmitterServices->BufferParam;
	unit.primType = EPT_TRIANGLE_STRIP;
	unit.sceneNode = this;
	unit.material = Material;
	unit.textures[0] = Texture;
	unit.matWorld = NULL_PTR;			//unit

	sceneRenderServices->addRenderUnit(&unit, ERT_RIBBON);
}

/*
void CRibbonSceneNode::onPreRender()
{
	SVertex_PCT*	vertices;
	if (RibbonEmitterServices->CurrentOffset + 2 * (1+LiveSegmentCount) > RibbonEmitterServices->BufferParam.vbuffer0->Size)
	{
		vertices = (SVertex_PCT*)RibbonEmitterServices->Vertices;
	}
	else
	{
		vertices = (SVertex_PCT*)&RibbonEmitterServices->Vertices[RibbonEmitterServices->CurrentOffset];
	}

	u32 vCount = 0;
	float len = 0;
	u32 count = 0;

	float above = tabove;
	float below = tbelow;
	for (PLENTRY e = LiveSegmentList.Flink; e != &LiveSegmentList;)
	{
		RibbonSegment* s = reinterpret_cast<RibbonSegment*>CONTAINING_RECORD(e, RibbonSegment, Link);
		e = e->Flink;

		float u = count / (float)LiveSegmentCount;
		vertices[vCount].Color = tcolor.toSColor();
		vertices[vCount].Pos.set(s->pos + s->up * above);
		vertices[vCount].TCoords.set(u, 0);

		vertices[vCount+1].Color = tcolor.toSColor();
		vertices[vCount+1].Pos.set(s->pos - s->up * below);
		vertices[vCount+1].TCoords.set(u, 1);

		vCount += 2;

		++count;
		//last segment
		if (count == LiveSegmentCount)
		{
			vertices[vCount].Color = tcolor.toSColor();
			vertices[vCount].Pos.set(vertices[vCount-2].Pos +  s->back * (s->len / s->len0));
			vertices[vCount].TCoords.set(1, 0);

			vertices[vCount+1].Color = tcolor.toSColor();
			vertices[vCount+1].Pos.set(vertices[vCount-1].Pos + s->back * (s->len / s->len0));
			vertices[vCount+1].TCoords.set(1, 1);

			vCount += 2;
		}
	}

// 	SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
// 
// 	unit->drawParam.baseVertIndex = RibbonEmitterServices->CurrentOffset;
// 	unit->drawParam.numVertices = vCount;
// 	unit->primCount = vCount / 4 * 2;
// 
// 	//update
// 	RibbonEmitterServices->updateVertices( vCount, unit->drawParam.baseVertIndex );
}
*/

u32 CRibbonSceneNode::onFillVertexBuffer( SVertex_PCT* vertices, u32 vertexCount ) const
{
	u32 vCount = 0;
	u32 count = 0;

	float above = tabove;
	float below = tbelow;
	for (PLENTRY e = LiveSegmentList.Flink; e != &LiveSegmentList;)
	{
		if (vCount + 2 > vertexCount)
			break;

		RibbonSegment* s = reinterpret_cast<RibbonSegment*>CONTAINING_RECORD(e, RibbonSegment, Link);
		e = e->Flink;

		float u = count / (float)LiveSegmentCount;
		vertices[vCount].Color = tcolor.toSColor();
		vertices[vCount].Pos.set(s->pos + s->up * above);
		vertices[vCount].TCoords.set(u, 0);

		vertices[vCount+1].Color = tcolor.toSColor();
		vertices[vCount+1].Pos.set(s->pos - s->up * below);
		vertices[vCount+1].TCoords.set(u, 1);

		vCount += 2;

		++count;

		//last segment
		if (count == LiveSegmentCount)
		{
			if (vCount + 2 > vertexCount)
				break;

			vertices[vCount].Color = tcolor.toSColor();
			vertices[vCount].Pos.set(vertices[vCount-2].Pos +  s->back * (s->len / s->len0));
			vertices[vCount].TCoords.set(1, 0);

			vertices[vCount+1].Color = tcolor.toSColor();
			vertices[vCount+1].Pos.set(vertices[vCount-1].Pos + s->back * (s->len / s->len0));
			vertices[vCount+1].TCoords.set(1, 1);

			vCount += 2;
		}
	}

	return vCount;
}

bool CRibbonSceneNode::isNodeEligible() const
{
	if (Parent)
		return Parent->isNodeEligible();

	return false;
}
