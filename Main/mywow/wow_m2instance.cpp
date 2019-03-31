#include "stdafx.h"
#include "wow_m2instance.h"
#include "mywow.h"
#include "CImage.h"
#include "CFileM2.h"
#include "CBlit.h"

#if WOW_VER >= 70
using namespace WowLegion;
#else
using namespace WowClassic;
#endif

const char* regionTextureItemPaths[NUM_REGIONS] =
{
	"",																				//base
	"Item\\TextureComponents\\ArmUpperTexture\\",
	"Item\\TextureComponents\\ArmLowerTexture\\",
	"Item\\TextureComponents\\HandTexture\\",
	"",																				//face upper
	"",																				//face lower
	"Item\\TextureComponents\\TorsoUpperTexture\\",
	"Item\\TextureComponents\\TorsoLowerTexture\\",
	"Item\\TextureComponents\\LegUpperTexture\\",
	"Item\\TextureComponents\\LegLowerTexture\\",
	"Item\\TextureComponents\\FootTexture\\"
};

const char* regionHeadPath = "Item\\ObjectComponents\\Head\\";
const char* regionShoulderPath = "Item\\ObjectComponents\\Shoulder\\";
const char* regionShieldPath = "Item\\ObjectComponents\\Shield\\";
const char* regionWeaponPath =  "Item\\ObjectComponents\\Weapon\\";
const char* regionQuiverPath = "Item\\ObjectComponents\\Quiver\\";
const char* regionCapePath = "Item\\ObjectComponents\\Cape\\";
const char* regionWaistPath = "Item\\ObjectComponents\\Waist\\";

CharTexture::CharTexture(bool isHD)
	: TexPartCount(0), IsHD(isHD)
{
	TextureParts = (CharTexturePart*)Z_AllocateTempMemory(sizeof(CharTexturePart) * MAX_TEX_PART_SIZE);
}

CharTexture::~CharTexture()
{
	Z_FreeTempMemory(TextureParts);
}

void CharTexture::addLayer( const char* name, int32_t region, int32_t layer )
{
	if (name == nullptr || strlen(name) == 0)
		return;

	CharTexturePart* part = &TextureParts[TexPartCount++];
	Q_strcpy(part->Name, QMAX_PATH, name);
	Q_strlwr(part->Name);
	part->Region = region;
	part->Layer = layer;

	ASSERT(TexPartCount < MAX_TEX_PART_SIZE);
}

bool CharTexture::addItemLayer( const char* name, int32_t region, uint32_t gender, int32_t layer )
{
	if (name == nullptr || strlen(name) == 0)
		return false;

	char outname[QMAX_PATH];
	bool success = makeItemTexture(region, gender, name, outname);
	if (success)
	{
		CharTexturePart* part = &TextureParts[TexPartCount++];
		part->Region = region;
		part->Layer = layer;
		Q_strcpy(part->Name, QMAX_PATH, outname);
	}

	ASSERT(TexPartCount < MAX_TEX_PART_SIZE);
	return success;
}

bool CharTexture::makeItemTexture(int32_t region, uint32_t gender, const char* name, char* outname)
{
	if (strlen(name) < 3)
	{
		Q_strcpy(outname, QMAX_PATH, "");
		return false;
	}

#if WOW_VER >= 60
	Q_strcpy(outname, QMAX_PATH, name);
	Q_strlwr(outname);
#else
	Q_strcpy(outname, QMAX_PATH, regionTextureItemPaths[region]);
	Q_strcat(outname, QMAX_PATH, name);
	Q_strcat(outname, QMAX_PATH, "_U.blp");
	Q_strlwr(outname);
#endif

	uint32_t len = (uint32_t)strlen(outname);
	if (len < 6)
	{
		Q_strcpy(outname, QMAX_PATH, "");
		return false;
	}

	bool bOk;
	if (Q_strcmp(&outname[len - 6], "_u.blp") == 0)
	{
		bOk = g_Engine->getWowEnvironment()->exists(outname);
		if(bOk)
			return true;
	}
	
	//尝试另一个版本, _U 换成 _F _M
	outname[strlen(outname)-5] = gender ? 'F' : 'M';
	Q_strlwr(outname);
	
	bOk = g_Engine->getWowEnvironment()->exists(outname);
	if(bOk)
		return true;
	
	return false;
}

ITexture* CharTexture::compose(bool pandaren)
{
	bool largeScale = pandaren || IsHD;

	heapsort<CharTexturePart>(TextureParts, TexPartCount);

	//熊猫人为1024X512, 其他为512X512
	uint32_t texWidth = largeScale ? REGION_PX * 2 : REGION_PX;
	uint32_t texHeight = REGION_PX;

	uint32_t* destData = (uint32_t*)Z_AllocateTempMemory(texWidth * texHeight * sizeof(uint32_t));
	memset(destData, 0xff, texWidth * texHeight * sizeof(uint32_t));

	for (uint32_t i=0; i<TexPartCount; ++i)
	{
		CharTexturePart* part = &TextureParts[i];
		const CharRegionCoords coords = largeScale ? pandaren_regions[part->Region] : regions[part->Region];

#ifdef MW_COMPILE_WITH_GLES2
#if defined(USE_PVR)
		string256 path = part->Name;
		path.changeExt(".blp", ".pvr");
		IImage* image = g_Engine->getResourceLoader()->loadPVRAsImage(path.c_str());
#elif defined(USE_KTX)
		string256 path = part->Name;
		path.changeExt(".blp", ".ktx");
		IImage* image = g_Engine->getResourceLoader()->loadKTXAsImage(path.c_str());
#endif
#else
		IImage* image = g_Engine->getResourceLoader()->loadBLPAsImage(part->Name);
#endif

		if(image)
		{
			CImage* srcImage = static_cast<CImage*>(image);
			CImage* newImage = srcImage;
			uint32_t* tmpData = nullptr;			//临时image
			bool needScale = srcImage->getDimension().Width != coords.xsize ||
				srcImage->getDimension().Height != coords.ysize;			//扩大

			if (needScale)
			{
				tmpData = (uint32_t*)Z_AllocateTempMemory(coords.xsize * coords.ysize * sizeof(uint32_t));
				newImage = new CImage(ECF_A8R8G8B8, dimension2du(coords.xsize, coords.ysize), tmpData, false);
				srcImage->copyToScaling(newImage);
			}

			uint32_t width = coords.xsize;
			uint32_t height = coords.ysize;

			ASSERT(newImage->getDimension().Width == coords.xsize &&
				newImage->getDimension().Height == coords.ysize );

			uint32_t srcX, srcY, dstX, dstY;
			for ( srcY=0, dstY=coords.ypos; srcY<height; ++srcY, ++dstY)
			{
				for ( srcX=0, dstX=coords.xpos; srcX<width; ++srcX, ++dstX )
				{
					SColor src = newImage->getPixel(srcX, srcY);
					uint32_t* dst = destData + dstY*texWidth + dstX;

					SColor d(*dst);
					d = SColor::interpolate(src, d, 1 - src.getAlpha() / 255.0f, true);

					*dst = d.color;
				}
			}

			if (needScale)
			{
				newImage->drop();
				Z_FreeTempMemory(tmpData);
			}

			image->drop();
		}
	}

	ITexture* tex;
#ifdef MW_COMPILE_WITH_GLES2
	//gles2版使用不压缩的 256X256
	const uint32_t len_px = 256;

	uint32_t* tmpData = (uint32_t*)Z_AllocateTempMemory(len_px * len_px * sizeof(uint8_t) * 2);
	CBlit::resizeBilinearA8R8G8B8(destData, texWidth, texHeight, tmpData, len_px, len_px, ECF_R5G6B5);
	//CBlit::resizeBicubicA8R8G8B8(destData, texWidth, texHeight, tmpData, len_px, len_px, ECF_R5G6B5);
	tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(len_px, len_px), ECF_R5G6B5, tmpData, true);
	Z_FreeTempMemory(tmpData);
#else

// 	if (largeScale)		//缩放回 512X512
// 	{
// 		CImage* composedImage = new CImage(ECF_A8R8G8B8, dimension2du(texWidth, texHeight), destData, false);
// 		uint32_t* tmpData = (uint32_t*)Z_AllocateTempMemory(REGION_PX * REGION_PX * sizeof(uint32_t));
// 		composedImage->copyToScaling(tmpData, REGION_PX, REGION_PX);
// 		tex = g_Engine->getManualTextureServices()->createCompressTextureFromData(dimension2du(REGION_PX, REGION_PX), ECF_A8R8G8B8, tmpData, true);
// 		Z_FreeTempMemory(tmpData);
// 		composedImage->drop();
// 	}
// 	else
	{
		tex = g_Engine->getManualTextureServices()->createCompressTextureFromData(dimension2du(texWidth, texHeight), ECF_A8R8G8B8, destData, true);
	}
#endif

	Z_FreeTempMemory(destData);

	return tex;
}

wow_m2instance::wow_m2instance( IFileM2* m2, bool npc )
{
	Mesh = static_cast<CFileM2*>(m2);

	CharacterInfo = nullptr;

	ShowParticles = true;
	ShowRibbons = false;
	TextureAnimHints = nullptr;
	ColorHints = nullptr;
	LastBoneAnim = LastColorAnim = LastTextureAnim = -1;
	LastBoneTime = LastColorTime = LastTextureTime = -1;

	for (uint32_t i=0; i<NUM_TEXTURETYPE; ++i)
		ReplaceTextures[i] = nullptr;

	CurrentSkin = Mesh->Skin;
	if (CurrentSkin)
		AnimatedBox = static_cast<CFileM2*>(Mesh)->getBoundingBox();

	InitializeListHead(&VisibleGeosetList);

	DynBones = new SDynBone[Mesh->NumBones];
	Calcs = new bool[Mesh->NumBones];
	BoneHints = new SHint[Mesh->NumBones];
	UseAttachments = new int8_t[Mesh->NumAttachments];
	::memset(UseAttachments, 0, sizeof(int8_t) * Mesh->NumAttachments);

	DynGeosets = nullptr;
	if (CurrentSkin)				//有skin
	{
		DynGeosets = new SDynGeoset[CurrentSkin->NumGeosets];
		memset(DynGeosets, 0, sizeof(SDynGeoset) * CurrentSkin->NumGeosets);

		for (uint32_t i=0; i<CurrentSkin->NumGeosets; ++i)
		{
			CGeoset* set = &CurrentSkin->Geosets[i];
			DynGeosets[i].NumUnits = (uint32_t)set->BoneUnits.size();
			DynGeosets[i].Units = new SDynGeoset::SUnit[DynGeosets[i].NumUnits];
			for (CGeoset::T_BoneUnits::const_iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[i].Units[itr->Index];
				unit->BoneMats = new matrix4[(*itr).BoneCount];
				unit->BoneMatrixArray = new SBoneMatrixArray(
					unit->BoneMats, 
					(*itr).BoneCount, 
					set->MaxWeights);
			}
		}

		ColorHints = new SColorHint[CurrentSkin->NumGeosets];
		TextureAnimHints = new SHint[CurrentSkin->NumGeosets];
	}
	
	if(Mesh->getType() == MT_CHARACTER && Mesh->isCharacter())
	{
		CharacterInfo = new SCharacterInfo(npc);

		bool isChar = g_Engine->getWowDatabase()->getRaceGender(Mesh->Name, CharacterInfo->Race, CharacterInfo->Gender, CharacterInfo->IsHD);
		ASSERT(isChar);
	}

	//color, alpha
	EnableModelColor = false;
	EnableModelAlpha = false;
	ModelAlpha = 1.0f;
}

