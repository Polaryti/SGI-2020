/***************************************************
ISGI:: Reloj 3D
Autor: Mario Campos Mocholí
***************************************************/

constexpr auto TITULO = "Reloj 3D";
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <freeglut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>
#include <ctime>
#include <cmath>
#include <chrono>

// Angulos y variables de animación
static float anguloSec;
static float anguloMin;
static float anguloHora;
static float rotacion = 0;
static float seno = 0.0f;

// Indices de las listas de dibujo
static GLint estrella;
static GLint triangulo;
static GLint manecilla;
static GLint circulo;

// Coordenadas de la cámara
static int posCam[] = { 3, 3, 5 };

void gen_estrella() {
	estrella = glGenLists(1);
	glNewList(estrella, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; i++) {
		float angulo = (1.0f + (i * 4) % 12) * M_PI / 6;
		glVertex3f(1.0f * cosf(angulo), 1.0f * sinf(angulo), 0.0f);
		glVertex3f(0.7f * cosf(angulo), 0.7f * sinf(angulo), 0.0f);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; i++) {
		float angulo = (3.0f + float((i * 4) % 12)) * M_PI / 6;
		glVertex3f(1.0f * cosf(angulo), 1.0f * sinf(angulo), 0.0);
		glVertex3f(0.7f * cosf(angulo), 0.7f * sinf(angulo), 0.0);
	}
	glEnd();
	glEndList();
}

void gen_manecilla() {
	manecilla = glGenLists(1);
	glNewList(manecilla, GL_COMPILE);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
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

void gen_circulo(float r, int n) {
	circulo = glGenLists(1);
	glNewList(circulo, GL_COMPILE);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < n; i++) {
		float aux = 2.0f * 3.1415926f * ((float)i) / ((float)n);
		float x = r * cosf(aux);
		float y = r * sinf(aux);
		glVertex2f(x, y);
	}
	glEnd();
	glEndList();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3);	// Grosor de las líneas
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Estrellas de David (práctica 4)
	gen_estrella();

	// Varilla de reloj
	gen_manecilla();

	// Triangulo marcador de horas y minutos
	gen_triangulo();

	// Circulo que envuelve el reloj
	gen_circulo(1, 100);
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
	float aux = abs(sinf(seno)) / 4;
	float angle = (90.0f + (anguloSec)) * M_PI / 180;
	glTranslatef(cosf(angle), sinf(angle), 0.0);
	for (int i = 1; i <= 6; i++) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor3f(i / 10.0f + 0.1f, i / 8.0f + 0.1f, i / 6.0f + 0.1f);
		glPushMatrix();
		glScalef(aux, aux, aux);
		glRotatef(30.0f * i + rotacion, 0, 1, 0);
		glCallList(estrella);
		glPopMatrix();
		glPopAttrib();
	}
	glPopMatrix();

	// Dibujamos las manecillas que marcan las horas, minutos y segundos
	// Manecilla que marca los segundos
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1, 0, 0);
	glScalef(1, 1, 1);
	glRotatef(anguloSec, 0, 0, 1);
	glCallList(manecilla);
	glPopAttrib();
	glPopMatrix();

	// Manecilla que marca los minutos
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glScalef(0.7f, 0.7f, 0.7f);
	glRotatef(anguloMin, 0, 0, 1);
	glCallList(manecilla);
	glPopAttrib();
	glPopMatrix();

	// Manecilla que marca las horas
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(anguloHora, 0, 0, 1);
	glCallList(manecilla);

	glPopAttrib();
	glPopMatrix();

	// Dibujamos los triangulos que marcan las horas y los minutos
	for (int i = 0; i < 60; i++) {
		glPushMatrix();
		float angle = i * 6.0f * M_PI / 180;
		glTranslatef(cosf(angle), sinf(angle), 0.0f);
		glRotatef(90.0f + i * 6, 0.0f, 0.0f, 1.0f);
		glPushAttrib(GL_CURRENT_BIT);
		if (i % 5 == 0) // Es un triangulo hora
		{
			glColor3f(1, 0, 0);
			glScalef(0.06f, 0.2f, 0.06f);
		}
		else // Es un triangulo minuto
		{
			glColor3f(0.0f, 0.0f, 0.3f);
			glScalef(0.03f, 0.06f, 0.03f);
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
	fovy = (asin(1 / distancia) * 2.0) * 180 / M_PI;
	gluPerspective(fovy, razon, 1, 10);
}

void onTimer(int valor)
{
	// Variables temporales
	static int antes = 0;
	int ahora, tiempo_transcurrido;
	ahora = glutGet(GLUT_ELAPSED_TIME);		// Tiempo transcurrido desde el inicio
	tiempo_transcurrido = ahora - antes;	// Tiempo transcurrido desde antes msg
	antes = ahora;

	// Calculos para las animaciones
	seno += 0.2f * tiempo_transcurrido / 1000.0f;
	rotacion += 1 % 360;

	// Angulos de las manecillas
	const std::time_t now = std::time(nullptr);
	const std::tm calendar_time = *std::localtime(std::addressof(now));
	anguloSec = 360.0f - (calendar_time.tm_sec * 6);
	anguloMin = 360.0f - (calendar_time.tm_min * 6);
	anguloHora = 360.0f - (calendar_time.tm_hour * 30);

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