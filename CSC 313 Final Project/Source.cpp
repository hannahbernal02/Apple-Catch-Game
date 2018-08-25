//from http://cs.lmu.edu/~ray/notes/openglexamples/
//referenced score from http://rohithsreeragam.blogspot.com/2012/12/how-to-display-scorevariable-value-in.html
//sounds from https://freesound.org/

#define _CRT_SECURE_NO_DEPRECATE
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <windows.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <stdio.h>

#include <GL\glut.h>
#include <cmath>
#include <iostream>
#include <vector>   
#include <cmath>
#include <random>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include <..\..\glm\glm.hpp>
#include <..\..\glm\vec4.hpp>
//#include <..\..\glm\mat4x4.hpp>
//#include <..\..\glm\matrix_transform.hpp>
//#include <..\..\glm\ext.hpp>

#include "loadbitmap.h"
#include "printscore.h"

//shadow matrix (unused)
glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

using namespace std;


/*This program uses OpenGL to create a simple catching game.
The goal for the user is to collect the falling apples to score points
and avoid the obstacles to prevent from losing a life.
Additional files include bmp images, sound files, and a text file.*/

// Colors
GLfloat WHITE[] = { 1, 1, 1 };
GLfloat BLACK[] = { 0, 0, 0 };
GLfloat RED[] = { 1, 0, 0 };
GLfloat GREEN[] = { 0, 1, 0 };
GLfloat MAGENTA[] = { 1, 0, 1 };
GLfloat YELLOW[] = { 1, 1, 0 };
GLfloat BROWN[] = { 0.1, 0.0, 0.0 };

GLUquadric *sphere;

//textures
int groundTexture;
int appleTexture;
int goldAppleTexture;
int basketTexture;

//angles of legs for walk cycle
static int leftLegAngle = 0, rightLegAngle = 0;
//flags to trigger events
static int flag = 0, powerFlag = 0, attackFlag = 0;

//score text
extern char text[] = { 'S', 'c', 'o', 'r', 'e', ':' };

extern int score = 0;

int highScore = 0;

bool gameOver = false;

//number of lives player has
int lives = 4;

int timePassed = 0; 

//int frameCount = 0;

void addShadow(double x, double z, double radius);

int removeFirstDigit(int num);

/*Camera code from blackboard source code*/
// A camera.  It moves horizontally in a circle centered at the origin of
// radius 10.  It moves vertically straight up and down.
class Camera {
	double theta;   
	// determines the x and z positions
	double y;          // the current y position
	double dTheta;     // increment in theta for swinging the camera around
	double dy;         // increment in y for moving the camera up/down
public:
	
	//initializes variables
	Camera() : theta(0), y(3), dTheta(0.04), dy(0.2) {}
	double getX() { return 10 * cos(theta); }
	double getY() { return y; }
	double getZ() { return 10 * sin(theta); }
	void moveRight() { theta += dTheta; }
	void moveLeft() { theta -= dTheta; }
	void moveUp() { y += dy; }
	void moveDown() { if (y > dy) y -= dy; }

};

//Class of Apple object. It moves vertically. If the player collects
//the apple, then they will earn 1 point. If it hits the ground,
//the apple will dissapear.
class Apple {
	double radius;
	GLfloat* color;
	double maximumHeight;
	double x;
	double y;
	double z;
	int direction;
public: bool hit;
public:
	Apple(double r, GLfloat* c, double h, double x, double z, bool hit) :
		radius(r), color(c), maximumHeight(h), direction(-1),
		y(h), x(x), z(z), hit(false) {
	}
	void update() {
		//apple dropping from position
		y += direction * 0.02;
		if (y < radius + .03) {
			//if hits ground, disappear
			hit = true;
		}

		//draw apple only if hasn't hit ground or basket
		if (!hit) {
			glPushMatrix();
			
			//add apple texture
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, appleTexture);
			glMatrixMode(GL_MODELVIEW);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glColor3ub(255, 255, 255);

			gluQuadricDrawStyle(sphere, GLU_FILL);
			glBindTexture(GL_TEXTURE_2D, appleTexture);
			gluQuadricTexture(sphere, GL_TRUE);
			gluQuadricNormals(sphere, GLU_SMOOTH);
			//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glRotatef(45, 1, 0, 0);
			gluSphere(sphere, radius, 30, 30);

			glDisable(GL_TEXTURE_2D);
			glPopMatrix();

			addShadow(x, z, radius);
		}
	}

	bool gotHit() {
		return hit;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getZ() {
		return z;
	}
};


