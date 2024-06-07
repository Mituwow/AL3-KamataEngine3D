#define NOMINMAX
#include "player.h"
#include "MapChipField.h"
#include <algorithm>
#include <numbers>

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	modelPlayer_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	worldTransform_.UpdateMatrix();

	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceralation = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceralation.x += kAcceralation;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceralation.x -= kAcceralation;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
				}
			}
			velocity_.x += acceralation.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAccelaration;
			onGround_ = false;
		}
	} else {
		velocity_.y -= kGravityAcceralation;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		if (landing) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
	float destinationRatationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	worldTransform_.rotation_.y = destinationRatationY;

	if (turnTimer_ > 0) {
		turnTimer_ -= 1.0f / 60;

		float timeRatio = 1 - turnTimer_ / kTimeTurn;
		float easing = timeRatio;
		float nowRatationY = std::lerp(turnFirstRotationY_, destinationRatationY, easing);
		worldTransform_.rotation_.y = nowRatationY;
	}
}

void Player::Draw() { modelPlayer_->Draw(worldTransform_, *viewProjection_); }
