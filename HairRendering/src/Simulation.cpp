#include "Simulation.h"
#include "Strand.h"
#include "Hair.h"

#define GRAVITY -29.8f
#define MASS 1.0f
#define DAMPENING 0.95f
#define TIMESTEP 0.01f

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
	ParticleSimulation(hair);
}

void Simulation::CalculateExternalForces(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		for (auto& vertex : guide->mVertices)
		{
			glm::vec3 force = glm::vec3(0.0f);

			//Gravity
			force += glm::vec3(0.0f, -9.8f, 0.0f);

			//Wind
			if (mTime > 2.0f)
			{
				force += glm::vec3(6.0f + 20.0f * ((rand() % 100) / 100.0f) - 10.0f, 0.0f, 0.0f);
			}
			
			vertex->forces = force;
		}
	}
}

void Simulation::CalculateConstraints(Hair* hair)
{
	
}

void Simulation::Integrate(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
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
	for (auto& guide : hair->mGuideHairs)
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

			float m1 = 0.4f;
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

void Simulation::Integrate3(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		for (int i = 3; i < numVertices; i++)
		{
			HairVertex* pivot = guide->mVertices[i - 3];
			HairVertex* vertex1 = guide->mVertices[i - 2];
			HairVertex* vertex2 = guide->mVertices[i - 1];
			HairVertex* vertex3 = guide->mVertices[i];

			glm::vec3 rodVector1 = vertex1->position - pivot->position;
			glm::vec3 rodVector2 = vertex2->position - vertex1->position;
			glm::vec3 rodVector3 = vertex3->position - vertex2->position;

			float length1 = glm::length(rodVector1);
			float length2 = glm::length(rodVector2);
			float length3 = glm::length(rodVector3);

			float theta1 = vertex1->theta;
			float theta2 = vertex2->theta;
			float theta3 = vertex3->theta;
			float omega1 = vertex1->omega;
			float omega2 = vertex2->omega;
			float omega3 = vertex3->omega;

			float m1 = 1.0f;
			float m2 = 1.0f;
			float m3 = 1.0f;

			//Angular acceleration
			auto omega1Dot = [length1, length2, length3, theta2, theta3, omega2, omega3, m1, m2, m3](double theta1, double omega1)
			{

				return (10.0 * GRAVITY * sin(theta1) +
						4.0 * GRAVITY * sin(theta1 - 2.0 * theta2) -
						GRAVITY * sin(theta1 + 2.0 * theta2 - 2.0 * theta3) -
						GRAVITY * sin(theta1 - 2.0 * theta2 + 2.0 * theta3) +
						4.0 * omega1 * omega1 * sin(2.0 * theta1 - 2.0 * theta2) +
						8.0 * omega2 * omega2 * sin(theta1 - theta2) +
						2.0 * omega3 * omega3 * sin(theta1 - theta3) +
						2.0 * omega3 * omega3 * sin(theta1 - 2.0 * theta2 + theta3)) /
					   (-10.0 + 4.0 * cos(2.0 * theta1 - 2.0 * theta2) + 2.0 * cos(2.0 * theta2 - 2.0 * theta3)) -
						DAMPENING * omega1 / (m1 * length1 * length1);
			};

			auto omega2Dot = [length1, length2, length3, theta1, theta3, omega1, omega3, m1, m2, m3](double theta2, double omega2)
			{
				return (-7.0 * GRAVITY * sin(2.0 * theta1 - theta2)
					+ 7.0 * GRAVITY * sin(theta2)
					+ GRAVITY * sin(theta2 - 2 * theta3)
					+ GRAVITY * sin(2 * theta1 + theta2 - 2 * theta3)
					+ 2.0 * omega1 * omega1 * sin(theta1 + theta2 - 2.0 * theta3)
					- 14.0 * omega1 * omega1 * sin(theta1 - theta2)
					+ 2.0 * omega2 * omega2 * sin(2.0 * theta2 - 2.0 * theta3)
					- 4.0 * omega2 * omega2 * sin(2.0 * theta1 - 2.0 * theta2)
					+ 6.0 * omega3 * omega3 * sin(theta2 - theta3)
					- 2.0 * omega3 * omega3 * sin(2.0 * theta1 - theta2 - theta3)
					) /
					(-10.0 + 4.0 * cos(2.0 * theta1 - 2.0 * theta2) + 2.0 * cos(2.0 * theta2 - 2.0 * theta3))
					- DAMPENING * omega2 / (m2 * length2 * length2);
			};

			auto omega3Dot = [length1, length2, length3, theta1, theta2, omega1, omega2, m1, m2, m3](double theta3, double omega3)
			{
				return -2.0 * sin(theta2 - theta3) *
					(GRAVITY * cos(2.0 * theta1 - theta2)
						+ GRAVITY * cos(theta2)
						+ 2.0 * omega1 * omega1 * cos(theta1 - theta2)
						+ 2.0 * omega2 * omega2
						+ omega3 * omega3 * cos(theta2 - theta3)
						) /
					(-5.0 + 2.0 * cos(2.0 * theta1 - 2.0 * theta2) + cos(2.0 * theta2 - 2.0 * theta3))
					- DAMPENING * omega3 / (m3 * length3 * length3);
			};

			//Angular velocity
			auto thetaDot = [](double theta, double omega)
			{

				return omega;
			};

			//Integrate
			vertex1->omega = omega1 + Integrator::RK4Integration(omega1Dot, theta1, omega1, TIMESTEP);
			vertex2->omega = omega2 + Integrator::RK4Integration(omega2Dot, theta2, omega2, TIMESTEP);
			vertex3->omega = omega3 + Integrator::RK4Integration(omega3Dot, theta3, omega3, TIMESTEP);
			vertex1->theta = theta1 + Integrator::RK4Integration(thetaDot, theta1, omega1, TIMESTEP);
			vertex2->theta = theta2 + Integrator::RK4Integration(thetaDot, theta2, omega2, TIMESTEP);
			vertex3->theta = theta3 + Integrator::RK4Integration(thetaDot, theta3, omega3, TIMESTEP);

			//Update positions
			vertex1->position.x = pivot->position.x + length1 * sin(vertex1->theta);
			vertex1->position.y = pivot->position.y + length1 * cos(vertex1->theta);
			vertex2->position.x = vertex1->position.x + length2 * sin(vertex2->theta);
			vertex2->position.y = vertex1->position.y + length2 * cos(vertex2->theta);
			vertex3->position.x = vertex2->position.x + length3 * sin(vertex3->theta);
			vertex3->position.y = vertex2->position.y + length3 * cos(vertex3->theta);
		}
	}
}

