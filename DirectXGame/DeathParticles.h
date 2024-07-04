#pragma once
#include "AABB.h"
#include "DebugCamera.h" //デバッグカメラ用
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector> //可変個配列用
#include <array>
#include <numbers>

class GameScene;

class DeathParticles {
public:
	DeathParticles();
	~DeathParticles();
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();

	Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);
	Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);


	static inline const uint32_t kNumParticles = 6;
	static inline const float kDuration = 1.0f;
	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = std::numbers::pi_v<float> / float(kNumParticles);

	std::array<WorldTransform, kNumParticles> worldTransform_;

private:
	Model* modelParticle_ = nullptr;
	ViewProjection* viewProjection_;

	bool isFinished_ = false;
	float counter_ = 0.0f;
	ObjectColor objectColor_;
	Vector4 color_;
};
