#include "Simulation.h"
#include "Strand.h"
#include "Hair.h"

#define GRAVITY -9.8f
#define MASS 1.0f
#define DAMPENING 0.3f
#define TIMESTEP 0.01

Simulation::Simulation()
{
	mTime = 0.0f;
}

Simulation::~Simulation()
{
}

void Simulation::Update(float time)
{
	mTime = time;
}

void Simulation::Simulate(Hair* hair)
{
	CalculateExternalForces(hair);
	CalculateConstraints(hair);
	Integrate2(hair);
}

void Simulation::CalculateExternalForces(Hair* hair)
{
	
}

void Simulation::CalculateConstraints(Hair* hair)
{
	
}

void Simulation::Integrate(Hair* hair)
{
	for (auto guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* vertex = guide->mVertices[i];
			HairVertex* pivot = guide->mVertices[i - 1];

			glm::vec3 rodVector = vertex->position - pivot->position;
			float rodLength = glm::length(rodVector);

			float inertia = MASS * rodLength * rodLength;
			float theta = vertex->theta;
			float omega = vertex->omega;

			bool useEuler = false;

			if (useEuler)
			{
				float thetaP2 = -GRAVITY / rodLength * sin(theta);

				float thetaP = vertex->omega + thetaP2 * TIMESTEP;
				vertex->omega = thetaP;
				theta += thetaP * TIMESTEP;
			}
			else
			{
				//Angular acceleration
				auto OmegaDot = [rodLength, omega, inertia](double theta, double omega)
				{
					return (-GRAVITY / rodLength) * sin(theta) - DAMPENING * omega / inertia;
				};

				//Angular velocity
				auto ThetaDot = [](double theta, double omega)
				{
					return omega;
				};

				omega += Integrator::RK4Integration(OmegaDot, theta, omega, TIMESTEP);
				theta += Integrator::RK4Integration(ThetaDot, theta, omega, TIMESTEP);
			}

			//Update positions
			vertex->theta = theta;
			vertex->position.x = pivot->position.x + rodLength * sin(theta);
			vertex->position.y = pivot->position.y + rodLength * cos(theta);
		}
	}
}

void Simulation::Integrate2(Hair* hair)
{
	for (auto guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		for (int i = 2; i < numVertices; i++)
		{
			HairVertex* pivot = guide->mVertices[i - 2];
			HairVertex* vertex1 = guide->mVertices[i - 1];
			HairVertex* vertex2 = guide->mVertices[i];

			//Vectors between vertices
			glm::vec3 rodVector1 = vertex1->position - pivot->position;
			glm::vec3 rodVector2 = vertex2->position - vertex1->position;

			float length1 = glm::length(rodVector1);
			float length2 = glm::length(rodVector2);

			float theta1 = vertex1->theta;
			float theta2 = vertex2->theta;
			float omega1 = vertex1->omega;
			float omega2 = vertex2->omega;

			float m1 = 0.5f;
			float m2 = 0.2f;

			//Angular acceleration
			auto omega1Dot = [length1, length2, theta2, omega2, m1, m2](double theta1, double omega1)
			{
				double deltaTheta = theta1 - theta2;
				double M = m2 / (m1 + m2);
				double l = length2 / length1;
				double wSquared = GRAVITY / length1;

				return (wSquared * l * (-sin(theta1) + M * cos(deltaTheta) * sin(theta2)) -
						M * l * (omega1 * omega1 * cos(deltaTheta) + l * omega2 * omega2) * sin(deltaTheta)) /
					   (l - M * l * cos(deltaTheta) * cos(deltaTheta)) - DAMPENING * omega1 / (m1 * length1 * length1);

			};

			auto omega2Dot = [length1, length2, theta1, omega1, m1, m2](double theta2, double omega2)
			{
				double deltaTheta = theta1 - theta2;
				double M = m2 / (m1 + m2);
				double l = length2 / length1;
				double wSquared = GRAVITY / length1;

				return (wSquared * cos(deltaTheta) * sin(theta1) - wSquared * sin(theta2) +
					   (omega1 * omega1 + M * l * omega2 * omega2 * cos(deltaTheta)) * sin(deltaTheta)) /
					   (l - M * l * cos(deltaTheta) * cos(deltaTheta)) - DAMPENING * omega2 / (m2 * length2 * length2);

			};

			//Angular velocity
			auto thetaDot = [](double theta, double omega) {

				return omega;

			};

			//Integrate
			vertex1->omega = omega1 + Integrator::RK4Integration(omega1Dot, theta1, omega1, TIMESTEP);
			vertex2->omega = omega2 + Integrator::RK4Integration(omega2Dot, theta2, omega2, TIMESTEP);
			vertex1->theta = theta1 + Integrator::RK4Integration(thetaDot, theta1, omega1, TIMESTEP);
			vertex2->theta = theta2 + Integrator::RK4Integration(thetaDot, theta2, omega2, TIMESTEP);

			//Update positions
			vertex1->position.x = pivot->position.x + length1 * sin(vertex1->theta);
			vertex1->position.y = pivot->position.y + length1 * cos(vertex1->theta);
			vertex2->position.x = vertex1->position.x + length2 * sin(vertex2->theta);
			vertex2->position.y = vertex1->position.y + length2 * cos(vertex2->theta);

		}
	}
}

double Simulation::OmegaDot(double theta, double omega, float rodLength, float inertia)
{
	return (-GRAVITY / rodLength) * sin(theta) - 0.1f * omega / inertia;
}
