#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector> //可変個配列用
#include "DebugCamera.h"//デバッグカメラ用
#include "Skydome.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ブロックのモデル
	Model* blockModel_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 行列の掛け算
	Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//天球
	Skydome* skydome_ = nullptr;
	

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
