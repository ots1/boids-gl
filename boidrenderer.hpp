#ifndef BOIDRENDERER_H
#define BOIDRENDERER_H
#include "boids.hpp"
#include <string>
// #include <tvmet/Matrix.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // C pointer conversion
#include <glm/gtc/matrix_transform.hpp> // perspective
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#ifdef __APPLE__
#    include <GLUT/glut.h>
#else
#    include <GL/glut.h>
#endif

void show_info_log(GLuint object, PFNGLGETSHADERIVPROC glGet__iv,
		   PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);

void *file_contents(const char *filename, GLint *length);

GLuint make_shader(GLenum type, const char *filename);

GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);

class BoidRenderer {
public:
	std::string vertex_shader_fname;
	std::string fragment_shader_fname;

	//float fov_ratio=0.7f;
	float fov_angle_deg;
	float near_plane;
	float far_plane;
	
	const BoidSystem& bs;

	glm::mat4 pmatrix;
	glm::mat4 mvmatrix;

// opengl handles 
	GLuint vertex_buffer;
	GLuint vertex_shader, fragment_shader;
	GLuint program;

	struct {
		GLint position;
	} attributes;

	struct {
		GLint pmatrix;
		GLint mvmatrix;
	} uniforms;
///////

	// transfer the vertex data to the current GL_ARRAY_BUFFER
	void transfer_vertex_data(void);
	
	void update_projection_matrix(float viewport_ratio);
	
	void render(void);

	BoidRenderer(const BoidSystem& bs_, 
		     std::string vshader_fname,
		     std::string fshader_fname,
		     int width, 
		     int height);

	~BoidRenderer();
};


#endif
