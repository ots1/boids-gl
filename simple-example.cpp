#include "util.hpp"
#include "boids.hpp"
#include <random>
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) 
{
	double dt = 1./60.;
	BoidSystem bs(2.0, 2000);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rn(-10.0,10.0);

	for (int i=0; i<2000; i++) {
		vec3 x(rn(gen), rn(gen), rn(gen));
		vec3 v(0, 0, 0);
		bs.insert(Boid(x, v));
	}

	for (int i=0; i<10000; i++) {
		cout << "timestep " << i << endl;
		bs.update(dt);
	}
}
