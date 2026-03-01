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
	void draw();

protected:
	Vector3f getPosition(int particleIdx);
	Vector3f getVelocity(int particleIdx);
	Vector3f springForce(Spring s);
	vector<Spring> springs;
	float drag = 1;
	float g = 0.3; //
	float particleMass = 1.0f; // kg, assume constant mass?
};

#endif
