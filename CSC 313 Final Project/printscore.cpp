#include "GL\glew.h"
#include "GL\freeglut.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#include <GL\glut.h>
#include <string>
#include <fstream>
#include <iostream>

#include "printscore.h"

using namespace std;

void printScore(int x, int y, int z) {
	//color not affecting the font
	glColor3f(1.0, 1.0, 1.0);

	//look over this through book to see if correct
	for (int i = 0; i < strlen(text); i++) {
		glRasterPos2i(x - 1, y);
		//m is only 0-5
		for (int m = 0; m <= i; m++) {
			//cout << " m: " << m << endl;

			glPushAttrib(GL_CURRENT_BIT);
			glColor3f(1, 1, 0);
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[m]);

		}
	}
	glPopAttrib();

	//print high score
	char highScoreText[] = { 'H', 'i', 'g', 'h', ' ', 'S', 'c', 'o', 'r', 'e', ':' };
	for (int i = 0; i < strlen(highScoreText); i++) {
		glRasterPos2f(x-0.82, y + 1);
		for (int m = 0; m <= i; m++) {
			glPushAttrib(GL_CURRENT_BIT);
			glColor3f(1, 0, 0);
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, highScoreText[m]);
			
		}
	}
	glPopAttrib();


	int charScore = score;
	int lastDigit = 0;
	int spacing = 1;
	double j = 0;


	while (charScore > 9) {
		lastDigit = charScore % 10;

		glRasterPos2d((x + 1) - (spacing*j), y);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + lastDigit);
		j += .2;
		charScore /= 10;
	}
	glRasterPos2d((x + 1) - (spacing*j), y);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + charScore);
	
	ifstream iFS;
	iFS.open("highScore.txt");
	if (!iFS.is_open()) {
		cout << "Cannot find/open file" << endl;
	}

	iFS >> highScore;
	iFS.close();

	charScore = highScore;

	while (charScore > 9) {
		lastDigit = charScore % 10;

		glRasterPos2d((x + 2.6) - (spacing*j), y + 1);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + lastDigit);
		j += .2;
		charScore /= 10;
	}
	glRasterPos2d((x + 2.6) - (spacing*j), y + 1);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + charScore);


}