#include "GameScene.h"
#include "ImGuiManager.h"    //デバッグテキストの表示用
#include "PrimitiveDrawer.h" //ライン描画用
#include "AxisIndicator.h"//軸方向表示用
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	textureHandle2_ = TextureManager::Load("kamata.ico");

	// スプライトの生成
	sprite_ = Sprite::Create(textureHandle2_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// サウンドデータの読み込み
	soundDataHandle_ = audio_->LoadWave("fanfare.wav");
	// 音声再生
	audio_->PlayWave(soundDataHandle_);
	// 音声再生
	voiceHandle_ = soundDataHandle_;

	// ライン描画が参照するビュープロジェクションを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	// デバッグカメラの生成(ヨコ幅タテ幅)
	debugCamera_ = new DebugCamera(16,16);

	//軸方向の表示を有効化
	AxisIndicator::GetInstance()->SetVisible(true);
	//参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

}

void GameScene::Update() {
	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を移動させる
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	// スペースキーを押した時
	if (input_->TriggerKey(DIK_SPACE)) {
		if (audio_->IsPlaying(soundDataHandle_)) {
		// 音声停止
		audio_->StopWave(soundDataHandle_);
		
		} else {
		 voiceHandle_ = audio_->PlayWave(soundDataHandle_);
		}
	}

#ifdef _DEBUG

	// デバッグテキストの表示
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::InputFloat3("InputFloat3", inputFloat3);
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	ImGui::End();
	ImGui::ShowDemoWindow();

#endif // DEBUG

	// デバッグカメラの更新
	debugCamera_->Update();
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

	// 3Dモデル描画
	// model_->Draw(worldTransform_, viewProjection_, textureHandle2_);

	// モデルを連動させる
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライトの描画
	sprite_->Draw();

	// ラインの描画(数値：始点座標xyz,終点座標xyz,色RGBA)
	for (float i = -5; i < 6; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d({-5, i, 0}, {5, i, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
		PrimitiveDrawer::GetInstance()->DrawLine3d({i, 5, 0}, {i, -5, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
	}
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
