/***************************************************
ISGI:: Videojuego de conduccion
Autor: Mario Campos Mocholí
***************************************************/

#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include <freeglut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>
#include <ctime>
#include <cmath>
#include <chrono>
#include <Utilidades.h>
#include <Mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "Winmm.lib")
using namespace std;

void control_texturas();
void control_luces();
void init_particulas(int i);

// Titulo de la practica
constexpr auto TITULO = "Videojuego de conduccion";

// Coordenadas de la cámara
float pos_cam[3] = { 0, 1.1, 0 };
float ant_pos = pos_cam[0];
int max_vision = 90;

// Velocidad y dirección del vehiculo
float velocidad = 0;
float angulo = 0;
float giro = 0;
float direccion[3] = { 1, 1, 0 };

// Propiedades del trazado del circuito
float vector_t[2] = { 1,0 };
float vector_n[2] = { 0,1 };
int numero_quads = 50;
int longitud = 4;
float amplitud = 6;
float periodo = 20;
float anchura = 4.5;
float factor_olas = 0;
float factor_ola = 0.1;

// Propiedades de los carteles
float pos_cartel_x = 35;

// Texturas
GLuint textura_manyana, textura_tarde, textura_noche, fondo, textura_agua, textura_cartel, textura_poste, textura_hud, textura_volante;
GLuint textura_carretera;
static GLfloat material_difuso[3] = { 0.8,0.8,0.8 };
static GLfloat material_especular[3] = { 0.8,0.8,0.8 };

// Luces
static float distancia_farolas = 20;
static int inicio_farolas[4] = { distancia_farolas, distancia_farolas * 2 , distancia_farolas * 3, distancia_farolas * 4 };

// MEJORAS
// Modo simple o completo
static enum Modo { COMPLETO, SIMPLE } modo;
// Mañna, tarde o noche
static enum Hora { MANYANA, TARDE, NOCHE } hora_dia;
// Visibilidad en carretera (niebla)
static enum Visibilidad { BUENA, REGULAR, MALA } visibilidad;
GLfloat niebla_manyana[] = { 0.7,0.7,0.7,0.4 };
GLfloat niebla_tarde[] = { 0.8,0.7,0.6,0.3 };
GLfloat niebla_noche[] = { 0,0.05,0.29,0.3 };
// HUD
static enum Hud { OCULTA, COMPLETA } hud;
// Sistema de particulas (basado en https://gist.github.com/thaenor/4d9531cc9a7d1c34b998)
static enum Tiempo { DESPEJADO, LLUVIA, NIEVE } tiempo;
float desaceleracion = 1.0;
float velocidad_caida = 0.0;
float zoom = -13;
typedef struct {
	// Vida
	bool vivo;
	float esperanza;
	float factor_desvanecimiento;
	// Color
	float red;
	float green;
	float blue;
	// Posicion
	float pos_x;
	float pos_y;
	float pos_z;
	// Velocidad
	float vel;
	float gravedad;
} particles;
particles par_sys[1000];
// Camara de depuración
bool camara_depuracion = false;
// Captura
float cap_w = 960;
float cap_h = 540;
// Giro dinamico
bool giro_dinamico = false;
// Musica
bool musica = false;
LPCWSTR a = L"open \"musica.mp3\" type mpegvideo alias mp3";
LPCWSTR b = L"play musica.mp3 repeat";
LPCWSTR c = L"pause musica.mp3";
LPCWSTR d = L"resume musica.mp3";

LPCWSTR e = L"open \"mar.mp3\" type mpegvideo alias mp3";
LPCWSTR e1 = L"play mar.mp3 repeat";
LPCWSTR e2 = L"pause mar.mp3";
LPCWSTR e3 = L"resume mar.mp3";
LPCWSTR f = L"open \"nieve.mp3\" type mpegvideo alias mp3";
LPCWSTR f1 = L"play nieve.mp3 repeat";
LPCWSTR f2 = L"pause nieve.mp3";
LPCWSTR f3 = L"resume nieve.mp3";
LPCWSTR g = L"open \"lluvia.mp3\" type mpegvideo alias mp3";
LPCWSTR g1 = L"play lluvia.mp3 repeat";
LPCWSTR g2= L"pause lluvia.mp3";
LPCWSTR g3 = L"resume lluvia.mp3";


