#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3& operator+=(const Vector3& rhs);

	Vector3& operator-=(const Vector3& rhs);

	Vector3& operator=(float a);
};

Vector3& operator+(const Vector3& mValue1, const Vector3& mValue2);

Vector3& operator-(const Vector3& mValue1, const Vector3& mValue2);

Vector3& operator*(const Vector3& mValue1, float a);