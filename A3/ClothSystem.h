#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>

#include "pendulumSystem.h"
#include "Spring.h"

class ClothSystem: public ParticleSpringSystem
{
public:
	/**
	 * @brief make structural, shear and flex springs in the cloth system.
	 */
	void setupBasicSprings() override;
	/**
	 * @brief create plane of particles 
	 */
	ClothSystem(unsigned numParticlesPerSide);
	vector<Vector3f> evalF(vector<Vector3f> state) override;
private:
	int m_numParticlesPerSide;
};



#endif