//maybe Apple and Obstacle should inherit from the same class?
//Class of hail. It moves vertically and if it drops on the
//character or the ground, then the hail would dissappear
class Hail {

	double radius;
	GLfloat* color;
	double maximumHeight;
	double x;
	double y;
	double z;
	double acc;
public: bool hit;
public:
	
	Hail(double r, GLfloat* c, double h, double x, double z, bool hit) :
		radius(r), color(c),
		y(h), x(x), z(z), hit(false), acc(0) {
	}
	
	//includes acceleration from simulated gravity
	void update() {
		y += -1 * acc;
		acc += .001;
		//if it its the ground
		if (y < radius + .03) {
			hit = true;
		}

		if (!hit) {
			glPushMatrix();
			//add color
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glRotatef(45, 1, 0, 0);
			glScalef(-.1, -.1, -.1);
			glutSolidDodecahedron();
			gluSphere(sphere, radius, 30, 30);

			glPopMatrix();
			addShadow(x, z, radius);
		}
	}

	bool gotHit() {
		return hit;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getZ() {
		return z;
	}


};

void addShadow(double x, double z, double radius) {
	//draw grey/black circle from bottom of object (falling objects)
	glColor3f(0, 0, 0);
	float theta = 0.0;
	const GLdouble TWO_PI = 6.283185;
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 1000; i++) {
		float theta = TWO_PI * float(i) / float(1000);
		float circX = radius * cos(theta) + x;
		float circZ = radius * sin(theta) + z;
		glVertex3f(circX, 0.01, circZ);
	}
	glEnd();
}


//Class of rock that moves horizontally from left of screen
//If it hits player or moves out of screen, then it will dissappear
//need to randomly change z position
//move rock in x direction (which is currently good)
class Rock {

	double radius;
	GLfloat* color;
	double x;
	double y;
	double z;
	double rot;
public: bool hit;
public:

	Rock(double r, GLfloat* c, double z, bool hit) :
		radius(r), color(c), z(z),
		hit(false), x(0), y(.5), rot(-1) {
	}

	void update() {

		x += 1 * 0.063; //move to other side of screen
		if (x >= 6.8) {
			hit = true;
		}

		//roll in 
		if (!hit) {
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glRotatef(rot, 0, 0, 1);
			rot-= 3;
			glScalef(-.1, -.1, -.1);
			glutSolidDodecahedron();
			glPopMatrix();
		}
	}

	bool gotHit() {
		return hit;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getZ() {
		return z;
	}

};

//Class of golden apple. If player collects golden apple,
//then they will earn 5 points and the apple will dissappear
//Moves slightly faster than regluar apple
class GoldenApple {

	double radius;
	GLfloat* color;
	double maximumHeight;
	double x;
	double y;
	double z;
public: bool hit;
public:

	GoldenApple(double r, GLfloat* c, double h, double x, double z, bool hit) :
		radius(r), color(c), maximumHeight(h),
		y(h), x(x), z(z), hit(false) {
	}

	void update() {

		y += -1 * .035;
		if (y < radius + .03) {
			hit = true;
		}

		if (!hit) {
			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, goldAppleTexture);
			glMatrixMode(GL_MODELVIEW);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glColor3ub(255, 255, 255);

			gluQuadricDrawStyle(sphere, GLU_FILL);
			glBindTexture(GL_TEXTURE_2D, goldAppleTexture);
			gluQuadricTexture(sphere, GL_TRUE);
			gluQuadricNormals(sphere, GLU_SMOOTH);
			//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glRotatef(45, 1, 0, 0);
			//glutSolidSphere(radius, 30, 30);
			gluSphere(sphere, radius, 30, 30);

			glDisable(GL_TEXTURE_2D);

			glPopMatrix();
			addShadow(x, z, radius);
		}
	}

	bool gotHit() {
		return hit;
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getZ() {
		return z;
	}

};

//Class of character. Character consists of a head, body, arms, legs, and a basket.
//The class defines how the character walks and the placement of the basket
class Character {
	//only move in x and z direction
	double theta;      // determines the x and z positions
	double z;          // the current y position
	double dz;  
	double x;
	double dx;
	double y;
	int lives;

public:
	Character() : theta(0), z(5), dz(0.2), x(2.0), dx(0.2), y(1.0) {}
	void moveDown() { 
		theta = 180.0;
		//if (z <= 6.8 && (theta == 0.0 || fabs(theta) == 180.0)) {
		if (z <= 6.8) {
			
			z += dz; 

			//walk cycle but backwards
			if (flag == 0) {
				leftLegAngle -= 20;
				rightLegAngle += 20;
				if (leftLegAngle <= -45) {
					flag = 1;
				}
			}

			else {
				leftLegAngle += 5;
				rightLegAngle -= 5;
				if (leftLegAngle >= 45) {
					flag = 0;
				}
			}
		}
		
	}
	void moveUp() {
		theta = 0.0;
		if (z >= 0.2 && (theta == 0.0 || fabs(theta) == 180.0)) {
			z -= dz; 

			//one walk cycle
			if (flag == 0) {
				leftLegAngle += 20;
				rightLegAngle -= 20;
				if (leftLegAngle >= 45) {
					flag = 1;
				}
			}

			else {
				leftLegAngle -= 5;
				rightLegAngle += 5;
				if (leftLegAngle <= -45) {
					flag = 0;
				}
			}
		}
	}

