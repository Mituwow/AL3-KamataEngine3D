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
#include "MapChipField.h"
#include "Player.h"
#include "enemy.h"
#include "CameraController.h"
#include "DeathParticles.h"

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

	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;
	
	//表示ブロック
	void GenerateBlocks();

	//自機
	Player* player_ = nullptr;

	//パーティクル
	DeathParticles* deathParticles_ = nullptr;

	//敵	
	std::list<Enemy*> enemies_;
	static inline const uint32_t kEnemyNum = 3;

	//すべての当たり判定を行う
	void CheckAllCollision();

	//カメラコントローラー
	CameraController* cameraController_ = nullptr;

	//ゲームのフェーズ(型)
	enum class Phase {
		kPlay,
		kDeath,
	};
	Phase phase_;
	void ChangePhase();
	bool isDead() const { return isDead_; }


private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	//デスフラグ
	bool isDead_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
