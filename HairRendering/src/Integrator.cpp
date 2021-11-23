#include "Integrator.h"

double Integrator::RK4Integration(std::function<double(double, double)> rate, double theta, double omega, double timeStep)
{
	double k1 = rate(theta, omega);
	double k2 = rate(theta + 0.5 * timeStep * k1, omega + 0.5 * timeStep * k1);
	double k3 = rate(theta + 0.5 * timeStep * k2, omega + 0.5 * timeStep * k2);
	double k4 = rate(theta + timeStep * k3, omega + timeStep * k3);

	return timeStep / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}
