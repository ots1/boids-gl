#include <stdexcept>
#include "boidrenderer.hpp"
#include "boids.hpp"

void show_info_log(GLuint object, PFNGLGETSHADERIVPROC glGet__iv,
			  PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
{
	GLint log_length;
	char *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = (char *)malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	fprintf(stderr, "%s", log);
	free(log);
}

void *file_contents(const char *filename, GLint *length)
{
	FILE *f = fopen(filename, "r");
	void *buffer;

	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';

	return buffer;
}

GLuint make_shader(GLenum type, const char *filename)
{
	GLint length;
	GLchar *source = (GLchar *)file_contents(filename, &length);
	GLuint shader;
	GLint shader_ok;

	if (!source)
		return 0;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &length);
	free(source);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
	if (!shader_ok) {
		fprintf(stderr, "Failed to compile %s:\n", filename);
		show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint make_program(GLuint vertex_shader, GLuint fragment_shader)
{
	GLint program_ok;

	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
	if (!program_ok) {
		fprintf(stderr, "Failed to link shader program:\n");
		show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}


void BoidRenderer::transfer_vertex_data(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	GLfloat *buffer_data = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!buffer_data)
		throw std::runtime_error("could not map vertex buffer");

	{
		int i=0;
		for (auto boid=bs.begin(); boid != bs.end(); ++boid) 
		{
			buffer_data[3*i] = (GLfloat)boid->pos[0];
			buffer_data[3*i + 1] = (GLfloat)boid->pos[1];
			buffer_data[3*i + 2] = (GLfloat)boid->pos[2];
			++i;
		}
	}
	
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void BoidRenderer::update_projection_matrix(float viewport_ratio)
{
	/*
	  GLfloat wf(w), hf(h);
	
	  GLfloat xy_scale = std::min(wf, hf) * 1.0f/fov_ratio;
	  GLfloat r_x = xy_scale / wf,
	  r_y = xy_scale / hf,
	  zw_scale = 1.0f/(far_plane - near_plane),
	  r_z = zw_scale * (near_plane + far_plane),
	  r_w = -2.0f * zw_scale * near_plane * far_plane;
	
	  pmatrix = 
	  r_x, 0.0f, 0.0f, 0.0f,
	  0.0f, r_y, 0.0f, 0.0f,
	  0.0f, 0.0f, r_z, 1.0f,
	  0.0f, 0.0f, r_w, 0.0f;
	*/
	pmatrix = glm::perspective(fov_angle_deg, (GLfloat)viewport_ratio, 
				   near_plane, far_plane);
}

void BoidRenderer::render(void)
{
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	transfer_vertex_data();
	glVertexAttribPointer(
		attributes.position, 3, GL_FLOAT, 
		GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(attributes.position);

	glEnable(GL_PROGRAM_POINT_SIZE);
	
	// model-view-projection matrices
	glUniformMatrix4fv(uniforms.pmatrix, 1, GL_FALSE, glm::value_ptr(pmatrix));
	glUniformMatrix4fv(uniforms.mvmatrix, 1, GL_FALSE, glm::value_ptr(mvmatrix));

	// draw them...
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, bs.size());
	glDisableVertexAttribArray(attributes.position);
}

BoidRenderer::BoidRenderer(const BoidSystem& bs_, 
			   std::string vshader_fname,
			   std::string fshader_fname,
			   int width, 
			   int height)
	: bs(bs_), 
	  fov_angle_deg(60.0f),
	  near_plane(0.0625f),
	  far_plane(1024.0f),
	  vertex_shader_fname(vshader_fname),
	  fragment_shader_fname(fshader_fname)
{
	update_projection_matrix((float)width/height);

	// vertex buffer
	GLuint vb;
	glGenBuffers(1, &vb);
	vertex_buffer = vb;
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 
		     bs.size()*3*sizeof(GL_FLOAT), 
		     NULL, GL_DYNAMIC_DRAW);

	if (!vertex_buffer) 
		throw std::runtime_error("failed to create vertex_buffer");

	vertex_shader = make_shader(
		GL_VERTEX_SHADER, vertex_shader_fname.c_str());
	if (!vertex_shader)
		throw std::runtime_error("failed to create vertex_shader");

	fragment_shader = make_shader(
		GL_FRAGMENT_SHADER, 
		fragment_shader_fname.c_str());
	if (!fragment_shader)
		throw std::runtime_error("failed to create fragment_shader");

	program = make_program(
		vertex_shader, fragment_shader);
	if (!program)
		throw std::runtime_error("failed to compile shader program");

	attributes.position = glGetAttribLocation(
		program, "position");
	if (attributes.position == -1)
		throw std::runtime_error("failed to find 'position' attribute in shader program");

	uniforms.pmatrix = glGetUniformLocation(
		program, "pmatrix");
	if (uniforms.pmatrix == -1)
		throw std::runtime_error("failed to find 'pmatrix' uniform in shader program");
	
	uniforms.mvmatrix = glGetUniformLocation(
		program, "mvmatrix");
	if (uniforms.mvmatrix == -1)
		throw std::runtime_error("failed to find 'mvmatrix' uniform in shader program");

}

BoidRenderer::~BoidRenderer()
{
	glDeleteBuffers(1, &vertex_buffer);
}
