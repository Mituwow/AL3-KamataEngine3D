#include "enemy.h"

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	modelEnemy_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	velocity_ = Vector3 (kWalkSpeed, 0, 0);
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	worldTransform_.translation_ -= velocity_;

	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	float param = std::sin(walkTimer_);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = std::numbers::pi_v<float> / 180.0f * (radian);

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { modelEnemy_->Draw(worldTransform_, *viewProjection_); }

