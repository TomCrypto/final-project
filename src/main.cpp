//---------------------------------------------------------------------------
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
// Copyright (c) 2012 by Taehyun Rhee
//
// Edited by Roma Klapaukh, Daniel Atkins, and Taehyun Rhee
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "define.h"
#include "G308_Skeleton.h"

#include "gui_bar.h"

#include <iostream>

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

gui_bar* bar; // example: tweakbar

void G308_keyboardListener(unsigned char, int, int);
void G308_Reshape(int w, int h);
void G308_display();
void G308_init();
void G308_SetCamera();
void G308_SetLight();

Skeleton* skeleton;
float rotate_angle = 0; //to keep track of the angle that gets passed to the skeleton based on how many times the r key is pressed.
int main(int argc, char** argv) {
	if (argc < 2 || argc > 3) {
		//Usage instructions for core and challenge
		printf("Usage\n");
		printf("./Ass2 priman.asf [priman.amc]\n");
		exit(EXIT_FAILURE);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(g_nWinWidth, g_nWinHeight);
	g_mainWnd = glutCreateWindow("COMP308 Assignment2");
	if (GLEW_OK != glewInit())
	{
	    printf("GLEW init failed!\n");
	    return EXIT_FAILURE;
	}

    bar = new gui_bar("Hello World");
    bar->keyboard_cb(G308_keyboardListener); // example: tweakbar
	//glutKeyboardFunc(G308_keyboardListener);
	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);

	G308_init();

	// [Assignment2] : Read ASF file
	skeleton = new Skeleton(argv[1]);

	glutMainLoop();

	return EXIT_SUCCESS;
}

// Init Light and Camera
void G308_init() {

	G308_SetLight();
	G308_SetCamera();
}

// Display call back
void G308_display() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}

	// [Assignmet2] : render skeleton
	if (skeleton != NULL) {
		//skeleton->angle = rotate_angle; //pass angle to skeleton
		
		// tweakbar example - read rotation from the GUI bar
		skeleton->angle = bar->rotation;
		
		skeleton->display();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

    bar->draw();

	glutSwapBuffers();
	glutPostRedisplay(); //Had to add this as it wouldn't display straight away on uni machines, worked fine through ssh though.
}

void G308_keyboardListener(unsigned char key, int x, int y) {
	//Code to respond to key events
  if(key=='r') { //when the r key is pressed
    rotate_angle += 5; //increment rotation
  }
  G308_display(); //redraw
}

// Reshape function
void G308_Reshape(int w, int h) {
	if (h == 0)
		h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
	
	bar->resize(g_nWinWidth, g_nWinHeight);
}

// Set Camera Position
void G308_SetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 7.0, 0.0, 0., 0.0, 0.0, 1.0, 0.0);
}

// Set View Position
void G308_SetLight() {
	float direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float diffintensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

