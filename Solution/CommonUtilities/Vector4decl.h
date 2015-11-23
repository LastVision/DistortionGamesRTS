#pragma once

#include "Vector3.h"

namespace CU
{
	template<typename T>

	class Vector4
	{
	public:
		Vector4();
		Vector4(T aX, T aY, T aZ, T aW);
		Vector4(Vector3<T> aVector);
		~Vector4();

		union
		{
			T x;
			T myX;
			T r;
			T myR;
		};

		union
		{
			T y;
			T myY;
			T g;
			T myG;
		};

		union
		{
			T z;
			T myZ;
			T b;
			T myB;
		};

		union
		{
			T w;
			T myW;
			T a;
			T myA;
		};
	};
}