wow_m2instance::~wow_m2instance()
{
	delete CharacterInfo;

	delete[] ColorHints;
	delete[] TextureAnimHints;
	delete[] DynGeosets;

	delete[] UseAttachments;
	delete[] BoneHints;
	delete[] Calcs;
	delete[] DynBones;

	for (uint32_t i=0; i<NUM_TEXTURETYPE; ++i)
		if(ReplaceTextures[i])
			ReplaceTextures[i]->drop();
}

void wow_m2instance::updateCharacter()
{
	if (!CharacterInfo)
		return;

	int* Geosets = CharacterInfo->Geosets;

	for (uint32_t i=0; i<NUM_GEOSETS; ++i)
	{
		Geosets[i] = 1;
		CharacterInfo->CapeID = 1;
	}
	Geosets[CG_HAIRSTYLE] = 0;
	Geosets[CG_GEOSET100] = Geosets[CG_GEOSET200] = Geosets[CG_GEOSET300] = 0;

	//update texture
	CharTexture	charTex(CharacterInfo->IsHD);
	
	//纹理装备
	dressupCharacter(charTex);

	//组合纹理
	setReplaceTexture(TEXTURE_BODY, charTex.compose(CharacterInfo->Race == RACE_PANDAREN));

#ifdef MW_EDITOR
	::memset(CharacterInfo->BodyTextureFileNames, 0, sizeof(CharacterInfo->BodyTextureFileNames));
	for (uint32_t i=0; i<charTex.TexPartCount; ++i)
	{
		CharTexturePart* part = &charTex.TextureParts[i];
		Q_strcpy(CharacterInfo->BodyTextureFileNames[part->Region], QMAX_PATH, part->Name);
	}
#endif

	buildVisibleGeosets();
}

void wow_m2instance::animateBones( uint32_t anim, uint32_t time,  uint32_t lastingtime, float blend)
{

	if (!CurrentSkin || (LastBoneAnim == anim && time == LastBoneTime))
		return;

	if (blend < 0)
		blend = 1.0f;

	::memset(Calcs, 0, sizeof(bool)*Mesh->NumBones);

	if (LastBoneAnim != anim || time < LastBoneTime)			//在anim改变或时间轴倒退时清除hint
		::memset(BoneHints, 0, sizeof(SHint)*Mesh->NumBones);
	
	AnimatedBox = static_cast<CFileM2*>(Mesh)->getBoundingBox();

	if (CharacterInfo)
	{		
		uint32_t blinkGeosets[256];
		uint32_t blinkGeosetCount = 0;
		uint32_t fistGeosets[256];
		uint32_t fistGeosetCount = 0;

		uint32_t closeFistIndex = Mesh->AnimationLookup[ANIMATION_HANDSCLOSED];
		for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
		{
			uint32_t c = (uint32_t)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - DynGeosets);
			p = p->Flink;

			if (DynGeosets[c].NoAlpha)
				continue;

			CGeoset* set = &CurrentSkin->Geosets[c];

			//eye blink
			if (isBlinkGeoset(c) || set->GeoID / 100 == CG_EYEGLOW)
			{
				ASSERT(blinkGeosetCount < 256);
				blinkGeosets[blinkGeosetCount] = c;
				++blinkGeosetCount;

				continue;
			}
				
			bool isFist = false;
			for (CGeoset::T_BoneUnits::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
				unit->Enable = itr->BoneCount > 0;

				for(uint8_t k=0; k <(*itr).BoneCount; ++k)
				{
					uint8_t idx = (*itr).local2globalMap[k];

					SModelBone* b = &Mesh->Bones[idx];

					if ((CharacterInfo->CloseLHand && b->bonetype == EBT_LEFTHAND) ||
						(CharacterInfo->CloseRHand && b->bonetype == EBT_RIGHTHAND))
					{
						isFist = true;
					}
					else
					{
						calcBone(idx, anim, time, blend, true, &AnimatedBox);
						unit->BoneMats[k] = *(const matrix4*)DynBones[idx].mat.pointer();
					}		
				}
			}

			if (isFist)
			{
				ASSERT(fistGeosetCount < 256);
				fistGeosets[fistGeosetCount] = c;
				++fistGeosetCount;
			}
		}

		//blink geosets
		for (uint32_t i=0; i<blinkGeosetCount; ++i)
		{
			uint32_t c = blinkGeosets[i];
			CGeoset* set = &CurrentSkin->Geosets[c];

			for (CGeoset::T_BoneUnits::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
				unit->Enable = itr->BoneCount > 0;

				for(uint8_t k=0; k <(*itr).BoneCount; ++k)
				{
					uint8_t idx = (*itr).local2globalMap[k];
					
					calcBone(idx, 0, lastingtime, blend, true, &AnimatedBox);
					unit->BoneMats[k] = *(const matrix4*)DynBones[idx].mat.pointer();
				}
			}
		}

		//fist geosets
		for (uint32_t i=0; i<fistGeosetCount; ++i)
		{
			uint32_t c = fistGeosets[i];
			CGeoset* set = &CurrentSkin->Geosets[c];

			for (CGeoset::T_BoneUnits::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
				unit->Enable = itr->BoneCount > 0;

				for(uint8_t k=0; k <(*itr).BoneCount; ++k)
				{
					uint8_t idx = (*itr).local2globalMap[k];

					SModelBone* b = &Mesh->Bones[idx];
					if ((CharacterInfo->CloseLHand && b->bonetype == EBT_LEFTHAND) ||
						(CharacterInfo->CloseRHand && b->bonetype == EBT_RIGHTHAND))
					{
						calcBone(idx, closeFistIndex, 0, blend, true, &AnimatedBox);				
						unit->BoneMats[k] = *(const matrix4*)DynBones[idx].mat.pointer();
					}
				}
			}
		}
	}
	else			//not character
	{
		for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
		{
			uint32_t c = (uint32_t)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - DynGeosets);
			p = p->Flink;

			if (DynGeosets[c].NoAlpha)
				continue;

			CGeoset* set = &CurrentSkin->Geosets[c];
			for (CGeoset::T_BoneUnits::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
				unit->Enable = itr->BoneCount > 0;

				for(uint8_t k=0; k <(*itr).BoneCount; ++k)
				{
					uint8_t idx = (*itr).local2globalMap[k];

					calcBone(idx, anim, time, blend, true, &AnimatedBox);
					unit->BoneMats[k] = *(const matrix4*)DynBones[idx].mat.pointer();	
				}
			}
		}
	}

	//attachments
	for (uint32_t c=0; c<Mesh->NumAttachments; ++c)
	{
		int32_t i = Mesh->Attachments[c].boneIndex;

#ifdef MW_EDITOR
		if (i != -1)
#else
		if (i != -1 && UseAttachments[c] > 0)
#endif
			calcAttachmentBone(i, anim, time, blend);
	}

	//particles
	if (ShowParticles)
	{
		for (uint32_t c=0; c<Mesh->NumParticleSystems; ++c)
		{
			int16_t i;
			if((i = Mesh->ParticleSystems[c].boneIndex) != -1)
				calcBone((uint8_t)i, anim, time, blend, false, &AnimatedBox);
		}
	}

	//ribbons
	if (ShowRibbons)
	{
		for (uint32_t c=0; c<Mesh->NumRibbonEmitters; ++c)
		{
			int16_t i;
			if((i= Mesh->RibbonEmitters[c].boneIndex) != -1)
				calcBone((uint8_t)i, anim, time, blend, false, &AnimatedBox);
		}
	}

	vector3df center = AnimatedBox.getCenter();
	vector3df ext = AnimatedBox.getExtent();
	AnimatedBox.set(center - ext * 0.6f, center + ext * 0.6f);

	LastBoneAnim = anim;
	LastBoneTime = time;
}

void wow_m2instance::calcBone( uint8_t i, uint32_t anim, uint32_t time, float blend, bool enableScale, aabbox3df* animatedBox)
{
	if (Calcs[i])
		return;

	ASSERT(i < Mesh->NumBones);
	const SModelBone* b = &Mesh->Bones[i];

	// 	{
	// 		uint32_t numtrans = b->trans.getNumAnims();
	// 		uint32_t numrot = b->rot.getNumAnims();
	// 		if (numtrans || numrot)
	// 		{
	// 			ASSERT(numtrans > anim || numrot > anim);
	// 		}
	// 	}

	matrix4 global, inverseGlobal;
	global.setTranslation(b->pivot);
	inverseGlobal.setTranslation(-b->pivot);

	vector3df t(0,0,0);
	vector3df s(1,1,1);
	quaternion r(0,0,0,1);

	bool _trans;
	bool _rot;
	bool soft = blend < 1.0f;

	BoneHints[i].transHint = b->trans.getValue(anim, time, t, BoneHints[i].transHint);
	_trans =  BoneHints[i].transHint != -1;

	if(soft && _trans)
		t = vector3df::interpolate(DynBones[i].trans, t, blend);
	DynBones[i].trans = t;

	BoneHints[i].rotHint = b->rot.getValue(anim, time, r, BoneHints[i].rotHint);
	_rot = BoneHints[i].rotHint != -1; 
	if(soft && _rot)
		r = quaternion::slerp(DynBones[i].rot, r, blend);

	DynBones[i].rot = r;

	if (enableScale)
	{
		BoneHints[i].scaleHint = b->scale.getValue(anim, time, s, BoneHints[i].scaleHint);
		bool _scale = BoneHints[i].scaleHint != -1;
		if(soft && _scale)
			s = vector3df::interpolate(DynBones[i].scale, s, blend);
		DynBones[i].scale = s;
	}

	matrix4 m;

	if (!b->billboard)
	{							//变换是基于pivot空间的
		r.getMatrix(m);
		m.setTranslation(t);
		m.setScale(s);		

		m = inverseGlobal * m * global;		//pivot
	}

	if (b->parent != -1)
	{
		calcBone((uint8_t)b->parent, anim, time, blend, enableScale, animatedBox);

		DynBones[i].mat = m * DynBones[b->parent].mat;
	}
	else
	{
		DynBones[i].mat = m;
	}

	DynBones[i].transPivot =b->pivot;
	DynBones[i].mat.transformVect(DynBones[i].transPivot);

	if (animatedBox && b->parent != -1)
		animatedBox->addInternalPoint(DynBones[i].transPivot);

	Calcs[i]= true;
}

void wow_m2instance::disableBones()
{
	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
	{
		uint32_t c = (uint32_t)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - DynGeosets);
		p = p->Flink;

		if (DynGeosets[c].NoAlpha)
			continue;

		CGeoset* set = &CurrentSkin->Geosets[c];
		for (CGeoset::T_BoneUnits::const_iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
		{
			SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
			unit->Enable = false;
		}
	}
}

