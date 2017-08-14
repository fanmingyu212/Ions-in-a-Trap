#pragma once
#include "Field2D.h"

struct IonInfo {
	double time;
	Field2D position;
	Field2D velocity;
	Field2D acceleration;
};