
#include "particleSpringSystem.h"
#include <cmath>
#include <iostream>

ParticleSpringSystem::ParticleSpringSystem(int numParticles) : ParticleSystem(numParticles) {}

Vector3f ParticleSpringSystem::getPosition(int particleIdx)
{
	return m_vVecState.at(particleIdx * 2);
}

Vector3f ParticleSpringSystem::getVelocity(int particleIdx)
{
	return m_vVecState.at(particleIdx * 2 + 1);
}

Vector3f ParticleSpringSystem::springForce(Spring s)
{
	// −k(||d|| − r)*d/||d|| (i.e. vector direction) , where d = xi − xj .
	Vector3f p0 = getPosition(s.p0);
	Vector3f p1 = getPosition(s.p1);
	Vector3f d = p1 - p0;
	return s.k * (d.abs() - s.r) * d / (d.abs());
}

// render the system (ie draw the particles)
void ParticleSpringSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++)
	{
		Vector3f pos = getPosition(i);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.075f, 10.0f, 10.0f);
		glPopMatrix();
	}
	for (const auto &spring : springs)
	{
		float f = springForce(spring).abs();

		glColor3f(f / 2, f / 2, f / 2);

		glLineWidth(2);
		glPushMatrix();
		glBegin(GL_LINES);

		// Define the start point of the line
		glVertex3fv(getPosition(spring.p0));
		// Define the end point of the line
		glVertex3fv(getPosition(spring.p1));
		glEnd(); // End the drawing block
		glPopMatrix();
	}
}