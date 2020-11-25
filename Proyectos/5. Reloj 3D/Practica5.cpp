/***************************************************
ISGI:: Reloj 3D
Autor: Mario Campos Mocholí
***************************************************/

#define TITULO "Reloj 3D"
#define _USE_MATH_DEFINES
#include <iostream>
#include <freeglut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>
#include <ctime>
#include <cmath>
#include <chrono>

int posicionSegundos;
int posicionMinutos;
int posicionHoras;

static int anguloSec;
static int anguloMin;
static int anguloHora;
static double seno = 0.0;
static int rotacion = 0;

static GLint individual;
static GLint triangulo;
static GLint varilla;
static GLint circulo;

int contadorSeg;
int contadorMin;
int contadorHora;

static int tiempoGlobal = 0;	// Marcador de tiempo global (ms)

int posCam[] = { 2, 2, 7 };		// Coordenadas de la cámara

void gen_estrella() {
	individual = glGenLists(1);
	glNewList(individual, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; i++) {
		double angle = (1.0 + (i * 4) % 12) * M_PI / 6;
		glVertex3f(1.0 * cos(angle), 1.0 * sin(angle), 0.0);
		glVertex3f(0.7 * cos(angle), 0.7 * sin(angle), 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; i++) {
		double angle = (3.0 + (i * 4) % 12) * M_PI / 6;
		glVertex3f(1.0 * cos(angle), 1.0 * sin(angle), 0.0);
		glVertex3f(0.7 * cos(angle), 0.7 * sin(angle), 0.0);
	}
	glEnd();
	glEndList();
}

void gen_varilla() {
	varilla = glGenLists(1);
	glNewList(varilla, GL_COMPILE);
	glBegin(GL_LINES);
	glVertex3f(0, 0.7, 0);
	glVertex3f(0, 0, 0);
	glEnd();
	glEndList();
}

void gen_triangulo() {
	triangulo = glGenLists(1);
	glNewList(triangulo, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	glVertex3f(-1, 0, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(1, 0, 0);
	glEnd();
	glEndList();
}

void gen_circulo(float cx, float cy, float r, int num_segments) {
	circulo = glGenLists(1);
	glNewList(circulo, GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex2f(x + cx, y + cy);
	}
	glEnd();
	glEndList();
}

#pragma warning(disable : 4996)
void init()
{
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3);	// Grosor de las líneas
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Calculo del tiempo inicial y los angulos iniciales
	const std::time_t now = std::time(nullptr);
	const std::tm calendar_time = *std::localtime(std::addressof(now));

	anguloSec = 360 - (calendar_time.tm_sec * 6);
	contadorSeg = calendar_time.tm_sec;

	anguloMin = 360 - (calendar_time.tm_min * 6);
	contadorMin = calendar_time.tm_min;

	anguloHora = 360 - (calendar_time.tm_hour * 30);
	contadorHora = calendar_time.tm_hour;

	// Estrellas de David (práctica 4)
	gen_estrella();

	// Varilla de reloj
	gen_varilla();

	// Triangulo marcador de horas y minutos
	gen_triangulo();

	// Circulo que envuelve el reloj
	gen_circulo(0, 0, 1, 100);
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Borra la pantalla
	glMatrixMode(GL_MODELVIEW);								// Selecciona la modelview
	glLoadIdentity();										// Carga la matriz identidad

	// Colocamos la cámara
	gluLookAt(posCam[0], posCam[1], posCam[2], 0, 0, 0, 0, 1, 0);

	// Dibujamos la estrella de David de la práctica anterior
	glPushMatrix();
	double aux = abs(sin(seno)) / 4;

	double angle = (45.0 + contadorSeg) * 6.0 * M_PI / 180;
	glTranslatef(cos(angle), sin(-angle), 0.0);
	//glTranslatef(0, 0, -2);
	for (int i = 1; i <= 6; i++) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor3f(i / 10.0 + 0.1, i / 8.0 + 0.1, i / 6.0 + 0.1);
		glPushMatrix();
		glScalef(aux, aux, aux);
		glRotatef(30 * i + rotacion, 0, 1, 0);
		glCallList(individual);
		glPopMatrix();
		glPopAttrib();
	}
	glPopMatrix();

	// Dibujamos las varillas que marcan las horas, minutos y segundos
	// Varilla que marca los segundos
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1, 0, 0);
	glScalef(1, 1, 1);
	glRotatef(anguloSec, 0, 0, 1);
	glCallList(varilla);
	glPopAttrib();
	glPopMatrix();

	// Varilla que marca los minutos
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glScalef(0.7, 0.7, 0.7);
	glRotatef(anguloMin, 0, 0, 1);
	glCallList(varilla);
	glPopAttrib();
	glPopMatrix();

	// Varilla que marca las horas
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(anguloHora, 0, 0, 1);
	glCallList(varilla);
	glPopAttrib();
	glPopMatrix();

	// Dibujamos los triangulos que marcan las horas y los minutos
	for (int i = 0; i < 60; i++) {
		glPushMatrix();
		double angle = i * 6.0 * M_PI / 180;
		glTranslatef(cos(angle), sin(angle), 0.0);
		glRotatef(90 + i * 6, 0, 0, 1);
		glPushAttrib(GL_CURRENT_BIT);
		if (i % 5 == 0) // Es un triangulo hora
		{
			glColor3f(1, 0, 0);
			glScalef(0.06, 0.2, 0.06);
		}
		else // Es un triangulo minuto
		{
			glColor3f(0, 0, 0.3);
			glScalef(0.03, 0.06, 0.03);
		}
		glCallList(triangulo);
		glPopMatrix();
		glPopAttrib();
	}

	// Dibujamos el círculo que rodea el reloj
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glCallList(circulo);
	glPopAttrib();
	glPopMatrix();

	glutSwapBuffers();

}
void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	float razon = (float)w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double distancia, fovy;
	distancia = sqrt(pow(posCam[0], 2) + pow(posCam[1], 2) + pow(posCam[2], 2));
	fovy = (asin(1 / (distancia / 2.0))) * 180 / M_PI;
	gluPerspective(fovy, razon, 1, 10);
}

