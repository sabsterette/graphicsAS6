/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 6-Local Illumination and Shading

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glut.h>
#include <windows.h>
#include "glprocs.h"


#define PI 3.14159265359

using namespace std;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illuminationMode = 1;
int shadingMode = 0;
int lightSource = 0;
char* currShaderFrag = "phongIllumination.frag";
char* currShaderVert = "gouraudInterp.vert";

//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;


//Sets the light positions and other parameters for the shaders
void setParameters()
{

	//sample variables used to demonstrate how uniform variables are used in vertex shaders.
	//can be defined as global and can change per primitive
	int ambient_loc, diffuse_loc, specular_loc;
	int exponent_loc, illumination_loc;

	GLfloat ambient_cont[] = { 0.02,0.02,0.02,1.0 };
	GLfloat diffuse_cont[] = { 0.70,0.27,0.08,1.0 };
	GLfloat specular_cont[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat exponent = 12.8;

	// Create light components
	GLfloat light_position[] = { 7.0, 7.0, 7.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//// initializing the light 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_cont);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_cont);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_cont);

	//Access uniform variables in shaders
	//The parameters in quotes are the names of the corresponding variables
	ambient_loc = glGetUniformLocationARB(p, "AmbientContribution");
	glUniform4fvARB(ambient_loc, 1, ambient_cont);

	diffuse_loc = glGetUniformLocationARB(p, "DiffuseContribution");
	glUniform4fvARB(diffuse_loc, 1, diffuse_cont);

	specular_loc = glGetUniformLocationARB(p, "SpecularContribution");
	glUniform4fvARB(specular_loc, 1, specular_cont);

	exponent_loc = glGetUniformLocationARB(p, "exponent");
	glUniform1fARB(exponent_loc, exponent);

	illumination_loc = glGetUniformLocationARB(p, "illuminationMode");
	glUniform1fARB(illumination_loc, illuminationMode);

	//Access attributes in vertex shader
	//tangent_loc = glGetAttribLocationARB(program, "tang");
	//glVertexAttrib1fARB(tangent_loc, tangent);

}


void DisplayFunc(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	setParameters();
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	glutSolidTeapot(1);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}




void setShaders() {

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead(currShaderVert);
	fs = shaderFileRead(currShaderFrag);

	const char * vv = vs;
	const char * ff = fs;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);
	
	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);
	
	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);
	setParameters();
}


//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illuminationMode == 0)
		{
			illuminationMode = 1;
			currShaderFrag = "cookTorranceIllum.frag";
			setShaders();
		}
		else
		{
			illuminationMode = 0;
			currShaderFrag = "phongIllumination.frag";
			setShaders();
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode = 1;
			currShaderVert = "gouraudInterp.vert";
			setShaders();
		}
		else
		{
			shadingMode = 0;
			currShaderVert = "phongInterp.vert";
			setShaders();
		}
		break;
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
		}
		else
		{
			lightSource =0;
		}
		break;
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
		}
		break;

    default:
		break;
    }
	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	const GLubyte* temp;
	temp = glGetString(GL_VERSION);
	printf("%s\n", temp);
	temp = glGetString(GL_VENDOR);
	printf("%s\n", temp);
	temp = glGetString(GL_EXTENSIONS);
	printf("%s\n", temp);

	setShaders();
	//setParameters();
	glutMainLoop();

	return 0;
}


//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}