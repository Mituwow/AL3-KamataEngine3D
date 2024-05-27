#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <Vector3.h>


enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
	
public:

	//block size
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	//block num
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

};