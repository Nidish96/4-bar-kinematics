#include <Header.h>
#include <Graphics.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>

void (*displayptr)();
int invalid = 0;
int tvect = 0;
int togglecoupler = 0;
int up=0,down=0;
MECHANISM MECH;
int driver,domain;
int mx,my;///MOUSE POSITION
int click = 0;
int reqd[] = {2,0,0,0};
int saved=0;

int getdriver( MECHANISM M )
{
  int driver;
  if( M.l1-M.l2<0 &&
      M.l1-(M.l3.r+M.l3.s[0])<0 &&
      pow(M.l1,2)-pow(M.x0,2)-pow(M.y0,2)<0 )
    driver = 1;
  else if( M.l2-M.l1<0 &&
	   M.l2-(M.l3.r+M.l3.s[0])<0 &&
	   pow(M.l2,2)-pow(M.x0,2)-pow(M.y0,2)<0 )
    driver = 2;
  else if( (M.l3.r+M.l3.s[0])-M.l1<0 &&
	   (M.l3.r+M.l3.s[0])-M.l2<0 &&
	   pow((M.l3.r+M.l3.s[0]),2)-pow(M.x0,2)-pow(M.y0,2)<0 )
    driver = 3;
  else
    driver = 1;

  return driver;
}

int Checkgrashof( MECHANISM M )
{
  type* link = (type*)calloc( 4,sizeof(type) );
  link[0] = pow(pow(M.x0,2)+pow(M.y0,2),0.5);
  link[1] = M.l1;
  link[2] = M.l2;
  link[3] = M.l3.r+M.l3.s[0];
  int i,j;
  type temp;
  for( i=1;i<4;i++ )
    for( j=0;j<i;j++ )
      if( link[i]<link[j] ){
	temp = link[i];
	link[i] = link[j];
	link[j] = temp;
      }
  int ret = (link[0]+link[3]<link[1]+link[2])?1:0;
  return ret;
}

void takemechanism( MECHANISM M, int d, int dom )
{
  MECH = M;
  d--;
  domain = dom-1;
  driver = d;

  int i,j;
  type llim = (domain==0)?0:-pi;
  type ulim = (domain==0)?2*pi:pi;

  type parameter[7];
  {
    for( i=0;i<N;i++ ){
      parameter[4] = M.theta[i];
      parameter[5] = M.alpha[i];
      parameter[6] = M.phi[i];
      for( j=1;j<3;j++ ){
	if( parameter[4+(driver+j)%3]<llim )
	  {parameter[4+(driver+j)%3] += 2*pi;j--;}
	else if( parameter[4+(driver+j)%3]>ulim )
	  {parameter[4+(driver+j)%3] -= 2*pi;j--;}
      }
      M.theta[i] = parameter[4];
      M.alpha[i] = parameter[5];
      M.phi[i] = parameter[6];
    }
  }
}

void keyboard( unsigned char key, int x, int y )
{
  switch(key){
  case 27: exit(1);
  }
}

void passivemouse( int x,int y )
{
  mx = x;
  my = y;
}

void mouse( int button, int state, int x, int y )
{
  if( state==GLUT_DOWN )
    switch(button){
    case GLUT_LEFT_BUTTON: if( up==0 && down==0 )
	{ up = 1; down = 0; }
      else
	{up = !up; down = !down;}
      click = 1; invalid = 0;  break;
    case GLUT_MIDDLE_BUTTON: up = 0; down = 0; togglecoupler = !(togglecoupler); click = 10; break;
    }
}

void idle()
{
  do{
    tvect = (up==1)?(tvect+1)%N:(down==1)?(tvect-1)%N:tvect;
    while( tvect<0 )
      tvect += N;
  }while( MECH.theta[tvect]==-15 || MECH.alpha[tvect]==-15);
  glutPostRedisplay();
}

void reshape( int width, int height )
{
  glutWarpPointer( 700/2-1,700/2-1 );
  glViewport( 0, 0, (GLsizei) width, (GLsizei) height );
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho( -40,40,-40, 40,-1,1 );
  gluPerspective( 0, (GLfloat)width/(GLfloat)height, 0.0, 10.0 );

  glMatrixMode (GL_MODELVIEW);
}

