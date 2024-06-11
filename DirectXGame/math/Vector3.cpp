#pragma once
#include "Vector3.h"


Vector3& Vector3::operator+=(const Vector3& rhs) {
	// TODO: return ステートメントをここに挿入します

	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return* this;
}

Vector3& Vector3::operator=(float a) {
	// TODO: return ステートメントをここに挿入します
	x = a;
	y = a;
	z = a;
	return* this;
}

Vector3& operator+(const Vector3& mValue1, const Vector3& mValue2) {
	// TODO: return ステートメントをここに挿入します
	Vector3 result = {};
	result.x = mValue1.x + mValue2.x;
	result.y = mValue1.y + mValue2.y;
	result.z = mValue1.z + mValue2.z;

	return result;
}

Vector3& operator*(const Vector3& mValue1, float a) { return Vector3(mValue1) = a; }