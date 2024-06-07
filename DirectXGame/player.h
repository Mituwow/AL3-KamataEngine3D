#pragma once
#include "DebugCamera.h" //デバッグカメラ用
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector> //可変個配列用

class Player {

public:
	Player();
	~Player();
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();

	ViewProjection* GetViewProjection() { return viewProjection_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	static inline const float kAcceralation = 0.01f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 5.0f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceralation = 0.2f;
	static inline const float kLimitFallSpeed = 10.0f;
	static inline const float kJumpAccelaration = 2.0f;
	static inline const float kAttenuationLanding = 0.1f;

	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	bool onGround_ = true;

private:
	Model* modelPlayer_ = nullptr;
	Vector3 velocity_ = {};
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_;
};