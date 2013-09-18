#include "boids.hpp"
#include "boidrenderer.hpp"
#include <random>
#include <iostream>

#include <GL/glew.h>
#ifdef __APPLE__
#    include <GLUT/glut.h>
#else
#    include <GL/glut.h>
#endif

using namespace std;


/* Global data for the callbacks */
struct {
	BoidSystem *bs;
	BoidRenderer *renderer;
	double dt;
	int timestep;
} glut_callback_data;

static void init_glut_callback_data(BoidSystem *bs,
				    BoidRenderer *br,
				    double dt)
{
	glut_callback_data.bs = bs;
	glut_callback_data.dt = dt;
	glut_callback_data.timestep = 0;
	glut_callback_data.renderer = br;
}


/* The callbacks */
extern "C"
void update(void)
{
	std::cout << "timestep " << glut_callback_data.timestep
		  << std::endl;
	glut_callback_data.bs->update(glut_callback_data.dt);
	glut_callback_data.timestep++;
}

extern "C"
void render(void)
{
	glut_callback_data.renderer->render();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	BoidSystem bs(2.0, 2000);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rn(-10.0,10.0);

	for (int i=0; i<2000; i++) {
		vec3 x(rn(gen), rn(gen), rn(gen));
		vec3 v(0, 0, 0);
		bs.insert(Boid(x, v));
	}

	// Initialize this after the last Boid is added to bs:
	// dealing with buffer objects etc.
	BoidRenderer br(bs);	
	
	init_glut_callback_data(&bs, &br, 1./60.);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Boids");
	glutDisplayFunc(&render);
	glutIdleFunc(&update);
	glutMainLoop();
}
