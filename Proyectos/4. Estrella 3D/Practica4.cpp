/***************************************************
ISGI:: Estrella 3D
***************************************************/
#define TITULO "Estrella 3D"
#include <iostream> // Biblioteca de entrada salida
#define _USE_MATH_DEFINES
#include <math.h>
#include <freeglut.h> // Biblioteca grafica

static GLint individual;

void estrella()
{
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

void display()
// Funcion de atencion al dibujo
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Borra la pantalla
	glMatrixMode(GL_MODELVIEW); // Selecciona la modelview
	glLoadIdentity(); // Carga la matriz identidad
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	gluLookAt(3, 3, 1, 0, 0, 0, 0, 1, 0); // Situa la camara

	estrella();

	for (int i = 1; i <= 6; i++) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor3f(0.2 * (i / 2.0), 0.1 * (i / 1.5), 0.3 * (i / 1.7));
		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(30 * i, 0, 1, 0);
		glCallList(individual);
		glPopMatrix();
		glPopAttrib();
	}

	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(0.8, 0.0, 0.0);
	glutWireSphere(1, 20, 20);
	glPopAttrib();

	glFlush();
}
void reshape(GLint w, GLint h)
// Funcion de atencion al redimensionamiento
{
	// Usamos toda el area de dibujo
	glViewport(0, 0, w, h);
	// Definimos la camara (matriz de proyeccion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	float distancia = sqrt(pow(3, 2) + pow(3, 2) + pow(1, 2));
	float fovy = (asin(1 / (distancia / 1.95))) * 180 / M_PI;
	gluPerspective(fovy, razon, 1, 10);
}
void main(int argc, char** argv)
// Programa principal
{
	glutInit(&argc, argv); // Inicializacion de GLUT
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); // Alta de buffers a usar
	glEnable(GL_DEPTH_TEST);
	glutInitWindowSize(400, 400); // Tamanyo inicial de la ventana
	glutInitWindowPosition(0, 0);
	glutCreateWindow(TITULO); // Creacion de la ventana con su titulo
	std::cout << TITULO << " running" << std::endl; // Mensaje por consola
	glutDisplayFunc(display); // Alta de la funcion de atencion a display
	glutReshapeFunc(reshape); // Alta de la funcion de atencion a reshape
	glutMainLoop(); // Puesta en marcha del programa
}