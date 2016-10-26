#include "stdafx.h"
#include "CMeshSceneNode.h"
#include "mywow.h"
#include "CSceneRenderServices.h"

CMeshSceneNode::CMeshSceneNode( IMesh* mesh, ISceneNode* parent )
	: IMeshSceneNode(parent)
{
	Mesh = mesh;

	Material.MaterialType = EMT_SOLID;
	Material.AntiAliasing = EAAM_LINE_SMOOTH;
	Material.Lighting = false;

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		Textures[i] = NULL_PTR;
		Material.TextureLayer[i].TextureWrapU =
		Material.TextureLayer[i].TextureWrapV = ETC_CLAMP;
	}

	if (hasTexture(Mesh->BufferParam.vType))
	{
		setTexture(0, g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite"));
	}
}

CMeshSceneNode::~CMeshSceneNode()
{

}

void CMeshSceneNode::setTexture( u32 i, ITexture* texture )
{
	if(i >= MATERIAL_MAX_TEXTURES)
		return;
	Textures[i] = texture;
}

void CMeshSceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update(false);

	if (!Visible ||
		(frustumcheck && !isNodeEligible()))
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	IMeshSceneNode::registerSceneNode(frustumcheck, sequence);
}

aabbox3df CMeshSceneNode::getBoundingBox() const
{
	if (!Mesh)
		return aabbox3df::Zero();

	return Mesh->getBoundingBox();
}

void CMeshSceneNode::tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible)
{
	if (visible)
	{
		if (Billboard)
		{
			Mat.makeIdentity();
			ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
			matrix4 view = cam->getInverseViewMatrix();
			Mat[0] = view[0]; Mat[4] = view[4]; Mat[8] = view[8]; 
			Mat[1] = view[1]; Mat[5] = view[5]; Mat[9] = view[9];
			Mat[2] = view[2]; Mat[6] = view[6]; Mat[10] = view[10];

			Mat[11] = AbsoluteTransformation[11];
			Mat[12] = AbsoluteTransformation[12];
			Mat[13] = AbsoluteTransformation[13];
			Mat[14] = AbsoluteTransformation[14];
		}
	}
}

void CMeshSceneNode::render() const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	SRenderUnit unit = {0};

	unit.bufferParam = Mesh->BufferParam;
	unit.primType = Mesh->PrimType;
	unit.drawParam.numVertices = Mesh->BufferParam.vbuffer0->Size;
	unit.primCount = Mesh->PrimCount;
	unit.sceneNode = this;
	unit.material = Material;
	unit.textures[0] = Textures[0];

	if (Billboard)
	{
		unit.matWorld = &Mat;
	}
	else
	{
		unit.matWorld = &AbsoluteTransformation;
	}

	sceneRenderServices->addRenderUnit(&unit, RenderInstType);
}

bool CMeshSceneNode::isNodeEligible() const
{
	aabbox3df box = getWorldBoundingBox();

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return false;
	return cam->getViewFrustum().isInFrustum(box);
}