void init()
{
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texturas
	control_texturas();

	// Luces
	control_luces();

	// Particulas
	for (int i = 0; i < 1000; i++) {
		init_particulas(i);
	}

	// Musica
	mciSendString(a, NULL, 0, NULL);
	mciSendString(b, NULL, 0, NULL);
	mciSendString(c, NULL, 0, NULL);
	mciSendString(e, NULL, 0, NULL);
	mciSendString(e1, NULL, 0, NULL);
	mciSendString(f, NULL, 0, NULL);
	mciSendString(f1, NULL, 0, NULL);
	mciSendString(f2, NULL, 0, NULL);
	mciSendString(g, NULL, 0, NULL);
	mciSendString(g1, NULL, 0, NULL);
	mciSendString(g2, NULL, 0, NULL);

	cout << "CONTROL DE VELOCIDAD: Para aumentar pulse \"flecha arriba\", para disminuir pulse \"flecha abajo\"." << endl;
	cout << "GIRO DEL VEHICULO: Para rotar a la derecha pulse \"flecha derecha\", para rotar a la izquierda pulse \"flecha izquierda\"." << endl;
	cout << "MODO DE RENDERIZACION: Alterne con \"S/s\" entre modo simple y completo." << endl;
	cout << "HORA DEL DIA: Alterne con \"L/l\" la hora del dia entre por la manyana, tarde o noche." << endl;
	cout << "VISIBILIDAD DE LA CARRETERA: Alterne con \"N/n\" entre visibilidad buena, regular o mala." << endl;
	cout << "MOSTRAR INTERFAZ: Alterne con \"C/c\" para mostrar u ocultar la interfaz de conduccion." << endl;
	cout << "TIEMPO ATMOSFERICO: Alterne con \"T/t\" entre despejado, lluvia o nieve." << endl;
	cout << "CAMARA DEPURACION: Alterne con \"D/d\" entre la camara normal o la camara depuracion, situada arriba para comprobar si la carretera se genera dinamicamente." << endl;
	cout << "CAPTURA: Pulsa \"P/p\" para realizar una captura de pantalla." << endl;
	cout << "GIRO DINAMICO: Alterne con \"G/g\" para activar o desactivar el giro dinamico. Este tiene en cuenta la velocidad actual del vehiculo para girar." << endl;
	cout << "MUSICA: Alterne con \"M/m\" para activar o desactivar la musica." << endl;
	cout << "NOTA IMPORTANTE: El coche que se utiliza es electrico y sumado a que el asfalto y los neumaticos absorben el ruido, este no hace nada de ruido." << endl;
}

void init_particulas(int i) {
	par_sys[i].vivo = true;
	par_sys[i].esperanza = 1.0;
	par_sys[i].factor_desvanecimiento = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].pos_x = (float)(rand() % 21) - 5;
	par_sys[i].pos_y = 10.0;
	par_sys[i].pos_z = (float)(rand() % 21) - 5;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocidad_caida;
	par_sys[i].gravedad = -0.8;

}

float funcion_trazado(float x, float amplitud, float periodo) {
	return amplitud * sin(x / periodo);
}

float derivada_trazado(float x, float amplitud, float periodo) {
	return amplitud * cos(x / periodo) / periodo;
}

void gen_suelo() {
	GLfloat v0[] = { 0,-0.005,0 };
	GLfloat v1[] = { 0,-0.005,0 };
	GLfloat v2[] = { 0,-0.005,0 };
	GLfloat v3[] = { 0,-0.005,0 };
	int summ = 10;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
	glMaterialf(GL_FRONT, GL_SHININESS, 1);

	glBindTexture(GL_TEXTURE_2D, textura_agua);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	for (int i = pos_cam[1] + ant_pos - 3; i < pos_cam[0] + numero_quads * summ; i += summ) {
		v0[0] = i;
		v1[0] = i;
		v2[0] = i + summ;
		v3[0] = i + summ;
		for (int j = -40; j < 40; j+= summ) {
			v0[2] = j;
			v1[2] = j + summ;
			v2[2] = j + summ;
			v3[2] = j;
			
			quad(v0, v1, v2, v3, 30, 30);
		}
	}
}