	void moveLeft() {
		theta = 90.0;
		if (x >= 0.2 && (theta == 90 || theta == -270)) {
			x -= dx;

			//one walk cycle
			if (flag == 0) {
				leftLegAngle += 20;
				rightLegAngle -= 20;
				if (leftLegAngle >= 45) {
					flag = 1;
				}
			}

			else {
				leftLegAngle -= 5;
				rightLegAngle += 5;
				if (leftLegAngle <= -45) {
					flag = 0;
				}
			}
		}
		
	}

	void moveRight() {
		theta = 270;
		if (x <= 6.8 && (theta == -90 || theta == 270)) {
			x += dx;

			//one walk cycle
			if (flag == 0) {
				leftLegAngle += 20;
				rightLegAngle -= 20;
				if (leftLegAngle >= 45) {
					flag = 1;
				}
			}

			else {
				leftLegAngle -= 5;
				rightLegAngle += 5;
				if (leftLegAngle <= -45) {
					flag = 0;
				}
			}
		}
	}

	double getX() {
		return x;
	}

	double getY() {
		return y;
	}

	double getZ() {
		return z;
	}

	//glTranslatef(0, -.28, -.5);
	double getBasketX() {
		return x;
	}

	double getBasketY() {
		return y - .28;
	}

	double getBasketZ() {
		return z - .5;
	}

	void update() {
		glColor3f(1, 1, 1);
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);

		glTranslatef(x, 1, z);
		glRotatef(theta, 0, 1, 0);
		glTranslatef(-x, -1, -z);
		//positive z is backwards
		glTranslatef(x, 1, z); //x and z can be moved  glTranslatef(2,1,5);
		
		//head
		glutSolidSphere(.3, 30, 30);
			glPushMatrix();
			//placement for body
			glTranslatef(0, -.5, 0);
			solidBox(.45, .5, .25);
			glPopMatrix();

			glPushMatrix();
			//placement for both legs
			glTranslatef(0, -.95, 0);

				glPushMatrix();
				//rotate for walk cycle (rotate and translate forward a bit and back)
				glTranslatef(0, .15, -.1);
				glRotatef((GLfloat)leftLegAngle, 1, 0, 0);  //45 highest angle?
				glTranslatef(0, -.12, .1);
				//placement for left leg
				glTranslatef(-.13, 0, 0); //in x direction (should remain the same)
				//leg l
				solidBox(.1, .2, .1);
				glPopMatrix();

				glPushMatrix();
				//rotate for walk cycle
				glTranslatef(0, .15, -.1);
				glRotatef((GLfloat)rightLegAngle, 1, 0, 0);
				glTranslatef(0, -.12, .1);
				//placement for right leg
				glTranslatef(.15, 0, 0);
				//leg r
				solidBox(.1, .2, .1);
				glPopMatrix();

			glPopMatrix();

			glPushMatrix();
			//placement for arms
			glTranslatef(0, -.3, -.2);
			glRotatef(90, 1, 0, 0);

				glPushMatrix();
				//placement for left arm
				glTranslatef(-.2, 0, 0);
				//arm l
				solidBox(.1, .5, .1);
				glPopMatrix();

				glPushMatrix();
				//placement for right arm
				glTranslatef(.22, 0, 0);
				//arm r
				solidBox(.1, .5, .1);
				glPopMatrix();

			glPopMatrix();

			glPushMatrix();
			//glColor4fv(YELLOW);
			//glEnable(GL_COLOR_MATERIAL);
			//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, YELLOW);
			//glColor3f(1, 1, 0);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, basketTexture);
			glMatrixMode(GL_MODELVIEW);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glColor3ub(255, 255, 255);

			glTranslatef(0, -.28, -.5);
			//basket
			//glColor3f(1, 1, 0);
			//glEnable(GL_LIGHT2);
			solidBox(.4, .3, .4);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		
		glPopMatrix();
		//glDisable(GL_LIGHT2);
	}