void display()
{
  glutSetCursor( GLUT_CURSOR_CROSSHAIR );
  int i;
  type* parameter = (type*)calloc( 36,sizeof(type) );
  {
    parameter[4] = MECH.theta[tvect];
    parameter[5] = MECH.alpha[tvect];
    parameter[6] = MECH.phi[tvect];

    parameter[13] = MECH.thetadot[tvect];
    parameter[14] = MECH.alphadot[tvect];
    parameter[15] = MECH.phidot[tvect];

    parameter[22] = MECH.thetadotdot[tvect];
    parameter[23] = MECH.alphadotdot[tvect];
    parameter[24] = MECH.phidotdot[tvect];

    parameter[31] = MECH.thetadotdotdot[tvect];
    parameter[32] = MECH.alphadotdotdot[tvect];
    parameter[33] = MECH.phidotdotdot[tvect];
  }
  type* tparameter = (type*)calloc( 36,sizeof(type) );

  glClearColor( 0.0,0.0,0.0,0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();

  //glScaled( 0.5,0.5,0.5 );
  glPushMatrix();
  glColor3f( 1.0,1.0,1.0 );
  char buffer[50];
  char *text;

  ///TEXT PRINTING---------------------------------------------------///
  text = (char*)"4R Mechanism Simulation";
  glRasterPos2d( -15,35 );
  for( i=0;text[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24,text[i]);

  if( saved ){
    glColor3f( 1.0,1.0,0.0 );
    text = (char*)"Mechanism Saved";
    glRasterPos2d( -5,32 );
    for( i=0;text[i]!='\0';i++)
      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,text[i]);
    glColor3f( 1.0,1.0,1.0 );
  }

  text = (char*)"Press the middle button to pause and show/hide the coupler curve";
  glRasterPos2d( -20,-37 );
  for( i=0;text[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,text[i]);
  text = (char*)"Left click to animate";
  glRasterPos2d( -5,-39 );
  for( i=0;text[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,text[i]);

  tparameter[4] = MECH.theta[tvect];
  tparameter[5] = MECH.alpha[tvect];
  tparameter[6] = MECH.phi[tvect];

  if( driver==0 )
    sprintf( buffer,"Theta = %.2f",tparameter[4]*180/pi );
  else if( driver==1 )
    sprintf( buffer,"Alpha = %.2f",tparameter[5]*180/pi );
  else
    sprintf( buffer,"Phi   = %.2f",tparameter[6]*180/pi );
  glRasterPos2d( -30,-35 );
  for( i=0;buffer[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,buffer[i]);

  glColor3f( 1.0,0.0,0.0 );
  if( driver==2 )
    sprintf( buffer,"Theta = %.2f",tparameter[4]*180/pi );
  else if( driver==0 )
    sprintf( buffer,"Alpha = %.2f",tparameter[5]*180/pi );
  else
    sprintf( buffer,"Phi   = %.2f",tparameter[6]*180/pi );
  //sprintf( buffer,"Alpha = %.2f",tparameter[5]*180/pi );
  glRasterPos2d( -5,-35 );
  for( i=0;buffer[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,buffer[i]);

  glColor3f( 0.0,0.0,1.0 );
  if( driver==1 )
    sprintf( buffer,"Theta = %.2f",tparameter[4]*180/pi );
  else if( driver==2 )
    sprintf( buffer,"Alpha = %.2f",tparameter[5]*180/pi );
  else
    sprintf( buffer,"Phi   = %.2f",tparameter[6]*180/pi );
  //sprintf( buffer,"Phi   = %.2f",tparameter[6]*180/pi );
  glRasterPos2d( 20,-35 );
  for( i=0;buffer[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12,buffer[i]);

  glColor3f( 1.0,1.0,1.0 );
  text = (char*)"Animation";
  glRasterPos2d( -45/8,14 );
  for( i=0;text[i]!='\0';i++ )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,text[i] );

  /*text = "Coupler Curve";
    glRasterPos2d(-MECH.x0/4,-10 );
    glColor3f( 1.0,1.0,0.0 );
    for( i=0;i<strlen(text);i++ )
    glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,text[i] );
    glColor3f( 1.0,1.0,1.0 );*/

  ///-----------------------------------------------------------------//

  ///ANIMATION---------------------------------------------///
  type scale = 30/(pow( pow(MECH.x0,2)+pow(MECH.y0,2),0.5 )+MECH.l3.r);
  MECH.x0 *= scale;
  MECH.y0 *= scale;
  MECH.l1 *= scale;
  MECH.l2 *= scale;
  MECH.l3.r *= scale;
  MECH.l3.s[tvect] *= scale;
  glTranslated( -MECH.x0/2,-5,0 );
  glBegin( GL_LINES );
  glLineWidth( 100.0f );
  //glColor3f( 1.0,0.0,0.0 );
  glVertex2d( 0.0, 0.0 );
  glVertex2d( MECH.l1*cos(MECH.theta[tvect]),MECH.l1*sin(MECH.theta[tvect]) );

  //glColor3f( 0.0,1.0,0.0 );
  glVertex2d( MECH.l1*cos(MECH.theta[tvect]),MECH.l1*sin(MECH.theta[tvect]) );
  glVertex2d( MECH.l1*cos(MECH.theta[tvect])+MECH.l2*cos(MECH.alpha[tvect]),MECH.l1*sin(MECH.theta[tvect])+MECH.l2*sin(MECH.alpha[tvect]) );

  //glColor3f( 0.0,0.0,1.0 );
  glVertex2d( MECH.l1*cos(MECH.theta[tvect])+MECH.l2*cos(MECH.alpha[tvect]),MECH.l1*sin(MECH.theta[tvect])+MECH.l2*sin(MECH.alpha[tvect]) );
  glVertex2d( MECH.x0,MECH.y0 );

  //glColor3f( 1.0,1.0,0.0 );
  glVertex2d( MECH.x0,MECH.y0 );
  glVertex2d( 0.0,0.0 );
  glEnd();

  glBegin( GL_POINTS );
  for( i=0;i<90;i++ ) {
    glVertex2d( 0.3*cos(4*i*pi/180),0.3*sin(4*i*pi/180));
    glVertex2d( MECH.l1*cos(MECH.theta[tvect])+0.3*cos(4*i*pi/180),MECH.l1*sin(MECH.theta[tvect])+0.3*sin(4*i*pi/180));
    glVertex2d( MECH.l1*cos(MECH.theta[tvect])+MECH.l2*cos(MECH.alpha[tvect])+0.3*cos(4*i*pi/180),MECH.l1*sin(MECH.theta[tvect])+MECH.l2*sin(MECH.alpha[tvect])+0.3*sin(4*i*pi/180));
    glVertex2d( MECH.x0+0.3*cos(4*i*pi/180),MECH.y0+0.3*sin(4*i*pi/180));
    ///COUPLER OUTPUT POINT
    if( togglecoupler ) {
      glColor3f( 1.0,1.0,0.0 );
      glVertex2d( MECH.l1*cos(MECH.theta[tvect])+MECH.l2*cos(MECH.alpha[tvect])/2+0.3*cos(4*i*pi/180),MECH.l1*sin(MECH.theta[tvect])+MECH.l2*sin(MECH.alpha[tvect])/2+0.3*sin(4*i*pi/180));
      glColor3f( 1.0,1.0,1.0);
    }
  }
  glEnd();

  ///COUPLER CURVE--------------------------------///
  if(togglecoupler ) {
    glBegin( GL_LINE_LOOP );
    glColor3f( 1.0,1.0,0.0);
    for( i=0;i<N;i++ ) {
      glVertex2d( MECH.l1*cos(MECH.theta[i%N])+MECH.l2*cos(MECH.alpha[i%N])/2,MECH.l1*sin(MECH.theta[i%N])+MECH.l2*sin(MECH.alpha[i%N])/2 );
    }
    glEnd();
  }
  glPopMatrix();

  ///DISPLACEMENT DIAGRAM---------------------///
  glTranslated( -45/4,-5,0 );
  glColor3f( 1.0,1.0,1.0);
  text = (char*)"Displacement Diagram";
  glRasterPos2d( -25+10-15/2+5,+25+10 );
  for( i=0;text[i]!='\0';i++ )
    glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,text[i] );

  glPushMatrix();
  glTranslated( -25+10-15/2,25,0 );

  glBegin( GL_LINES );
  glVertex2d( 0,-9+domain*9 );
  glVertex2d( 18,-9+domain*9 );

  glVertex2d( 0,-9 );
  glVertex2d( 0,9 );

  glVertex2d( parameter[4+(driver)%3]*180/(20*pi),-9 );
  glVertex2d( parameter[4+(driver)%3]*180/(20*pi),9 );
  glEnd();

  glBegin( GL_POINTS );
  for( i=0;i<N;i++ ) {
    tparameter[4] = MECH.theta[i];
    tparameter[5] = MECH.alpha[i];
    tparameter[6] = MECH.phi[i];

    glColor3f( 1.0,0.0,0.0 );
    glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+1)%3]*180/(20*pi)-9+domain*9 );
    glColor3f( 0.0,0.0,1.0 );
    glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+2)%3]*180/(20*pi)-9+domain*9 );
  }
  glEnd();
  glPopMatrix();
  ///VELOCITY DIAGRAM-----------------------------------------------------------------///
  if( reqd[1] ) {
    glColor3f( 1.0,1.0,1.0 );
    text = (char*)"Velocity Diagram";
    glRasterPos2d( (-45/4)+25+10-15/2+5+45/4,5+(-5)+25+10);
    for( i=0;text[i]!='\0';i++ )
      glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,text[i] );

    glPushMatrix();
    glTranslated( 25+10-15/2,25,0 );

    glBegin( GL_LINES );
    glColor3f( 1.0,1.0,1.0 );
    glVertex2d( 0,0 );
    glVertex2d( 18,0 );

    glVertex2d( 0,-9 );
    glVertex2d( 0,9);

    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),-9 );
    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),9 );
    glEnd();

    glBegin( GL_POINTS );
    glColor3f( 0.0,1.0,0.0 );
    for( i=0;i<N;i++ ) {
      tparameter[4] = MECH.theta[i];
      tparameter[5] = MECH.alpha[i];
      tparameter[6] = MECH.phi[i];

      tparameter[13] = MECH.thetadot[i];
      tparameter[14] = MECH.alphadot[i];
      tparameter[15] = MECH.phidot[i];

      glColor3f( 1.0,0.0,0.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+1)%3+9]*180/(20*pi) );
      glColor3f( 0.0,0.0,1.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+2)%3+9]*180/(20*pi) );
    }
    glEnd();
    glPopMatrix();
  }
  ///ACCELERATION DIAGRAM------------------------------------------------------------///
  if( reqd[2] ) {
    glColor3f( 1.0,1.0,1.0 );
    text = (char*)"Acceleration Diagram";
    glRasterPos2d( -25+10-15/2+5,-15+10);
    for( i=0;text[i]!='\0';i++ )
      glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,text[i] );

    glPushMatrix();
    glTranslated( -25+10-15/2,-15,0 );

    glBegin( GL_LINES );
    glColor3f( 1.0,1.0,1.0 );
    glVertex2d( 0,0 );
    glVertex2d( 18,0 );

    glVertex2d( 0,-9 );
    glVertex2d( 0,9);

    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),-9 );
    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),9 );
    glEnd();

    glBegin( GL_POINTS );
    glColor3f( 0.0,0.0,1.0 );
    for( i=0;i<N;i++ ) {
      tparameter[4] = MECH.theta[i];
      tparameter[5] = MECH.alpha[i];
      tparameter[6] = MECH.phi[i];

      tparameter[13] = MECH.thetadot[i];
      tparameter[14] = MECH.alphadot[i];
      tparameter[15] = MECH.phidot[i];

      tparameter[22] = MECH.thetadotdot[i];
      tparameter[23] = MECH.alphadotdot[i];
      tparameter[24] = MECH.phidotdot[i];

      glColor3f( 1.0,0.0,0.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+1)%3+18]*180/(20*pi) );
      glColor3f( 0.0,0.0,1.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+2)%3+18]*180/(20*pi) );
    }
    glEnd();
    glPopMatrix();
  }
  ///JERK DIAGRAM------------------------------------------------------------///
  if( reqd[3] ) {
    glColor3f( 1.0,1.0,1.0 );
    text = (char*)"Jerk Diagram";
    glRasterPos2d( 25+10-15/2+5,-15+10 );
    for( i=0;text[i]!='\0';i++ )
      glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,text[i] );

    glPushMatrix();
    glTranslated( 25+10-15/2,-15,0 );

    glBegin( GL_LINES );
    glColor3f( 1.0,1.0,1.0 );
    glVertex2d( 0,0 );
    glVertex2d( 18,0 );

    glVertex2d( 0,-9 );
    glVertex2d( 0,9);

    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),-9 );
    glVertex2d( parameter[4+(driver)%3]*180/(20*pi),9 );
    glEnd();

    glBegin( GL_POINTS );
    glColor3f( 1.0,0.0,1.0 );
    for( i=0;i<N;i++ ) {
      tparameter[4] = MECH.theta[i];
      tparameter[5] = MECH.alpha[i];
      tparameter[6] = MECH.phi[i];

      tparameter[13] = MECH.thetadot[i];
      tparameter[14] = MECH.alphadot[i];
      tparameter[15] = MECH.phidot[i];

      tparameter[22] = MECH.thetadotdot[i];
      tparameter[23] = MECH.alphadotdot[i];
      tparameter[24] = MECH.phidotdot[i];

      tparameter[31] = MECH.thetadotdotdot[i];
      tparameter[32] = MECH.alphadotdotdot[i];
      tparameter[33] = MECH.phidotdotdot[i];

      glColor3f( 1.0,0.0,0.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+1)%3+27]*180/(20*pi) );
      glColor3f( 0.0,0.0,1.0 );
      glVertex2d( tparameter[4+(driver)%3]*180/(20*pi),tparameter[4+(driver+2)%3+27]*180/(20*pi) );
    }
    glEnd();
    glPopMatrix();
  }
  //    delay(600/N);
  glutSwapBuffers();
}

