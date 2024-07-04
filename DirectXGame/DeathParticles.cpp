#include "DeathParticles.h"
#include <algorithm>

DeathParticles::DeathParticles() {}

DeathParticles::~DeathParticles() {}

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	modelParticle_ = model;
	viewProjection_ = viewProjection;
	for (auto& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {

	if (isFinished_) {
		return;
	}
	//worldTransformの更新
	for (auto& worldTransform : worldTransform_) {

		// 拡縮行列の作成
		Matrix4x4 scaleMatrix = {0};
		scaleMatrix.m[0][0] = worldTransform.scale_.x;
		scaleMatrix.m[1][1] = worldTransform.scale_.y;
		scaleMatrix.m[2][2] = worldTransform.scale_.z;
		scaleMatrix.m[3][3] = 1;

		Matrix4x4 rotateXMatrix = {0};
		//X
		rotateXMatrix.m[0][0] = 1;
		rotateXMatrix.m[1][1] = std::cos(worldTransform.rotation_.x);
		rotateXMatrix.m[1][2] = std::sin(worldTransform.rotation_.x);
		rotateXMatrix.m[2][1] = -std::sin(worldTransform.rotation_.x);
		rotateXMatrix.m[2][2] = std::cos(worldTransform.rotation_.x);
		rotateXMatrix.m[3][3] = 1;
		//Y
		Matrix4x4 rotateYMatrix = {0};
		rotateYMatrix.m[0][0] = std::cos(worldTransform.rotation_.y);
		rotateYMatrix.m[0][2] = -std::sin(worldTransform.rotation_.y);
		rotateYMatrix.m[1][1] = 1;
		rotateYMatrix.m[2][0] = std::sin(worldTransform.rotation_.y);
		rotateYMatrix.m[2][2] = std::cos(worldTransform.rotation_.y);
		rotateYMatrix.m[3][3] = 1;
		// Z
		Matrix4x4 rotateZMatrix = {0};
		rotateZMatrix.m[0][0] = std::cos(worldTransform.rotation_.z);
		rotateZMatrix.m[0][1] = std::sin(worldTransform.rotation_.z);
		rotateZMatrix.m[1][0] = -std::sin(worldTransform.rotation_.z);
		rotateZMatrix.m[1][1] = std::cos(worldTransform.rotation_.z);
		rotateZMatrix.m[2][2] = 1;
		rotateZMatrix.m[3][3] = 1;
		// XYZ回転行列の合成
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		// 平行移動行列の作成
		Matrix4x4 translationMatrix = {0};
		translationMatrix.m[0][0] = 1;
		translationMatrix.m[1][1] = 1;
		translationMatrix.m[2][2] = 1;
		translationMatrix.m[3][0] = worldTransform.translation_.x;
		translationMatrix.m[3][1] = worldTransform.translation_.y;
		translationMatrix.m[3][2] = worldTransform.translation_.z;
		translationMatrix.m[3][3] = 1;
		// すべてを掛け合わせてワールド行列に代入
		worldTransform.matWorld_ = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translationMatrix));

		// 定数バッファに転送する
		worldTransform.TransferMatrix();
	}
	
	//パーティクルの更新
	for (int i = 0; i < kNumParticles; i++) {
		Vector3 velocity_ = {kSpeed,0,0};
		float angle = i * kAngleUnit;
		Matrix4x4 matrixRotation = {0};
		matrixRotation.m[0][0] = std::cos(angle);
		matrixRotation.m[0][1] = std::sin(angle);
		matrixRotation.m[1][0] = -std::sin(angle);
		matrixRotation.m[1][1] = std::cos(angle);
		matrixRotation.m[2][2] = 1;
		matrixRotation.m[3][3] = 1;

		velocity_ = Transform(velocity_,matrixRotation);
		worldTransform_[i].translation_+= velocity_;

	}
	//一定時間で消す
	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
	//フェードアウト時の色
	color_.w = std::clamp(1.0f - counter_,0.0f,1.0f);
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();
}

void DeathParticles::Draw() {
	if (isFinished_) {
		return;
	}
	for (auto& worldTransform : worldTransform_) {
		modelParticle_->Draw(worldTransform, *viewProjection_,&objectColor_);
	}
}

Matrix4x4 DeathParticles::Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result = {0};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return result;
}
Vector3 DeathParticles::Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}