#include "stdafx.h"
#include "CMeshSceneNode.h"
#include "mywow.h"

CMeshSceneNode::CMeshSceneNode( IMesh* mesh, ISceneNode* parent )
	: IMeshSceneNode(parent)
{
	Mesh = mesh;

	Material.MaterialType = EMT_SOLID;
	Material.Lighting = false;

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Textures[i] = NULL;
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

void CMeshSceneNode::registerSceneNode(bool frustumcheck)
{
	update(false);

	if (!Visible)
		return;

	if (frustumcheck && !isNodeEligible())
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this);

	IMeshSceneNode::registerSceneNode(frustumcheck);
}

aabbox3df CMeshSceneNode::getBoundingBox() const
{
	if (!Mesh)
		return aabbox3df(vector3df(0,0,0));

	return Mesh->getBoundingBox();
}

void CMeshSceneNode::render()
{
	SRenderUnit unit = {0};

	unit.vbuffer = Mesh->VertexBuffer;
	unit.ibuffer = Mesh->IndexBuffer;
	unit.primType = Mesh->PrimType;
	unit.drawParam.numVertices = Mesh->VertexBuffer->Size;
	unit.primCount = Mesh->PrimCount;
	unit.sceneNode = this;
	unit.material = Material;
	unit.textures[0] = Textures[0];

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

		unit.matWorld = &Mat;
	}
	else
	{
		unit.matWorld = &AbsoluteTransformation;
	}

	g_Engine->getSceneRenderServices()->addRenderUnit(&unit, RenderInstType);
}

bool CMeshSceneNode::isNodeEligible()
{
	aabbox3df box = getWorldBoundingBox();

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return false;
	return cam->getViewFrustum().isInFrustum(box);
}