void wow_m2instance::calcAttachmentBone( uint8_t i, uint32_t anim, uint32_t time, float blend )
{
	if (Calcs[i])
		return;

	ASSERT(i < Mesh->NumBones);
	SModelBone* b = &Mesh->Bones[i];

	matrix4 global, inverseGlobal;
	global.setTranslation(b->pivot);
	inverseGlobal.setTranslation(-b->pivot);

	vector3df t(0,0,0);
	quaternion r(0,0,0,1);

	bool _trans;
	bool _rot;

	bool soft = blend < 1.0f;

	BoneHints[i].transHint = Mesh->Bones[i].trans.getValue(0, time, t, BoneHints[i].transHint);
	_trans =  BoneHints[i].transHint != -1;

	if(soft && _trans)
		t = vector3df::interpolate(DynBones[i].trans, t, blend);
	DynBones[i].trans = t;

	BoneHints[i].rotHint = Mesh->Bones[i].rot.getValue(0, time, r, BoneHints[i].rotHint);
	_rot = BoneHints[i].rotHint != -1; 
	if(soft && _rot)
		r = quaternion::slerp(DynBones[i].rot, r, blend);

	DynBones[i].rot = r;

	matrix4 m;

	if (b->billboard)
	{
		//ASSERT(!_rot && !_trans && !_scale);
		//m = (*billboardRot);
	}
	else
	{							//变换是基于pivot空间的
		r.getMatrix(m);
		m.setTranslation(t);		

		m = inverseGlobal * m * global;		//pivot
	}

	if (b->parent != -1)
	{
		calcBone((uint8_t)b->parent, anim, time, blend, false, nullptr);

		DynBones[i].mat = m * DynBones[b->parent].mat;
	}
	else
	{
		DynBones[i].mat = m;
	}

	DynBones[i].transPivot =b->pivot;
	DynBones[i].mat.transformVect(DynBones[i].transPivot);

	Calcs[i]= true;
}

void wow_m2instance::animateColors(uint32_t anim, uint32_t time)
{
	if ( !CurrentSkin || (LastColorAnim == anim && time == LastColorTime))
		return;
 
	if (LastColorAnim != anim || time < LastColorTime)			//在anim改变或时间轴倒退时清除hint
		::memset(ColorHints, 0, sizeof(SColorHint)*CurrentSkin->NumGeosets);

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
	{
		uint32_t c = (uint32_t)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - DynGeosets);
		p = p->Flink;

		uint32_t realAnim = anim;

		//eye blink
		if (CharacterInfo && isBlinkGeoset(c)/*|| (isDeathKnightBlinkGeoset(c))*/)
		{
			realAnim = 0;
		}
	
		DynGeosets[c].NoAlpha = false;

		CGeoset* set = &CurrentSkin->Geosets[c];
		int16_t colorIndex = set->getTexUnit(0)->ColorIndex;
		int16_t transIndex = set->getTexUnit(0)->TransIndex;

		SColorf colorf(1.0f, 1.0f, 1.0f, 1.0f);
        
		vector3df v;
		//color
		if(colorIndex != -1 &&
			(ColorHints[c].colorHint = Mesh->Colors[colorIndex].colorAnim.getValue(0, time, v, ColorHints[c].colorHint)) != -1)
			colorf.set(v.X, v.Y, v.Z);

        float alpha = 1.0f;
		//alpha
		if ( colorIndex != -1 &&
			(ColorHints[c].opacityHint = Mesh->Colors[colorIndex].opacityAnim.getValue(realAnim, time, alpha, ColorHints[c].opacityHint)) != -1)
			colorf.a = alpha;	

		uint16_t t = 32767;
		if (transIndex != -1 && 
			(ColorHints[c].transparencyHint = Mesh->Transparencies[transIndex].tranAnim.getValue(realAnim, time, t, ColorHints[c].transparencyHint)) != -1)
			colorf.a *= (t / 32767.0f);

		DynGeosets[c].emissive = colorf.toSColor();
		DynGeosets[c].NoAlpha = colorf.a < 0.01f;
		if (CharacterInfo && !CharacterInfo->Blink && isBlinkGeoset(c))
			DynGeosets[c].NoAlpha = true; 
	}

	LastColorAnim = anim;
	LastColorTime = time;
}

void wow_m2instance::solidColors()
{
	if (!CurrentSkin)
		return;

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
	{
		SDynGeoset* geoset = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link);
		p = p->Flink;

		geoset->emissive = SColor(0xffffffff);
		geoset->NoAlpha = false;
	}
}

void wow_m2instance::animateTextures( uint32_t anim, uint32_t time )
{
	if ( !CurrentSkin || (LastTextureAnim == anim && time == LastTextureTime))
		return;

	if (LastTextureAnim != anim || time < LastTextureTime)			//在anim改变或时间轴倒退时清除hint
		::memset(TextureAnimHints, 0, sizeof(SHint)*CurrentSkin->NumGeosets);

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList;)
	{
		uint32_t c = (uint32_t)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - DynGeosets);
		p = p->Flink;

		if (DynGeosets[c].NoAlpha)
			continue;

		calcTextureAnim(c, anim, time);
	}

	LastTextureAnim = anim;
	LastTextureTime = time;
}

void wow_m2instance::calcTextureAnim( uint32_t c, uint32_t anim, uint32_t time )
{
	CGeoset* set = &CurrentSkin->Geosets[c];
	int16_t texAnimIndex = set->getTexUnit(0)->TexAnimIndex;
	if(texAnimIndex == -1)
	{
		DynGeosets[c].UseTextureAnim = false;
		return;
	}

	SModelTextureAnim& textureAnim = Mesh->TextureAnim[texAnimIndex];

	matrix4 mat;

	vector3df s, t, r;

	bool _trans;
	bool _rot;
	bool _scale;

	TextureAnimHints[c].scaleHint = textureAnim.scale.getValue(anim, time, s, TextureAnimHints[c].scaleHint);
	TextureAnimHints[c].transHint = textureAnim.trans.getValue(anim, time, t, TextureAnimHints[c].transHint); 
	TextureAnimHints[c].rotHint = textureAnim.rot.getValue(anim, time, r, TextureAnimHints[c].rotHint);

	_scale = -1 != TextureAnimHints[c].scaleHint;
	_trans = -1 != TextureAnimHints[c].transHint;
	_rot = -1 != TextureAnimHints[c].rotHint;

	if (_scale || _trans || _rot)
	{
		mat.makeIdentity();
		DynGeosets[c].UseTextureAnim = true;
	}

	if (_rot && r.X != 0.0f)
	{
		float theta = r.X;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		mat.m00 = cosTheta;
		mat.m01 = sinTheta;
		mat.m10 = -sinTheta;
		mat.m11 = cosTheta;

		mat.m20 = 0.5f + (0.5f * (sinTheta - cosTheta));
		mat.m21 = 0.5f + (-0.5f * (sinTheta + cosTheta));
	}

	if (_trans &&
		(t.X != 0 || t.Y != 0))
	{
		matrix4 trans;
		trans.m20 = t.X;
		trans.m21 = t.Y;

		mat = mat * trans;		
	}

	if (_scale &&
		(s.X != 0 || s.Y != 0))
	{
		matrix4 scale;
		scale.m00 = s.X;
		scale.m11 = s.Y;
		scale.m20 = (-0.5f * s.X) + 0.5f;
		scale.m21 = (-0.5f * s.Y) + 0.5f;

		mat = mat * scale;
	}

	DynGeosets[c].TextureMatrix = mat;
}

void wow_m2instance::updateEquipments( int32_t slot, int32_t itemid )
{
	ASSERT(slot >= 0 && slot < NUM_CHAR_SLOTS);
	if (slot >= 0 && slot < NUM_CHAR_SLOTS)
		CharacterInfo->Equipments[slot]= itemid;
}

int32_t wow_m2instance::getSlotItemId( int32_t slot ) const
{
	ASSERT(slot >= 0 && slot < NUM_CHAR_SLOTS);
	if (slot >= 0 && slot < NUM_CHAR_SLOTS)
		return CharacterInfo->Equipments[slot];

	return 0;
}

int32_t wow_m2instance::getItemSlot( int32_t itemid ) const
{
	SItemInfo info;
	if (getItemInfo(itemid, CharacterInfo->IsNpc, info))
	{
		return getSlot(info.itemType);
	}

	return -1;
}

bool wow_m2instance::slotHasModel( int32_t slot ) const
{
	return slot==CS_HEAD || slot == CS_SHOULDER || slot == CS_HAND_LEFT ||
		slot == CS_HAND_RIGHT || slot == CS_QUIVER || slot == CS_BELT;
}

