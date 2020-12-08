/***************************************************
ISGI:: Interfaz de conducción
Autor: Mario Campos Mocholí
***************************************************/

constexpr auto TITULO = "Velocidad: 0.0m/s";
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream> // Biblioteca de manejo de strings
#include <freeglut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>
#include <ctime>
#include <cmath>
#include <chrono>
#include <Utilidades.h>

using namespace std;

// Coordenadas de la cámara
float posCam[] = { 0, 1, 0 };

// Velocidad y dirección del vehiculo
float velocidad = 0.0f;
float angulo = 0.0f;
float direccion[] = { 0, 0, 1 };

static GLint circuito;

void gen_circuito() {
	circuito = glGenLists(1);
	glNewList(circuito, GL_COMPILE);
	glBegin(GL_QUAD_STRIP);

	GLfloat v0[3] = { 0,0,5 }, v1[3] = { 20,0,5 }, v2[3] = { 20,0,-5 }, v3[3] = { 0,0,-5 };
	glPolygonMode(GL_FRONT, GL_LINE);
	quad(v0, v1, v2, v3, 10, 5);

	glEnd();
	glEndList();
}


void init()
{
	glEnable(GL_DEPTH_TEST);
	//glLineWidth(3);	// Grosor de las líneas
	glClearColor(1.0, 1.0, 1.0, 1.0);	// Color de fondo

	gen_circuito();
	
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Borra la pantalla
	glMatrixMode(GL_MODELVIEW);								// Selecciona la modelview
	glLoadIdentity();										// Carga la matriz identidad

	// Colocamos la cámara
	gluLookAt(posCam[0], posCam[1], posCam[2], 0, 0, 0, 0, 1, 0);

	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glCallList(circuito);
	glutWireTeapot(2);
	glPopMatrix();
	glPopAttrib();

	glutSwapBuffers();
}

void onSpecialKey(int specialKey, int x, int y)
// Funcion de atencion al teclado
{
	switch (specialKey) {
	case GLUT_KEY_UP:
		velocidad += 0.1f;
		break;
	case GLUT_KEY_DOWN:
		if (velocidad > 0) {
			velocidad -= 0.1f;
		}
		break;
	case GLUT_KEY_RIGHT:
		angulo -= 0.25f;
		break;
	case GLUT_KEY_LEFT:
		angulo += 0.25f;
		break;
	}

	stringstream titulo;
	titulo << "Velocidad: " << velocidad << "m/s";
	glutSetWindowTitle(titulo.str().c_str());

	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	float razon = (float)w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double distancia, fovy;
	distancia = sqrt(pow(posCam[0], 2) + pow(posCam[1], 2) + pow(posCam[2], 2));
	fovy = (asin(1 / distancia) * 2.0) * 180 / M_PI;
	gluPerspective(fovy, razon, 1, 100);
}

void onTimer(int valor)
{
	// Calculo vector unitario

	// Actualizar posición camara
	posCam[0] += direccion[0] * velocidad;
	posCam[2] += direccion[2] * velocidad;

	

	glutTimerFunc(1000 / 60, onTimer, 60);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);													// Inicializacion de GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);				// Alta de buffers a usar
	glutInitWindowSize(800, 600);											// Tamanyo inicial de la ventana
	glutCreateWindow(TITULO);												// Creacion de la ventana con su titulo
	init();																	// Inicializacion propia
	std::cout << "Interfaz de conducción" << " por Mario Campos Mocholi" << std::endl;		// Mensaje por consola
	glutDisplayFunc(display);												// Alta de la funcion de atencion a display
	glutReshapeFunc(reshape);												// Alta de la funcion de atencion a reshape
	glutSpecialFunc(onSpecialKey);											// Alta de la funcion de atencion al teclado
	glutTimerFunc(1000 / 60, onTimer, 60);									// Se encola un nuevo timer
	glutMainLoop();															// Puesta en marcha del programa
	return 1;
}