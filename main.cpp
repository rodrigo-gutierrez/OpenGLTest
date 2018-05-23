#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#include "shader.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "OpenGL Test", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("../vertex.shader", "../fragment.shader");

	// Dark blue background
	//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	const int numVertices = 36;

	typedef vec4 point4;
	typedef vec4 color4;

	class Cube
	{
	public:
		Cube()
		{
			colorcube();
		}
		
		point4 vPositions[numVertices];
		color4 vColors[numVertices];

	private:
		point4 positions[8] =
		{
			point4(-0.5, -0.5,  0.5, 1.0),
			point4(-0.5,  0.5,  0.5, 1.0),
			point4(0.5,  0.5,  0.5, 1.0),
			point4(0.5, -0.5,  0.5, 1.0),
			point4(-0.5, -0.5, -0.5, 1.0),
			point4(-0.5,  0.5, -0.5, 1.0),
			point4(0.5,  0.5, -0.5, 1.0),
			point4(0.5, -0.5, -0.5, 1.0)
		};

		color4 colors[8] =
		{
			color4(0.0, 0.0, 0.0, 1.0),
			color4(1.0, 0.0, 0.0, 1.0),
			color4(1.0, 1.0, 0.0, 1.0),
			color4(0.0, 1.0, 0.0, 1.0),
			color4(0.0, 0.0, 1.0, 1.0),
			color4(1.0, 0.0, 1.0, 1.0),
			color4(1.0, 1.0, 1.0, 1.0),
			color4(0.0, 1.0, 1.0, 1.0)
		};

		int index = 0;
		
		void quad(int a, int b, int c, int d)
		{
			
			vColors[index] = colors[a]; vPositions[index] = positions[a]; index++;
			vColors[index] = colors[b]; vPositions[index] = positions[b]; index++;
			vColors[index] = colors[c]; vPositions[index] = positions[c]; index++;
			vColors[index] = colors[a]; vPositions[index] = positions[a]; index++;
			vColors[index] = colors[c]; vPositions[index] = positions[c]; index++;
			vColors[index] = colors[d]; vPositions[index] = positions[d]; index++;
		}

		void colorcube()
		{
			quad(1, 0, 3, 2);
			quad(2, 3, 7, 6);
			quad(3, 0, 4, 7);
			quad(6, 5, 1, 2);
			quad(4, 5, 6, 7);
			quad(5, 4, 0, 1);
		}
	};

	Cube cube = Cube();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube.vPositions) + sizeof(cube.vColors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube.vPositions), cube.vPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube.vPositions), sizeof(cube.vColors), cube.vColors);

	GLuint vPosition = glGetAttribLocation(programID, "vPosition");
	GLuint vColor = glGetAttribLocation(programID, "vColor");

	do
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(
			vPosition,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			BUFFER_OFFSET(0)
		);

		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(
			vColor,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			BUFFER_OFFSET(sizeof(cube.vPositions))
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, numVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