bool wow_m2instance::updateNpc(int32_t npcid)
{
	auto rSkin = g_Engine->getWowDatabase()->getCreatureDisplayInfoDB()->getByID(npcid);
	if (!rSkin.isValid())
		return false;

	uint32_t modelId = rSkin.getUInt(creatureDisplayInfoDB::ModelID);
	uint32_t npcId = rSkin.getUInt(creatureDisplayInfoDB::NPCExtraID);

	if (!CharacterInfo)
	{
		auto r = g_Engine->getWowDatabase()->getCreatureModelDB()->getByID(modelId);
		if (!r.isValid())
			return false;

#if WOW_VER >= 70
		int32_t fileId = r.getByte3(creatureModelDB::FileNameID);
		char filename[256];
		g_Engine->getWowDatabase()->getFilePath(fileId, filename, 256);
#elif WOW_VER >= 60
		int32_t fileId = r.getInt(creatureModelDB::FileNameID);
		char filename[256];
		g_Engine->getWowDatabase()->getFilePath(fileId, filename, 256);
#else
		const char* filename = r.getString(creatureModelDB::Filename);
#endif
		if (strlen(filename) < 4 || strlen(filename) > 255)
			return false;

		char path[QMAX_PATH];
		getFileDirA(filename, path, QMAX_PATH);
		Q_strcat(path, QMAX_PATH, "\\");

		{
#if WOW_VER >= 70
			uint32_t texId = rSkin.getInArray<int>(creatureDisplayInfoDB::Texture1, 0);
			char t[256];
			g_Engine->getWowDatabase()->getFilePath(texId, t, 256);
			if (strlen(t) > 0)
			{
				setReplaceTexture(TEXTURE_GAMEOBJECT1, g_Engine->getResourceLoader()->loadTexture(t));
			}
#else
			const char* t = rSkin.getString(creatureDisplayInfoDB::Texture1 );
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT1, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}
#endif
			
#if WOW_VER >= 70
			texId = rSkin.getInArray<int>(creatureDisplayInfoDB::Texture1, 1);
			g_Engine->getWowDatabase()->getFilePath(texId, t, 256);
			if (strlen(t) > 0)
			{
				setReplaceTexture(TEXTURE_GAMEOBJECT2, g_Engine->getResourceLoader()->loadTexture(t));
			}
#else
			t = rSkin.getString(creatureDisplayInfoDB::Texture1 + 1);
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT2, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}
#endif

#if WOW_VER >= 70
			texId = rSkin.getInArray<int>(creatureDisplayInfoDB::Texture1, 2);
			g_Engine->getWowDatabase()->getFilePath(texId, t, 256);
			if (strlen(t) > 0)
			{
				setReplaceTexture(TEXTURE_GAMEOBJECT3, g_Engine->getResourceLoader()->loadTexture(t));
			}
#else
			t = rSkin.getString(creatureDisplayInfoDB::Texture1 + 2);
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT3, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}
#endif
		}

		buildVisibleGeosets();
	}
	else			//character
	{
		auto r = g_Engine->getWowDatabase()->getCreatureDisplayInfoExtraDB()->getByID(npcId);

		ASSERT(r.isValid());
		if (!r.isValid())
			return false;

		CharacterInfo->SkinColor = r.getUInt(creatureDisplayInfoExtraDB::SkinColor);
		CharacterInfo->FaceType = r.getUInt(creatureDisplayInfoExtraDB::Face);
		CharacterInfo->HairColor = r.getUInt(creatureDisplayInfoExtraDB::HairColor);
		CharacterInfo->HairStyle = r.getUInt(creatureDisplayInfoExtraDB::HairStyle);
		CharacterInfo->FacialHair = r.getUInt(creatureDisplayInfoExtraDB::FacialHair);

		int* Equipments = CharacterInfo->Equipments;

#if WOW_VER >= 70
		const npcModelItemSlotDisplayInfoDB* npcItemDB = g_Engine->getWowDatabase()->getNpcModelItemSlotDisplayInfoDB();
		for(uint32_t i= 0; i < npcItemDB->getNumRecords(); ++i)
		{
			auto rItem = npcItemDB->getRecord(i);
			if(npcId != rItem.getUInt(npcModelItemSlotDisplayInfoDB::NPCExtraID))
				continue;

			uint32_t itemDisplayID = rItem.getUInt(npcModelItemSlotDisplayInfoDB::ItemDisplayInfoID);
			uint32_t itemType = rItem.getUInt(npcModelItemSlotDisplayInfoDB::ItemType);

			switch(itemType)
			{
				case creatureDisplayInfoExtraDB::HelmID: Equipments[CS_HEAD] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::ShoulderID: Equipments[CS_SHOULDER] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::ShirtID: Equipments[CS_SHIRT] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::ChestID: Equipments[CS_CHEST] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::BeltID: Equipments[CS_BELT] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::PantsID: Equipments[CS_PANTS] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::BootsID: Equipments[CS_BOOTS] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::BracersID: Equipments[CS_BRACERS] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::GlovesID: Equipments[CS_GLOVES] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::CapeID: Equipments[CS_CAPE] = itemDisplayID; break;
				case creatureDisplayInfoExtraDB::TabardID: Equipments[CS_TABARD] = itemDisplayID; break;
				default:
					break;
			}
		}
#else
		Equipments[CS_HEAD] = r.getUInt(creatureDisplayInfoExtraDB::HelmID);
		Equipments[CS_SHOULDER] = r.getUInt(creatureDisplayInfoExtraDB::ShoulderID);
		Equipments[CS_SHIRT]  =	r.getUInt(creatureDisplayInfoExtraDB::ShirtID);	
		Equipments[CS_CHEST] = r.getUInt(creatureDisplayInfoExtraDB::ChestID);
		Equipments[CS_BELT] = r.getUInt(creatureDisplayInfoExtraDB::BeltID);
		Equipments[CS_PANTS] = r.getUInt(creatureDisplayInfoExtraDB::PantsID);
		Equipments[CS_BOOTS] = r.getUInt(creatureDisplayInfoExtraDB::BootsID);
		Equipments[CS_BRACERS] = r.getUInt(creatureDisplayInfoExtraDB::BracersID);
		Equipments[CS_GLOVES] = r.getUInt(creatureDisplayInfoExtraDB::GlovesID);
		Equipments[CS_CAPE] = r.getUInt(creatureDisplayInfoExtraDB::CapeID);
		Equipments[CS_TABARD] = r.getUInt(creatureDisplayInfoExtraDB::TabardID);
#endif
	}
	
	return true;
}

void wow_m2instance::setReplaceTexture( ETextureTypes type, ITexture* texture )
{
	int idx = (int)type;

	if (ReplaceTextures[idx])
	{
		ReplaceTextures[idx]->drop();
	}

	ReplaceTextures[idx] = texture;

	if (ReplaceTextures[idx])
	{
		IVideoResource::buildVideoResources(ReplaceTextures[idx]);
	}
}

void wow_m2instance::dressupCharacter(CharTexture& charTex)
{
	const charSectionsDB* charSection = g_Engine->getWowDatabase()->getCharSectionDB();
	uint32_t Race = CharacterInfo->Race;
	uint32_t Gender = CharacterInfo->Gender;
	uint32_t SkinColor = CharacterInfo->SkinColor;
	uint32_t FaceType = CharacterInfo->FaceType;
	uint32_t FacialHair = CharacterInfo->FacialHair;
	uint32_t HairColor = CharacterInfo->HairColor;
	uint32_t HairStyle = CharacterInfo->HairStyle;
	const int* Equipments = CharacterInfo->Equipments;
	bool isHD = CharacterInfo->IsHD;

	//base
	{
		auto r = charSection->getByParams(Race, Gender, (isHD ? charSectionsDB::SkinTypeHD : charSectionsDB::SkinType), 0, SkinColor);
		if(!r.isValid())
			r = charSection->getByParams(Race, Gender, (isHD ? charSectionsDB::SkinTypeHD : charSectionsDB::SkinType), 0, 0);
		if (r.isValid())
		{
#if WOW_VER >= 70
			uint32_t f1 = r.getInArray<int>(charSectionsDB::Tex1, 0);
			char t1[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f1, t1, 256);

			int f2 = r.getInArray<int>(charSectionsDB::Tex1, 1);
			char t2[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f2, t2, 256);
#else
			const char* t1 = r.getString(charSectionsDB::Tex1);
			const char* t2 = r.getString(charSectionsDB::Tex1 + 1);
#endif
			//base
			charTex.addLayer(t1, CR_BASE, 0);

			if (strlen(t2))			//tauren fur
			{
				ITexture* tex = g_Engine->getResourceLoader()->loadTexture(t2);
				setReplaceTexture(TEXTURE_FUR, tex);
			}

		}
	}

	//underwear
	{
		auto r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::UnderwearTypeHD : charSectionsDB::UnderwearType), 0, SkinColor);
		if(!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::UnderwearTypeHD : charSectionsDB::UnderwearType), 0, 0);
		if (r.isValid())
		{
#if WOW_VER >= 70
			uint32_t f1 = r.getInArray<int>(charSectionsDB::Tex1, 0);
			char t1[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f1, t1, 256);

			int f2 = r.getInArray<int>(charSectionsDB::Tex1, 1);
			char t2[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f2, t2, 256);
#else
			const char* t1 = r.getString(charSectionsDB::Tex1);
			const char* t2 = r.getString(charSectionsDB::Tex1 + 1);
#endif

			charTex.addLayer(t1, CR_PELVIS_UPPER, 1);		//pants
			charTex.addLayer(t2, CR_TORSO_UPPER, 1);		//top
		}
	}

	//face
	{
		auto r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FaceTypeHD : charSectionsDB::FaceType), FaceType, SkinColor);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FaceTypeHD : charSectionsDB::FaceType), FaceType, 0);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FaceTypeHD : charSectionsDB::FaceType), 0, 0);
		if (r.isValid())
		{
#if WOW_VER >= 70
			uint32_t f1 = r.getInArray<int>(charSectionsDB::Tex1, 0);
			char t1[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f1, t1, 256);

			int f2 = r.getInArray<int>(charSectionsDB::Tex1, 1);
			char t2[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f2, t2, 256);
#else
			const char* t1 = r.getString(charSectionsDB::Tex1);
			const char* t2 = r.getString(charSectionsDB::Tex1 + 1);
#endif

			charTex.addLayer(t1, CR_FACE_LOWER, 1);
			charTex.addLayer(t2, CR_FACE_UPPER, 1);
		}
	}

	//facial hair
	{
		//texture
		auto r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FacialHairTypeHD : charSectionsDB::FacialHairType), FacialHair, HairColor);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FacialHairTypeHD : charSectionsDB::FacialHairType), FacialHair, 0);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::FacialHairTypeHD : charSectionsDB::FacialHairType), 0, 0);
		if (r.isValid())
		{
#if WOW_VER >= 70
			uint32_t f1 = r.getInArray<int>(charSectionsDB::Tex1, 0);
			char t1[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f1, t1, 256);

			int f2 = r.getInArray<int>(charSectionsDB::Tex1, 1);
			char t2[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f2, t2, 256);
#else
			const char* t1 = r.getString(charSectionsDB::Tex1);
			const char* t2 = r.getString(charSectionsDB::Tex1 + 1);
#endif

			charTex.addLayer(t1, CR_FACE_LOWER, 2);
			charTex.addLayer(t2, CR_FACE_UPPER, 2);
		}
	}	

	//hair texture
	{
		auto r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), HairStyle, HairColor);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), HairStyle, 0);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, (isHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), 0, 0);
		if (r.isValid())
		{
#if WOW_VER >= 70
			uint32_t f1 = r.getInArray<int>(charSectionsDB::Tex1, 0);
			char t1[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f1, t1, 256);
#else
			const char* t1 = r.getString(charSectionsDB::Tex1);
#endif
			string256 texname;
			if (strlen(t1) == 0)
			{
				string256 s = Mesh->Dir;
				if (Gender == 1)
					s.subString(0, (uint32_t)strlen(Mesh->Dir)-7, texname);
				else
					s.subString(0, (uint32_t)strlen(Mesh->Dir)-5, texname);

				char hairname[32];
				Q_sprintf(hairname, 32, "Hair00_%02d.blp", (int32_t)HairColor);
				texname.append(hairname);
			}
			else
			{
				texname = t1;
			}

			ITexture* tex = g_Engine->getResourceLoader()->loadTexture(texname.c_str());
			setReplaceTexture(TEXTURE_HAIR, tex);	

#if WOW_VER >= 70
			uint32_t f2 = r.getInArray<int>(charSectionsDB::Tex1, 1);
			char t2[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f2, t2, 256);

			int f3 = r.getInArray<int>(charSectionsDB::Tex1, 2);
			char t3[256];
			g_Engine->getWowDatabase()->getTextureFilePath(f3, t3, 256);
#else
			const char* t2 = r.getString(charSectionsDB::Tex1 + 1);
			const char* t3 = r.getString(charSectionsDB::Tex1 + 2);
#endif

			charTex.addLayer(t2, CR_FACE_LOWER, 3);
			charTex.addLayer(t3, CR_FACE_UPPER, 3);	
		}	
	}

	//clothes

	if (Equipments[CS_CHEST] != 0)
	{
		auto r = g_Engine->getWowDatabase()->getItemDB()->getByID(Equipments[CS_CHEST]);
		if (r.isValid() &&  r.getInt(itemDB::InventorySlot) == IT_ROBE)
		{
			// 			if (r.getUInt(itemDisplayDB::RobeGeosetFlags) == 1)
			CharacterInfo->HasRobe = true;
		}
	}

	if (Equipments[CS_PANTS] != 0)
	{
		auto r = g_Engine->getWowDatabase()->getItemDB()->getByID(Equipments[CS_PANTS]);
		if (r.isValid() &&  r.getInt(itemDB::InventorySlot) == IT_PANTS)
		{
			if (r.getUInt(itemDisplayDB::GeosetGroup + 2) == 1)
				CharacterInfo->HasRobe = true;
		}
	}

	for (uint32_t i=0; i<NUM_CHAR_SLOTS; ++i)
	{
		if (Equipments[i] != 0)
			addClothesEquipment(i, Equipments[i], getClothesSlotLayer(i), charTex, CharacterInfo->IsNpc);
	}

