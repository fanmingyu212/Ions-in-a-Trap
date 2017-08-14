#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Trap2D.h"
#include "Sr88Ion.h"
#include "IonInfo.h"
#include "Field2D.h"

using namespace std;

int main() {
	double d = 7.778e-3;
	double r = 2.5e-3;
	double v_rf = 56;
	double v_bias1 = 0;
	double v_bias2 = -0.3;
	double v_bias3 = 0;
	double v_bias4 = -0.3;
	double f = 1e6;
	double phase = 0;

	double time = 1e-2;
	double step = 1e-9;

	double v_rf_low = 1;
	double v_rf_high = 100;
	for (double v_rf_trial = v_rf_low; v_rf_trial < v_rf_high; v_rf_trial+=1) {
		Trap2D trap(d, r, v_rf_trial, v_bias1, v_bias2, v_bias3, v_bias4, f, phase);
		Sr88Ion ion;
		trap.add_ion(ion, d / 2 + 0.7e-3, d / 2 + 0.7e-3, 700, 0);
		bool success = trap.start_simulation(time, step);
		vector<IonInfo> results = trap.get_simulation_results();
		IonInfo last = results[results.size() - 1];
		if (success) {
			cout << v_rf_trial << " V: " << "trapped till " << last.time * 1000000 << " us.\n";
		}
		else {
			cout << v_rf_trial << " V: " << "lost at " << last.time * 1000000 << " us.\n";
		}
	}
	string a;
	cin >> a;

	/*ofstream out_file;
	out_file.open("C:\\Users\\Mingyu Fan\\Desktop\\data.txt", std::ios::out);
	out_file << scientific;
	for (int i = 0; i < results.size(); i++) {
		out_file << results[i].time << " " << results[i].position.x << " " << results[i].position.y << " " << results[i].velocity.x << " " << results[i].velocity.y << " " << results[i].acceleration.x << " " << results[i].acceleration.y << "\n";
	}
	out_file.close();*/

	return 0;
}