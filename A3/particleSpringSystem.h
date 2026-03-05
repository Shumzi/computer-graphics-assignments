#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "particleSystem.h"
#include "Spring.h"

class ParticleSpringSystem: public ParticleSystem
{
public:
	ParticleSpringSystem(int numParticles);
	ParticleSpringSystem();
	virtual void setupBasicSprings() = 0;
	virtual void draw();

protected:
	Vector3f getPosition(int particleIdx);
	Vector3f getVelocity(int particleIdx);
	Vector3f springForce(Spring s);
	vector<Spring> springs;
	float drag = 0.5f;
	float g = 1.0f;
	float particleMass = .1f; // kg, assume constant mass?
};

#endif
