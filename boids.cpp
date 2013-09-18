#include "boids.hpp"
#include <iostream>
#include <algorithm>

int Boid::update_acc(SpatialHash<Boid*>::iterator visible)
{ 
	vec3 CoM(0.0), mean_heading(0.0), avoid(0.0);
	int count=0;
	// calculate centre of mass, mean heading and avoidance vector in one pass
	for (; !visible.end(); ++visible) {
		if ((*visible) == this) continue;
		if (tvmet::norm2((*visible)->pos - pos) > 2.0) continue;
		count++;
		CoM += (*visible)->pos - pos;
		mean_heading += (*visible)->vel;
		vec3 r((*visible)->pos - pos);
		if (tvmet::norm2(r) < avoidance_dist) {
			avoid -= normalize(r) / (norm2(r) * norm2(r));
		}
	}
	CoM /= count;
	mean_heading /= count;
	mean_heading -= vel;

	// move towards centre of mass of visible boids
	acc += group_scale * CoM;

	// adjust velocity to match heading of the group
	acc += heading_scale * mean_heading;

	// avoid very close boids
	acc += avoidance_scale * avoid;

	return count;
}

Boid::Boid(vec3 pos_, vec3 vel_)
	: age(0), 
	  pos(pos_), 
	  vel(vel_),
	  acc(0.,0.,0.)
{ }


BoidSystem::BoidSystem(double visibility_, int n_boids)
	: visibility(visibility_), 
	  h(visibility_), 
	  max_velocity(1.0), 
	  max_acceleration(1.0)
{
	boids.reserve(n_boids);
}

void BoidSystem::insert(Boid b)
{
	b.vel = restrict_vector(b.vel, max_velocity);
	boids.push_back(b); // push a copy;
	h.insert(b.pos, &boids.back()); // include in the spatial hash
}

void BoidSystem::update(double dt)
{
	// rebuild spatial hash
	h.clear();

	// (reinsert all boids)
	for (auto ii=boids.begin(); ii != boids.end(); ++ii) {
		h.insert(ii->pos, &(*ii));
	}

	// update the accelerations
	int max_c=0, min_c=10000000;
	int sum_c=0;
	int count;
	for (auto ii=boids.begin(); ii != boids.end(); ++ii) {
		count = ii->update_acc(h.neighbours(ii->pos));
		sum_c += count;
		min_c = std::min(min_c, count);
		max_c = std::max(max_c, count);
		ii->acc = restrict_vector(ii->acc, max_acceleration);
	}
	       
	std::cout << "Counts: min=" << min_c << " max=" << max_c << " mean=" << (double)sum_c/boids.size() << std::endl;
			
	// integrate positions and velocities
	for (auto ii=boids.begin(); ii != boids.end(); ++ii) {
		ii->pos += dt * ii->vel;
		ii->vel += dt * ii->acc;
		ii->vel = restrict_vector(ii->vel, max_velocity);
	}
}	