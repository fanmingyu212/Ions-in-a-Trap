#pragma once
#include "PointCharge.h"

class Electron : public PointCharge {
public:
	Electron::Electron() {
		m = 9.10938356e-31;
		q = -1.60217662e-19;
	}
};