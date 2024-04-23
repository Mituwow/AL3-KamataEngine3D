#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

//8.Playerのヘッダーとcppファイルおよびメンバ関数とメンバ変数の作成

class Player {	
public:

	void Initialize(Model* model, uint32_t textureHandle,ViewProjection* viewProjection);
	void Update();
	void Draw();

private:

	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	
	//15.メンバ変数を追加
	ViewProjection* viewProjection_ = nullptr;
};