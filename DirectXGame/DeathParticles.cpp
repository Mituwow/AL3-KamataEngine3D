#include "DeathParticles.h"

DeathParticles::DeathParticles() {}

DeathParticles::~DeathParticles() {}

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	modelParticle_ = model;
	viewProjection_ = viewProjection;
	for (auto& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {
	for (int i = 0; i < kNumParticles; i++) {
		Vector3 velocity_ = {};
		//float angle = i * kAngleUnit;

	}
}

void DeathParticles::Draw() {

}