	void solidBox(GLfloat width, GLfloat height, GLfloat depth) {	
		glPushMatrix();
		glScalef(width, height, depth);
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
		glutSolidCube(1.0);
		glPopMatrix();
	}
};


//Class for the ground. The ground stretches across the xz plane
//and its width and depth is 8 units long
class Ground {
	int width;
	int depth;
public:
	Ground(int width, int depth) : width(width), depth(depth) {}
	double centerx() { return width / 2; }
	double centerz() { return depth / 2; }
	void create() {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, groundTexture);
		glMatrixMode(GL_MODELVIEW);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glColor3ub(255, 255, 255);

		
		GLfloat lightPosition[] = { 4, 3, 7, 1 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		glBegin(GL_QUADS);
		glNormal3d(0, 1, 0);  //set normal vector going in y direction
	
		//apply grass texture
		glTexCoord3f(0, 0, 0);
		glVertex3f(0, 0, 0);
		glTexCoord3f(width, 0, 0);
		glVertex3f(width, 0, 0);
		glTexCoord3f(width, 0, depth);
		glVertex3f(width, 0, depth);
		glTexCoord3f(0, 0, depth);
		glVertex3f(0, 0, depth);

		glEnd();
			
		glDisable(GL_TEXTURE_2D);
	}
};

/**where loadbitmap was**/

/**where printScore was**/


//displays the number of lives represented by the character head
void displayLives(int lives) {
	for (int i = 0; i < lives; i++) {
		//draw representation of lives (head of character)
		//at the right corner of the screen
		GLfloat space;
		glColor3f(1, 1, 1);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
		glPushMatrix();
		glTranslatef(7 + i, 1.2, 0);
		glutSolidSphere(.15, 30, 30);
		glPopMatrix();
	}
}


// Global variables: a camera, ground, character, apples, and obstacles.
Ground ground(8, 8);
Camera camera;
Character character;

//added colors to each object in case texture does not work
vector <Apple> apples = { Apple(.1, RED, 3, rand() % 6 + 1, rand() % 5 + 1, false) };

vector <Hail> hail = { Hail(.1, WHITE, 3, rand() % 6 + 1, rand() % 5 + 1, false) };

vector <Rock> rocks = { Rock(.1, BLACK, rand() % 6 + 1, false)};

vector <GoldenApple> gApple = { GoldenApple(.1, YELLOW, 3, rand() % 6 + 1, rand() % 5 + 1, false) };



// Application-specific initialization: Set up global lighting parameters
// and create display lists.
void init() {
	//added seeded rand to get "random" values of placement
	srand(time(0));
	glClearDepth(1.0f);                   // Set background depth to farthest
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glEnable(GL_COLOR_MATERIAL);
	//add more lights to individually go on objects
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


groundTexture = LoadBitmap("grass.bmp");

basketTexture = LoadBitmap("basket.bmp");

sphere = gluNewQuadric();
gluQuadricDrawStyle(sphere, GLU_FILL);
gluQuadricTexture(sphere, GL_TRUE);
gluQuadricNormals(sphere, GLU_SMOOTH);
appleTexture = LoadBitmap("apple.bmp");

goldAppleTexture = LoadBitmap("gold.bmp");
}

