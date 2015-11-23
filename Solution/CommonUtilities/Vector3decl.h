#pragma once


namespace CU
{

	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T aX, T aY, T aZ);
		~Vector3();


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
	};
}