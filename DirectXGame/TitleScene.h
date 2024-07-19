#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class TitleScene {
public:
	void Initialize(Model* model);
	void Update();
	void Draw();

	// シーンの終了フラグ
	bool IsFinished() const { return finished_; };

private:
	Model* modelTitle_ = nullptr;
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	// シーンの終了フラグ
	bool finished_ = false;

};