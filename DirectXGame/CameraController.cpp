#define NOMINMAX
#include "CameraController.h"
#include "player.h"
#include <algorithm>

void CameraController::Initialize() {
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	movableArea_ = {kMovableLimitStartX, kMovableLimitEndX, kMovableLimitStartY, kMovableLimitEndY};
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	cameraEndPoint.x = targetWorldTransform.translation_.x + targetOffset_.x;
	cameraEndPoint.y = targetWorldTransform.translation_.y + targetOffset_.y;
	cameraEndPoint.z = targetWorldTransform.translation_.z + targetOffset_.z;
	viewProjection_.translation_ = Lerp(viewProjection_.translation_,cameraEndPoint,kInterpolationRate);

	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movableArea_.left, movableArea_.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movableArea_.bottom, movableArea_.top);

	viewProjection_.UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
