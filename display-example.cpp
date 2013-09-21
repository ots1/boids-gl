#include "boids.hpp"
#include "boidrenderer.hpp"
#include <random>
#include <iostream>
#include <tvmet/Matrix.h>

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
	int window_shape[2];

	struct {
		glm::vec3 pos;
		glm::vec3 look_dirn;
		glm::vec3 up_dirn;
	} player;

} glut_callback_data;

static void init_glut_callback_data(BoidSystem *bs,
				    BoidRenderer *br,
				    double dt,
				    int width,
				    int height)
{
	glut_callback_data.bs = bs;
	glut_callback_data.renderer = br;

	glut_callback_data.dt = dt;
	glut_callback_data.timestep = 0;

	glut_callback_data.window_shape[0] = width;
	glut_callback_data.window_shape[1] = height;

	glut_callback_data.player.pos =       glm::vec3(0.0f, 0.0f, -51.0f);
	glut_callback_data.player.look_dirn = glm::vec3(0.0f, 0.0f, 1.0f);
	glut_callback_data.player.up_dirn =   glm::vec3(0.0f, 1.0f, 0.0f);

}


/* The callbacks */
void render(void)
{
	std::cout << "rendering frame " << glut_callback_data.timestep << std::endl;
	glut_callback_data.renderer->render();
	glutSwapBuffers();
}

void update(void)
{
	std::cout << "timestep " << glut_callback_data.timestep
		  << std::endl;
	glut_callback_data.bs->update(glut_callback_data.dt);
	glut_callback_data.timestep++;
	
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glut_callback_data.window_shape[0] = w;
	glut_callback_data.window_shape[1] = h;
	glut_callback_data.renderer->update_projection_matrix((float)w/h);
	glViewport(0, 0, w, h);
}

void mouse_move(int x, int y)
{
	if ((x == 0) && (y == 0))
		return;
	
	float xrel = (float)x/glut_callback_data.window_shape[0] - 0.5,
	      yrel = (float)y/glut_callback_data.window_shape[1] - 0.5;

	std::cout << "mouse motion: " << xrel << " " << yrel << std::endl;

	glm::vec3 intent = glm::normalize(
		xrel*glm::cross(glut_callback_data.player.look_dirn,
				    glut_callback_data.player.up_dirn)
		+ yrel*glut_callback_data.player.up_dirn
		);
	
	glut_callback_data.player.look_dirn 
		+= glm::normalize(glut_callback_data.player.look_dirn + intent);

	glut_callback_data.renderer->mvmatrix 
		= glm::lookAt(glut_callback_data.player.pos,
			      glut_callback_data.player.pos + glut_callback_data.player.look_dirn,
			      glut_callback_data.player.up_dirn);
	
	glutWarpPointer(glut_callback_data.window_shape[0]/2,
			glut_callback_data.window_shape[1]/2);
}

int main(int argc, char *argv[])
{
	BoidSystem bs(2.0, 10000);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rn(-17.0,17.0);

	for (int i=0; i<10000; i++) {
		vec3 x(rn(gen), rn(gen), rn(gen));
		vec3 v(0, 0, 0);
		bs.insert(Boid(x, v));
	}

	const int init_window_shape[2] = { 800, 800 };
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(init_window_shape[0], 
			   init_window_shape[1]);
	glutCreateWindow("Boids");
	glutDisplayFunc(&render);
       	glutIdleFunc(&update);
	glutReshapeFunc(&reshape);
	glutMotionFunc(&mouse_move);

	glewInit();

	BoidRenderer br(bs, "boidrenderer.vertex", "boidrenderer.frag",
			init_window_shape[0], init_window_shape[1]);
	
	// organise this a bit better...
	init_glut_callback_data(&bs, &br, 1./30., 
				init_window_shape[0], 
				init_window_shape[1]);

	br.mvmatrix = glm::lookAt(glut_callback_data.player.pos,
				  glut_callback_data.player.pos + glut_callback_data.player.look_dirn,
				  glut_callback_data.player.up_dirn);


	glutMainLoop();
}
