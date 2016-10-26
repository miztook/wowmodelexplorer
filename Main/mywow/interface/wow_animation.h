#pragma once

#include "core.h"
#include "SColor.h"
#include "wow_m2_structs.h"

struct SAnimFile
{
	u8* data;
	u32 size;
};

template<>
inline quaternion interpolate<quaternion>(const float r, const quaternion& v1, const quaternion& v2)
{
	return quaternion::slerp(v1, v2, r);
}

template<>
inline vector2df interpolate<vector2df>(const float r, const vector2df& v1, const vector2df& v2)
{
	return vector2df::interpolate(v1, v2, r);
}

template<>
inline SColorf interpolate<SColorf>(const float r, const SColorf& v1, const SColorf& v2)
{
	return SColorf::interpolate(v1, v2, r, true);
}

template<>
inline SColor interpolate<SColor>(const float r, const SColor& v1, const SColor& v2)
{
	return SColor::interpolate(v1, v2, r, true);
}

enum Interpolations : int32_t
{
	INTERPOLATION_NONE = 0,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE,
};

template <class T>
class Identity {
public:
	 static const T& conv(const T& t)
	{
		return t;
	}
};

class ShortToFloat {
public:
	static const float conv(const short& t)
	{
		return t/32767.0f;
	}
};

class Vec3ToVec3
{
public:
	 static const vector3df conv(const vector3df& t)
	{
		return vector3df(t.X, t.Z, t.Y);
	}
};

struct PACK_QUATERNION {  
	s16 x,y,z,w;  
}; 

class Quat16ToMinusQuat32 {
public:
	 static const quaternion conv(const PACK_QUATERNION& t)
	{
		return quaternion(
			(f32)(t.x < 0? t.x + 32768 : t.x - 32767)/ 32767.0f, 
			(f32)(t.z < 0? t.z + 32768 : t.z - 32767)/ 32767.0f,
			(f32)(t.y < 0? t.y + 32768 : t.y - 32767)/ 32767.0f,	
			-(f32)(t.w < 0? t.w + 32768 : t.w - 32767)/ 32767.0f);
	}
};

//对应一个animation block，animation block包括多个track，即包含多个 time&value 的组合
template <class T, class D=T, class Conv=Identity<T> >
class SWowAnimation						
{
public:
	SWowAnimation() 
		: Animations(NULL_PTR), NumAnimations(0),
		Type(INTERPOLATION_NONE), Seq(-1), 
		GlobalSeq(NULL_PTR), NumGlobalSeq(0) { }

	 void init(const M2::animblock* block, const u8* fileData, s32* globalSeq, u32 numGlobalSeq);
	 void init(const M2::animblock* block, const u8* m2FileData, SAnimFile* animFiles, s32* globalSeq, u32 numGlobalSeq);

	~SWowAnimation()
	{
		delete[] Animations;
	}

public:
	 s32		getValue(u32 anim, u32 time, T& v, s32 hint=0) const;			//第几个动画在某时间的插值
	 u32		getNumAnims() const { return NumAnimations; }
	 bool		hasAnimation(u32 anim) const { return anim < NumAnimations; }
	 u32		getGlobalSeq(u32 idx) const;

	s16		Type;	
private:
	struct	SAnimationEntry				//单个animation
	{
		SAnimationEntry() : numKeys(0), times(NULL_PTR), values(NULL_PTR), values1(NULL_PTR), values2(NULL_PTR) { }
		~SAnimationEntry() 
		{
			delete times;
		}

		u32		numKeys;
		u32*		times;
		T*		values;
		T*		values1;					//for hermite interpolation
		T*		values2;
	};

	u32		NumAnimations;		
	u32		NumGlobalSeq;		
	s32		Seq;					//索引一个外部的时间长度，动画的时间值在这个时间长度内
	SAnimationEntry*		Animations;
	
	s32*		GlobalSeq;
};

template <class T, class D, class Conv>
u32 SWowAnimation<T, D, Conv>::getGlobalSeq( u32 idx ) const
{
	if (idx >= NumGlobalSeq)
		return 0;
	return GlobalSeq[idx];
}

