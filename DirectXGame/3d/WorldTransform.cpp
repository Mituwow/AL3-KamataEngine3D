#include "WorldTransform.h"
#include "affin.h"

void WorldTransform::UpdateMatrix() {

	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	TransferMatrix();
}