void gen_circuito() {
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
	glMaterialf(GL_FRONT, GL_SHININESS, 3);

	glBindTexture(GL_TEXTURE_2D, textura_carretera);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	int cont = 0;
	for (int i = pos_cam[1] + ant_pos - 1; i < pos_cam[0] + numero_quads * longitud; i += longitud) {
		cont++;
		glPushMatrix();
		float f_x = funcion_trazado(i, amplitud, periodo);
		vector_t[1] = derivada_trazado(i, amplitud, periodo);
		vector_n[0] = -vector_t[1];
		float aux = sqrt(pow(vector_n[0], 2) + pow(vector_n[1], 2));
		vector_n[0] = vector_n[0] / aux;
		vector_n[1] = vector_n[1] / aux;
		float v2[3] = { (float)i - vector_n[0] * anchura, 0, f_x - vector_n[1] * anchura };
		float v3[3] = { (float)i + vector_n[0] * anchura, 0, f_x + vector_n[1] * anchura };

		f_x = funcion_trazado(i + longitud, amplitud, periodo);
		vector_t[1] = derivada_trazado(i + longitud, amplitud, periodo);
		vector_n[0] = -vector_t[1];
		aux = sqrt(pow(vector_n[0], 2) + pow(vector_n[1], 2));
		vector_n[0] = vector_n[0] / aux;
		vector_n[1] = vector_n[1] / aux;
		float v0[3] = { (float)i + longitud + vector_n[0] * anchura, 0, f_x + vector_n[1] * anchura };
		float v1[3] = { (float)i + longitud - vector_n[0] * anchura, 0, f_x - vector_n[1] * anchura };

		quad(v0, v1, v2, v3, 30, 30);

		glPopMatrix();
	}
	
}

void gen_cartel() {
	if (pos_cartel_x < pos_cam[0]) {
		pos_cartel_x += 100;
	}

	float f_x = funcion_trazado(pos_cartel_x, amplitud, periodo);
	vector_t[1] = derivada_trazado(pos_cartel_x, amplitud, periodo);
	vector_n[0] = -vector_t[1];
	float aux = sqrt(pow(vector_n[0], 2) + pow(vector_n[1], 2));
	vector_n[0] = vector_n[0] / aux;
	vector_n[1] = vector_n[1] / aux;
	float v1[3] = { pos_cartel_x,4.5,f_x + anchura };
	float v2[3] = { pos_cartel_x,4.5,f_x - anchura };
	float v3[3] = { pos_cartel_x,2.5,f_x - anchura };
	float v4[3] = { pos_cartel_x,2.5,f_x + anchura };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
	glMaterialf(GL_FRONT, GL_SHININESS, 3);
	glBindTexture(GL_TEXTURE_2D, textura_cartel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	quadtex(v3, v4, v1, v2, 0, 1, 0, 1, 25, 25);

	f_x = funcion_trazado(pos_cartel_x - 3.0, amplitud, periodo);
	vector_t[1] = derivada_trazado(pos_cartel_x - 3.0, amplitud, periodo);
	vector_n[0] = -vector_t[1];
	aux = sqrt(pow(vector_n[0], 2) + pow(vector_n[1], 2));
	vector_n[0] = vector_n[0] / aux;
	vector_n[1] = vector_n[1] / aux;
	f_x = funcion_trazado(pos_cartel_x + 0.1, amplitud, periodo);

	// Poste derecho
	float v12[3] = { pos_cartel_x + 0.1,4.5,f_x + anchura };
	float v22[3] = { pos_cartel_x + 0.1,4.5,f_x + anchura + 1 };
	float v32[3] = { pos_cartel_x + 0.1,0,f_x + anchura + 1 };
	float v42[3] = { pos_cartel_x + 0.1,0,f_x + anchura };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
	glMaterialf(GL_FRONT, GL_SHININESS, 3);
	glBindTexture(GL_TEXTURE_2D, textura_poste);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	quadtex(v42, v32, v22, v12, 0, 1, 0, 1);

	// Poste izquierdo
	float v13[3] = { pos_cartel_x + 0.1,4.5,f_x - anchura };
	float v23[3] = { pos_cartel_x + 0.1,4.5,f_x - anchura - 1 };
	float v33[3] = { pos_cartel_x + 0.1,0,f_x - anchura - 1 };
	float v43[3] = { pos_cartel_x + 0.1,0,f_x - anchura };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
	glMaterialf(GL_FRONT, GL_SHININESS, 3);
	glBindTexture(GL_TEXTURE_2D, textura_poste);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	quadtex(v13, v23, v33, v43, 0, 1, 0, 1);
}

void gen_farolas() {
	GLfloat direccion_foco[] = { 0.0,-1.0,0.0 };
	int lights[4] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5 };
	float f_x, aux;

	for (int i = 0; i < 4; i++) {
		f_x = funcion_trazado(inicio_farolas[i], amplitud, periodo);
		vector_t[1] = derivada_trazado(inicio_farolas[i], amplitud, periodo);
		vector_n[0] = -vector_t[1];
		aux = sqrt(pow(vector_n[0], 2) + pow(vector_n[1], 2));
		vector_n[0] = vector_n[0] / aux;
		vector_n[1] = vector_n[1] / aux;
		GLfloat pos[] = { inicio_farolas[i], 4.0, f_x + vector_n[1] * anchura - anchura, 1 };

		if (pos_cam[0] > inicio_farolas[i] + 4) {
			inicio_farolas[i] += distancia_farolas * 4;
		}

		glPushMatrix();
		glLightfv(lights[i], GL_POSITION, pos);
		glLightfv(lights[i], GL_SPOT_DIRECTION, direccion_foco);
		glPopMatrix();
	}
}

