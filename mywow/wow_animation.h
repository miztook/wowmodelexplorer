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
inline quaternion interpolate<quaternion>(const float r, const quaternion &v1, const quaternion &v2)
{
	return quaternion::slerp(v1, v2, r);
}

template<>
inline vector2df interpolate<vector2df>(const float r, const vector2df &v1, const vector2df &v2)
{
	return vector2df::interpolate(v1, v2, r);
}

template<>
inline SColorf interpolate<SColorf>(const float r, const SColorf &v1, const SColorf &v2)
{
	return SColorf::interpolate(v1, v2, r, true);
}

enum Interpolations {
	INTERPOLATION_NONE = 0,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE,

	INTERPOLATION_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
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

class Quat16ToQuat32 {
public:
	static const quaternion conv(const PACK_QUATERNION& t)
	{
		return quaternion(
			f32(t.x < 0? t.x + 32768 : t.x - 32767)/ 32767.0f, 
			f32(t.z < 0? t.z + 32768 : t.z - 32767)/ 32767.0f,
			f32(t.y < 0? t.y + 32768 : t.y - 32767)/ 32767.0f,	
			f32(t.w < 0? t.w + 32768 : t.w - 32767)/ 32767.0f);
	}
};

//对应一个animation block，animation block包括多个track，即包含多个 time&value 的组合
template <class T, class D=T, class Conv=Identity<T>>
class SWowAnimation						
{
public:
	SWowAnimation() : Animations(NULL), NumAnimations(0), Type(INTERPOLATION_NONE), Seq(-1), GlobalSeq(NULL){ }

	void init(const M2::animblock* block, const u8* fileData, s32* globalSeq, u32 numGlobalSeq);
	void init(const M2::animblock* block, const u8* m2FileData, SAnimFile* animFiles, s32* globalSeq, u32 numGlobalSeq);

	~SWowAnimation()
	{
		delete[] Animations;
	}

public:
	s32		getValue(u32 anim, u32 time, T& v, s32 hint=0);			//第几个动画在某时间的插值
	u32		getNumAnims() const { return NumAnimations; }
	bool		hasAnimation(u32 anim) const { return anim < NumAnimations; }

	s16		Type;	
private:
	struct	SAnimationEntry				//单个animation
	{
		SAnimationEntry() : numKeys(0), times(NULL), values(NULL), values1(NULL), values2(NULL) { }
		~SAnimationEntry() 
		{
			delete[] times;
			delete[] values;
			delete[] values1;
			delete[] values2;
		}

		u32		numKeys;
		u32*		times;
		T*		values;
		T*		values1;					//for hermite interpolation
		T*		values2;
	};

