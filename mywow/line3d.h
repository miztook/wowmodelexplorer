#pragma once

template <class T>
class line3d
{
public:
	//
	line3d(){}
	line3d(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {}
	line3d(const vector3d<T>& start, const vector3d<T>& end) : start(start), end(end) {}

	//
	inline line3d<T> operator+(const vector3d<T>& point) const { return line3d<T>(start + point, end + point); }
	inline line3d<T>& operator+=(const vector3d<T>& point) { start += point; end += point; return *this; }

	inline line3d<T> operator-(const vector3d<T>& point) const { return line3d<T>(start - point, end - point); }
	inline line3d<T>& operator-=(const vector3d<T>& point) { start -= point; end -= point; return *this; }

	inline bool operator==(const line3d<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
	inline bool operator!=(const line3d<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

	//
	inline void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb) {start.set(xa, ya, za); end.set(xb, yb, zb);}
	inline void setLine(const vector3d<T>& nstart, const vector3d<T>& nend) {start.set(nstart); end.set(nend);}
	inline void setLine(const line3d<T>& line) {start.set(line.start); end.set(line.end);}

	T getLength() const { return start.getDistanceFrom(end); }
	T getLengthSQ() const { return start.getDistanceFromSQ(end); }
	inline vector3d<T> getMiddle() const { return (start + end) * (T)0.5; }
	inline vector3d<T> getVector() const { return end - start; }

public:
	vector3d<T> start;
	vector3d<T> end;

};

typedef line3d<f32> line3df;
typedef line3d<s32> line3di;
