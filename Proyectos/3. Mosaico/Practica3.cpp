/***************************************************
ISGI:: Mosaico
***************************************************/
#define TITULO "Mosaico"
#include <iostream> // Biblioteca de entrada salida
#define _USE_MATH_DEFINES
#include <math.h>
#include <freeglut.h> // Biblioteca grafica

static GLint indiviudal;
static GLint doble;

void estrella() 
{
	indiviudal = glGenLists(1);
	doble = glGenLists(1);
	glNewList(indiviudal, GL_COMPILE);
	glColor3f(0.0, 0.0, 0.3);

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

	glNewList(doble, GL_COMPILE);
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	glCallList(indiviudal);
	glRotatef(30, 0, 0, 1);	 
	glScalef(0.4, 0.4, 0.4); 
	glCallList(indiviudal);
	glPopMatrix();
	glEndList(); // Cierra la lista
}

void display()
// Funcion de atencion al dibujo
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); // Borra la pantalla
	glMatrixMode(GL_MODELVIEW); // Selecciona la modelview
	glLoadIdentity(); // Carga la matriz identidad
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	estrella();

	//estrella superior izquierda
	glPushMatrix();
	glTranslatef(-0.5, 0.5, 0);
	glRotatef(15, 0, 0, 1);
	glCallList(doble);
	glPopMatrix();

	//estrella superior derecha
	glPushMatrix();
	glTranslatef(0.5, 0.5, 0);
	glRotatef(-15, 0, 0, 1);
	glCallList(doble);
	glPopMatrix();

	//estrella inferior izquierda
	glColor3f(0.0, 0.0, 0.8);
	glPushMatrix();
	glTranslatef(-0.5, -0.5, 0);
	glRotatef(-15, 0, 0, 1);
	glCallList(doble);
	glPopMatrix();

	//estrella inferior derecha
	glPushMatrix();
	glTranslatef(0.5, -0.5, 0);
	glRotatef(15, 0, 0, 1);
	glCallList(doble);
	glPopMatrix();

	glFlush();
}
void reshape(GLint w, GLint h)
// Funcion de atencion al redimensionamiento
{
}
void main(int argc, char** argv)
// Programa principal
{
	glutInit(&argc, argv); // Inicializacion de GLUT
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Alta de buffers a usar
	glutInitWindowSize(400, 400); // Tamanyo inicial de la ventana
	glutInitWindowPosition(0, 0);
	glutCreateWindow(TITULO); // Creacion de la ventana con su titulo
	std::cout << TITULO << " running" << std::endl; // Mensaje por consola
	glutDisplayFunc(display); // Alta de la funcion de atencion a display
	glutReshapeFunc(reshape); // Alta de la funcion de atencion a reshape
	glutMainLoop(); // Puesta en marcha del programa
}