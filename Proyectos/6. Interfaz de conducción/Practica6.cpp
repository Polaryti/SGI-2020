/***************************************************
ISGI:: Interfaz de conducción
Autor: Mario Campos Mocholí
***************************************************/

constexpr auto TITULO = "Velocidad: 0.0m/s";
#define _USE_MATH_DEFINES
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
float velocidad = 0;
float angulo = 0;
float direccion[] = { 1, 1, 0 };

float funcion_trazado(float x, float amplitud, float periodo) {
	return amplitud * sin(x / periodo);
}

float derivada_trazado(float x, float amplitud, float periodo) {
	return amplitud * cos(x / periodo) / periodo;
}

void rotateVector(float x1, float y1, float* x2, float* y2, float angle) {
}


void gen_circuito() {
	float vt[2] = { 1,0 };
	float vn[2] = { 0,1 };
	int numero_quads = 50;
	int longitud = 4;
	float amplitud = 6;
	float periodo = 20;
	float mitadancho = 8;
	float aux;

	for (int i = posCam[0] - 1; i < posCam[0] + numero_quads * longitud; i += longitud) {
		glPushMatrix();
		float fx = funcion_trazado(i, amplitud, periodo);
		vt[1] = derivada_trazado(i, amplitud, periodo);
		vn[0] = -vt[1];
		aux = sqrt(pow(vn[0], 2) + pow(vn[1], 2));
		vn[0] = vn[0] / aux;
		vn[1] = vn[1] / aux;
		float v0[3] = { (float)i + vn[0] * mitadancho, 0,fx + vn[1] * mitadancho };
		float v3[3] = { (float)i - vn[0] * mitadancho, 0,fx - vn[1] * mitadancho };
		fx = funcion_trazado(i + longitud, amplitud, periodo);
		vt[1] = derivada_trazado(i + longitud, amplitud, periodo);
		vn[0] = -vt[1];
		aux = sqrt(pow(vn[0], 2) + pow(vn[1], 2));
		vn[0] = vn[0] / aux;
		vn[1] = vn[1] / aux;
		float v1[3] = { (float) i + longitud + vn[0] * mitadancho,0, fx + vn[1] * mitadancho };
		float v2[3] = { (float) i + longitud - vn[0] * mitadancho,0, fx - vn[1] * mitadancho };

		quadtex(v1, v2, v3, v0, 0, 1, 0, 1, 20, 30);

		glPopMatrix();
	}
}


void init()
{
	glClearColor(1, 1, 1, 1);	// Color de fondo
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1);	// Grosor de las líneas

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Borra la pantalla
	glMatrixMode(GL_MODELVIEW);								// Selecciona la modelview
	glLoadIdentity();										// Carga la matriz identidad


	// Colocamos la cámara
	gluLookAt(posCam[0], posCam[1], posCam[2], direccion[0] + posCam[0], 1, direccion[2] + posCam[2], 0, 1, 0);

	// Circuito
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glPolygonMode(GL_FRONT, GL_FILL);
	gen_circuito();
	glPopAttrib();
	glPopMatrix();

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
		direccion[2] += rad(0.25);

		break;
	case GLUT_KEY_LEFT:
		direccion[2] -= rad(0.25);
		break;
	}

	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	// Definimos la camara (matriz de proyeccion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, 100);
}

void onTimer(int valor)
{
	// Soluciona error de precisión de variables de coma flotante
	if (velocidad < 0) {
		velocidad = 0;
	}

	// Actualizar posición camara
	static int antes = glutGet(GLUT_ELAPSED_TIME);
	int ahora = glutGet(GLUT_ELAPSED_TIME);
	float tiempo_transcurrido = ahora - antes;
	posCam[0] += velocidad / 10 * direccion[0] * (tiempo_transcurrido / 1000.0);
	posCam[2] += velocidad / 10 * direccion[2] * (tiempo_transcurrido / 1000.0);

	stringstream titulo;
	titulo << "Velocidad: " << velocidad << "m/s";
	glutSetWindowTitle(titulo.str().c_str());

	glutTimerFunc(1000 / 60, onTimer, 60);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);													// Inicializacion de GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);				// Alta de buffers a usar
	glutInitWindowSize(800, 600);											// Tamanyo inicial de la ventana
	glutCreateWindow(TITULO);												// Creacion de la ventana con su titulo
	cout << "Interfaz de conduccion por Mario Campos Mocholi" << endl;		// Mensaje por consola
	glutDisplayFunc(display);												// Alta de la funcion de atencion a display
	glutReshapeFunc(reshape);												// Alta de la funcion de atencion a reshape
	glutSpecialFunc(onSpecialKey);											// Alta de la funcion de atencion al teclado
	glutTimerFunc(1000 / 60, onTimer, 60);									// Se encola un nuevo timer
	init();																	// Inicializacion propia

	glutMainLoop();															// Puesta en marcha del programa
	return 1;
}