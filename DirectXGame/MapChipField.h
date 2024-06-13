#pragma once
#include <Vector3.h>
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:
	// block size
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	// block num
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};
	IndexSet index{0, 0};

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	struct Rect {
		float top;
		float bottom;
		float left;
		float right;
	};
	Rect GetRectByIndex(uint32_t xIndex,uint32_t yIndex);
};