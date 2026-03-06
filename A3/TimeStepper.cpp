#include "TimeStepper.hpp"

/// TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem *particleSystem, float stepSize)
{
    const vector<Vector3f> &oldState = particleSystem->getState();
    vector<Vector3f> eval = particleSystem->evalF(oldState);
    vector<Vector3f> newState;
    for (unsigned i = 0; i < eval.size(); ++i)
        newState.push_back(oldState.at(i) + stepSize * eval.at(i));
    particleSystem->setState(newState);
}

/// TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem *particleSystem, float stepSize)
{
    const vector<Vector3f> &oldState = particleSystem->getState();
    vector<Vector3f> eval = particleSystem->evalF(oldState);
    vector<Vector3f> evalNext;
    vector<Vector3f> newState;
    // getting "next" state
    for (unsigned i = 0; i < eval.size(); ++i)
        evalNext.push_back(oldState.at(i) + stepSize * eval.at(i));
    evalNext = particleSystem->evalF(evalNext);
    // averaging evaluations.
    for (unsigned i = 0; i < eval.size(); ++i)
        newState.push_back(oldState.at(i) + stepSize * (eval.at(i) + evalNext.at(i)) / 2);
    particleSystem->setState(newState);
}