void drawingpad()
{
  glutSetCursor( GLUT_CURSOR_NONE );
  glClearColor( 0.0,0.0,0.0,0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glColor3f( 1.0,1.0,1.0 );

  glPushMatrix();

  static int mc = 0;
  if( click==0 )
    mc = 0;

  static int followtrace;
  if( click==0 )
    followtrace = 1;
  char buffer[50],*text;

  static type** positions;
  int i,j;
  glRasterPos2d( -5,30 );
  text = (char*)"Drawing Pad";
  for( i=0;text[i]!='\0';i++ )
    glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, text[i] );

  if( invalid==1 ) {
    glRasterPos2d( -8,20 );
    text = (char*)"First draw a mechanism";
    for( i=0;text[i]!='\0';i++ )
      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text[i] );
  }

  text = (char*)"Right Click for options";
  glRasterPos2d( -5,-30 );
  for( i=0;text[i]!='\0';i++ )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text[i] );

  glRasterPos2f( 40*((double)2*(mx+1)/700-2*700/(2*700)),40*(2*700/(2*700)-(double)2*(my+1)/700) );
  sprintf(buffer,"(%.2f,%.2f)",40*((double)2*(mx+1)/700-2*700/(2*700)),40*((double)2*(mx+1)/700-2*700/(2*700)) );
  for( i=0;buffer[i]!='\0';i++)
    glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24,buffer[i]);
  glPushMatrix();

  ///MOUSE POINTER--------------------------------------------------///
  glTranslated( 40*((double)2*(mx+1)/700-2*700/(2*700)),40*(2*700/(2*700)-(double)2*(my+1)/700),0.0 );
  glBegin( GL_LINE_STRIP );
  for( i=0;i<30;i++ )
    glVertex2d( 0.3*cos(12*i*pi/180),0.3*sin(12*i*pi/180) );
  glEnd();
  glPopMatrix();
  ///---------------------------------------------------------------///

  if( click==1 && mc<4 ) {
    mc++;
    positions = (type**)realloc( positions, mc*sizeof(type*) );
    positions[mc-1] = (type*)calloc( 2,sizeof(type) );
    i = mc-1;
    positions[i][0] = 40*((double)2*(mx+1)/700-2*700/(2*700));
    positions[i][1] = 40*(2*700/(2*700)-(double)2*(my+1)/700);
    click++;
  }

  ///CHECKING FOR GRASHOF---------------------------------------------///
  else if( mc==4 ) {
    followtrace = 0;
    MECHANISM M;
    M.x0 = positions[3][0] - positions[0][0];
    M.y0 = positions[3][1] - positions[0][1];

    M.l1 = pow( pow(positions[1][0]-positions[0][0],2)+pow(positions[1][1]-positions[0][1],2),0.5 );
    M.l2 = pow( pow(positions[2][0]-positions[1][0],2)+pow(positions[2][1]-positions[1][1],2),0.5 );
    M.l3.r = pow( pow(positions[3][0]-positions[2][0],2)+pow(positions[3][1]-positions[2][1],2),0.5 );

    M.theta = (type*)calloc(N,sizeof(type));
    M.alpha = (type*)calloc(N,sizeof(type));
    M.phi = (type*)calloc(N,sizeof(type));
    M.l3.s = (type*)calloc( N,sizeof(type));

    M.theta[0] = acos( (positions[1][0]-positions[0][0])/M.l1 );
    M.alpha[0] = acos( (positions[2][0]-positions[1][0])/M.l2 );
    M.phi[0] = acos( (positions[3][0]-positions[2][0])/M.l3.r );

    if( Checkgrashof( M ) ) {
      text = (char*)"GRASHOF MECHANISM";
      glRasterPos2d( -13,25 );
      for( i=0;text[i]!='\0';i++ )
	glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, text[i] );

      if( click==1 ) {
	followtrace = 0;
	int d;
	d = getdriver( M );
	M = analyze( M,d,reqd );
	takemechanism( M,d,reqd[0] );

	displayptr = &display;
	up = 0;
	down = 0;
	mc = 0;
	click = 0;
      }
    }
    else {
      glRasterPos2d( -7,25 );
      text = (char*)"NOT GRASHOF";
      for( i=0;text[i]!='\0';i++ )
	glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, text[i] );
      followtrace = 0;

      if( click==1 ) {
	mc = 0;
	click = 0;
      }
    }
  }


  ///RENDERING THE DRAWING TILL NOW--------------------------------------///
  if( mc>0 ) {
    glBegin( GL_LINE_LOOP );
    for( i=0;i<mc;i++ )
      glVertex2d( positions[i][0],positions[i][1] );
    if( followtrace==1 )
      glVertex2d( 40*((double)2*(mx+1)/700-2*700/(2*700)),40*(2*700/(2*700)-(double)2*(my+1)/700) );
    glEnd();

    for( i=0;i<mc;i++ ) {
      glPushMatrix();

      sprintf( buffer,"(%.2f,%.2f)",positions[i][0],positions[i][1] );
      glRasterPos2d( positions[i][0],positions[i][1] );
      for( j=0;buffer[j]!='\0';j++ )
	glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10,buffer[j] );

      glTranslated( positions[i][0],positions[i][1],0 );
      glBegin( GL_LINE_STRIP );
      for( j=0;j<30;j++ )
	glVertex2d( 0.3*cos(12*j*pi/180),0.3*sin(12*j*pi/180) );
      glEnd();
      glPopMatrix();
    }
  }
  glutSwapBuffers();
}

