#pragma once

static const u16 g_aabboxLineIndex[] = { 5,1, 1,3, 3,7, 7,5, 
		0,2, 2,6, 6,4, 4,0, 
		1,0, 3,2, 7,6, 5,4 };

template <class T>
class aabbox3d
{
public:
	aabbox3d(){ MinEdge.clear(); MaxEdge.clear(); }	
	aabbox3d(const vector3d<T>& min, const vector3d<T>& max): MinEdge(min), MaxEdge(max) { }	
	aabbox3d(const vector3d<T>& init): MinEdge(init), MaxEdge(init) { }
	aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) { }

	public:		//operator
	inline bool operator==(const aabbox3d<T>& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);}
	inline bool operator!=(const aabbox3d<T>& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);}

	public:

	//intersection
	EIntersectionRelation3D classifyPlaneRelation(const plane3d<T>& plane) const;
			
	void set(const vector3d<T>& min, const vector3d<T>& max) { MinEdge=min; MaxEdge=max; }
	void reset( const vector3d<T>& v ) { MaxEdge = MinEdge = v; }
	void addInternalPoint( const vector3d<T>& p );
	void addInternalBox( const aabbox3d<T>& b );

	bool isZero() const { return MaxEdge == MinEdge; }

	vector3d<T> getCenter() const { return (MinEdge + MaxEdge)*0.5f; }
	vector3d<T> getExtent() const { return MaxEdge - MinEdge; }
	T getVolume() const { const vector3d<T> e = getExtent(); return e.X * e.Y * e.Z; }
	aabbox3d<T>	getInterpolated( const aabbox3d<T>& other, f32 d ) const;

	bool isPointInside(const vector3d<T>& p) const
	{
		return (p.X >= MinEdge.X && p.X <= MaxEdge.X &&
			p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
			p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
	}

	bool intersectsWithBox(const aabbox3d<T>& other) const
	{
		return (MinEdge.X <= other.MaxEdge.X && MinEdge.Y <= other.MaxEdge.Y && MinEdge.Z <= other.MaxEdge.Z &&
			MaxEdge.X >= other.MinEdge.X && MaxEdge.Y >= other.MinEdge.Y && MaxEdge.Z >= other.MinEdge.Z);
	}

	bool intersectsWithLine(const line3d<T>& line) const
	{
		return intersectsWithLine(line.getMiddle(), line.getVector().normalize(),
			(T)(line.getLength() * 0.5));
	}

	bool intersectsWithLine(const vector3d<T>& linemiddle,
		const vector3d<T>& linevect, T halflength) const;

	public:
	vector3d<T>	MinEdge;			//a
	vector3d<T>	MaxEdge;			//b
		
	void makePoints(vector3d<T>* points) const;
		
};

		template <class T>
		void aabbox3d<T>::makePoints(vector3d<T>* points) const
		{
			vector3d<T> center = getCenter();
			vector3d<T> ext = getExtent() / 2;

			/*
			  /3--------/7
			 / |           / |
			/  |         /  |
			1---------5  |
			|  /2- - -|- -6
			| /           |  /
			|/            | /
			0---------4/
			*/

			points[0] = center + vector3d<T>(-ext.X, -ext.Y, -ext.Z);
			points[1] = center + vector3d<T>(-ext.X, ext.Y, -ext.Z);
			points[2] = center + vector3d<T>(-ext.X, -ext.Y, ext.Z);
			points[3] = center + vector3d<T>(-ext.X, ext.Y, ext.Z);
			points[4] = center + vector3d<T>(ext.X, -ext.Y, -ext.Z);
			points[5] = center + vector3d<T>(ext.X, ext.Y, -ext.Z);
			points[6] = center + vector3d<T>(ext.X, -ext.Y, ext.Z);
			points[7] = center + vector3d<T>(ext.X, ext.Y, ext.Z);
		}

		template <class T>
		void aabbox3d<T>::addInternalBox( const aabbox3d<T>& b )
		{
			addInternalPoint(b.MaxEdge);
			addInternalPoint(b.MinEdge);
		}

		template <class T>
		void aabbox3d<T>::addInternalPoint( const vector3d<T>& p )
		{
			if (p.X >MaxEdge.X)	MaxEdge.X = p.X;
			if (p.Y >MaxEdge.Y)		MaxEdge.Y = p.Y;
			if (p.Z >MaxEdge.Z)		MaxEdge.Z = p.Z;

			if(p.X <MinEdge.X)		MinEdge.X = p.X;
			if(p.Y <MinEdge.Y)		MinEdge.Y = p.Y;
			if(p.Z <MinEdge.Z)		MinEdge.Z = p.Z;
		}

		template <class T>
		EIntersectionRelation3D aabbox3d<T>::classifyPlaneRelation( const plane3d<T>& plane ) const
		{	
			vector3d<T> nearPoint(MaxEdge);
			vector3d<T> farPoint(MinEdge);

			if (plane.Normal.X > (T)0)
			{
				nearPoint.X = MinEdge.X;
				farPoint.X = MaxEdge.X;
			}

			if (plane.Normal.Y > (T)0)
			{
				nearPoint.Y = MinEdge.Y;
				farPoint.Y = MaxEdge.Y;
			}

			if (plane.Normal.Z > (T)0)
			{
				nearPoint.Z = MinEdge.Z;
				farPoint.Z = MaxEdge.Z;
			}

			if (plane.Normal.dotProduct(nearPoint) + plane.D > (T)0)
				return ISREL3D_FRONT;

			if (plane.Normal.dotProduct(farPoint) + plane.D > (T)0)
				return ISREL3D_CLIPPED;

			return ISREL3D_BACK;
		}


		template <class T>
		aabbox3d<T> aabbox3d<T>::getInterpolated( const aabbox3d<T>& other, f32 d ) const
		{
			f32 inv = 1.0f - d;
			return aabbox3d<T>((other.MinEdge*inv) + (MinEdge*d),
				(other.MaxEdge*inv) + (MaxEdge*d));
		}

		template <class T>
		bool aabbox3d<T>::intersectsWithLine( const vector3d<T>& linemiddle, const vector3d<T>& linevect, T halflength ) const
		{
			const vector3d<T> e = getExtent() * (T)0.5;
			const vector3d<T> t = getCenter() - linemiddle;

			if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) ||
				(fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
				(fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)) )
				return false;

			T r = e.Y * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.Y);
			if (fabs(t.Y*linevect.Z - t.Z*linevect.Y) > r )
				return false;

			r = e.X * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.X);
			if (fabs(t.Z*linevect.X - t.X*linevect.Z) > r )
				return false;

			r = e.X * (T)fabs(linevect.Y) + e.Y * (T)fabs(linevect.X);
			if (fabs(t.X*linevect.Y - t.Y*linevect.X) > r)
				return false;

			return true;
		}

		typedef aabbox3d<f32> aabbox3df;
		typedef aabbox3d<s32> aabbox3di;

	
