#include "Skydome.h"

void Skydome::Initialize() {}

void Skydome::Update() {}

void Skydome::Draw() { model_->Draw(worldTransform_, viewProjection_); }