// 	if (Equipments[CS_GLOVES])
// 		CharacterInfo->Geosets[CG_WRISTBANDS] = 0;

	if (CharacterInfo->Geosets[CG_TROUSERS] == 2)
	{
		CharacterInfo->Geosets[CG_BOOTS] = 0;
//		CharacterInfo->Geosets[CG_FEET] = Equipments[CS_BOOTS] ? 2 : 1;
		CharacterInfo->Geosets[CG_TABARD2] = 0;
		CharacterInfo->Geosets[CG_TABARD] = 0;
	}

	if (CharacterInfo->Geosets[CG_TROUSERS] || CharacterInfo->Geosets[CG_TABARD])
	{
		CharacterInfo->Geosets[CG_TABARD2] = 0;
	}
}

void wow_m2instance::addClothesEquipment( int32_t slot, int32_t itemnum, int32_t layer, CharTexture& tex, bool npc/*=true*/ )
{
	int* Geosets = CharacterInfo->Geosets;
	uint32_t Gender = CharacterInfo->Gender;

	if (slot == CS_PANTS && Geosets[CG_TROUSERS]==2)
		return;

	wowDatabase* database = g_Engine->getWowDatabase();

	int32_t itemID = 0;
	int32_t type = 0;
	
	if (!npc)
	{
		auto r = database->getItemDB()->getByID(itemnum);
		ASSERT(r.isValid());
		if(!r.isValid())
			return;

		type = r.getInt(itemDB::InventorySlot);
		itemID = database->getItemDisplayId(itemnum);
	}
	else
	{
		//70 itemDisplayInfoMaterialResDB, 60 itemDisplayDB
		itemID = itemnum;		
	}
	
#if WOW_VER >= 70
	const itemDisplayInfoMaterialResDB* itemDisplayInfoResDB = database->getItemDisplayInfoMaterialResDB();	
#else
	auto r = database->getItemDisplayDB()->getByID(itemID);
	ASSERT(r.isValid());
	if (!r.isValid())
		return;
#endif

	if (slot == CS_CHEST || slot == CS_SHIRT)
	{
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
			Geosets[CG_WRISTBANDS] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 0);

#if WOW_VER >= 70
		char armUpper[256];
		char armLower[256];
		char torsoUpper[256];
		char torsoLower[256];

		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperArm, armUpper, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerArm, armLower, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperTorso, torsoUpper, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerTorso, torsoLower, 256);
#elif WOW_VER >= 60
		char armUpper[256];
		char armLower[256];
		char torsoUpper[256];
		char torsoLower[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexArmUpper), armUpper, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexArmLower), armLower, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexChestUpper), torsoUpper, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexChestLower), torsoLower, 256);
#else
		const char* armUpper = r.getString(itemDisplayDB::TexArmUpper);
		const char* armLower = r.getString(itemDisplayDB::TexArmLower);
		const char* torsoUpper = r.getString(itemDisplayDB::TexChestUpper);
		const char* torsoLower = r.getString(itemDisplayDB::TexChestLower);
#endif

		tex.addItemLayer(armUpper, CR_ARM_UPPER, Gender, layer);
		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);

		tex.addItemLayer(torsoUpper, CR_TORSO_UPPER, Gender, layer);
		tex.addItemLayer(torsoLower, CR_TORSO_LOWER, Gender, layer);

		if(type == IT_ROBE || 
			(r.isValid() && r.getUInt(itemDisplayDB::GeosetGroup + 2) == 1))
		{
			if (Geosets[CG_FEET] == 1)
				Geosets[CG_FEET] = 2;

#if WOW_VER >= 70
			char legUpper[256];
			char legLower[256];

			itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperLeg, legUpper, 256);
			itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerLeg, legLower, 256);

#elif WOW_VER >= 60
			char legUpper[256];
			char legLower[256];
			database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegUpper), legUpper, 256);
			database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegLower), legLower, 256);
#else
			const char* legUpper = r.getString(itemDisplayDB::TexLegUpper);
			const char* legLower = r.getString(itemDisplayDB::TexLegLower);
#endif
			tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);
			tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);
		}
	}
	else if (slot == CS_BELT)
	{
#if WOW_VER >= 70
		char torsolower[256];
		char legUpper[256];

		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerTorso, torsolower, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperLeg, legUpper, 256);

#elif WOW_VER >= 60
		char torsolower[256];
		char legUpper[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexChestLower), torsolower, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegUpper), legUpper, 256);
#else
		const char* torsolower = r.getString(itemDisplayDB::TexChestLower);
		const char* legUpper = r.getString(itemDisplayDB::TexLegUpper);
#endif

		tex.addItemLayer(torsolower, CR_TORSO_LOWER, Gender, layer);
		tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);
	}
	else if (slot == CS_BRACERS)
	{
#if WOW_VER >= 70
		char armLower[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerArm, armLower, 256);
#elif WOW_VER >= 60
		char armLower[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexArmLower), armLower, 256);
#else
 		const char* armLower = r.getString(itemDisplayDB::TexArmLower);
#endif
 		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);
	}
	else if (slot == CS_PANTS)
	{
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
		{
			Geosets[CG_PANTS2] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 0);
			Geosets[CG_KNEEPADS] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 1);
		}

		if (Geosets[CG_FEET] == 1)
			Geosets[CG_FEET] = 2;

#if WOW_VER >= 70
		char legUpper[256];
		char legLower[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperLeg, legUpper, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerLeg, legLower, 256);
#elif WOW_VER >= 60
		char legUpper[256];
		char legLower[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegUpper), legUpper, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegLower), legLower, 256);
#else
		const char* legUpper = r.getString(itemDisplayDB::TexLegUpper);
		const char* legLower = r.getString(itemDisplayDB::TexLegLower);
#endif

		tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);

		if (r.isValid() && r.getUInt(itemDisplayDB::GeosetGroup + 2) == 1)		//法袍类型的裤子要优先
		{	
			layer = getClothesSlotLayer(CS_BOOTS) + 1;
		}
		tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);
	}
	else if (slot == CS_GLOVES)
	{
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
			Geosets[CG_GLOVES] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 0);

#if WOW_VER >= 60
		if (CharacterInfo->Race == RACE_UNDEAD && CharacterInfo->Gender == 0 && Geosets[CG_GLOVES] > 3)
			Geosets[CG_GLOVES] = 2;
#endif

#if WOW_VER >= 70
		char hand[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::Hands, hand, 256);
#elif WOW_VER >= 60
		char hand[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexHands), hand, 256);
#else
		const char* hand = r.getString(itemDisplayDB::TexHands);
#endif
		tex.addItemLayer(hand, CR_HAND, Gender, layer);
		if (Geosets[CG_GLOVES] > 1)				//有模型，这时覆盖法袍,衬衣纹理
		{
			layer = getClothesSlotLayer(CS_CHEST) + 1;
		}
		
#if WOW_VER >= 70
		char armLower[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerArm, armLower, 256);
#elif WOW_VER >= 60
		char armLower[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexArmLower), armLower, 256);
#else
		const char* armLower = r.getString(itemDisplayDB::TexArmLower);	
#endif
		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);
	}
	else if (slot == CS_BOOTS)
	{
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
 			Geosets[CG_BOOTS] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 0);
 
#if WOW_VER >= 60
		if (CharacterInfo->Race == RACE_UNDEAD && CharacterInfo->Gender == 0)
		{
			if(Geosets[CG_BOOTS] == 4)
				Geosets[CG_BOOTS] = 2;
			else if (Geosets[CG_BOOTS] == 5)
				Geosets[CG_BOOTS] = 1;
		}
#endif

		if (Geosets[CG_FEET] == 1)
			Geosets[CG_FEET] = 2;

#if WOW_VER >= 70
		char legLower[256];
		char feet[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerLeg, legLower, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::Foot, feet, 256);
#elif WOW_VER >= 60
		char legLower[256];
		char feet[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexLegLower), legLower, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexFeet), feet, 256);
#else
 		const char* legLower = r.getString(itemDisplayDB::TexLegLower);
		const char* feet = r.getString(itemDisplayDB::TexFeet);
#endif

		tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);

		if (CharacterInfo->Race != RACE_TROLL &&
			CharacterInfo->Race != RACE_TAUREN &&
			CharacterInfo->Race != RACE_WORGEN)
			tex.addItemLayer(feet, CR_FOOT, Gender, layer);
	}
	else if (slot == CS_TABARD)
	{
		Geosets[CG_TABARD] = 2;

#if WOW_VER >= 70
		char chestUpper[256];
		char chestLower[256];
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::UpperTorso, chestUpper, 256);
		itemDisplayInfoResDB->getTexturePath(itemID, itemDisplayInfoMaterialResDB::LowerTorso, chestLower, 256);
#elif WOW_VER >= 60
		char chestUpper[256];
		char chestLower[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexChestUpper), chestUpper, 256);
		database->getTextureFilePath(r.getInt(itemDisplayDB::TexChestLower), chestLower, 256);
#else
		const char* chestUpper = r.getString(itemDisplayDB::TexChestUpper);
		const char* chestLower = r.getString(itemDisplayDB::TexChestLower);
#endif

		tex.addItemLayer(chestUpper, CR_TORSO_UPPER, Gender, layer);
		tex.addItemLayer(chestLower, CR_TORSO_LOWER, Gender, layer);
	}
	else if (slot == CS_CAPE)
	{	
#if WOW_VER >= 70
		char rt[256];
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
			g_Engine->getWowDatabase()->getFilePath(r.getByte3(itemDisplayDB::Skin), rt, 256);
		else
			rt[0] = '\0';

		if (strlen(rt) > 0)
		{
			char path[QMAX_PATH];
			Q_strcpy(path, QMAX_PATH, rt);
#elif WOW_VER >= 60
		char rt[256];
		database->getTextureFilePath(r.getInt(itemDisplayDB::Skin), rt, 256);
		if (strlen(rt) > 0)
		{
			char path[QMAX_PATH];
			Q_strcpy(path, QMAX_PATH, rt);
#else
		const char* rt = r.getString(itemDisplayDB::Skin);
		if (rt && strlen(rt) > 0)
		{
			char path[QMAX_PATH];
			Q_strcpy(path, QMAX_PATH, regionCapePath);
			Q_strcat(path, QMAX_PATH, rt);
			Q_strcat(path, QMAX_PATH, ".blp");
#endif
			ITexture* cape = g_Engine->getResourceLoader()->loadTexture(path);
			if (cape)
			{
				setReplaceTexture(TEXTURE_CAPE, cape);
				Geosets[CG_CAPE] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 0);
				CharacterInfo->CapeID = Geosets[CG_CAPE];
			}		
		}	
	}
	else
	{
		return;
	}

	if (Geosets[CG_TROUSERS] == 1)
	{
		auto r = database->getItemDisplayDB()->getByID(itemID);
		if(r.isValid())
			Geosets[CG_TROUSERS] = 1 + r.getUInt(itemDisplayDB::GeosetGroup + 2);
	}
}

