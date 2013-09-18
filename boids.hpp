#ifndef BOIDS_H
#define BOIDS_H

#include <memory>
#include <vector>
#include <tvmet/Vector.h>
#include "util.hpp"
#include "spatialhash.hpp"

class Boid {
	friend class BoidSystem;
	friend class BoidRenderer;

	constexpr static double group_scale = 1.0;
	constexpr static double heading_scale = 1.0;
	constexpr static double avoidance_scale = 100.0;
	constexpr static double avoidance_dist = 1.0;

	int age; // age in timesteps
	vec3 pos, vel, acc;

public:
	// update acceleration based on Boid rules
	int update_acc(SpatialHash<Boid*>::iterator visible);

	Boid(vec3 pos_=vec3(0.,0.,0.), vec3 vel_=vec3(0.,0.,0.));
};


class BoidSystem {
	SpatialHash<Boid*> h;
	double visibility;
	double max_velocity;
	double max_acceleration;
public:
	std::vector<Boid> boids;

	void insert(Boid b);

	void update(double dt);

	BoidSystem(double visibility_, int n_boids=0);
};

#endif