void gen_lluvia() {
	float x, y, z;
	for (int i = 0; i < 1000; i += 2) {
		if (par_sys[i].vivo == true) {
			x = par_sys[i].pos_x;
			y = par_sys[i].pos_y;
			z = par_sys[i].pos_z + zoom;

			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
			glMaterialf(GL_FRONT, GL_SHININESS, 3);

			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			par_sys[i].pos_y += par_sys[i].vel / (desaceleracion * 1000);
			par_sys[i].vel += par_sys[i].gravedad;
			par_sys[i].esperanza -= par_sys[i].factor_desvanecimiento;

			if (par_sys[i].pos_y <= -2) {
				par_sys[i].esperanza = -1.0;
			}

			if (par_sys[i].esperanza < 0.0) {
				init_particulas(i);
			}
		}
	}
}

void gen_nieve() {
	float x, y, z;
	for (int i = 0; i < 1000; i = i + 2) {
		if (par_sys[i].vivo == true) {
			x = par_sys[i].pos_x;
			y = par_sys[i].pos_y;
			z = par_sys[i].pos_z + zoom;

			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
			glMaterialf(GL_FRONT, GL_SHININESS, 3);

			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.1, 16, 16);
			glPopMatrix();

			par_sys[i].pos_y += par_sys[i].vel / (desaceleracion * 1000);
			par_sys[i].vel += par_sys[i].gravedad;

			par_sys[i].esperanza -= par_sys[i].factor_desvanecimiento;

			if (par_sys[i].pos_y <= -2) {
				par_sys[i].esperanza = -1.0;
			}

			if (par_sys[i].esperanza < 0.0) {
				init_particulas(i);
			}
		}
	}
}

