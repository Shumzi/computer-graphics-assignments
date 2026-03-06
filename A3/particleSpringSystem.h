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
	Vector3f getPosition(int particleIdx, const vector<Vector3f> &state);
	Vector3f getVelocity(int particleIdx, const vector<Vector3f> &state);	
	Vector3f getPosition(int particleIdx);
	Vector3f getVelocity(int particleIdx);
	Vector3f springForce(const Spring &s, const vector<Vector3f> &state);
	Vector3f springForce(const Spring &s);
	vector<Spring> springs;
	float drag = 0.5f;
	float g = 1.f;
	float particleMass = .05f; // kg
};

#endif
