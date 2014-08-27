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

#ifndef SKELETONH
#define SKELETONH

#include <stdio.h>
#include <GL/glut.h>

using namespace std;

// Using bitmasking to denote different degrees of freedom for joint motion
typedef int DOF;

#define DOF_NONE 0
#define DOF_RX 1
#define DOF_RY 2
#define DOF_RZ 4
#define DOF_ROOT 8 // Root has 6, 3 translation and 3 rotation

//Type to represent a bone
typedef struct bone {
	char* name;
	float dirx, diry, dirz;
	float rotx, roty, rotz;
	DOF dof;
	float length;
	bone** children;
	int numChildren;

	//Challenge stuff
	float currentTranslatex, currentTranslatey, currentTranslatez;
	float currentRotationx, currentRotationy, currentRotationz;

} bone;

void trim(char**);

class Skeleton {

private:
	int buffSize, maxBones;
	bone* root;
	bool readASF(char*);
	void readHeading(char*, FILE*);
	void decomment(char*);
	void deleteBones(bone*);
	void readBone(char*, FILE*);
	void readHierarchy(char*, FILE*);
	void display(bone*, GLUquadric*);
	DOF dofFromString(char*);
	void cylinderORaxis(GLUquadric*, float, float, float, float, bool);

public:
	int numBones;
	float angle;
	Skeleton(char*);
	~Skeleton();
	void display();
	void readAMC(FILE*,int*);
};

#endif