void control_texturas() {
	// Fondos 
	glGenTextures(1, &textura_manyana);
	glBindTexture(GL_TEXTURE_2D, textura_manyana);
	loadImageFile((char*)"cieloManyana.jpg");

	glGenTextures(1, &textura_tarde);
	glBindTexture(GL_TEXTURE_2D, textura_tarde);
	loadImageFile((char*)"cieloTarde.jpg");

	glGenTextures(1, &textura_noche);
	glBindTexture(GL_TEXTURE_2D, textura_noche);
	loadImageFile((char*)"cieloNoche.jpg");

	fondo = textura_manyana;

	// Otros
	glGenTextures(1, &textura_carretera);
	glBindTexture(GL_TEXTURE_2D, textura_carretera);
	loadImageFile((char*)"carretera.jpg");

	glGenTextures(1, &textura_agua);
	glBindTexture(GL_TEXTURE_2D, textura_agua);
	loadImageFile((char*)"agua.jpg");

	// Cartel
	glGenTextures(1, &textura_cartel);
	glBindTexture(GL_TEXTURE_2D, textura_cartel);
	loadImageFile((char*)"cartel.jpg");

	// Poste
	glGenTextures(1, &textura_poste);
	glBindTexture(GL_TEXTURE_2D, textura_poste);
	loadImageFile((char*)"poste.jpg");

	// HUD
	glGenTextures(1, &textura_hud);
	glBindTexture(GL_TEXTURE_2D, textura_hud);
	loadImageFile((char*)"hud.png");
}

void control_visibilidad() {
	switch (visibilidad) {
	case BUENA:
		glDisable(GL_FOG);
		break;
	case REGULAR:
		glEnable(GL_FOG);
		switch (hora_dia) {
		case MANYANA:
			glFogfv(GL_FOG_COLOR, niebla_manyana);
			break;
		case TARDE:
			glFogfv(GL_FOG_COLOR, niebla_tarde);
			break;
		case NOCHE:
			glFogfv(GL_FOG_COLOR, niebla_noche);
			break;
		}
		glFogf(GL_FOG_DENSITY, 0.01);
		break;
	case MALA:
		glEnable(GL_FOG);
		switch (hora_dia) {
		case MANYANA:
			glFogfv(GL_FOG_COLOR, niebla_manyana);
			break;
		case TARDE:
			glFogfv(GL_FOG_COLOR, niebla_tarde);
			break;
		case NOCHE:
			glFogfv(GL_FOG_COLOR, niebla_noche);
			break;
		}
		glFogf(GL_FOG_DENSITY, 0.1);
		break;
	default:
		break;
	}
}

void control_fondo() {
	// Basado, principalmente, en http://www.songho.ca/opengl/gl_cylinder.html
	float x, z, seccion = 0, radio = max_vision;
	float x_anterior = radio * sin(rad(0)) + pos_cam[0];
	float z_anterior = radio * cos(rad(0)) + pos_cam[2];
	float aux = 0.1;

	// Cilindro en 20 secciones, no 10 como indicaba el seminario
	for (int i = 18; i <= 324; i += 18) {
		x = radio * sin(rad(i)) + pos_cam[0];
		z = radio * cos(rad(i)) + pos_cam[2];
		float v0[3] = { x_anterior,40,z_anterior };
		float v1[3] = { x,40,z };
		float v2[3] = { x,-20,z };
		float v3[3] = { x_anterior,-20,z_anterior };

		glMaterialfv(GL_FRONT, GL_DIFFUSE, material_difuso);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_especular);
		glMaterialf(GL_FRONT, GL_SHININESS, 3);
		glBindTexture(GL_TEXTURE_2D, fondo);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		quadtex(v0, v1, v2, v3, seccion, seccion + aux, 0, 1, 50, 50);
		x_anterior = x;
		z_anterior = z;
		seccion += aux;
	}
}

void control_hora_dia() {

	switch (hora_dia) {
	case MANYANA:
		glDisable(GL_LIGHTING);
		fondo = textura_manyana;
		break;
	case TARDE:
		gen_farolas();
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT6);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glEnable(GL_LIGHT4);
		glEnable(GL_LIGHT5);
		glEnable(GL_LIGHT7);

		fondo = textura_tarde;

		break;
	case NOCHE:
		gen_farolas();
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT6);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glEnable(GL_LIGHT4);
		glEnable(GL_LIGHT5);
		glEnable(GL_LIGHT7);

		fondo = textura_noche;
		break;
	default:
		break;
	}

	control_fondo();
}

void control_modo()
{
	switch (modo) {
	case COMPLETO:
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1.0, 1.0, 1.0);
		break;
	case SIMPLE:
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);
		break;
	default:
		break;
	}
}

