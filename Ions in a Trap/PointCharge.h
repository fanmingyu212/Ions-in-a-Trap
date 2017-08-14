#pragma once

class PointCharge {
public:
	double m, q;
	PointCharge::PointCharge() {}
	PointCharge::PointCharge(double m, double q) {
		this->m = m;
		this->q = q;
	}
};