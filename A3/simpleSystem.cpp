
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem() : ParticleSystem(1)
{
	m_vVecState.push_back(Vector3f(1,0,0));
}

// TODO: implement evalF
// for a given state, evaluate f(X,t) - i.e. for a given particle.
// since state is completely described by its position, our 1-particle system
// will just be a single Vector3f of its position.
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> eval;
	for (unsigned i = 0; i < state.size(); ++i)
	{
		Vector3f particleEval;
		particleEval.x() = -state.at(i).y();
		particleEval.y() = state.at(i).x();
		eval.push_back(particleEval);
	}
	return eval;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
	Vector3f pos = m_vVecState.at(0); // YOUR PARTICLE POSITION
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glutSolidSphere(0.075f, 10.0f, 10.0f);
	glPopMatrix();
}