// Draws one frame, the ground then the objects, from the current camera
// position.
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(4.0, 6, 13.0,
		ground.centerx(), 0.0, ground.centerz(),
		0.0, 1.0, 0.0);

	ground.create();

	//while the player has not lost
	if (!gameOver) {
		for (int i = 0; i < apples.size(); i++) {
			//if apple instance collides with character's basket
			if (fabs(apples[i].getX() - character.getBasketX()) <= .5 && fabs(apples[i].getY() - character.getBasketY()) <= .3 && fabs(apples[i].getZ() - character.getBasketZ()) <= .5) {
				//increment score by 1
				score++;
				//add collected sound
				mciSendString("open \"scored.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
				mciSendString("play mp3 from 0", NULL, 0, NULL);
				apples[i].hit = true;
			}
			//continue animation/actions
			apples[i].update();
		}

		//for now
		if (powerFlag == 0) {
			for (int i = 0; i < hail.size(); i++) {
				//if hail instance hits character
				if ((fabs(hail[i].getX() - character.getBasketX()) <= .5 && fabs(hail[i].getY() - character.getBasketY()) <= .5 && fabs(hail[i].getZ() - character.getBasketZ()) <= .5)
					|| fabs(hail[i].getX() - character.getX()) <= .5 && fabs(hail[i].getY() - character.getY()) <= .5 && fabs(hail[i].getZ() - character.getZ()) <=.5) {
					//substract number of lives from character
					lives--;
					//if no more lives, then it is game over
					if (lives == 0) {
						gameOver = true;
					}
					//add hit sound
					mciSendString("open \"hit.mp3\" type mpegvideo alias hit", NULL, 0, NULL);
					mciSendString("play hit from 0", NULL, 0, NULL);
					hail[i].hit = true;
				}
				hail[i].update();
			}
		}

		//for now....
		if (powerFlag == 1) {
			for (int i = 0; i < rocks.size(); i++) {
				//if rock collides with character
				if (fabs(rocks[i].getX() - character.getX()) <= .05 && fabs(rocks[i].getY() - character.getY()) <= .5 && fabs(rocks[i].getZ() - character.getZ()) <= .8) {
					//substract lives
					lives--;
					if (lives == 0) {
						gameOver = true;
					}
					//add hit sound
					mciSendString("open \"hit.mp3\" type mpegvideo alias hit", NULL, 0, NULL);
					mciSendString("play hit from 0", NULL, 0, NULL);
					rocks[i].hit = true;
				}
				rocks[i].update();
			}
		}

		if (powerFlag == 1) {
			for (int i = 0; i < gApple.size(); i++) {
				//if golden apple is collected in character's basket
				if (fabs(gApple[i].getX() - character.getBasketX()) <= .5 && fabs(gApple[i].getY() - character.getBasketY()) <= .3 && fabs(gApple[i].getZ() - character.getBasketZ()) <= .5) {
					//increment score by 5
					score += 5;
					//add collected sound
					mciSendString("open \"scored.mp3\" type mpegvideo alias mp3", NULL, 0, 0);
					mciSendString("play mp3 from 0", NULL, 0, 0);
					gApple[i].hit = true;
				}
				gApple[i].update();
			}
		}

		/*If object hit the character/the ground, then add a new object to the object list*/
		if (apples[0].gotHit()) {
			apples.pop_back();
			apples = { Apple(.1, RED, 3, rand() % 6 + 1, rand() % 5 + 1, false) };
		}

		if (hail[0].gotHit()) {
			hail.pop_back();
			hail = { Hail(.1, BROWN, 3, rand() % 6 + 1, rand() % 5 + 1, false) };
		}

		if (rocks[0].gotHit()) {
			rocks.pop_back();
			rocks = { Rock(.1, BLACK, rand() % 6 + 1, false)};
		}

		if (gApple[0].gotHit()) {
			gApple.pop_back();
			gApple = { GoldenApple(.1, YELLOW, 3, rand() % 6 + 1, rand() % 5 + 1, false) };
		}
		character.update();
	}
	//if game is over
	else {
		//write score into file for high score
		ofstream oFS;
		oFS.open("highScore.txt");
		if (!oFS.is_open()) {
			cout << "Cannot find/open file" << endl;
		}
		//update high score if player beats current high score
		if (score >= highScore) {
			highScore = score;
		}
		oFS << highScore;
		oFS.close();
	}

	//prints the score on left side of screen
	printScore(0, 1, 1);
	//dispalys lives on right side of screen
	displayLives(lives);
	glFlush();
	glutSwapBuffers();
}

// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

// Requests to draw the next frame.
void timer(int v) {
	glutPostRedisplay();
	timePassed++;
	if (timePassed == 600) {
		timePassed = 0;
		if (powerFlag == 0) {
			powerFlag = 1;
		}
		else {
			powerFlag = 0;
		}
	}
	//frameCount = (timePassed + 600) % 600;
	glutTimerFunc(1000 / 60, timer, v);
}

// Moves the camera according to the key pressed, then ask to refresh the
// display.
void special(int key, int, int) {
	switch (key) {
	case GLUT_KEY_LEFT: character.moveLeft(); break;
	case GLUT_KEY_RIGHT: character.moveRight(); break;
	case GLUT_KEY_UP: character.moveUp(); break;
	case GLUT_KEY_DOWN: character.moveDown(); break;
	}
	glutPostRedisplay();
}

void mouseFunc(GLint button, GLint action, GLint xMouse, GLint yMouse) {
	if (button == GLUT_RIGHT_BUTTON) {
		exit(0);
	}
}

// Initializes GLUT and enters the main loop.
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CSC 313 Apple Catch Game");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutMouseFunc(mouseFunc);
	glutTimerFunc(100, timer, 0);
	init();
	glutMainLoop();
}