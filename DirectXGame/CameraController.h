#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

class Player;

class CameraController {

public:
	void Initialize();
	void Update();
	void Reset();

	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 7, -25.0f};
	Vector3 cameraEndPoint;
	static inline const float kInterpolationRate = 0.2f;

	void SetTarget(Player* target) { target_ = target; }
	ViewProjection& GetViewProjection() { return viewProjection_; }
	Vector3 Lerp(Vector3 start, Vector3 end, float t) {
		Vector3 result;
		result.x = (1 - t) * start.x + t * end.x;
		result.y = (1 - t) * start.y + t * end.y;
		result.z = (1 - t) * start.z + t * end.z;

		return result;
	}

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	Rect movableArea_ = {5, 100, 5, 100};
	void SetMovableArea(Rect area) { movableArea_ = area; }
	static inline const uint32_t kMovableLimitStartX = 17;
	static inline const uint32_t kMovableLimitEndX = 85;
	static inline const uint32_t kMovableLimitStartY = 10;
	static inline const uint32_t kMovableLimitEndY = 25;

private:
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;
};