void Simulation::Integrate4(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		for (int i = 4; i < numVertices; i++)
		{
			HairVertex* pivot = guide->mVertices[i - 4];
			HairVertex* vertex1 = guide->mVertices[i - 3];
			HairVertex* vertex2 = guide->mVertices[i - 2];
			HairVertex* vertex3 = guide->mVertices[i - 1];
			HairVertex* vertex4 = guide->mVertices[i];

			glm::vec3 rodVector1 = vertex1->position - pivot->position;
			glm::vec3 rodVector2 = vertex2->position - vertex1->position;
			glm::vec3 rodVector3 = vertex3->position - vertex2->position;
			glm::vec3 rodVector4 = vertex4->position - vertex3->position;

			float length1 = glm::length(rodVector1);
			float length2 = glm::length(rodVector2);
			float length3 = glm::length(rodVector3);
			float length4 = glm::length(rodVector4);

			float theta1 = vertex1->theta;
			float theta2 = vertex2->theta;
			float theta3 = vertex3->theta;
			float theta4 = vertex4->theta;
			float omega1 = vertex1->omega;
			float omega2 = vertex2->omega;
			float omega3 = vertex3->omega;
			float omega4 = vertex4->omega;

			float m1 = 1.0f;
			float m2 = 1.0f;
			float m3 = 1.0f;
			float m4 = 1.0f;

			float inertia1 = m1 * length1 * length1;
			float inertia2 = m2 * length2 * length2;
			float inertia3 = m3 * length3 * length3;
			float inertia4 = m4 * length4 * length4;

			//Angular acceleration
			auto omega1Dot = [length1, length2, length3, length4, theta2, theta3, theta4, omega2, omega3, omega4, m1](double theta1, double omega1)
			{

				return (3 * (493 * GRAVITY * sin(theta1) - 2 * omega2 * omega2 * (
					-187. + 45 * cos(2 * (theta3 - theta4))) * sin(theta1 - theta2)
					+ 3 * omega2 * omega2 * (-9 * sin(theta1 + theta2 - 2 * theta3)
						+ sin(theta1 + theta2 - 2 * theta4))
					+ 3 * omega1 * omega1 * ((73. - 18 * cos(2 * (theta3 - theta4))) * sin(2 * (theta1 - theta2))
						- 9 * sin(2 * (theta1 - theta3)) + sin(2 * (theta1 - theta4)))
					+ omega4 * omega4 * (sin(theta1 - theta4)
						+ 27 * sin(theta1 - 2 * theta2 + 2 * theta3 - theta4)
						+ 6 * sin(theta1 - 2 * theta2 + theta4) + 18 * sin(theta1 - 2 * theta3 + theta4))
					+ 3 * omega3 * omega3 * (21 * sin(theta1 - theta3) + 36 * sin(theta1 - 2 * theta2 + theta3)
						- 2 * sin(theta1 + theta3 - 2 * theta4) - 3 * sin(theta1 - 2 * theta2 - theta3 + 2 * theta4))
					+ 3 * GRAVITY * (73 * sin(theta1 - 2 * theta2) - 9 * sin(theta1 - 2 * theta3)
						- 27 * sin(theta1 + 2 * theta2 - 2 * theta3) - 27 * sin(theta1 - 2 * theta2 + 2 * theta3)
						- 9 * sin(theta1 - 2 * (theta2 + theta3 - theta4)) + sin(theta1 - 2 * theta4)
						+ 3 * (sin(theta1 + 2 * theta2 - 2 * theta4) - 7 * sin(theta1 + 2 * theta3 - 2 * theta4)
							+ sin(theta1 - 2 * theta2 + 2 * theta4) - 7 * sin(theta1 - 2 * theta3 + 2 * theta4)
							- 3 * sin(theta1 - 2 * (theta2 - theta3 + theta4)))))) /
					(-1310. + 657 * cos(2 * (theta1 - theta2)) - 81 * cos(2 * (theta1 - theta3))
						+ 405 * cos(2 * (theta2 - theta3)) + 9 * cos(2 * (theta1 - theta4))
						- 45 * cos(2 * (theta2 - theta4)) + 333 * cos(2 * (theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 + theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 - theta3 + theta4)))
					- DAMPENING * omega1 / (m1 * length1 * length1);
			};

			auto omega2Dot = [length1, length2, length3, length4, theta1, theta3, theta4, omega1, omega3, omega4, m2](double theta2, double omega2)
			{
				return (-3 * (758 * omega1 * omega1 * sin(theta1 - theta2)
					- 18 * cos(2 * (theta3 - theta4)) * (11 * omega1 * omega1 * sin(theta1 - theta2)
						+ 3 * omega2 * omega2 * sin(2 * (theta1 - theta2))
						+ 6 * GRAVITY * sin(2 * theta1 - theta2) - 5 * GRAVITY * sin(theta2))
					+ 15 * omega1 * omega1 * (-9 * sin(theta1 + theta2 - 2 * theta3)
						+ sin(theta1 + theta2 - 2 * theta4)) + GRAVITY * (411 * sin(2 * theta1 - theta2)
							- 347 * sin(theta2) - 54 * sin(theta2 - 2 * theta3)
							- 81 * sin(2 * theta1 + theta2 - 2 * theta3) + 6 * sin(theta2 - 2 * theta4)
							+ 9 * sin(2 * theta1 + theta2 - 2 * theta4))
					+ 3 * omega3 * omega3 * (36 * sin(2 * theta1 - theta2 - theta3)
						- 3 * (37 * sin(theta2 - theta3) + sin(2 * theta1 - theta2 + theta3 - 2 * theta4))
						+ 8 * sin(theta2 + theta3 - 2 * theta4))
					+ 3 * omega2 * omega2 * (73 * sin(2 * (theta1 - theta2))
						+ 5 * (-9 * sin(2 * (theta2 - theta3)) + sin(2 * (theta2 - theta4))))
					+ omega4 * omega4 * (6 * sin(2 * theta1 - theta2 - theta4)
						- 31 * sin(theta2 - theta4) + 27 * sin(2 * theta1 - theta2 - 2 * theta3 + theta4)
						- 72 * sin(theta2 - 2 * theta3 + theta4)))) /
					(-1310. + 657 * cos(2 * (theta1 - theta2))
						- 81 * cos(2 * (theta1 - theta3)) + 405 * cos(2 * (theta2 - theta3))
						+ 9 * cos(2 * (theta1 - theta4)) - 45 * cos(2 * (theta2 - theta4))
						+ 333 * cos(2 * (theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 + theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 - theta3 + theta4)))
					- DAMPENING * omega2 / (m2 * length2 * length2);
			};

			auto omega3Dot = [length1, length2, length3, length4, theta1, theta2, theta4, omega1, omega2, omega4, m3](double theta3, double omega3)
			{
				return (3 * (3 * omega2 * omega2 * (18 * sin(2 * theta1 - theta2 - theta3)
					- 3 * (49 * sin(theta2 - theta3) + sin(2 * theta1 - theta2 + theta3 - 2 * theta4))
					+ 22 * sin(theta2 + theta3 - 2 * theta4))
					+ omega4 * omega4 * (9 * sin(2 * theta1 - theta3 - theta4)
						- 45 * sin(2 * theta2 - theta3 - theta4) + 14 * (
							+17. - 9 * cos(2 * (theta1 - theta2))) * sin(theta3 - theta4))
					+ 3 * omega3 * omega3 * (9 * sin(2 * (theta1 - theta3))
						- 45 * sin(2 * (theta2 - theta3)) + (
							+37. - 18 * cos(2 * (theta1 - theta2))) * sin(2 * (theta3 - theta4)))
					+ 3 * omega1 * omega1 * (-39 * sin(theta1 - theta3)
						+ 90 * sin(theta1 - 2 * theta2 + theta3) + 4 * sin(theta1 + theta3 - 2 * theta4)
						- 15 * sin(theta1 - 2 * theta2 - theta3 + 2 * theta4))
					+ 3 * GRAVITY * (-27 * sin(2 * theta1 - theta3) - 36 * sin(2 * theta2 - theta3)
						+ 12 * sin(theta3) + 54 * sin(2 * theta1 - 2 * theta2 + theta3)
						+ sin(theta3 - 2 * theta4) + 3 * sin(2 * theta1 + theta3 - 2 * theta4)
						+ 6 * sin(2 * theta2 + theta3 - 2 * theta4)
						- 9 * sin(2 * theta1 - 2 * theta2 - theta3 + 2 * theta4)))) /
					(-1310. + 657 * cos(2 * (theta1 - theta2))
						- 81 * cos(2 * (theta1 - theta3)) + 405 * cos(2 * (theta2 - theta3))
						+ 9 * cos(2 * (theta1 - theta4)) - 45 * cos(2 * (theta2 - theta4))
						+ 333 * cos(2 * (theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 + theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 - theta3 + theta4)))
					- DAMPENING * omega3 / (m3 * length3 * length3);
			};

			auto omega4Dot = [length1, length2, length3, length4, theta1, theta2, theta3, omega1, omega2, omega3, m4](double theta4, double omega4)
			{
				return (-3 * (omega3 * omega3 * (9 * sin(2 * theta1 - theta3 - theta4)
					- 45 * sin(2 * theta2 - theta3 - theta4) + 2 * (
						+251. - 117 * cos(2 * (theta1 - theta2))) * sin(theta3 - theta4))
					+ 3 * omega4 * omega4 * (sin(2 * (theta1 - theta4)) - 5 * sin(2 * (theta2 - theta4))
						+ (37. - 18 * cos(2 * (theta1 - theta2))) * sin(2 * (theta3 - theta4)))
					+ omega1 * omega1 * (sin(theta1 - theta4)
						+ 135 * sin(theta1 - 2 * theta2 + 2 * theta3 - theta4)
						- 60 * sin(theta1 - 2 * theta2 + theta4)
						- 36 * sin(theta1 - 2 * theta3 + theta4))
					+ omega2 * omega2 * (-12 * sin(2 * theta1 - theta2 - theta4)
						+ 73 * sin(theta2 - theta4) + 27 * sin(2 * theta1 - theta2 - 2 * theta3 + theta4)
						- 198 * sin(theta2 - 2 * theta3 + theta4)) + GRAVITY * (3 * sin(2 * theta1 - theta4)
							+ 24 * sin(2 * theta2 - theta4) + 9 * sin(2 * theta3 - theta4)
							+ 81 * sin(2 * (theta1 - theta2 + theta3) - theta4) + 2 * sin(theta4)
							- 9 * (4 * sin(2 * theta1 - 2 * theta2 + theta4) + 3 * sin(2 * theta1 - 2 * theta3 + theta4)
								+ 6 * sin(2 * theta2 - 2 * theta3 + theta4))))) /
					(-1310. + 657 * cos(2 * (theta1 - theta2))
						- 81 * cos(2 * (theta1 - theta3)) + 405 * cos(2 * (theta2 - theta3))
						+ 9 * cos(2 * (theta1 - theta4)) - 45 * cos(2 * (theta2 - theta4))
						+ 333 * cos(2 * (theta3 - theta4)) - 81 * cos(2 * (theta1 - theta2 + theta3 - theta4))
						- 81 * cos(2 * (theta1 - theta2 - theta3 + theta4)))
					- DAMPENING * omega4 / (m4 * length4 * length4);
			};

			//Angular velocity
			auto thetaDot = [](double theta, double omega)
			{
				return omega;
			};

			//Integrate
			vertex1->omega = omega1 + Integrator::RK4Integration(omega1Dot, theta1, omega1, TIMESTEP);
			vertex2->omega = omega2 + Integrator::RK4Integration(omega2Dot, theta2, omega2, TIMESTEP);
			vertex3->omega = omega3 + Integrator::RK4Integration(omega3Dot, theta3, omega3, TIMESTEP);
			vertex4->omega = omega4 + Integrator::RK4Integration(omega4Dot, theta4, omega4, TIMESTEP);
			vertex1->theta = theta1 + Integrator::RK4Integration(thetaDot, theta1, omega1, TIMESTEP);
			vertex2->theta = theta2 + Integrator::RK4Integration(thetaDot, theta2, omega2, TIMESTEP);
			vertex3->theta = theta3 + Integrator::RK4Integration(thetaDot, theta3, omega3, TIMESTEP);
			vertex4->theta = theta4 + Integrator::RK4Integration(thetaDot, theta4, omega4, TIMESTEP);

			//Update position
			vertex1->position.x = pivot->position.x + length1 * sin(vertex1->theta);
			vertex1->position.y = pivot->position.y + length1 * cos(vertex1->theta);
			vertex2->position.x = vertex1->position.x + length2 * sin(vertex2->theta);
			vertex2->position.y = vertex1->position.y + length2 * cos(vertex2->theta);
			vertex3->position.x = vertex2->position.x + length3 * sin(vertex3->theta);
			vertex3->position.y = vertex2->position.y + length3 * cos(vertex3->theta);
			vertex4->position.x = vertex3->position.x + length4 * sin(vertex4->theta);
			vertex4->position.y = vertex3->position.y + length4 * cos(vertex4->theta);
		}
	}
}

