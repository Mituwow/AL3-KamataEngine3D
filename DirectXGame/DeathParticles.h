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

class DeathParticles {
public:
	DeathParticles();
	~DeathParticles();
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();

	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransform_;

private:
	Model* modelParticle_ = nullptr;
	ViewProjection* viewProjection_;
};
