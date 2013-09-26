#include "boids.hpp"
#include "boidrenderer.hpp"
#include <random>
#include <iostream>
#include <tvmet/Matrix.h>
#include <fenv.h>

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

	int mouse_pos_old[2];

	std::vector<bool> keybuffer;

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

	glut_callback_data.keybuffer.resize(256, false);
	
	glut_callback_data.player.pos =       glm::vec3(0.0f, 0.0f, -120.0f);
	glut_callback_data.player.look_dirn = glm::vec3(0.0f, 0.0f, 1.0f);
	glut_callback_data.player.up_dirn =   glm::vec3(0.0f, 1.0f, 0.0f);
}


/* The callbacks */
void render(void)
{
	//std::cout << "rendering frame " << glut_callback_data.timestep << std::endl;
	glut_callback_data.renderer->render();
	glutSwapBuffers();
}

void update(void)
{
//	std::cout << "timestep " << glut_callback_data.timestep
//		  << std::endl;
	glut_callback_data.bs->update(glut_callback_data.dt);
	glut_callback_data.timestep++;

	// handle keys
	static const float speed = 1.0;
	if (glut_callback_data.keybuffer['w'])
		glut_callback_data.player.pos += speed * glut_callback_data.player.look_dirn;
	if (glut_callback_data.keybuffer['s'])
		glut_callback_data.player.pos -= speed * glut_callback_data.player.look_dirn;
	if (glut_callback_data.keybuffer['a'])
		glut_callback_data.player.pos -= speed * glm::cross(glut_callback_data.player.look_dirn, 
								    glut_callback_data.player.up_dirn);
	if (glut_callback_data.keybuffer['d'])
		glut_callback_data.player.pos += speed * glm::cross(glut_callback_data.player.look_dirn, 
								    glut_callback_data.player.up_dirn);

 	glut_callback_data.renderer->mvmatrix 
 		= glm::lookAt(glut_callback_data.player.pos,
 			      glut_callback_data.player.pos + glut_callback_data.player.look_dirn,
 			      glut_callback_data.player.up_dirn);
	
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glut_callback_data.window_shape[0] = w;
	glut_callback_data.window_shape[1] = h;
	glut_callback_data.renderer->update_projection_matrix((float)w/h);
	glViewport(0, 0, w, h);
}

void mouse_click(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		glut_callback_data.mouse_pos_old[0] = x;
		glut_callback_data.mouse_pos_old[1] = y;
	}
}

void mouse_move(int x, int y)
{	
	const float scale = 0.005f;	// speed of the mouse control

	int xrel = x - glut_callback_data.mouse_pos_old[0],
	    yrel = y - glut_callback_data.mouse_pos_old[1];

	glut_callback_data.mouse_pos_old[0] = x;
	glut_callback_data.mouse_pos_old[1] = y;

	// // experiment with warping the mouse back to the centre if it gets to the edge
	// if ((x<=0) || (x>=glut_callback_data.window_shape[0]-1) ||
	//      (y<=0) || (y>=glut_callback_data.window_shape[1]-1)) {
	// 	glutWarpPointer(glut_callback_data.window_shape[0]/2,
	// 			glut_callback_data.window_shape[1]/2);
	// 	glut_callback_data.mouse_pos_old[0] = glut_callback_data.window_shape[0]/2 - xrel;
	// 	glut_callback_data.mouse_pos_old[0] = glut_callback_data.window_shape[1]/2 - yrel;
		
	// }

	if ((xrel == 0) && (yrel == 0)) return;

	glm::vec3 intent = float(xrel)*glm::cross(glut_callback_data.player.look_dirn, 
						  glut_callback_data.player.up_dirn)
		- float(yrel)*glut_callback_data.player.up_dirn;
		
	glut_callback_data.player.look_dirn 
		= glm::normalize(glut_callback_data.player.look_dirn + scale*intent);

	glut_callback_data.renderer->mvmatrix 
		= glm::lookAt(glut_callback_data.player.pos,
			      glut_callback_data.player.pos + glut_callback_data.player.look_dirn,
			      glut_callback_data.player.up_dirn);
}

// void key_press(unsigned char key, int x, int y)
// {
// 	static const float speed = 1.0;
// 	switch (key) {
// 	case 'w':
// 		glut_callback_data.player.pos += speed * glut_callback_data.player.look_dirn;
// 		break;
// 	case 's':
// 		glut_callback_data.player.pos -= speed * glut_callback_data.player.look_dirn;
// 		break;
// 	}
// 	glut_callback_data.renderer->mvmatrix 
// 		= glm::lookAt(glut_callback_data.player.pos,
// 			      glut_callback_data.player.pos + glut_callback_data.player.look_dirn,
// 			      glut_callback_data.player.up_dirn);
// }

void key_down(unsigned char key, int x, int y)
{
	glut_callback_data.keybuffer[key] = true;
}

void key_up(unsigned char key, int x, int y)
{
	glut_callback_data.keybuffer[key] = false;
}

int main(int argc, char *argv[])
{
	feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

	BoidSystem bs(2.1, 20000);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rn(-50.0,50.0);

	for (int i=0; i<bs.boids.capacity(); i++) {
		vec3 x(rn(gen), rn(gen), rn(gen));
		vec3 v(0, 0, 0);
		bs.insert(Boid(x, v));
	}

	const int init_window_shape[2] = { 800, 800 };
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
//	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(init_window_shape[0], 
			   init_window_shape[1]);
	glutCreateWindow("Boids");
	glutDisplayFunc(&render);
       	glutIdleFunc(&update);
	glutReshapeFunc(&reshape);
	glutMotionFunc(&mouse_move);
	glutMouseFunc(&mouse_click);
	glutKeyboardFunc(&key_down);
	glutKeyboardUpFunc(&key_up);

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