//从m2文件中读取time, key数据, sequence
template <class T, class D, class Conv>
void SWowAnimation<T,D,Conv>::init( const M2::animblock* block, const u8* fileData, s32* globalSeq, u32 numGlobalSeq )
{
	Type = block->_Interpolation;
	Seq = block->_SequenceID;
	GlobalSeq = globalSeq;
	NumGlobalSeq = numGlobalSeq;

	ASSERT(block->_Ntimings == block->_Nvalues);

	NumAnimations = block->_Ntimings;
	if (NumAnimations == 0)
		return;

	Animations = new SAnimationEntry[NumAnimations];

	//time
	for (u32 i=0; i<NumAnimations; ++i)
	{
		M2::sequence* p = (M2::sequence*)(fileData + block->_TimingsOfs + i*sizeof(M2::sequence));
		Animations[i].numKeys = p->_NValues;
		if (Animations[i].numKeys == 0)
			continue;

		u32 num = Animations[i].numKeys;

		M2::sequence* v = (M2::sequence*)(fileData + block->_ValuesOfs +i*sizeof(M2::sequence));

		u32* times = (u32*)(fileData + p->_SequencesOfs);	
		const D* values = reinterpret_cast<const D*>(fileData + v->_SequencesOfs);

		switch (Type)
		{
		case INTERPOLATION_NONE:
		case INTERPOLATION_LINEAR:
			{
				Animations[i].times = (u32*)new u8[sizeof(u32) * num + sizeof(T) * num];
				Animations[i].values = (T*)((u8*)Animations[i].times + sizeof(u32) * num);

				Q_memcpy(Animations[i].times, sizeof(u32)*num, times, sizeof(u32)*num);
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j]);
				}
			}
			break;
		case INTERPOLATION_HERMITE:
			{
				Animations[i].times = (u32*)new u8[sizeof(u32) * num + sizeof(T) * num + sizeof(T) * num + sizeof(T) * num];
				Animations[i].values = (T*)((u8*)Animations[i].times + sizeof(u32) * num);
				Animations[i].values1 = (T*)((u8*)Animations[i].times + sizeof(u32) * num + sizeof(T) * num);
				Animations[i].values2 = (T*)((u8*)Animations[i].times + sizeof(u32) * num + sizeof(T) * num + sizeof(T) * num);

				Q_memcpy(Animations[i].times, sizeof(u32)*num, times, sizeof(u32)*num);
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j*3]);
					Animations[i].values1[j] = Conv::conv(values[j*3+1]);
					Animations[i].values2[j] = Conv::conv(values[j*3+2]);
				}
			}
			break;
		default:
			ASSERT(false);
		}
	}

}

