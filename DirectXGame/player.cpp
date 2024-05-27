#define NOMINMAX
#include "player.h"
#include "MapChipField.h"
#include <algorithm>
#include <numbers>

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model, const Vector3& position) {

	model = Model::Create();
	worldTransform_.Initialize();
	viewProjection_.Initialize();

	worldTransform_.translation_ = position;
	// worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Draw() { modelPlayer_->Draw(worldTransform_, viewProjection_); }

void Player::Update() {
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	worldTransform_.UpdateMatrix();

	if (onGround_) {

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
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
	} else {
		velocity_.y += kGravityAcceralation;
		velocity_.y = std::max(velocity_.y,- kLimitFallSpeed);
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
