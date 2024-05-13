#include "Skydome.h"

void Skydome::Initialize() {
	worldTransform_.Initialize();
	viewProjection_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw() { modelSkydome_->Draw(worldTransform_, viewProjection_); }
