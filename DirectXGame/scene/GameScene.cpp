#include "GameScene.h"
#include "Skydome.h"
#include "Player.h"
#include "TextureManager.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete blockModel_;
	delete debugCamera_;
	delete skydome_;
	delete mapChipField_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

Matrix4x4 GameScene::Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result = {0};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return result;
}
void GameScene::GenerateBlocks() {
	// ブロックのモデル
	blockModel_ = Model::Create();
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->kNumBlockVirtical;
	uint32_t numBlockHorizontal = mapChipField_->kNumBlockHorizontal;

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform(); 
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j,i);
			}
		}
	}}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロの初期化
	viewProjection_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 天球の生成
	skydome_ = new Skydome();
	skydome_->Initialize();
	skydome_->modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	//マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	//表示ブロックの生成
	GenerateBlocks();

	//自機
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2,10);
	player_ = new Player();
	player_->Initialize(player_->modelPlayer_,playerPosition);
	player_->modelPlayer_ = Model::CreateFromOBJ("player", true);
}

void GameScene::Update() {

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}
			// 拡縮行列の作成
			Matrix4x4 scaleMatrix = {0};
			scaleMatrix.m[0][0] = worldTransformBlock->scale_.x;
			scaleMatrix.m[1][1] = worldTransformBlock->scale_.y;
			scaleMatrix.m[2][2] = worldTransformBlock->scale_.z;
			scaleMatrix.m[3][3] = 1;

			// XYZ軸それぞれの回転行列の作成(これをヘッダーにまとめて呼びやすくするとなお良し)
			// X
			Matrix4x4 rotateXMatrix = {0};
			rotateXMatrix.m[0][0] = 1;
			rotateXMatrix.m[1][1] = std::cos(worldTransformBlock->rotation_.x);
			rotateXMatrix.m[1][2] = std::sin(worldTransformBlock->rotation_.x);
			rotateXMatrix.m[2][1] = -std::sin(worldTransformBlock->rotation_.x);
			rotateXMatrix.m[2][2] = std::cos(worldTransformBlock->rotation_.x);
			rotateXMatrix.m[3][3] = 1;
			// Y
			Matrix4x4 rotateYMatrix = {0};
			rotateYMatrix.m[0][0] = std::cos(worldTransformBlock->rotation_.y);
			rotateYMatrix.m[0][2] = -std::sin(worldTransformBlock->rotation_.y);
			rotateYMatrix.m[1][1] = 1;
			rotateYMatrix.m[2][0] = std::sin(worldTransformBlock->rotation_.y);
			rotateYMatrix.m[2][2] = std::cos(worldTransformBlock->rotation_.y);
			rotateYMatrix.m[3][3] = 1;
			// Z
			Matrix4x4 rotateZMatrix = {0};
			rotateZMatrix.m[0][0] = std::cos(worldTransformBlock->rotation_.z);
			rotateZMatrix.m[0][1] = std::sin(worldTransformBlock->rotation_.z);
			rotateZMatrix.m[1][0] = -std::sin(worldTransformBlock->rotation_.z);
			rotateZMatrix.m[1][1] = std::cos(worldTransformBlock->rotation_.z);
			rotateZMatrix.m[2][2] = 1;
			rotateZMatrix.m[3][3] = 1;
			// XYZ回転行列の合成
			Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

			// 平行移動行列の作成
			Matrix4x4 translationMatrix = {0};
			translationMatrix.m[0][0] = 1;
			translationMatrix.m[1][1] = 1;
			translationMatrix.m[2][2] = 1;
			translationMatrix.m[3][0] = worldTransformBlock->translation_.x;
			translationMatrix.m[3][1] = worldTransformBlock->translation_.y;
			translationMatrix.m[3][2] = worldTransformBlock->translation_.z;
			translationMatrix.m[3][3] = 1;
			// すべてを掛け合わせてワールド行列に代入
			worldTransformBlock->matWorld_ = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translationMatrix));

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}
	for (int i = 0; i < 1; i++) {

		// 拡縮行列の作成
		Matrix4x4 scaleMatrix = {0};
		scaleMatrix.m[0][0] = skydome_->worldTransform_.scale_.x;
		scaleMatrix.m[1][1] = skydome_->worldTransform_.scale_.y;
		scaleMatrix.m[2][2] = skydome_->worldTransform_.scale_.z;
		scaleMatrix.m[3][3] = 1;

		// XYZ軸それぞれの回転行列の作成(これをヘッダーにまとめて呼びやすくするとなお良し)
		// X
		Matrix4x4 rotateXMatrix = {0};
		rotateXMatrix.m[0][0] = 1;
		rotateXMatrix.m[1][1] = std::cos(skydome_->worldTransform_.rotation_.x);
		rotateXMatrix.m[1][2] = std::sin(skydome_->worldTransform_.rotation_.x);
		rotateXMatrix.m[2][1] = -std::sin(skydome_->worldTransform_.rotation_.x);
		rotateXMatrix.m[2][2] = std::cos(skydome_->worldTransform_.rotation_.x);
		rotateXMatrix.m[3][3] = 1;
		// Y
		Matrix4x4 rotateYMatrix = {0};
		rotateYMatrix.m[0][0] = std::cos(skydome_->worldTransform_.rotation_.y);
		rotateYMatrix.m[0][2] = -std::sin(skydome_->worldTransform_.rotation_.y);
		rotateYMatrix.m[1][1] = 1;
		rotateYMatrix.m[2][0] = std::sin(skydome_->worldTransform_.rotation_.y);
		rotateYMatrix.m[2][2] = std::cos(skydome_->worldTransform_.rotation_.y);
		rotateYMatrix.m[3][3] = 1;
		// Z
		Matrix4x4 rotateZMatrix = {0};
		rotateZMatrix.m[0][0] = std::cos(skydome_->worldTransform_.rotation_.z);
		rotateZMatrix.m[0][1] = std::sin(skydome_->worldTransform_.rotation_.z);
		rotateZMatrix.m[1][0] = -std::sin(skydome_->worldTransform_.rotation_.z);
		rotateZMatrix.m[1][1] = std::cos(skydome_->worldTransform_.rotation_.z);
		rotateZMatrix.m[2][2] = 1;
		rotateZMatrix.m[3][3] = 1;
		// XYZ回転行列の合成
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		// 平行移動行列の作成
		Matrix4x4 translationMatrix = {0};
		translationMatrix.m[0][0] = 1;
		translationMatrix.m[1][1] = 1;
		translationMatrix.m[2][2] = 1;
		translationMatrix.m[3][0] = skydome_->worldTransform_.translation_.x;
		translationMatrix.m[3][1] = skydome_->worldTransform_.translation_.y;
		translationMatrix.m[3][2] = skydome_->worldTransform_.translation_.z;
		translationMatrix.m[3][3] = 1;
		// すべてを掛け合わせてワールド行列に代入
		skydome_->worldTransform_.matWorld_ = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translationMatrix));

		// 定数バッファに転送する
		skydome_->worldTransform_.TransferMatrix();
	}

	skydome_->Update();

	// デバッグカメラのon/off
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ ^= true;
	}
	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	player_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	skydome_->Draw();
	player_->Draw();
	

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			blockModel_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
