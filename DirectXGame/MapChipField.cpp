#include "MapChipField.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace {

std::map<std::string, MapChipField::MapChipType> mapChpTable = {
    {"0", MapChipField::MapChipType::kBlank},
    {"1", MapChipField::MapChipType::kBlock},
};

}

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	// reset mapchip data
	ResetMapChipData();
	// open file
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	// mapchip csv
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	// ayashii.siryou ni nai
	std::string line;

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {

		getline(mapChipCsv, line);
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

			std::string word;
			getline(line_stream, word, ',');
			if (mapChpTable.contains(word)) {
				mapChipData_.data[i][j] = mapChpTable[word];
			}
		}
	}
}

MapChipField::MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {

	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}

	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); }

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);

	indexSet.yIndex = kNumBlockVirtical - 1 - indexSet.yIndex;

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.top = center.y + kBlockHeight / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;

	return rect;
}
