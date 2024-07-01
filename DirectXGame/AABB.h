#pragma once
#include "Vector3.h"

class aabb {
public:

	struct AABB {
		Vector3 min;
		Vector3 max;
	};
	static bool AABBCollision(const AABB& a, const AABB& b);
};