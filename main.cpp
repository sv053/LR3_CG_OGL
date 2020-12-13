#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>


#include <GL/glut.h>

extern "C" _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

GLuint glslProgId;     
GLuint vshId, pshId;   
GLuint positionAttribute, colorAttribute;

GLuint uniformModelProjMatrix;

GLuint vao;

float w = 920.0f, h = 612.0f;
static float angle = 0.0f;
#define degreesToRadians(x) x*(3.141592f/180.0f)

glm::mat4 projectionMatrix;
glm::mat4 modelMatrix;
glm::mat4 modelProjMatrix;

struct VertexPC
{
    float x, y, z, r, g, b, a;
    VertexPC(float ix, float iy, float iz, float ir, float ig, float ib, float ia) :
        x(ix), y(iy), z(iz), r(ir), g(ig), b(ib), a(ia)
    {}
};

static VertexPC verts[] = {
	// верхние грани кристалла
	VertexPC(-1.5 / 2, 1 / 2,-4 / 2,    0,1,1,1), 
	VertexPC(2.5 / 2, 1 / 2,-2.5 / 2,   0.59,1,0,1), 
	VertexPC(0,1.5,0,                   1,0.58,0,1), 

	VertexPC(2.5 / 2, 1 / 2,-2.5 / 2,   1,0,0,1), 
	VertexPC(2.5 / 2, 1 / 2,2.5 / 2,    0,1,1,1), 
	VertexPC(0,1.5,0,                   0.35,0.7,1,1), 

	VertexPC(2.5 / 2, 1 / 2,2.5 / 2,    1,1,0,1),
	VertexPC(-1.5 / 2, 1 / 2,4 / 2,     1,0.13,0,1), 
	 VertexPC(0,1.5,0,                  1,0,0,1),

	 VertexPC(-1.5 / 2, 1 / 2,4 / 2,    1,0.75,0,1),
	 VertexPC(-3.5 / 2, 1 / 2,0,        0,1,1,1), 
	 VertexPC(0,1.5,0,                  0,0.47,0.34,1),

	 VertexPC(-3.5 / 2, 1 / 2, 0,      0,1,0,1),
	 VertexPC(-1.5 / 2, 1 / 2,-4 / 2,  1,0,1,1), 
	 VertexPC(0,1.5,0,                 1,1,0,1),

	 //---------------------
// нижние грани кристалла
	VertexPC(-1.5 / 2, 1 / 2,-4 / 2,    1,1,0.89,1), 
	VertexPC(2.5 / 2, 1 / 2,-2.5 / 2,   0.59,1,0,1), 
	VertexPC(0,-1.5,0,                  1,0.58,1,1), 

	VertexPC(2.5 / 2, 1 / 2,-2.5 / 2,   0,1,1,1), 
	VertexPC(2.5 / 2, 1 / 2,2.5 / 2,    1,0,0,1), 
	VertexPC(0,-1.5,0,                  0.35,0.5,0,1), 

	VertexPC(2.5 / 2, 1 / 2,2.5 / 2,    0,0,0,1),
	VertexPC(-1.5 / 2, 1 / 2,4 / 2,     0,0.13,0.86,1), 
	 VertexPC(0,-1.5,0,                 0.58,0,0,1),

	 VertexPC(-1.5 / 2, 1 / 2, 4 / 2,   0.81,0.75,1,1),
	 VertexPC(-3.5 / 2, 1 / 2,0,        0,0,0.11,1), 
	 VertexPC(0,-1.5,0,                 1,0.47,0.34,1),

	 VertexPC(-3.5 / 2, 1 / 2, 0,      1,0,0,1),
	 VertexPC(-1.5 / 2, 1 / 2,-4 / 2,  1,0,1,1), 
	 VertexPC(0,-1.5,0,                0,0,1,1)

};

static int indices[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29 };


void setupShaders()
{
    const char *vs =
        "#version 330 core                                          \n"
        "                                                             "
        "uniform mat4 u_modelProjMatrix;                              "
        "                                                             "
        "in vec3 a_position;                                          "
        "in vec4 a_color;                                             "
        "                                                             "
        "out vec4 v_color;                                            "
        "                                                             "
        "void main()                                                  "
        "{                                                            "
        "    v_color = a_color;                                       "
        "    gl_Position = u_modelProjMatrix * vec4(a_position, 1.0); "
        "}                                                            ";

    const char *fs =
        "#version 330 core      \n"
        "                         "
        "in vec4 v_color;         "
        "out vec4 fragColor;      "
        "                         "
        "void main()              "
        "{                        "
        "    fragColor = v_color; "
        "}                        ";

    vshId = glCreateShader(GL_VERTEX_SHADER); 
	 glShaderSource(vshId, 1, &vs, NULL); 
	 glCompileShader(vshId); 
	 pshId = glCreateShader(GL_FRAGMENT_SHADER); 
	 glShaderSource(pshId, 1, &fs, NULL); 
	 glCompileShader(pshId); 
	glslProgId = glCreateProgram();
    glAttachShader(glslProgId, vshId);
    glAttachShader(glslProgId, pshId);

    glLinkProgram(glslProgId);
    positionAttribute = glGetAttribLocation(glslProgId, "a_position");
    colorAttribute = glGetAttribLocation(glslProgId, "a_color");

    uniformModelProjMatrix = glGetUniformLocation(glslProgId, "u_modelProjMatrix");

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(
        positionAttribute,  
        3,          
        GL_FLOAT,   
        0,          
        sizeof(VertexPC),          
        0           
    );

    glEnableVertexAttribArray(colorAttribute);

    int offset = 3 * sizeof(float);
    glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, 0, sizeof(VertexPC),

        (void*)offset  
    );

}

void draw()
{
    glViewport(0, 0, w, h);

    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(glslProgId);
	
	modelMatrix =
		glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(), glm::radians(angle), glm::vec3(2.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(), glm::vec3(0.4f, 0.4f, 0.4f));
	modelProjMatrix = projectionMatrix * modelMatrix;

    glUniformMatrix4fv(uniformModelProjMatrix, 1, GL_TRUE, &modelProjMatrix[0][0]);

		
		glDrawElements(
			GL_TRIANGLES,
			30, 
			GL_UNSIGNED_INT, 
			0  
		);

	angle += 1.0;
	angle = angle > 360 ? 360 - angle : angle;

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(w, h);
	glutCreateWindow("двойная пятиугольная пирамида");
	
    glewInit();
      	
    glutIdleFunc(draw);
	glutDisplayFunc(draw);
	
    glEnable(GL_DEPTH_TEST);
 
	setupShaders();

	// начальный угол поворота фигуры
	angle = 15.0f;
    glutMainLoop();

    return 0;
}
