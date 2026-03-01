#include "pendulumSystem.h"
#include <iostream>
PendulumSystem::PendulumSystem(int numParticles): ParticleSpringSystem(numParticles)
{
    // first particle is stationary.
    // fill in code for initializing the state based on the number of particles
    // let's assume the first particle described is always the fixed one.

    m_vVecState.push_back(Vector3f(0, 1, 0)); // fixed particle location (just 1 up)
	m_vVecState.push_back(Vector3f(0, 0, 0)); // particle starts at rest (and is fixes so won't change)

	for (int i = 1; i < m_numParticles; i++) // we're starting w. a fixed particle always.
	{
		m_vVecState.push_back(Vector3f(float(i), 1, float(i) + 1)); // starting at spring rest distance.
		m_vVecState.push_back(Vector3f(0, 0, 0));					// particle starts at rest.
	}
    setupBasicSprings();
}

void PendulumSystem::setupBasicSprings()
{
	for (int i = 1; i < m_numParticles; i++) // we're starting w. a fixed particle always.
	{
		springs.push_back({i - 1, i, 2.f, .1f});
		if (i > 1)
			springs.push_back({i - 2, i, .5f, .1f});
	}
}


vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
    vector<Vector3f> f;
    for (int i = 0; i < m_numParticles; ++i)
    {
        // gravity
        f.push_back(Vector3f(0, -particleMass * g, 0)); // positively down maaannn.
        // drag
        f.at(i) -= drag * getVelocity(i); // already a vector so we're good.
    }
    // passing over springs and filling in the forces.
    for (const auto &spring : springs)
    {
        Vector3f sf = springForce(spring);
        f.at(spring.p0) += sf;
        f.at(spring.p1) -= sf;
    }
    vector<Vector3f> newState;
    newState.push_back(Vector3f::ZERO); // first particle is stationary.
    newState.push_back(Vector3f::ZERO);

    for (int i = 1; i < m_numParticles; ++i)
    {
        newState.push_back(getVelocity(i));
        newState.push_back(f.at(i) / particleMass);
    }
    return newState;
}