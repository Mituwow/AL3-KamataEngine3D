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

	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix();
	bool landing = false;

	Input* input = Input::GetInstance();

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	// 接地状態の時
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
		bool isLeftKey = input->PushKey(DIK_LEFT);
		bool isRightKey = input->PushKey(DIK_RIGHT);

		// 移動入力
		if (isLeftKey || isRightKey) {
			Vector3 acceralation = {};
			if (isRightKey) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceralation.x += kAcceralation;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
				}
			} else if (isLeftKey) {
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

			// 衝突情報を初期化
			CollisionMapInfo collisionMapInfo;
			// 移動量に速度の情報をコピー
			collisionMapInfo.move = velocity_;
			// マップ衝突チェック
			CollisionMap(collisionMapInfo);

		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		// ジャンプ入力
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

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	CollisionMap(collisionMapInfo);

	// 旋回制御
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

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	velocity_ += Vector3(0.0f, 1.0f, 0.0f);
}

void Player::CollisionMap(CollisionMapInfo& info) {
	// 上
	CollisionMapTop(info);
	// 下
	CollisionMapBottom(info);
	// 左
	CollisionMapLeft(info);
	// 右
	CollisionMapRight(info);
	// 接地している場合
	OnGroundSwitch(info);
}

aabb::AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	aabb::AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::CollisionMapTop(CollisionMapInfo& info) {
	// 上昇していなければ早期リターン
	if (info.move.y <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::IndexSet indexSet;
	// 真上の当たり判定
	bool hit = false;
	// 左上の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右上も同様に
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[int(Corner::kRightTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// hit時
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveY = worldTransform_.translation_.y - rect.top;

		info.move.y = std::max(0.0f, moveY);
		info.ceiling = true;
	}
	// 天井に当たった?
	if (info.ceiling) {
		worldTransform_.translation_ += info.move;
		velocity_.y = 0;
	}
}
void Player::CollisionMapBottom(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType;
	MapChipField::IndexSet indexSet;
	// 真下の当たり判定
	bool hit = false;
	// 左下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// hit時
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveY = rect.top - worldTransform_.translation_.y;

		info.move.y = std::min(0.0f, moveY);
		info.landing = true;
	}
	if (info.landing) {
		worldTransform_.translation_ -= info.move;
		velocity_.y = 0;
	}
}
void Player::CollisionMapLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType;
	MapChipField::IndexSet indexSet;
	// 真下の当たり判定
	bool hit = false;
	// 左下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// hit時
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveX = worldTransform_.translation_.x + rect.left;

		info.move.x = std::min(0.0f, moveX);
		info.hitWall = true;
	}
	if (info.hitWall) {
		worldTransform_.translation_ -= info.move;
		velocity_.x = 0;
	}
}
void Player::CollisionMapRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipField::MapChipType mapChipType;
	MapChipField::IndexSet indexSet;
	// 真下の当たり判定
	bool hit = false;
	// 左下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// hit時
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveX = worldTransform_.translation_.x - rect.right;

		info.move.x = std::max(0.0f, moveX);
		info.hitWall = true;
	}
	if (info.hitWall) {
		worldTransform_.translation_ += info.move;
		velocity_.x = 0;
	}
}

void Player::OnGroundSwitch(CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			//  移動後の４つの角の座標
			std::array<Vector3, static_cast<uint32_t>(Corner::kNumCorner)> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}
			MapChipField::MapChipType mapChipType;
			MapChipField::IndexSet indexSet;
			bool hit = false;
			// 左下の当たり判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kBlank, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipField::MapChipType::kBlock) {
				hit = true;
			}
			// 右下の当たり判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kBlank, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipField::MapChipType::kBlock) {
				hit = true;
			}
			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0}, //  右下
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  左下
	    {kWidth / 2.0f,  kHeight / 2.0f,  0}, //  右上
	    {-kWidth / 2.0f, kHeight / 2.0f,  0}  //  左上
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
