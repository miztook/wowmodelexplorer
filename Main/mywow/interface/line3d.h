#pragma once

template <class T>
class line3d
{
public:
	//
	 line3d(){}
	 line3d(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {}
	 line3d(const vector3d<T>& start, const vector3d<T>& end) : start(start), end(end) {}
	 line3d(const line3d<T>& other) { (*this) = other; }

	 line3d& operator=(const line3d<T>& other)
	{
		ASSERT(this != &other);
		start = other.start;
		end = other.end;
		return *this;
	}

	//
	 line3d<T> operator+(const vector3d<T>& point) const { return line3d<T>(start + point, end + point); }
	 line3d<T>& operator+=(const vector3d<T>& point) { start += point; end += point; return *this; }

	 line3d<T> operator-(const vector3d<T>& point) const { return line3d<T>(start - point, end - point); }
	 line3d<T>& operator-=(const vector3d<T>& point) { start -= point; end -= point; return *this; }

	 bool operator==(const line3d<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
	 bool operator!=(const line3d<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

	 bool equals(const line3d& other) const { return start.equals(other.start) && end.equals(other.end); }

	//
	 void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb) {start.set(xa, ya, za); end.set(xb, yb, zb);}
	 void setLine(const vector3d<T>& nstart, const vector3d<T>& nend) {start.set(nstart); end.set(nend);}
	 void setLine(const line3d<T>& line) {start.set(line.start); end.set(line.end);}

	 T getLength() const { return start.getDistanceFrom(end); }
	 T getLengthSQ() const { return start.getDistanceFromSQ(end); }
	 vector3d<T> getMiddle() const { return (start + end) / (T)2; }
	 vector3d<T> getVector() const { return end - start; }

public:
	vector3d<T> start;
	vector3d<T> end;

};

typedef line3d<f32> line3df;
typedef line3d<s32> line3di;
