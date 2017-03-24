#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <Header.h>
#include <GL/glut.h>
#include <stdlib.h>

extern void (*displayptr)();
extern int invalid;
extern int tvect;
extern int togglecoupler;
extern int up,down;
extern MECHANISM MECH;
extern int driver,domain;
extern int mx,my; /* MOUSE POSITION */
extern int click;
extern int reqd[4];
extern int saved;

int getdriver( MECHANISM M );
int Checkgrashof( MECHANISM M );
void takemechanism( MECHANISM M, int d, int dom );
void keyboard( unsigned char key, int x, int y );
void passivemouse( int x,int y );
void mouse( int button, int state, int x, int y );
void idle();
void reshape( int width, int height );
void display();
void drawingpad();

#endif // GRAPHICS_H_INCLUDED
