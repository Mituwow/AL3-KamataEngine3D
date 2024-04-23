#include "Player.h"
#include <cassert>

// 13.初期化
void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
}

// 14.更新
void Player::Update() { worldTransform_.TransferMatrix(); }

void Player::Draw() { 

	model_->Draw(worldTransform_, *(viewProjection_), textureHandle_);

}