//从m2文件中读取time, key数据, sequence, 也从anim文件中读取动画数据，如果对应的anim文件存在，则使用动画文件
template <class T, class D, class Conv>
void SWowAnimation<T,D,Conv>::init( const M2::animblock* block, const u8* m2FileData, SAnimFile* animFiles, s32* globalSeq, u32 numGlobalSeq )
{
	Type = block->_Interpolation;
	Seq = block->_SequenceID;
	GlobalSeq = globalSeq;
	NumGlobalSeq = numGlobalSeq;

	ASSERT(block->_Ntimings == block->_Nvalues);

	NumAnimations = block->_Ntimings;
	if (NumAnimations == 0)									//骨骼无动画
		return;

	Animations = new SAnimationEntry[NumAnimations];

	//time
	for (u32 i=0; i<NumAnimations; ++i)
	{
		M2::sequence* p = (M2::sequence*)(m2FileData + block->_TimingsOfs + i*sizeof(M2::sequence));
		Animations[i].numKeys = p->_NValues;
		if (Animations[i].numKeys == 0)
			continue;

		u32 num = Animations[i].numKeys;

		u32* times;
		if (animFiles[i].size && (animFiles[i].size >= p->_SequencesOfs + sizeof(u32)*num) )
			times = (u32*)(animFiles[i].data + p->_SequencesOfs);
		else
			times = (u32*)(m2FileData + p->_SequencesOfs);

		M2::sequence* v = (M2::sequence*)(m2FileData + block->_ValuesOfs + i*sizeof(M2::sequence));

		const D* values;
		if (animFiles[i].size && (animFiles[i].size >= (v->_SequencesOfs + sizeof(D)*num)))
			values = reinterpret_cast<const D*>(animFiles[i].data + v->_SequencesOfs);
		else
			values = reinterpret_cast<const D*>(m2FileData + v->_SequencesOfs);	

		switch (Type)
		{
		case INTERPOLATION_NONE:
		case INTERPOLATION_LINEAR:
			{
				Animations[i].times = (u32*)new u8[sizeof(u32) * num + sizeof(T) * num];
				Animations[i].values = (T*)((u8*)Animations[i].times + sizeof(u32) * num);

				Q_memcpy(Animations[i].times, sizeof(u32)*num, times, sizeof(u32)*num);
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j]);
				}
			}
			break;
		case INTERPOLATION_HERMITE:
			{
				Animations[i].times = (u32*)new u8[sizeof(u32) * num + sizeof(T) * num + sizeof(T) * num + sizeof(T) * num];
				Animations[i].values = (T*)((u8*)Animations[i].times + sizeof(u32) * num);
				Animations[i].values1 = (T*)((u8*)Animations[i].times + sizeof(u32) * num + sizeof(T) * num);
				Animations[i].values2 = (T*)((u8*)Animations[i].times + sizeof(u32) * num + sizeof(T) * num + sizeof(T) * num);

				Q_memcpy(Animations[i].times, sizeof(u32)*num, times, sizeof(u32)*num);
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j*3]);
					Animations[i].values1[j] = Conv::conv(values[j*3+1]);
					Animations[i].values2[j] = Conv::conv(values[j*3+2]);
				}
			}
			break;
		default:
			ASSERT(false);
		}
	}
}

template <class T, class D, class Conv>
s32 SWowAnimation<T,D,Conv>::getValue( u32 anim, u32 time, T& v, s32 hint ) const
{
	if(anim >= NumAnimations)
		return -1;

	const SAnimationEntry& entry = Animations[anim];

	s32 pos = -1;
	if (entry.numKeys > 1)
	{
		//adjust time
		if (Seq > -1 && Seq < (s32)NumGlobalSeq)
		{
			if (GlobalSeq[Seq]==0)
				time = 0;
			else
				time = time % GlobalSeq[Seq];
		}

		if (time <= entry.times[0])			//小于最小帧
		{
 			v = entry.values[0];
 			return 0;
		}
		else if (time >= entry.times[entry.numKeys-1])		//大于最大帧
		{
 			v = entry.values[entry.numKeys-1];
 			return (s32)entry.numKeys-1;
		}

		u32 i = 1;
		if (hint >1 && time > entry.times[hint-1])					//加速
			i = hint;			
	
		for (; i<entry.numKeys; ++i )
		{
			if (time <= entry.times[i])
			{
				pos = i;
				break;
			}
		}
		
		if (pos != -1)
		{
			u32 t1 = entry.times[pos];
			u32 t2 = entry.times[pos-1];
			float r = (time-t2)/(float)(t1-t2);

			switch (Type)
			{
			case INTERPOLATION_NONE:
				v = entry.values[pos-1];
				break;
			case INTERPOLATION_LINEAR:
				v = interpolate<T>(r, entry.values[pos-1], entry.values[pos]);	
				break;
			case INTERPOLATION_HERMITE:
				v = interpolateHermite<T>(r, entry.values[pos-1], entry.values[pos], entry.values1[pos-1], entry.values2[pos-1]);
				break;
			default:
				ASSERT(false);
			}

		}
	}
	else if (entry.numKeys == 1)
	{
		v = entry.values[0];
		pos = 0;
	}

	return pos;
}


typedef SWowAnimation<float,short,ShortToFloat> SWowAnimationShort;
typedef SWowAnimation<vector3df,vector3df,Vec3ToVec3>	SWowAnimationVec3;
typedef SWowAnimation<quaternion,PACK_QUATERNION,Quat16ToMinusQuat32>	SWowAnimationQuat;