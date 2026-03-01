#include "ClothSystem.h"
#include <iostream>
struct Dir
{
	int dx, dy;
};

ClothSystem::ClothSystem(unsigned numParticlesPerSide) : ParticleSpringSystem(numParticlesPerSide * numParticlesPerSide)
{
	m_numParticlesPerSide = numParticlesPerSide;
	// setup particles	
	for(int j=0;j<m_numParticlesPerSide; ++j)
	{
		for(int i=0;i<m_numParticlesPerSide; ++i)
		{
			m_vVecState.push_back(Vector3f(i,j,0));
			m_vVecState.push_back(Vector3f(0,0,0));
		}
	}	
	setupBasicSprings();
}
void ClothSystem::setupBasicSprings()
{
	vector<Dir> structuralSpringDir;
	structuralSpringDir.push_back({-1, 0});
	structuralSpringDir.push_back({0, -1});
	vector<Dir> shearSpringDir;
	shearSpringDir.push_back({-1, -1});
	shearSpringDir.push_back({-1, 1});
	vector<Dir> flexSpringDir;
	flexSpringDir.push_back({-2, 0});
	flexSpringDir.push_back({0, -2});

	for (int i = 0; i < m_numParticlesPerSide; ++i) 
	{
		for (int j = 0; j < m_numParticlesPerSide; ++j)
		{
			int curIdx = i * m_numParticlesPerSide + j;
			// structural springs
			for (const auto &dir : structuralSpringDir)
			{
				int iNeighbor = i + dir.dx;
				int jNeighbor = j + dir.dy;
				if(iNeighbor >= 0 && iNeighbor < m_numParticlesPerSide && jNeighbor >= 0 && jNeighbor < m_numParticlesPerSide)
				{
					int neighborIdx = iNeighbor * m_numParticlesPerSide + jNeighbor;
					springs.push_back(Spring{curIdx, neighborIdx, 5.0f, 1.0f});
				}
			}
			// shear springs
			for (const auto &dir : shearSpringDir)
			{
				int iNeighbor = i + dir.dx;
				int jNeighbor = j + dir.dy;
				if(iNeighbor >= 0 && iNeighbor < m_numParticlesPerSide && jNeighbor >= 0 && jNeighbor < m_numParticlesPerSide)
				{
					int neighborIdx = iNeighbor * m_numParticlesPerSide + jNeighbor;
					springs.push_back(Spring{curIdx, neighborIdx, 0.3f, 1.0f});
				}
			}
			// flex springs
			for (const auto &dir : flexSpringDir)
			{
				int iNeighbor = i + dir.dx;
				int jNeighbor = j + dir.dy;
				if(iNeighbor >= 0 && iNeighbor < m_numParticlesPerSide && jNeighbor >= 0 && jNeighbor < m_numParticlesPerSide)
				{
					int neighborIdx = iNeighbor * m_numParticlesPerSide + jNeighbor;
					springs.push_back(Spring{curIdx, neighborIdx, 0.3f, 1.0f});
				}
			}
		}
	}
	cout << "total num of springs: " << springs.size() << endl;
}
// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
    vector<Vector3f> f;
    for (unsigned i = 0; i < m_numParticles; ++i)
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

	for (unsigned i = 0; i < m_numParticles - m_numParticlesPerSide; ++i)
    {
        newState.push_back(getVelocity(i));
        newState.push_back(f.at(i) / particleMass);
    }
	for (int i = m_numParticles - m_numParticlesPerSide; i < m_numParticles; ++i)
	{
		newState.push_back(Vector3f::ZERO); // first row is stationary.
		newState.push_back(Vector3f::ZERO);
	}
    return newState;
}

