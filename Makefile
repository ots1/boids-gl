CXX=g++-mp-4.5 
CXXFLAGS=-I/usr/local/include -std=c++0x -I/opt/local/include -L/opt/local/lib -framework OpenGL -framework GLUT -lGLEW -g
simple-example : simple-example.cpp boids.cpp
display-example : display-example.cpp boids.cpp boidrenderer.cpp
