#pragma once
#include "PointCharge.h"

class Sr88Ion : public PointCharge {
public:
	Sr88Ion::Sr88Ion() {
		m = 1.4549642e-25;
		q = 1.60217662e-19;
	}
};