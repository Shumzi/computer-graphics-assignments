#include "ClothSystem.h"
#include <iostream>

ClothSystem::ClothSystem(unsigned numParticlesPerSide) : ParticleSpringSystem(numParticlesPerSide * numParticlesPerSide)
{
	m_numParticlesPerSide = numParticlesPerSide;
	// setup particles. I know isn't the best to go from top to down,
	// but this way top corners are just in the first row so deal w it.
	for (int j = 0; j < m_numParticlesPerSide; ++j)
	{
		for (int i = 0; i < m_numParticlesPerSide; ++i)
		{
			m_vVecState.push_back(Vector3f((float)i, (float)j, 0));
			m_vVecState.push_back(Vector3f(0, 0, 0));
		}
	}
	setupBasicSprings();
}

Vector3f ClothSystem::getPosition(int i, int j, const vector<Vector3f> &state)
{
	return ParticleSpringSystem::getPosition(i * m_numParticlesPerSide + j, state);
}

Vector3f ClothSystem::getVelocity(int i, int j, const vector<Vector3f> &state)
{
	return ParticleSpringSystem::getVelocity(i * m_numParticlesPerSide + j, state);
}

Vector3f ClothSystem::getPosition(int i, int j)
{
	return ParticleSpringSystem::getPosition(i * m_numParticlesPerSide + j);
}

Vector3f ClothSystem::getVelocity(int i, int j)
{
	return ParticleSpringSystem::getVelocity(i * m_numParticlesPerSide + j);
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

	// structural springs
	structuralSpringsRange.start = springs.size();
	for (int i = 0; i < m_numParticlesPerSide; ++i)
		for (int j = 0; j < m_numParticlesPerSide; ++j)
			addSpringsAroundParticle(structuralSpringDir, i, j);
	structuralSpringsRange.end = springs.size();
	// shear springs
	shearSpringsRange.start = springs.size();
	for (int i = 0; i < m_numParticlesPerSide; ++i)
		for (int j = 0; j < m_numParticlesPerSide; ++j)
			addSpringsAroundParticle(shearSpringDir, i, j);
	shearSpringsRange.end = springs.size();
	// flex springs
	flexSpringsRange.start = springs.size();
	for (int i = 0; i < m_numParticlesPerSide; ++i)
		for (int j = 0; j < m_numParticlesPerSide; ++j)
			addSpringsAroundParticle(flexSpringDir, i, j);
	flexSpringsRange.end = springs.size();

	cout << "total num of springs: " << springs.size() << endl;
}
void ClothSystem::addSpringsAroundParticle(std::vector<Dir> &SpringDirs, int i, int j)
{
	for (const auto &dir : SpringDirs)
	{
		int iNeighbor = i + dir.dx;
		int jNeighbor = j + dir.dy;
		int curIdx = i * m_numParticlesPerSide + j;
		if (iNeighbor >= 0 && iNeighbor < m_numParticlesPerSide && jNeighbor >= 0 && jNeighbor < m_numParticlesPerSide)
		{
			int neighborIdx = iNeighbor * m_numParticlesPerSide + jNeighbor;
			float distBetweenNeighbors = (getPosition(i, j) - getPosition(iNeighbor, jNeighbor)).abs();
			springs.push_back(Spring{curIdx, neighborIdx, 1.f, distBetweenNeighbors});
		}
	}
}

vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
	for (int i = 0; i < m_numParticles; ++i)
	{
		// gravity
		f.push_back(Vector3f(0, -particleMass * g, 0)); // positively down maaannn.
		// drag
		Vector3f v = getVelocity(i, state);
		f.at(i) -= drag * v; // already a vector so we're good.
	}
	// passing over springs and filling in the forces.
	if (toggleStructure)
		addSpringForces(f, structuralSpringsRange, state);
	if (toggleShear)
		addSpringForces(f, shearSpringsRange, state);
	if (toggleFlex)
		addSpringForces(f, flexSpringsRange, state);
	vector<Vector3f> newState;

	for (int i = 0; i < m_numParticles; ++i)
	{
		newState.push_back(getVelocity(i, state));
		newState.push_back(f.at(i) / particleMass);
	}
	if (toggleMoveAnchors)
		moveAnchorsLineMotion(newState);
	else
	{
		// top corner particles are stationary
		// top right corner
		newState.at(newState.size() - 1) = 0;
		newState.at(newState.size() - 2) = 0;
		// top left corner
		newState.at(newState.size() - 2 * m_numParticlesPerSide) = 0;
		newState.at(newState.size() - 2 * m_numParticlesPerSide + 1) = 0;
	}
	return newState;
}

void ClothSystem::addSpringForces(std::vector<Vector3f> &f, const SpringRange &sr, const vector<Vector3f> &state)
{
	for (int i = sr.start; i < sr.end; ++i)
	{
		Spring spring = springs.at(i);
		Vector3f sf = springForce(spring, state);
		f.at(spring.p0) += sf;
		f.at(spring.p1) -= sf;
	}
}

void ClothSystem::moveAnchorsLineMotion(vector<Vector3f> &d)
{
	static int dir = 1;
	const float speed = 0.5f;
	const float endZ = 20;
	// move top corners by some func.
	float currentZ = getPosition(m_numParticles - 1).z();
	if (currentZ > endZ)
		dir = -1;
	else if (currentZ < 0)
		dir = 1;

	d.at(d.size() - 2) = Vector3f(0, 0, speed * dir);
	// top left corner
	d.at(d.size() - 2 * m_numParticlesPerSide) = Vector3f(0, 0, speed * dir);
}

void ClothSystem::drawLines(const SpringRange &sr)
{
	for (int i = sr.start; i < sr.end; ++i)
	{
		Spring spring = springs.at(i);
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

void ClothSystem::draw()
{
	if (showWireframe)
	{
		// draw particles
		for (int i = 0; i < m_numParticles; i++)
		{
			Vector3f pos = getPosition(i);
			glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2]);
			glutSolidSphere(0.075f, 10.0f, 10.0f);
			glPopMatrix();
		}

		// draw activated springs.
		if (toggleStructure)
			drawLines(structuralSpringsRange);
		if (toggleShear)
			drawLines(shearSpringsRange);
		if (toggleFlex)
			drawLines(flexSpringsRange);
	}
	else // show mesh
	{
		glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
		glDisable(GL_CULL_FACE);
		// glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		for (int i = 0; i < m_numParticlesPerSide - 1; ++i)
			for (int j = 0; j < m_numParticlesPerSide - 1; ++j)
			{
				/**
				 * v3---v2
				 * |    |
				 * v0---v1
				 */
				Vector3f v0 = getPosition(i, j);
				Vector3f v1 = getPosition(i, j + 1);
				Vector3f v2 = getPosition(i + 1, j + 1);
				Vector3f v3 = getPosition(i + 1, j);
				Vector3f normal1 = Vector3f::cross(v1 - v0, v2 - v1).normalized();
				Vector3f normal2 = Vector3f::cross(v3 - v2, v0 - v3).normalized();
				glBegin(GL_TRIANGLES);
				glNormal3fv(normal1);
				glVertex3fv(v0);
				glVertex3fv(v1);
				glVertex3fv(v2);
				glNormal3fv(normal2);
				glVertex3fv(v2);
				glVertex3fv(v3);
				glVertex3fv(v0);
				glEnd();
			}
		glPopAttrib();
	}
}