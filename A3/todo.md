basically - make them subclasses of particlespring system.
Current hierarch

ParticleSystem (abstract: evalF, draw)
  └── ParticleSpringSystem (springs, evalF, springForce, getPosition/Velocity, draw)
       └── ClothSystem (overrides setupBasicSprings, evalF, draw)
Problems with the current design
ClothSystem re-declares vector<Spring> springs as private (ClothSystem.h:21), which shadows the base class's protected springs. So ClothSystem::setupBasicSprings() fills its own vector, but if you call the base evalF/springForce, they'd use the empty base vector. This is a bug.

evalF hardcodes pendulum behavior - particle 0 is always fixed (particleSpringSystem.cpp:43-44). Cloth needs different fixed particles (e.g. top-row corners), so you can't reuse evalF as-is.

ParticleSpringSystem constructor has an empty if() on line 11 - this won't compile. The constructor mixes "generic particle-spring init" with "pendulum-specific init".

draw() isn't declared virtual in the base header (particleSpringSystem.h:18).

Recommendation: Make ParticleSpringSystem the reusable base, pendulum becomes a subclass

ParticleSystem (abstract)
  └── ParticleSpringSystem (generic spring physics)
       ├── PendulumSystem (chain init, particle 0 fixed)
       └── ClothSystem (grid init, corners fixed)
Concretely:

ParticleSpringSystem becomes a clean, reusable base that owns:

springs, drag, g, particleMass
getPosition(), getVelocity(), springForce() (unchanged)
A generic evalF() that uses a set<int> fixedParticles instead of hardcoding index 0. For each particle: if it's in the fixed set, emit zero derivatives; otherwise, accumulate gravity + drag + spring forces as you already do.
A generic draw() that draws spheres + spring lines (what you have now works for both).
setupBasicSprings() stays pure virtual (or empty) - subclasses define their topology.
PendulumSystem (rename from current ParticleSpringSystem usage):

Constructor: places particles in a chain, inserts particle 0 into fixedParticles
setupBasicSprings(): the sequential chain + skip-one springs you already have
ClothSystem:

Constructor: places particles in a grid, inserts top-row corners into fixedParticles
setupBasicSprings(): structural/shear/flex springs (what you already have)
Remove the shadowed springs declaration
evalF() can just call ParticleSpringSystem::evalF() - no need to rewrite it
Override draw() only if you want triangulated cloth rendering
The key change is just one thing in evalF:


// In ParticleSpringSystem::evalF
for (unsigned i = 0; i < m_numParticles; ++i)
{
    if (fixedParticles.count(i)) {
        newState.push_back(Vector3f::ZERO);
        newState.push_back(Vector3f::ZERO);
    } else {
        newState.push_back(getVelocity(i));
        newState.push_back(f.at(i) / particleMass);
    }
}
This way your entire force computation (gravity + drag + springs) is written once and both systems reuse it. The only things subclasses need to define are particle placement, spring topology, and which particles are pinned. Want me to implement this?