void wow_m2instance::buildVisibleGeosets()
{
	if (!CurrentSkin)
		return;

	InitializeListHead(&VisibleGeosetList);

	if (!CharacterInfo || 
		CharacterInfo->Race == RACE_NAGA)
	{
		for (uint32_t i=0; i<CurrentSkin->NumGeosets; ++i)
		{
			CGeoset* submesh = &CurrentSkin->Geosets[i];
			int16_t texID = submesh->getTexUnit(0)->TexID;

			if (texID != -1)
			{
				ETextureTypes texType = Mesh->TextureTypes[texID];
				if (texType == TEXTURE_FILENAME)
					DynGeosets[i].Texture0 = Mesh->getTexture(texID);
				else
					DynGeosets[i].Texture0 = ReplaceTextures[(uint32_t)texType];
			}
			else
			{
				DynGeosets[i].Texture0 = nullptr;
			}

			DynGeosets[i].Texture1 = g_Engine->getSpecialTextureServices()->getTexture(EST_ARMORREFLECT);
			DynGeosets[i].Texture2 = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");

			InsertTailList(&VisibleGeosetList, &DynGeosets[i].Link);
		}

		return;
	}

	bool isBald = true;

	int* Geosets = CharacterInfo->Geosets;
	uint32_t Race = CharacterInfo->Race;
	uint32_t Gender = CharacterInfo->Gender;
	bool isHD = CharacterInfo->IsHD;

	//eye glow
	if (CharacterInfo->DeathKnight)
		Geosets[CG_EYEGLOW] = 3;
	else if(Race == RACE_UNDEAD || Race == RACE_NIGHTELF || Race == RACE_SKELETON)
		Geosets[CG_EYEGLOW] = 2;

	//ears
	Geosets[CG_EARS]= 2;

	if (!CharacterInfo->ShowCape)
		Geosets[CG_CAPE] = 1;
	else
		Geosets[CG_CAPE] = CharacterInfo->CapeID;

	//hair facial visbility
	{
		auto r = g_Engine->getWowDatabase()->getCharFacialHairDB()->getByParams( Race, Gender, CharacterInfo->FacialHair );
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharFacialHairDB()->getByParams( Race, Gender, 0 );
		if (r.isValid())
		{
			Geosets[CG_GEOSET100] = r.getInArray<uint32_t>(charFacialHairDB::Geoset100V400, 0);		
			Geosets[CG_GEOSET200] = r.getInArray<uint32_t>(charFacialHairDB::Geoset100V400, charFacialHairDB::Geoset200V400);
			Geosets[CG_GEOSET300] = r.getInArray<uint32_t>(charFacialHairDB::Geoset100V400, charFacialHairDB::Geoset300V400);
		}
	}
	
	//visibility
	bool* geoShow = (bool*)Z_AllocateTempMemory(sizeof(bool) * CurrentSkin->NumGeosets);
	memset(geoShow, 0, sizeof(bool) * CurrentSkin->NumGeosets);

	const charHairGeosetsDB* hairGeosetDb = g_Engine->getWowDatabase()->getCharHairGeosetDB();
	auto rh = hairGeosetDb->getByParams(Race, Gender, CharacterInfo->HairStyle);
	if (rh.isValid())
	{
		uint32_t id = rh.getUInt(charHairGeosetsDB::Geoset);

		if(id != 0)
		{
			for (uint32_t k=0; k<CurrentSkin->NumGeosets; ++k)
			{
				CGeoset* set = &CurrentSkin->Geosets[k];
				if (set->GeoID == id )
				{
					geoShow[k] = !CharacterInfo->HelmHideHair;
					if (geoShow[k])
						isBald = false;
				}
			}
		}
	}

	int gloves = 0;
	int wrist = 0;

	//show
	for (uint32_t i=0; i<CurrentSkin->NumGeosets; ++i)
	{
		CGeoset* submesh = &CurrentSkin->Geosets[i];
		uint32_t id  = submesh->GeoID;
		int16_t texID = submesh->getTexUnit(0)->TexID;

		if (texID != -1)
		{
			ETextureTypes texType = Mesh->TextureTypes[texID];
			if (texType == TEXTURE_FILENAME)
			{
				DynGeosets[i].Texture0 = Mesh->getTexture(texID);
			}
			else
			{
				DynGeosets[i].Texture0 = ReplaceTextures[(uint32_t)texType];
			}
		}
		else
		{
			DynGeosets[i].Texture0 = nullptr;
		}


		DynGeosets[i].Texture1 = g_Engine->getSpecialTextureServices()->getTexture(EST_ARMORREFLECT);
		DynGeosets[i].Texture2 = nullptr; //DynGeosets[i].Texture0;

		if (id == 0)		
		{
#if WOW_VER >= 60
			if (Race == RACE_WORGEN)		//i>=3狼人问题
				geoShow[i] = Gender ? (i<2 || i == 14) : (i < 3 || i == 15);
			else if (Race == RACE_NIGHTELF)
				geoShow[i] = Gender ? (i != 8 && i != 9) : ( isHD ? (i == 6 || i == 11) : i == 17);
			else
				geoShow[i] = true;
#else
			if (Race == RACE_WORGEN)		//i>=3狼人问题
				geoShow[i] = Gender ? (i<2) : (i<3);
			else if (Race == RACE_NIGHTELF)
				geoShow[i] = Gender ? (i != 1) : (i == 0);
			else
				geoShow[i] = true;
#endif
			continue;
		}
	
		if (id == 1)
		{
			geoShow[i] |= isBald;				//秃头的头顶部
			continue;
		}

		int geotype = (int)(id / 100);
		if (isBald && geotype == CG_HAIRSTYLE)							//隐藏头发
		{
			geoShow[i] = false;
			continue;
		}

		if (geotype == CG_GEOSET100)
		{
			geoShow[i] = (id == geotype * 100 + Geosets[geotype] && !CharacterInfo->HelmHideFacial1);
		}
		else if (geotype == CG_GEOSET200)
		{
			geoShow[i] = (id == geotype * 100 + Geosets[geotype] && !CharacterInfo->HelmHideFacial2);
		}
		else if (geotype == CG_GEOSET300)
		{
			geoShow[i] = (id == geotype * 100 + Geosets[geotype] && !CharacterInfo->HelmHideFacial3);
		}

// 		if ( geotype == 18 || geotype == 19 ||geotype == 20)
// 		{
// 			geoShow[i] = true;
// 			continue;
// 		}

		if (geotype > CG_GEOSET300 && geotype < NUM_GEOSETS)				//except hair, facial
		{
			geoShow[i] = (id == geotype * 100 + Geosets[geotype]);
		}

		if (geoShow[i] && Race == RACE_WORGEN)				//狼人多手问题fix
		{
			if (geotype == CG_GLOVES)
			{
				if (gloves >= 1)
					geoShow[i] = false;
				else
					++gloves; 
			}
			else
			if (geotype == CG_WRISTBANDS)
			{
				if (wrist >= 1)
					geoShow[i] = false;
				else
					++wrist; 
			}
		}

// 		ECharGeosets t = (ECharGeosets)geotype;
// 		if (geoShow[i])
// 		{
// 			int m = 0;	
// 		}	
	}	

	for (uint32_t i=0; i<CurrentSkin->NumGeosets; ++i)
	{
		if (geoShow[i])
			InsertTailList(&VisibleGeosetList, &DynGeosets[i].Link);
	}

	Z_FreeTempMemory(geoShow);
}

bool wow_m2instance::setGeosetMaterial(uint32_t subset, SMaterial& material)
{
	CGeoset* set = &CurrentSkin->Geosets[subset];
	const STexUnit* texUnit = set->getTexUnit(0);
	int16_t rfIndex = texUnit->rfIndex;
	
	if (rfIndex == -1 || Mesh->RenderFlags[rfIndex].invisible)
		return false;

	const IFileM2::SRenderFlag& renderflag = Mesh->RenderFlags[rfIndex];

	material.ZWriteEnable = renderflag.zwrite;
	material.Lighting = renderflag.lighting;

	if (set->BillBoard)
	{
		material.Cull = ECM_BACK;
	}
	else
	{
		material.Cull = (renderflag.frontCulling && renderflag.blend == 0) ? ECM_FRONT : ECM_NONE;
	}

	material.AmbientColor.set(0.9f, 0.9f, 0.9f);
	material.EmissiveColor = DynGeosets[subset].emissive;

	switch(renderflag.blend)
	{
	case M2::BM_OPAQUE:
		material.MaterialType = EMT_SOLID;
		break;
	case M2::BM_ALPHA_TEST:
		material.MaterialType =  EMT_ALPHA_TEST;
		break;
	case M2::BM_ALPHA_BLEND:
		material.MaterialType= EMT_TRANSPARENT_ALPHA_BLEND;
		break;
	case M2::BM_ADDITIVE_COLOR:
		material.MaterialType = EMT_TRANSPARENT_ADD_COLOR;
		break;
	case M2::BM_ADDITIVE_ALPHA:
		material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	case M2::BM_MODULATE:
		material.MaterialType = EMT_TRANSPARENT_MODULATE;
		break;
	case M2::BM_MODULATEX2:
		material.MaterialType = EMT_TRANSPARENT_MODULATE_X2;
		break;
	case M2::BM_MODULATEX4:
		material.MaterialType = EMT_TRANSPARENT_MODULATE_X2;
		break;
	default:
		material.MaterialType = EMT_SOLID;
		ASSERT(false);
	}

	if(EnableModelColor)
		material.setMaterialColor(ModelColor);

	if (EnableModelAlpha)
	{
		material.setMaterialAlpha(ModelAlpha);
		if (ModelAlpha < 1.0f)
		{
			switch(material.MaterialType)
			{
			case EMT_SOLID:
				material.MaterialType = EMT_TRANSPARENT_ALPHA_BLEND;
				break;
			case EMT_ALPHA_TEST:
				material.MaterialType = EMT_TRANSAPRENT_ALPHA_BLEND_TEST;
				break;
            default:
                break;
			}
		}
	}

	DynGeosets[subset].finalColor = material.Lighting ? material.AmbientColor.toSColor() : material.EmissiveColor.toSColor();

	if (!setMaterialShaders(material, texUnit, set->BillBoard))
		return false;

	material.TextureLayer[0].TextureWrapU = set->getTexUnit(0)->WrapX ? ETC_CLAMP : ETC_REPEAT;
	material.TextureLayer[0].TextureWrapV = set->getTexUnit(0)->WrapY ? ETC_CLAMP : ETC_REPEAT;

	return true;
}