void Simulation::ParticleSimulation(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		guide->mVertices[0]->tempPosition = guide->mVertices[0]->position;

		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* vertex = guide->mVertices[i];

			vertex->velocity = vertex->velocity + TIMESTEP * (vertex->forces * (1.0f / vertex->mass));
			vertex->tempPosition += (vertex->velocity * TIMESTEP);
			vertex->forces = glm::vec3(0.0f);
			vertex->velocity *= 0.99f;
		}

		glm::vec3 direction;
		glm::vec3 currentPos;
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->mVertices[i - 1];
			HairVertex* current = guide->mVertices[i];
			currentPos = current->tempPosition;
			direction = glm::normalize(current->tempPosition - previous->tempPosition);
			current->tempPosition = previous->tempPosition + direction * previous->segmentLength;
			current->correction = currentPos - current->tempPosition;
		}

		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->mVertices[i - 1];
			HairVertex* current = guide->mVertices[i];
			previous->velocity = ((previous->tempPosition - previous->position) / TIMESTEP) + DAMPENING * (current->correction / TIMESTEP);
			previous->position = previous->tempPosition;
		}

		HairVertex* last = guide->mVertices.back();
		last->position = last->tempPosition;
	}
}

double Simulation::OmegaDot(double theta, double omega, float rodLength, float inertia)
{
	return (-GRAVITY / rodLength) * sin(theta) - 0.1f * omega / inertia;
}
