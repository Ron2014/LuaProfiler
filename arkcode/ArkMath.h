#ifndef	__ArkMath_H__
#define	__ArkMath_H__
#include "ArkBaseTypes.h"
#include <math.h>
template<class T> struct TPos;
typedef TPos<int> CPos;
typedef TPos<uint16> CPos16;
typedef TPos< float> CPosf;

template<class T>
struct TPos
{
	T x, y;

	TPos() :x(), y(){}
	TPos(T px, T py){ x = px; y = py; }

	void Init(){ x = 0; y = 0; }
	void Init(T px, T py){ x = px; y = py; }

	void SetX(T px)
	{
		x = px;
	}
	void SetY(T py)
	{
		y = py;
	}

	//这里用float了，因为只需要float
	float Dist(const TPos<T>& b)const
	{
		T xDiff = x - b.x;
		T yDiff = y - b.y;
		return sqrt(float(xDiff*xDiff + yDiff*yDiff));
	}

	
	T Mag() const
	{
		return (T)sqrtf(float(x*x) + float(y*y));
	}

	// 矢量长度的平方
	T MagSqr() const
	{
		return x*x + y*y;
	}

	T Dot(const TPos<T>& b) const
	{
		return x*b.x + y*b.y;
	}

	bool operator == (const TPos<T>& b) const
	{
		return (b.x == x && b.y == y);
	}

	bool operator != (const TPos<T>& b) const
	{
		return (b.x != x || b.y != y);
	}

	const TPos<T> operator + (const TPos<T>& b) const
	{
		return TPos<T>(x + b.x, y + b.y);
	}

	const TPos<T> operator - (const TPos<T>& b) const
	{
		return TPos<T>(x - b.x, y - b.y);
	}

	T operator * (const TPos<T>& b) const
	{
		return Dot(b);
	}

	const TPos<T> operator * (const T s) const
	{
		return TPos<T>(x*s, y*s);
	}


	const TPos<T> operator / (const T s) const
	{
		return TPos<T>(x / s, y / s);
	}

	const TPos<T>& operator = (const TPos<T>& b)
	{
		x = b.x;
		y = b.y;

		return *this;
	}

	const TPos<T>& operator += (const TPos<T>& b)
	{
		x += b.x;
		y += b.y;

		return *this;
	}

	const TPos<T>& operator -= (const TPos<T>& b)
	{
		x -= b.x;
		y -= b.y;

		return *this;
	}

	const TPos<T>& operator *= (const T s)
	{
		x *= s;
		y *= s;

		return *this;
	}

	const TPos<T>& operator /= (const T s)
	{
		x /= s;
		y /= s;

		return *this;
	}
};






#define PosZerof  CPosf(0.0f, 0.0f)

#endif