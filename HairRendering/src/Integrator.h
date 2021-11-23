#pragma once
#include <functional>

class Integrator
{
public:
	Integrator() = default;
	~Integrator() = default;

	static double RK4Integration(std::function<double(double, double)> rate, double theta, double omega, double timeStep);
};