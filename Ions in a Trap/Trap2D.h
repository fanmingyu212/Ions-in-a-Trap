#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "PointCharge.h"
#include "Field2D.h"
#include "IonInfo.h"

using namespace std;

class Trap2D {
/*
q1, q2, q3, q4 are in clockwise order. v1 is the voltage of q2 - q1, v2 -> q3 - q2, and v3 -> q4 - q3.
q1 is at (0, 0), and q3 is at (d, d).
rf is applied on q1 and q3.
*/
private:
	double d, r;
	double d_diag;

	double v_rf, v_bias1, v_bias2, v_bias3, v_bias4;
	double f, omega, phase;

	const double E_CONST = 8.85418782e-12;
	double c_next, c_diag;
	double q1, q2, q3, q4;

	PointCharge ion;
	double q_to_m;
	double x, y, vx, vy;

	vector<IonInfo> simulation_results;

	void Trap2D::calculate_capacitance() {
		c_next = M_PI * E_CONST / log(d / (2.0 * r) + sqrt(pow(d, 2) / (4.0 * pow(r, 2)) - 1));
		c_diag = M_PI * E_CONST / log(d / (2.0 * r) + sqrt(pow(d, 2) / (4.0 * pow(r, 2)) - 1));
	}

	void Trap2D::calculate_charge(double v1, double v2, double v3) {
		q1 = c_next * v1 + c_diag * (v1 + v2) + c_next * (v1 + v2 + v3);
		q2 = c_next * v2 + c_diag * (v2 + v3) - c_next * v1;
		q3 = c_next * v3 - c_diag * (v1 + v2) - c_next * v2;
		q4 = -c_next * (v1 + v2 + v3) - c_diag * (v2 + v3) - c_next * v1;
	}

	inline Field2D Trap2D::e_field(double x, double y) {
		double x_sq = pow(x, 2);
		double y_sq = pow(y, 2);
		double x_inv_sq = pow((d - x), 2);
		double y_inv_sq = pow((d - y), 2);

		Field2D result;
		result.x = q1 / (x_sq + y_sq) * y
			+ q2 / (x_inv_sq + y_sq) * y
			+ q3 / (x_inv_sq + y_inv_sq) * (y - d)
			+ q4 / (x_sq + y_inv_sq) * (y - d);
		result.x /= E_CONST * 2 * M_PI;

		result.y = q1 / (x_sq + y_sq) * (-x)
			+ q2 / (x_inv_sq + y_sq) * (d - x)
			+ q3 / (x_inv_sq + y_inv_sq) * (d - x)
			+ q4 / (x_sq + y_inv_sq) * (-x);
		result.y /= E_CONST * 2 * M_PI;
		return result;
	}

	inline double Trap2D::v_rf_at_t(double t) {
		return v_rf * cos(omega * t + phase / 360 * 2 * M_PI);
	}

	inline double Trap2D::v1_at_t(double t) {
		return v_bias2 - (v_rf_at_t(t) + v_bias1);
	}

	inline double Trap2D::v2_at_t(double t) {
		return (v_bias3 + v_rf_at_t(t)) - v_bias2;
	}

	inline double Trap2D::v3_at_t(double t) {
		return v_bias4 - (v_rf_at_t(t) + v_bias3);
	}

	Field2D Trap2D::e_field_at_t(double x, double y, double t) {
		calculate_charge(v1_at_t(t), v2_at_t(t), v3_at_t(t));
		return e_field(x, y);
	}

	Field2D Trap2D::acceleration_at_t(double x, double y, double t) {
		Field2D field = e_field_at_t(x, y, t);
		field.x *= q_to_m;
		field.y *= q_to_m;
		return field;
	}

public:
	Trap2D::Trap2D(double d, double r, double v_rf, double v_bias1, double v_bias2, double v_bias3, double v_bias4, double f, double phase = 0) {
		this->d = d;
		this->d_diag = d * sqrt(2.0);
		this->r = r;
		calculate_capacitance();

		this->v_rf = v_rf;
		this->v_bias1 = v_bias1;
		this->v_bias2 = v_bias2;
		this->v_bias3 = v_bias3;
		this->v_bias4 = v_bias4;

		this->f = f;
		this->omega = 2 * M_PI * f;
		this->phase = phase;
	}

	void Trap2D::add_ion(PointCharge ion, double x, double y, double vx, double vy) {
		this->ion = ion;
		q_to_m = ion.q / ion.m;
		this->x = x;
		this->y = y;
		this->vx = vx;
		this->vy = vy;
	}

	bool Trap2D::start_simulation(double time, double step) {
		simulation_results.clear();

		double current_time = 0;
		Field2D current_acceleration, current_velocity, current_position;

		current_acceleration.x = 0;
		current_acceleration.y = 0;
		current_velocity.x = vx;
		current_velocity.y = vy;
		current_position.x = x;
		current_position.y = y;

		while (current_time < time) {
			current_acceleration = acceleration_at_t(current_position.x, current_position.y, current_time);
			current_velocity.x += current_acceleration.x * step;
			current_velocity.y += current_acceleration.y * step;
			current_position.x += current_velocity.x * step;
			current_position.y += current_velocity.y * step;

			IonInfo result;
			result.acceleration = current_acceleration;
			result.velocity = current_velocity;
			result.position = current_position;
			result.time = current_time;
			simulation_results.push_back(result);

			if (!(current_position.x < d - r && current_position.x > r && current_position.y < d - r && current_position.y > r)) {
				return false;
			}
			current_time += step;
		}
		return true;
	}

	vector<IonInfo> Trap2D::get_simulation_results() {
		return simulation_results;
	}
};