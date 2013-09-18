#ifndef BOIDRENDERER_H
#define BOIDRENDERER_H
#include "boids.hpp"
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
	static const char vertex_shader_fname[];
	static const char fragment_shader_fname[];

	GLuint vertex_buffer;
	GLuint vertex_shader, fragment_shader;
	GLuint program;

	const BoidSystem& bs;

	struct {
		GLuint position;
	} attributes;

	// transfer the vertex data to the current GL_ARRAY_BUFFER
	void transfer_vertex_data(void)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		GLfloat *buffer_data = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (!buffer_data)
			throw std::runtime_error("could not map vertex buffer");
		for (int i=0; i<bs.boids.size(); ++i) {
			buffer_data[3*i] = (GLfloat)bs.boids[i].pos[0];
			buffer_data[3*i + 1] = (GLfloat)bs.boids[i].pos[1];
			buffer_data[3*i + 2] = (GLfloat)bs.boids[i].pos[2];
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	void render(void)
	{
		glUseProgram(program);

		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		transfer_vertex_data();
		glVertexAttribPointer(
			attributes.position, 3, GL_FLOAT, 
			GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(attributes.position);

		glEnable(GL_PROGRAM_POINT_SIZE);

		// projection matrix etc

		// general things

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(2);
		glDrawArrays(GL_POINTS, 0, bs.boids.size());
		glDisableVertexAttribArray(attributes.position);
	}

	BoidRenderer(const BoidSystem& bs_)
		: bs(bs_)
	{
		// vertex buffer
		GLuint vb;
		glGenBuffers(1, &vb);
		vertex_buffer = vb;
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, 
			     bs.boids.size()*3*sizeof(GL_FLOAT), 
			     NULL, GL_DYNAMIC_DRAW);

		if (!vertex_buffer) 
			throw std::runtime_error("failed to create vertex_buffer");

		vertex_shader = make_shader(
			GL_VERTEX_SHADER, vertex_shader_fname);
		if (!vertex_shader)
			throw std::runtime_error("failed to create vertex_shader");

		fragment_shader = make_shader(
			GL_FRAGMENT_SHADER, 
			fragment_shader_fname);
		if (!fragment_shader)
			throw std::runtime_error("failed to create fragment_shader");

		program = make_program(
			vertex_shader, fragment_shader);
		if (!program)
			throw std::runtime_error("failed to compile shader program");

		attributes.position = glGetAttribLocation(
			program, "position");
		if (!attributes.position == -1)
			throw std::runtime_error("failed to find 'position' attribute in shader program");
	}

	~BoidRenderer()
	{
		glDeleteBuffers(1, &vertex_buffer);
	}
};


#endif
