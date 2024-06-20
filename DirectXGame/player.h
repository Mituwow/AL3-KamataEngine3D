#pragma once
#include "DebugCamera.h" //デバッグカメラ用
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector> //可変個配列用

class MapChipField;

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
	static inline const float kTimeTurn = 1.0f;
	static inline const float kGravityAcceralation = 0.1f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAccelaration = 1.5f;
	static inline const float kAttenuationLanding = 0.1f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.8f;

	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	bool onGround_ = true;

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};	
	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		// 要素数
		kNumCorner
	};

private:
	Model* modelPlayer_ = nullptr;
	Vector3 velocity_ = {};
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_;

	MapChipField* mapChipField_ = nullptr;

	void CollisionMap(CollisionMapInfo& info);
	void CollisionMapTop(CollisionMapInfo& info);
	void CollisionMapBottom(CollisionMapInfo& info);
	void CollisionMapLeft(CollisionMapInfo& info);
	void CollisionMapRight(CollisionMapInfo& info);

	void OnGroundSwitch(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);
};