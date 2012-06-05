#pragma once

template <class T>
class line2d
{
public:
	//
	line2d(){}
	line2d(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {}
	line2d(const vector2d<T>& start, const vector2d<T>& end) : start(start), end(end) {}
	line2d(const line2d<T>& other) : start(other.start), end(other.end) {}

	//
	inline line2d<T> operator+(const vector2d<T>& point) const { return line2d<T>(start + point, end + point); }
	inline line2d<T>& operator+=(const vector2d<T>& point) { start += point; end += point; return *this; }

	inline line2d<T> operator-(const vector2d<T>& point) const { return line2d<T>(start - point, end - point); }
	inline line2d<T>& operator-=(const vector2d<T>& point) { start -= point; end -= point; return *this; }

	inline bool operator==(const line2d<T>& other) const
	{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
	inline bool operator!=(const line2d<T>& other) const
	{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

	//
	inline void setLine(const T& xa, const T& ya, const T& xb, const T& yb){start.set(xa, ya); end.set(xb, yb);}
	inline void setLine(const vector2d<T>& nstart, const vector2d<T>& nend){start.set(nstart); end.set(nend);}
	inline void setLine(const line2d<T>& line){start.set(line.start); end.set(line.end);}

	f32 getLength() const { return start.getDistanceFrom(end); }

	T getLengthSQ() const { return start.getDistanceFromSQ(end); }

	inline vector2d<T> getMiddle() const { return (start + end) * (T)0.5; }

	inline vector2d<T> getVector() const { return vector2d<T>(end.X - start.X, end.Y - start.Y); }

public:
	vector2d<T> start;
	vector2d<T> end;
};

typedef line2d<f32> line2df;
typedef line2d<s32> line2di;