bool wow_m2instance::getItemInfo( int32_t itemid, bool npc, SItemInfo& itemInfo ) const
{
	itemInfo.id = -1;
	itemInfo.itemType = 0;
	itemInfo.sheathtype = 0;

	auto r = g_Engine->getWowDatabase()->getItemDB()->getByID(itemid);
	if (!r.isValid())
		return false;

	itemInfo.id = npc ? itemid : g_Engine->getWowDatabase()->getItemDisplayId(itemid);
	itemInfo.itemType = r.getInt(itemDB::InventorySlot);
	itemInfo.sheathtype = r.getInt(itemDB::Sheath);
	
	return true;
}

void wow_m2instance::setM2Equipment( int32_t slot, int32_t itemid, bool sheath, SAttachmentEntry* entry1, SAttachmentInfo* info1, SAttachmentEntry* entry2, SAttachmentInfo* info2 )
{
	entry1->attachIndex = -1;
	entry2->attachIndex = -1;

	if (itemid <= 0 || !CharacterInfo)
		return;

	SItemInfo itemInfo;

	float equipScale = 1.0f;

	if ( !getItemInfo(itemid, CharacterInfo->IsNpc, itemInfo) )
		return;

	bool dagger = itemInfo.itemType == IT_DAGGER;			//双手

	//scale
	float head, shoulder, weapon, waist;
	getEquipScale(head, shoulder, weapon, waist);
	switch(slot)
	{
	case CS_HEAD:
		equipScale = head;
		break;
	case CS_SHOULDER:
		equipScale = shoulder;
		break;
	case CS_HAND_RIGHT:
	case CS_HAND_LEFT:
		equipScale = weapon;
		break;
	case CS_BELT:
		equipScale = waist;
		break;
	}

	auto display = g_Engine->getWowDatabase()->getItemDisplayDB()->getByID(itemInfo.id);
	if (!display.isValid())
		return;

	uint32_t Race = CharacterInfo->Race;
	uint32_t Gender = CharacterInfo->Gender;

	int16_t id1 = -1;
	int16_t id2 = -1;

	string256 path;
	if (slot == CS_HEAD)
	{
		id1 = ATT_HELMET;
		path = regionHeadPath;

		uint32_t hair = 0;

		auto helmet = dbc::record::EMPTY();
		if (Gender == 0)
		{
			uint32_t vis1 = display.getUInt(itemDisplayDB::GeosetVisID + 0);
			helmet = g_Engine->getWowDatabase()->getHelmGeosetDB()->getByID(vis1);
		}
		else
		{
			uint32_t vis2 = display.getUInt(itemDisplayDB::GeosetVisID + 1);
			helmet = g_Engine->getWowDatabase()->getHelmGeosetDB()->getByID(vis2);
		}

		if (helmet.isValid())
		{
			hair = helmet.getUInt(helmGeosetDB::HideHair);
		}

		CharacterInfo->HelmHideHair = hair != 0;
		CharacterInfo->HelmHideFacial1 = false; //facial1 != 0;
		CharacterInfo->HelmHideFacial2 = false; //facial2 != 0;
		CharacterInfo->HelmHideFacial3 = false; //facial3 != 0;
	}
	else if(slot == CS_SHOULDER)
	{
		id1 = ATT_LEFT_SHOULDER;
		id2 = ATT_RIGHT_SHOULDER;
		path = regionShoulderPath;	
	}
	else if (slot == CS_HAND_LEFT)
	{
		id1 = ATT_LEFT_PALM;	
	}
	else if (slot == CS_HAND_RIGHT)
	{
		id1 = ATT_RIGHT_PALM;
	}
	else if (slot == CS_QUIVER)
	{
		id1 = ATT_LEFT_BACK_SHEATH;
		path = regionQuiverPath;
	}
	else if (slot == CS_BELT)
	{
		id1 = ATT_WAIST;
		path = regionWaistPath;
	}
	else
	{
		return;
	}

	if (slot == CS_HAND_LEFT || slot == CS_HAND_RIGHT)
	{
		if (itemInfo.itemType == IT_SHIELD)
		{
			path = regionShieldPath;
			id1 = ATT_LEFT_WRIST;
		}
		else
		{
			path = regionWeaponPath;	
		}

		if (sheath)
		{
			switch(itemInfo.sheathtype)
			{
			case SHEATHETYPE_SHIELD:
				id1 = ATT_MIDDLE_BACK_SHEATH;
				break;
			case SHEATHETYPE_MAINHAND:
				id1 = slot == CS_HAND_RIGHT ?  ATT_RIGHT_BACK_SHEATH : ATT_LEFT_BACK_SHEATH;
				break;
			case SHEATHETYPE_LARGEWEAPON:
				id1 = slot == CS_HAND_RIGHT ?  ATT_LEFT_BACK : ATT_RIGHT_BACK;
				break;
			case SHEATHETYPE_HIPWEAPON:
				id1 = slot == CS_HAND_RIGHT ? ATT_LEFT_HIP_SHEATH : ATT_RIGHT_HIP_SHEATH;
				break;
			}
		}
	}

	string256 modelPath;
	string256 texPath;

	if (id1 >= 0)	
	{
		string256 model1 = display.getString(itemDisplayDB::Model);
#if WOW_VER >= 60
		int32_t skin1Id = display.getInt(itemDisplayDB::Skin);
		string256 skin1;
		g_Engine->getWowDatabase()->getTextureFilePath(skin1Id, skin1);
#else
		string256 skin1 = display.getString(itemDisplayDB::Skin);
#endif
		if (dagger)		//dagger fix
		{
			if (slot == CS_HAND_RIGHT && model1.endWith("Left.mdx"))
			{
				string256 tmp;
				model1.subString(0, model1.length()-8, tmp);
				tmp.append("Right.mdx");
				model1 = tmp;
			}
			else if (slot == CS_HAND_LEFT && model1.endWith("Right.mdx"))
			{
				string256 tmp;
				model1.subString(0, model1.length()-9, tmp);
				tmp.append("Left.mdx");
				model1 = tmp;
			}
		}

		modelPath = path;
		modelPath.append(model1);

		if (slot == CS_HEAD)
		{	
			string256 headPath;
			if (modelPath.endWith(".mdx"))
			{
				modelPath.subString(0, modelPath.length()-4, headPath);
				headPath.append("_");
				auto race = g_Engine->getWowDatabase()->getCharRacesDB()->getByID(Race);
				ASSERT(race.isValid());

				headPath.append(race.getString(charRacesDB::ShortName));
				headPath.append(Gender ? "F" : "M");
				headPath.append(".M2");

				modelPath = headPath;
			}
			else
			{
				CharacterInfo->HelmHideHair = false;
				CharacterInfo->HelmHideFacial1 = false;
				CharacterInfo->HelmHideFacial2 = false;
				CharacterInfo->HelmHideFacial3 = false;
				id1 = -1;
			}
		}
		else
		{
			modelPath.changeExt("mdx", "m2");
		}

		if (id1 != -1)
		{
#if WOW_VER >= 60
			texPath = skin1;
#else
			texPath = path;
			texPath.append(skin1);
			texPath.append(".blp");
#endif
			entry1->slot = slot;
			entry1->id = id1;
			entry1->scale = equipScale;
			entry1->attachIndex = (id1 >= 0 && id1 < (int32_t)Mesh->NumAttachLookup) ? Mesh->AttachLookup[id1] : -1;
			Q_strcpy(info1->modelpath, QMAX_PATH, modelPath.c_str());
			Q_strcpy(info1->texpath, QMAX_PATH, texPath.c_str());
		}
	}

	if (id2 >= 0)
	{
		string256 model2 = display.getString(itemDisplayDB::Model + 1);

#if WOW_VER >= 60
		int32_t skin2Id = display.getInt(itemDisplayDB::Skin+ 1);
		string256 skin2;
		g_Engine->getWowDatabase()->getTextureFilePath(skin2Id, skin2);
#else
		string256 skin2 = display.getString(itemDisplayDB::Skin2);
#endif

		modelPath = path;
		modelPath.append(model2);

		string256 headPath;
		if (modelPath.endWith(".mdx"))
		{
			modelPath.subString(0, modelPath.length()-4, headPath);
			headPath.append(".M2");

			modelPath = headPath;

#if WOW_VER >= 60
			texPath = skin2;
#else
			texPath = path;
			texPath.append(skin2);
			texPath.append(".blp");
#endif

			entry2->slot = slot;
			entry2->id = id2;
			entry2->scale = equipScale;
			entry2->attachIndex = (id2 >= 0 && id2 < (int32_t)Mesh->NumAttachLookup) ? Mesh->AttachLookup[id2] : -1;
			Q_strcpy(info2->modelpath, QMAX_PATH, modelPath.c_str());
			Q_strcpy(info2->texpath, QMAX_PATH, texPath.c_str());
		}	
	}
}

void wow_m2instance::setM2Mount( SAttachmentEntry* entry )
{
	entry->id = ATT_MOUNT_POINT;
	entry->attachIndex = ATT_MOUNT_POINT < (int32_t)Mesh->NumAttachLookup ? Mesh->AttachLookup[ATT_MOUNT_POINT] : -1;
	entry->scale = 1.0f;
	entry->slot = CS_MOUNT;
}

void wow_m2instance::setHeadDecal( SAttachmentEntry* entry )
{
	entry->id = ATT_TOP_OF_HEAD;
	entry->attachIndex = ATT_TOP_OF_HEAD < (int32_t)Mesh->NumAttachLookup ? Mesh->AttachLookup[ATT_TOP_OF_HEAD] : -1;
	entry->scale = 1.0f;
	entry->slot = CS_HEADDECAL;
}

bool wow_m2instance::isBothHandsDagger( int32_t itemid ) const
{
	SItemInfo itemInfo;
	if ( !getItemInfo(itemid, CharacterInfo->IsNpc, itemInfo) )
		return false;
	bool dagger = itemInfo.itemType == IT_DAGGER;			//双手
	if (!dagger)
		return false;

	auto display = g_Engine->getWowDatabase()->getItemDisplayDB()->getByID(itemInfo.id);
	if (!display.isValid())
		return false;

 	string256 model1 = display.getString(itemDisplayDB::Model);
 	return model1.endWith("left.mdx") || model1.endWith("right.mdx");
}

//是否没有规定左右顺序的
bool wow_m2instance::isOrdinaryDagger( int32_t itemid ) const
{
	SItemInfo itemInfo;
	if ( !getItemInfo(itemid, CharacterInfo->IsNpc, itemInfo) )
		return false;
	bool dagger = isTwoHanded(itemInfo.itemType);			//双手
	if (!dagger)
		return false;

	auto display = g_Engine->getWowDatabase()->getItemDisplayDB()->getByID(itemInfo.id);
	if (!display.isValid())
		return false;

	string256 model1 = display.getString(itemDisplayDB::Model);
	return !model1.endWith("left.mdx") && !model1.endWith("right.mdx");
}

