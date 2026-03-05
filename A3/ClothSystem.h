#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>

#include "pendulumSystem.h"
#include "Spring.h"
struct Dir
{
	int dx, dy;
};

struct SpringRange
{
	int start, end;
};

class ClothSystem : public ParticleSpringSystem
{
public:
	using ParticleSpringSystem::getPosition;
	using ParticleSpringSystem::getVelocity;
	Vector3f getPosition(int i, int j);
	Vector3f getVelocity(int i, int j);
	/**
	 * @brief make structural, shear and flex springs in the cloth system.
	 */
	void setupBasicSprings() override;
	/**
	 * @brief create plane of particles
	 */
	ClothSystem(unsigned numParticlesPerSide);
	vector<Vector3f> evalF(vector<Vector3f> state) override;
	void draw() override;
	bool toggleStructure = true;
	bool toggleShear = false;
	bool toggleFlex = false;
	bool showWireframe = true;
	int m_numParticlesPerSide;

private:
	void addSpringsAroundParticle(vector<Dir> &SpringDirs, int i, int j);
	void addSpringForces(std::vector<Vector3f> &f, const SpringRange &sr);
	void drawLines(const SpringRange &sr);
	SpringRange structuralSpringsRange;
	SpringRange shearSpringsRange;
	SpringRange flexSpringsRange;
};

#endif