	SAnimationEntry*		Animations;
	u32		NumAnimations;				
	s32		Seq;					//索引一个外部的时间长度，动画的时间值在这个时间长度内
	s32*		GlobalSeq;
	u32		NumGlobalSeq;
};

//从m2文件中读取time, key数据, sequence
template <class T, class D, class Conv>
void SWowAnimation<T,D,Conv>::init( const M2::animblock* block, const u8* fileData, s32* globalSeq, u32 numGlobalSeq )
{
	Type = block->_Interpolation;
	Seq = block->_SequenceID;
	GlobalSeq = globalSeq;
	NumGlobalSeq = numGlobalSeq;

	_ASSERT(block->_Ntimings == block->_Nvalues);

	NumAnimations = block->_Ntimings;
	if (NumAnimations == 0)
		return;

	Animations = new SAnimationEntry[NumAnimations];

	u32 numValidKeys = 0;

	//time
	for (u32 i=0; i<block->_Ntimings; ++i)
	{
		M2::sequence* p = (M2::sequence*)(fileData + block->_TimingsOfs + i*sizeof(M2::sequence));
		
		numValidKeys = p->_NValues;

		u32* times = (u32*)(fileData + p->_SequencesOfs);	
		/*
		s32 last = 0;
		for (u32 k=0; k<p->_NValues; ++k)
		{
 			s32 t = (s32)times[k];
			if (t < last)						//invalid
			{
				numValidKeys = k;
				break;
			}
			last = t;
		}
		*/

		Animations[i].numKeys = numValidKeys;

		if (numValidKeys== 0)
				continue;

		Animations[i].times = new u32[numValidKeys];
		memcpy_s(Animations[i].times, sizeof(u32)*numValidKeys, times, sizeof(u32)*numValidKeys);
	}

	//value
	for (u32 i=0; i<block->_Nvalues; ++i)
	{
		M2::sequence* p = (M2::sequence*)(fileData + block->_ValuesOfs +i*sizeof(M2::sequence));
		if (p->_NValues < Animations[i].numKeys)						//没有足够的value
			Animations[i].numKeys = p->_NValues;
		if (Animations[i].numKeys == 0)
			continue;

		u32 num = Animations[i].numKeys;

		D* values = (D*)(fileData + p->_SequencesOfs);
		switch (Type)
		{
		case INTERPOLATION_NONE:
		case INTERPOLATION_LINEAR:
			{
				Animations[i].values = new T[num];
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j]);
				}
			}
			break;
		case INTERPOLATION_HERMITE:
			{
				Animations[i].values = new T[num];
				Animations[i].values1 = new T[num];
				Animations[i].values2 = new T[num];
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j*3]);
					Animations[i].values1[j] = Conv::conv(values[j*3+1]);
					Animations[i].values2[j] = Conv::conv(values[j*3+2]);
				}
			}
			break;
		default:
			_ASSERT(false);
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

	_ASSERT(block->_Ntimings == block->_Nvalues);

	NumAnimations = block->_Ntimings;
	if (NumAnimations == 0)									//骨骼无动画
		return;

	Animations = new SAnimationEntry[NumAnimations];

	u32 numValidKeys = 0;

	//time
	for (u32 i=0; i<block->_Ntimings; ++i)
	{
		M2::sequence* p = (M2::sequence*)(m2FileData + block->_TimingsOfs);

		u32* times;
		if (animFiles[i].size && (animFiles[i].size >= p[i]._SequencesOfs + sizeof(u32)*p->_NValues) )
			times = (u32*)(animFiles[i].data + p[i]._SequencesOfs);
		else
			times = (u32*)(m2FileData + p[i]._SequencesOfs);

		numValidKeys = p[i]._NValues;

		//check valid keys
		s32 last = 0;
		for (u32 k=0; k<p[i]._NValues; ++k)
		{
			s32 t = (s32)times[k];
			if (t < last)						//invalid
			{
				numValidKeys = k;
				break;
			}
			last = t;
		}

		Animations[i].numKeys = numValidKeys;

		if (numValidKeys== 0)
			continue;

		Animations[i].times = new u32[numValidKeys];
		memcpy_s(Animations[i].times, sizeof(u32)*numValidKeys, times, sizeof(u32)*numValidKeys);
	}

	//value
	for (u32 i=0; i<block->_Nvalues; ++i)
	{
		M2::sequence* p = (M2::sequence*)(m2FileData + block->_ValuesOfs);
		if (p[i]._NValues < Animations[i].numKeys)						//没有足够的value
			Animations[i].numKeys = p[i]._NValues;

		if (Animations[i].numKeys == 0)
			continue;

		u32 num = Animations[i].numKeys;

		D* values;
		if (animFiles[i].size && (animFiles[i].size >= (p[i]._SequencesOfs + sizeof(D)*num)))
			values = (D*)(animFiles[i].data + p[i]._SequencesOfs);
		else
			values = (D*)(m2FileData + p[i]._SequencesOfs);

		switch (Type)
		{
		case INTERPOLATION_NONE:
		case INTERPOLATION_LINEAR:
			{
				Animations[i].values = new T[num];
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j]);
				}
			}
			break;
		case INTERPOLATION_HERMITE:
			{
				Animations[i].values = new T[num];
				Animations[i].values1 = new T[num];
				Animations[i].values2 = new T[num];
				for (u32 j=0; j<num; ++j)
				{
					Animations[i].values[j] = Conv::conv(values[j*3]);
					Animations[i].values1[j] = Conv::conv(values[j*3+1]);
					Animations[i].values2[j] = Conv::conv(values[j*3+2]);
				}
			}
			break;
		default:
			_ASSERT(false);
		}
	}
}

template <class T, class D, class Conv>
s32 SWowAnimation<T,D,Conv>::getValue( u32 anim, u32 time, T& v, s32 hint )
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

		if (time >= entry.times[entry.numKeys-1])		//大于最大帧
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
				_ASSERT(false);
			}

		}
	}
	else
	if (entry.numKeys == 1)
	{
		v = entry.values[0];
		pos = 0;
	}

	return pos;
}


typedef SWowAnimation<float,short,ShortToFloat> SWowAnimationShort;
typedef SWowAnimation<vector3df,vector3df,Vec3ToVec3>	SWowAnimationVec3;
typedef SWowAnimation<quaternion,PACK_QUATERNION,Quat16ToQuat32>	SWowAnimationQuat;