void wow_m2instance::dressStartOutfit( int32_t startid )
{
	auto r = g_Engine->getWowDatabase()->getStartOutfitDB()->getByID(startid);
	if (!r.isValid())
		return;

	int* Equipments = CharacterInfo->Equipments;

	for (int32_t i=0; i<NUM_CHAR_SLOTS; ++i)
		Equipments[i] = 0;
	CharacterInfo->HelmHideHair = false;
	CharacterInfo->HelmHideFacial1 = false;
	CharacterInfo->HelmHideFacial2 = false;
	CharacterInfo->HelmHideFacial3 = false;
	CharacterInfo->DeathKnight = false;

	for (uint32_t i=0; i<startOutfitDB::NumItems; ++i)
	{
		int id = r.getInt(startOutfitDB::ItemIDBase + i);
		if (id == 0)
			continue;

		auto rItem = g_Engine->getWowDatabase()->getItemDB()->getByID(id);
		if (rItem.isValid() && rItem.getInt(itemDB::InventorySlot) > 0)
		{
			int32_t slot = getItemSlot(id);

			if (slot != -1)
				updateEquipments(slot, id);
		}
	}
}

void wow_m2instance::dressSet( int32_t setid )
{
	auto r = g_Engine->getWowDatabase()->getItemSetDB()->getByID(setid);
	if (!r.isValid())
		return;

	for (uint32_t i=0; i<itemSetDB::NumItems; ++i)
	{
#if WOW_VER >= 70
		int32_t id = r.getInArray<int>(itemSetDB::ItemIDBaseV400, i);
#else
		int32_t id = r.getInt(itemSetDB::ItemIDBaseV400 + i);
#endif
		if (id == 0)
			continue;

		auto rItem = g_Engine->getWowDatabase()->getItemDB()->getByID(id);
		if (rItem.isValid() && rItem.getInt(itemDB::InventorySlot) > 0)
		{
			int32_t slot = getItemSlot(id);

			if (slot != -1)
				updateEquipments(slot, id);
		}	
	}
}

bool wow_m2instance::isBlinkGeoset( uint32_t index ) const
{
	if (CurrentSkin && index < CurrentSkin->NumGeosets)
	{
		uint32_t Race = CharacterInfo->Race;
		uint32_t Gender = CharacterInfo->Gender;
		bool isHD = CharacterInfo->IsHD;

		if (isHD)
			return false;

#if WOW_VER >= 60
		switch(Race)
		{
		case RACE_HUMAN:
			if (Gender)
				return index == 3;
			else
				return index == 15;
		case RACE_DWARF:
			if (Gender)
				return index == 8;
			else
				return index == 3 || index == 16;
		case RACE_GNOME:
			if (Gender)
				return index == 6;
			else
				return index == 9;
		case RACE_ORC:
		case RACE_FEL_ORC:
			if (Gender)
				return index == 9 || index == 10;
			else
				return index == 14;
		case RACE_TROLL:
		case RACE_FOREST_TROLL:
		case RACE_ICE_TROLL:
			if(Gender)
				return false;
			else
				return index == 3;
		case RACE_NIGHTELF:
			if (Gender)
				return index == 8 || index == 9;
		case RACE_BLOODELF:
			if (Gender)
				return index == 11;
			else
				return index == 2;
		case RACE_DRAENEI:
				return index == 6;
		case RACE_WORGEN:
			return false;
		case RACE_TAUREN:
			if (Gender)
				return false;
			else
				return index == 20;
		case RACE_GOBLIN:
			if (Gender)
				return index == 2 || index == 3;
			else
				return index == 1 || index == 2;
		case RACE_SKELETON:
			return false;
		case RACE_PANDAREN:
			return false;
		}
#else
		switch(Race)
		{
		case RACE_HUMAN:
			if (Gender)
				return index == 2;
			else
				return index == 1 || index == 2;
		case RACE_DWARF:
			return index == 1 || index == 2;
		case RACE_GNOME:
			return index == 1;
		case RACE_ORC:
		case RACE_FEL_ORC:
			if (Gender)
				return index == 1 || index == 2;
			else
				return index == 1;
		case RACE_TROLL:
		case RACE_FOREST_TROLL:
		case RACE_ICE_TROLL:
			return index == 1;
		case RACE_NIGHTELF:
		case RACE_BLOODELF:
			return index == 1 || index == 2;
		case RACE_DRAENEI:
			if (Gender)
				return index == 1|| index == 2;
			else
				return index == 2;
		case RACE_WORGEN:
			return false;
		case RACE_TAUREN:
			return index == 2;
		case RACE_GOBLIN:
			if (Gender)
				return index == 2 || index == 3;
			else
				return index == 1 || index == 2;
		case RACE_SKELETON:
			return false;
		case RACE_PANDAREN:
			return false;
		}
#endif
	}
	return false;
}

int32_t wow_m2instance::getSlot(int32_t type) const
{
	switch(type)
	{
	case IT_HEAD:
		return CS_HEAD;
	case IT_NECK:
		return CS_NECK;
	case IT_SHIRT:
		return CS_SHIRT;
	case IT_SHOULDER:
		return CS_SHOULDER;
	case IT_CHEST:
	case IT_ROBE:
		return CS_CHEST;
	case IT_BELT:
		return CS_BELT;
	case IT_PANTS:
		return CS_PANTS;
	case IT_BOOTS:
		return CS_BOOTS;
	case IT_BRACERS:
		return CS_BRACERS;
	case IT_GLOVES:
		return CS_GLOVES;
	case IT_RIGHTHANDED:
	case IT_GUN:
		return CS_HAND_RIGHT;
	case IT_LEFTHANDED:
	case IT_BOW:
	case IT_SHIELD:
	case IT_OFFHAND:
		return CS_HAND_LEFT;
	case IT_CAPE:
		return CS_CAPE;
	case IT_TABARD:
		return CS_TABARD;
	case IT_QUIVER:
		return CS_QUIVER;
	case IT_2HANDED:
		return CS_HAND_RIGHT;

	case IT_DAGGER:
		return CS_HAND_RIGHT;

	case IT_THROWN:
		return -1;			//投掷武器不挂在挂点上
	default:
		return -1;
	}
}

int wow_m2instance::getClothesSlotLayer( int slot )			//装备的叠加顺序
{
	const int base = 5;
	switch(slot)
	{
	case CS_BELT:
		return base + 8;
	case CS_TABARD:
		return base + 7;
	case CS_CHEST:
		return base + 6;
	case CS_BRACERS:
		return base + 5;
	case CS_GLOVES:
		return base + 4;
	case CS_SHIRT:
		return base + 3;
	case CS_BOOTS:
		return base + 2;
	case CS_PANTS:
		return base + 1;
	default:
		return base;
	}
}

void wow_m2instance::getEquipScale(float& head, float& shoulder, float& weapon, float& waist) const
{
	uint32_t race = CharacterInfo->Race;
	uint32_t gender = CharacterInfo->Gender;
	bool isHD = CharacterInfo->IsHD;

	head = shoulder = weapon = waist = 1.0f;

	switch(race)
	{
	case RACE_HUMAN:
		{
			if (gender == 1)
			{
				shoulder = 0.6f;
				weapon = 0.8f;
				waist = 0.6f;
			}
			else
			{
				shoulder = 1.0f;
				waist = 0.9f;

				if (isHD)
				{
					head = 1.15f;
				}
			}
		}
		break;
	case RACE_DWARF:
	case RACE_BLOODELF:
		{
			if (gender == 1)
			{
				shoulder = 0.6f;
				weapon = 0.8f;
				waist = 0.6f;
			}
			else
			{
				shoulder = 0.9f;
				waist = 0.8f;
			}
		}
		break;
	case RACE_UNDEAD:
		{
			if (gender == 1)
			{
				shoulder = 0.6f;
				weapon = 0.8f;
				waist = 0.6f;
			}
			else
			{
				shoulder = 0.8f;
				waist = 0.8f;
			}
		}
		break;
	case RACE_NIGHTELF:
	case RACE_TROLL:
		{
			if (gender == 1)
			{
				shoulder = 0.8f;
				waist = 0.8f;
			}
		}
		break;
	case RACE_DRAENEI:
		{
			if (gender == 0)
			{
				shoulder = 1.5f;
				waist = 1.2f;
			}
			else
			{
				shoulder = 0.8f;
				waist = 0.8f;
			}
		}
		break;
	case RACE_WORGEN:
		{
			if (gender == 0)
			{
				shoulder = 1.2f;
				weapon = 1.2f;
				waist = 1.2f;
			}
		}
		break;
	case RACE_PANDAREN:
		{
			if (gender == 0)
			{
				shoulder = 1.4f;
				weapon = 1.2f;
				waist = 1.2f;
			}
		}
		break;
	case RACE_TAUREN:
	case RACE_ORC:
		{
			if (gender == 0)
			{
				shoulder = 1.5f;
				weapon = 1.2f;
				waist = 1.2f;
			}
		}
		break;
	case RACE_GNOME:
	case RACE_GOBLIN:
		{
			shoulder = 0.6f;
			weapon = 0.6f;
			waist = 0.6f;
		}
		break;
	}
}

bool wow_m2instance::setMaterialShaders( SMaterial& material, const STexUnit* texUnit, bool billboard )
{	
	IShaderServices* shaderServices = g_Engine->getDriver()->getShaderServices();

	if (billboard)
	{
		material.VertexShader = shaderServices->getVertexShader(EVST_CDIFFUSE_T1);
		material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE : EPST_COMBINERS_MOD);
		return true;
	}

	//shader
	if (texUnit->Mode == 1)
	{
		material.VertexShader = shaderServices->getVertexShader(EVST_DIFFUSE_T1);
		material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE : EPST_COMBINERS_MOD);
	}
	else if (texUnit->Mode == 2)
	{
		uint16_t special = ((texUnit->Shading & 0x8000) == 0x8000 || texUnit->Shading == 0x000e);
		if (special)
		{
  			material.VertexShader = shaderServices->getVertexShader(EVST_DIFFUSE_T1_ENV);
  			material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE_MOD2XNA : EPST_COMBINERS_MOD_MOD2XNA); 
		}
		else
		{
			material.VertexShader = shaderServices->getVertexShader(EVST_DIFFUSE_T1);
			material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE : EPST_COMBINERS_MOD);
		}
	}
	else if (texUnit->Mode == 3)
	{
		uint16_t special = (texUnit->Shading == 0x8003);
		if (special)
		{
			//0x8003
			material.VertexShader = shaderServices->getVertexShader(EVST_DIFFUSE_T1_ENV_T2);
			material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA : EPST_COMBINERS_MOD_MOD2XNA);
		}
		else
		{
			material.VertexShader = shaderServices->getVertexShader(EVST_DIFFUSE_T1);
			material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE : EPST_COMBINERS_MOD);
		}
	}
	else
	{
		material.PsType = (material.MaterialType == EMT_SOLID ? EPST_COMBINERS_OPAQUE : EPST_COMBINERS_MOD);
	}

	return true;
}