void control_luces() {
	// LIGHT0: Luna 
	float ambiental0[] = { 0.2,0.29,0.68,1.0 };
	float difusa0[] = { 0.05,0.05,0.1,1.0 };
	float especular0[] = { 0.0,0.0,0.0,1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambiental0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, especular0);
	float posicion0[4] = { 0, 10, 0, 0 };

	glLightfv(GL_LIGHT0, GL_POSITION, posicion0);

	// LIGHT6: Atardecer 
	float ambiental6[] = { 1,0.64,0.43,1.0 };
	float difusa6[] = { 0.05,0.05,0.1,1.0 };
	float especular6[] = { 0.05,0.05,0.1,1.0 };
	glLightfv(GL_LIGHT6, GL_AMBIENT, ambiental6);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, difusa6);
	glLightfv(GL_LIGHT6, GL_SPECULAR, especular6);
	float posicion6[4] = { 0, 7,0 , 0 };
	glLightfv(GL_LIGHT6, GL_POSITION, posicion6);

	// LIGHT1/7: Focal
	float ambiental1[] = { 0.2,0.2,0.2 };
	float difusa1[] = { 0.7,0.7,0.7 };
	float especular1[] = { 0.3,0.3,0.3 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambiental1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, especular1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 10);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 30);
	GLfloat posicion1[] = { 0.3,1,-0.5,1 };
	GLfloat direccion1[] = { 0.2,-0.8,-1.5 };
	glLightfv(GL_LIGHT1, GL_POSITION, posicion1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direccion1);

	glLightfv(GL_LIGHT7, GL_AMBIENT, ambiental1);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, difusa1);
	glLightfv(GL_LIGHT7, GL_SPECULAR, especular1);
	glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, 10);
	glLightf(GL_LIGHT7, GL_SPOT_EXPONENT, 30);
	GLfloat posicion7[] = { -0.3,1,-0.5,1 };
	GLfloat direccion7[] = { -0.2,-0.8,-1.5 };
	glLightfv(GL_LIGHT7, GL_POSITION, posicion7);
	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, direccion7);

	// LIGHT2-5: Farolas
	float ambientalF[] = { 0,0,0 };
	float difusaF[] = { 0.5,0.5,0.2 };
	float especularF[] = { 0,0,0 };
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientalF);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, difusaF);
	glLightfv(GL_LIGHT2, GL_SPECULAR, especularF);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 60);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 10);

	glLightfv(GL_LIGHT3, GL_AMBIENT, ambientalF);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, difusaF);
	glLightfv(GL_LIGHT3, GL_SPECULAR, especularF);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 60);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 10);

	glLightfv(GL_LIGHT4, GL_AMBIENT, ambientalF);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, difusaF);
	glLightfv(GL_LIGHT4, GL_SPECULAR, especularF);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 60);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 10);

	glLightfv(GL_LIGHT5, GL_AMBIENT, ambientalF);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, difusaF);
	glLightfv(GL_LIGHT5, GL_SPECULAR, especularF);
	glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 60);
	glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 10);
}

void control_tiempo() {
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();

	switch (tiempo) {
	case DESPEJADO:
		break;
	case LLUVIA:
		gen_lluvia();
		break;
	case NIEVE:
		gen_nieve();
		break;
	default:
		break;
	}
	glPopMatrix();
	glPopAttrib();
}

void control_hud() {
	float v0[3] = { -1,-1,0 };
	float v1[3] = { 1,-1,0 };
	float v2[3] = { 1,1.1,0 };
	float v3[3] = { -1,1.1,0 };

	switch (hud) {
	case OCULTA:
		break;
	case COMPLETA:
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

		glBindTexture(GL_TEXTURE_2D, textura_hud);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		quadtex(v0, v1, v2, v3);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		break;
	}
}

