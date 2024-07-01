#pragma once
#include "DebugCamera.h" //デバッグカメラ用
#include "DirectXCommon.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <numbers>
#include <vector> //可変個配列用
#include "AABB.h"

class MapChipField;
class Player;

class Enemy {

public:
	Enemy();
	~Enemy();
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();

	static inline const float kWalkSpeed = 0.01f;
	static inline const float kWalkMotionAngleStart = -30.0f;
	static inline const float kWalkMotionAngleEnd = 60.0f;
	static inline const float kWalkMotionTime = 0;
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	Vector3 GetWorldPosition();
	aabb::AABB GetAABB(); 	
	void OnCollision(const Player* player);

private:
	Model* modelEnemy_ = nullptr;
	Vector3 velocity_ = {};
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_;

	MapChipField* mapChipField_ = nullptr;

	float walkTimer_ = 0.0f;
};