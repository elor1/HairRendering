#pragma once
#include <vector>
#include <glm.hpp>
#include "Integrator.h"
#include "Mesh.h"
#include <map>
#include <stdexcept>

class Hair;

class Simulation
{
public:
	Simulation(Mesh* mesh);
	virtual ~Simulation();

	void Update(float time);
	void Simulate(Hair* hair);

	std::vector<glm::vec3> mForces;
	glm::mat4 GetTransform();

	//Gui
	bool shake;
	bool nod;

	void ResetPosition();

private:
	float mTime;
	Mesh* mMesh;
	glm::mat4 mTransform;

	std::map<std::tuple<double, double, double>, double> mDensityGrid;
	std::map<std::tuple<double, double, double>, glm::vec3> mVelocityGrid;

	void Move(Hair* hair);
	void CalculateExternalForces(Hair* hair);
	void CalculateGrid(Hair* hair);
	void CalculateFriction(Hair* hair);

	void ParticleSimulation(Hair* hair);
	void AddToGrid(std::map<std::tuple<double, double, double>, double>& grid, std::tuple<double, double, double> key, double value);
	void AddToGrid(std::map<std::tuple<double, double, double>, glm::vec3>& grid, std::tuple<double, double, double> key, glm::vec3 value);
	//TODO: Do these need to pass grids as parameters?
	glm::vec3 GetGradient(std::map<std::tuple<double, double, double>, double>& grid, glm::vec3 point);
	glm::vec3 GetVelocity(std::map<std::tuple<double, double, double>, glm::vec3>& velocityGrid, std::map<std::tuple<double, double, double>, double>& densityGrid, glm::vec3 point);

	template <typename K, typename V>
	V GetGridValue(std::map<K, V>& grid, K key, V defaultValue);
};

template<typename K, typename V>
inline V Simulation::GetGridValue(std::map<K, V>& grid, K key, V defaultValue)
{
	V valueAtKey = defaultValue;
	try
	{
		valueAtKey = grid.at(key);
	}
	catch (const std::out_of_range) {}

	return valueAtKey;
}