/* De Utilidades.h, copiado y modificado para alargar los ejes */
void ejes_mejorados() {
	//Construye la Display List compilada de una flecha vertical
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	//Brazo de la flecha
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);
	glEnd();
	//Punta de la flecha
	glPushMatrix();
	glTranslatef(0, 10, 0);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0.0, 0.0, -1 / 10.0);
	glutSolidCone(1 / 50.0, 1 / 10.0, 10, 1);
	glPopMatrix();
	glEndList();

	//Ahora construye los ejes
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	//Eje X en rojo
	glColor3fv(ROJO);
	glPushMatrix();
	glRotatef(-90, 0, 0, 1);
	glCallList(id);
	glPopMatrix();
	//Eje Y en verde
	glColor3fv(VERDE);
	glPushMatrix();
	glCallList(id);
	glPopMatrix();
	//Eje Z en azul
	glColor3fv(AZUL);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glCallList(id);
	glPopMatrix();
	//Esferita en el origen
	glColor3f(0.5, 0.5, 0.5);
	glutSolidSphere(0.05, 8, 8);
	glPopAttrib();
	//Limpieza
	glDeleteLists(id, 1);
}

void control_interfaz_avanzada() {
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	// Velocidad
	texto(10, cap_h * 0.96, (char*)"Velocidad: ", NEGRO, GLUT_BITMAP_HELVETICA_18, false);
	stringstream aux;
	aux << velocidad << "m/s";
	texto(102, cap_h * 0.96, (char*)aux.str().c_str(), ROJO, GLUT_BITMAP_HELVETICA_18, false);

	// Dirección
	texto(16, cap_h * 0.92, (char*)"Direccion: ", NEGRO, GLUT_BITMAP_HELVETICA_18, false);
	stringstream aux1;
	aux1 << deg(direccion[2]) << "g";
	texto(102, cap_h * 0.92, (char*)aux1.str().c_str(), ROJO, GLUT_BITMAP_HELVETICA_18, false);

	// GPS
	texto(36, cap_h * 0.88, (char*)"Coord.: ", NEGRO, GLUT_BITMAP_HELVETICA_18, false);
	stringstream aux2;
	aux2 << "[" << pos_cam[0] << ", " << pos_cam[2] << "]";
	texto(102, cap_h * 0.88, (char*)aux2.str().c_str(), ROJO, GLUT_BITMAP_HELVETICA_18, false);

	glPopAttrib();
	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// MEJORA: Modo
	control_modo();

	// MEJORA: Tiempo
	control_tiempo();

	

	// Colocar la camara
	if (camara_depuracion) {
		gluLookAt(pos_cam[0], 20, static_cast<double>(pos_cam[2]) - 1,
			static_cast<double>(direccion[0]) + pos_cam[0], 1, static_cast<double>(direccion[2]) + pos_cam[2],
			0, 1, 0);

		glLineWidth(3);
		ejes_mejorados();
		glLineWidth(1);
	}
	else {
		gluLookAt(pos_cam[0], pos_cam[1], pos_cam[2],
			static_cast<double>(direccion[0]) + pos_cam[0], 1.1, static_cast<double>(direccion[2]) + pos_cam[2],
			0, 1, 0);
	}

	

	// Mejora: Hora del dia
	control_hora_dia();

	// MEJORA: Visibilidad
	control_visibilidad();

	// Generar suelo
	if (pos_cam[0] >= ant_pos + longitud) {
		ant_pos = pos_cam[0];
	}
	glPushMatrix();
	glTranslatef(factor_olas, 0, 0);
	gen_suelo();
	glPopMatrix();
	

	// Generar circuito
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1, 1, 1);
	glPushMatrix();
	gen_circuito();
	glPopAttrib();
	glPopMatrix();

	// Generar carteles
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1, 1, 1);
	gen_cartel();
	glPopAttrib();

	// MEJORA: HUD
	glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1, 1, 1);
	glEnable(GL_BLEND);
	control_hud();
	glDisable(GL_BLEND);
	glPopAttrib();

	control_interfaz_avanzada();


	glutSwapBuffers();


}