void onTimer(int valor)
{
	// Calculos temporales
	static int antes = 0;
	int ahora, tiempo_transcurrido;
	ahora = glutGet(GLUT_ELAPSED_TIME);		// Tiempo transcurrido desde el inicio
	tiempo_transcurrido = ahora - antes;	// Tiempo transcurrido desde antes msg

	// Calculos para las animaciones
	seno += 0.2 * tiempo_transcurrido / 1000.0;
	rotacion += 1 % 360;

	tiempoGlobal += tiempo_transcurrido;

	// Controlador de los segundos
	if (tiempoGlobal > 1000) {
		tiempoGlobal %= 1000;
		anguloSec = anguloSec - 6;
		contadorSeg += 1;
	}
	// Controlador de los minutos
	if (contadorSeg == 60) {
		contadorSeg = 0;
		anguloMin = anguloMin - 6;
		contadorMin += 1;
	}
	// Controlador de las horas
	if (contadorMin == 60) {
		contadorMin = 0;
		anguloHora = anguloHora - 30;
		contadorHora += 1;
		contadorHora %= 24;
	}

	antes = ahora;

	glutTimerFunc(1000 / 60, onTimer, 60);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);													// Inicializacion de GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);				// Alta de buffers a usar
	glutInitWindowSize(400, 400);											// Tamanyo inicial de la ventana
	glutCreateWindow(TITULO);												// Creacion de la ventana con su titulo
	init();																	// Inicializacion propia
	std::cout << TITULO << " por Mario Campos Mocholi" << std::endl;		// Mensaje por consola
	glutDisplayFunc(display);												// Alta de la funcion de atencion a display
	glutReshapeFunc(reshape);												// Alta de la funcion de atencion a reshape
	glutTimerFunc(1000 / 60, onTimer, 60);									// Se encola un nuevo timer
	glutMainLoop();															// Puesta en marcha del programa
	return 1;
}