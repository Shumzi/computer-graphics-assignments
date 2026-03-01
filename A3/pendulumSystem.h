#pragma once

#include "particleSpringSystem.h"

class PendulumSystem: public ParticleSpringSystem
{
public:
	/**
	 * @brief create particles in a line
	 */
	PendulumSystem(int numParticles);
	PendulumSystem();
	void setupBasicSprings() override;
	vector<Vector3f> evalF(vector<Vector3f> state) override;
};