#pragma warning(disable:4996)
void onKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 's':
	case 'S':
		if (modo == COMPLETO) {
			modo = SIMPLE;
		}
		else {
			modo = COMPLETO;
		}
		break;
	case 'l':
	case 'L':
		if (hora_dia == MANYANA) {
			hora_dia = TARDE;
		}
		else if (hora_dia == TARDE) {
			hora_dia = NOCHE;
		}
		else {
			hora_dia = MANYANA;
		}
		break;
	case 'n':
	case 'N':
		if (visibilidad == BUENA) {
			visibilidad = REGULAR;
		}
		else if (visibilidad == REGULAR) {
			visibilidad = MALA;
		}
		else {
			visibilidad = BUENA;
		}
		break;
	case 'c':
	case 'C':
		switch (hud) {
		case OCULTA:
			hud = COMPLETA;
			break;
		case COMPLETA:
			hud = OCULTA;
			break;
		}
		break;
	case 't':
	case 'T':
		switch (tiempo) {
		case DESPEJADO:
			tiempo = LLUVIA;
			mciSendString(g3, NULL, 0, NULL);
			break;
		case LLUVIA:
			tiempo = NIEVE;
			mciSendString(g2, NULL, 0, NULL);
			mciSendString(f3, NULL, 0, NULL);
			break;
		case NIEVE:
			tiempo = DESPEJADO;
			mciSendString(f2, NULL, 0, NULL);
			break;
		}
		break;
	case 'd':
	case 'D':
		camara_depuracion = !camara_depuracion;
		break;
	case 'p':
	case 'P':
		saveScreenshot((char*)"Captura.png", cap_w, cap_h);
		break;
	case 'g':
	case 'G':
		giro_dinamico = !giro_dinamico;
		break;
	case 'm':
	case 'M':
		if (musica) {
			mciSendString(c, NULL, 0, NULL);
			musica = false;
		}
		else {
			mciSendString(d, NULL, 0, NULL);
			musica = true;
		}
		break;
	default:
		break;
	}
}

void onSpecialKey(int specialKey, int x, int y)
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
		if (giro_dinamico) {
			direccion[2] += rad(0.05 * velocidad);
		}
		else {
			direccion[2] += rad(0.1);
		}
		break;
	case GLUT_KEY_LEFT:
		if (giro_dinamico) {
			direccion[2] -= rad(0.05 * velocidad);
		}
		else {
			direccion[2] -= rad(0.1);
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
	cap_w = w;
	cap_h = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, max_vision);
}

void onTimer(int valor)
{
	// Soluciona error de precisión de variables de coma flotante
	if (velocidad <= 0) {
		velocidad = 0;
	}

	// Actualizar posición camara
	static int antes = 0;
	int ahora, tiempo_transcurrido;
	ahora = glutGet(GLUT_ELAPSED_TIME); //Tiempo transcurrido desde el inicio
	tiempo_transcurrido = ahora - antes; //Tiempo transcurrido desde antes msg

	pos_cam[0] += static_cast<double>(velocidad) * direccion[0] * (tiempo_transcurrido / 1000.0);
	pos_cam[2] += static_cast<double>(velocidad) * direccion[2] * (tiempo_transcurrido / 1000.0);

	factor_olas += (tiempo_transcurrido / 1000.0) * factor_ola;
	if (factor_olas >= 3) {
		factor_ola = -factor_ola;
	}
	else if (factor_olas < 0) {
		factor_ola = abs(factor_ola);
	}

	stringstream titulo;
	titulo << "Velocidad: " << velocidad << "m/s";
	glutSetWindowTitle(titulo.str().c_str());

	antes = ahora;

	glutTimerFunc(1000 / 60, onTimer, 60);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);													// Inicializacion de GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);				// Alta de buffers a usar
	glutInitWindowSize(cap_w, cap_h);										// Tamanyo inicial de la ventana
	glutCreateWindow(NULL);													// Creacion de la ventana con su titulo
	cout << "Juego de conduccion por Mario Campos Mocholi" << endl;			// Mensaje por consola
	glutDisplayFunc(display);												// Alta de la funcion de atencion a display
	glutReshapeFunc(reshape);												// Alta de la funcion de atencion a reshape
	glutSpecialFunc(onSpecialKey);											// Alta de la funcion de atencion al teclado
	glutKeyboardFunc(onKey);												// Alta de la funcion de atencion al teclado
	glutTimerFunc(1000 / 60, onTimer, 60);									// Se encola un nuevo timer
	init();																	// Inicializacion propia

	glutMainLoop();															// Puesta en marcha del programa
	return 1;
}