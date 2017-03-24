#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Header.h>
#include <Graphics.h>

int saves = 0;
void writetofile( MECHANISM M )
{
  FILE *f;
  int i;
    
  char filename[20];
  sprintf(filename,"Savefile%d.dat",saves);
  saves++;
  f = fopen( filename,"w+");
  fprintf(f,"d_{theta} d_{alpha} d_{phi} v_{theta} v_{alpha} v_{phi} a_{theta} a_{alpha} a_{phi} j_{theta} j_{alpha} j_{phi}\n");
  for( i=0;i<N-2;i++ ) {
    fprintf(f,"%lf %lf %lf ",M.theta[i]*180/pi,M.alpha[i]*180/pi,M.phi[i]*180/pi);
    fprintf(f,"%lf %lf %lf ",M.thetadot[i]*180/pi,M.alphadot[i]*180/pi,M.phidot[i]*180/pi);
    fprintf(f,"%lf %lf %lf ",M.thetadotdot[i]*180/pi,M.alphadotdot[i]*180/pi,M.phidotdot[i]*180/pi);
    fprintf(f,"%lf %lf %lf\n",M.thetadotdotdot[i]*180/pi,M.alphadotdotdot[i]*180/pi,M.phidotdotdot[i]*180/pi);
  }
  fclose(f);
  saved = 1;
}

void mainloop_mainloop()
{
  (*displayptr)();
}

void choices( int r )
{
  invalid = 0;
  switch(r) {
  case 0: displayptr = drawingpad; click = 0; saved=0; break;
  case 1: up=0; down=0; break;
  case 2: if(displayptr==display && saved==0) {
      writetofile(MECH);
      printf("MECHANISM SAVED!");
    }
    else
      invalid = 1;
    break;
  case 3: exit(1); break;
  }
}

void diagrams( int r )
{
  int i,ch;
  if( r==0 )
    togglecoupler = !togglecoupler;
  else if( r<4 )
    reqd[r] = (reqd[r]==0)?1:0;
  else {
    ch = (reqd[1]==0)?1:0;
    togglecoupler = !togglecoupler;
    for( i=1;i<4;i++ )
      reqd[i] = ch;
  }
}

void choosedomain( int r )
{
  reqd[0] = r;
  int d = getdriver(MECH);
  takemechanism( MECH,d,reqd[0] );
}

int main( int argc,char **argv)
{
  N = 360; ///Sampling domain of 360
  int inconds,choice;
  char c[10];
  MECHANISM MECH1;
  MECH1.theta = (type*)calloc(N,sizeof(type));
  MECH1.alpha = (type*)calloc(N,sizeof(type));
  MECH1.phi = (type*)calloc(N,sizeof(type));
  MECH1.l3.s = (type*)calloc( N,sizeof(type));

  printf("\t\t\t4R MECHANISM SIMULATION\n");

  printf("\n\t\t\t1. Enter Dimensions\n\t\t\t2. Draw Mechanism\n\t\t\t\t");
  scanf("%s",c);
  choice = atoi(c);
  //choice = 2;
  if( choice==1 ) {
    do {
      printf("\n\t\tFixed point 1 is taken at origin\n");
      printf("\t\tEnter coordinates of second fixed point\n\t\t\tx0\t: ");
      scanf("%s",c);
      MECH1.x0 = atof(c);

      printf("\t\t\ty0\t: ");
      scanf("%s",c);
      MECH1.y0 = atof(c);
      printf("\n\t\tEnter link dimensions for a Grashof Mechanism\n");
      printf("\t\tl1\t\t: ");
      scanf("%s",c);
      MECH1.l1 = atof(c);
      printf("\t\tl2\t\t: ");
      scanf("%s",c);
      MECH1.l2 = atof(c);
      printf("\t\tl3\t\t: ");
      scanf("%s",c);
      MECH1.l3.r = atof(c);
      /*printf("\t\tl3-ext\t: ");
	scanf("%lf",&MECH1.l3.s[0]);
	for( i=1;i<N;i++ )
	MECH1.l3.s[i] = MECH1.l3.s[0];*/
      if(Checkgrashof(MECH1)) {
	break;
      }
      else
	printf("\n\n\t\t\tERROR:NOT A GRASHOF MECHANISM\t");
    }while(1);

    printf("\n\t\tWant to input initial conditions?\t1. Yes\t2. No\n\t\t\t\t");
    scanf("%s",c);
    inconds = atoi(c);
    if( inconds==1 ) {
      printf("\t\tTheta(degs)\t: ");
      scanf("%s",c);
      MECH1.theta[0] = atof(c);//*pi/180;
      printf("\t\talpha(degs)\t: ");
      scanf("%s",c);
      MECH1.alpha[0] = atof(c)*pi/180;
      printf("\t\tPhi(degs)\t: ");
      scanf("%s",c);
      MECH1.phi[0] = atof(c)*pi/180;
    }
    else {
      MECH1.theta[0] = 0;
      MECH1.alpha[0] = 1;
      MECH1.phi[0] = 2;
    }

    printf("\n\t\tPreferred domain\t: 1. 0 to 360\t2. -180 to +180\n\t\t\t\t");
    scanf("%s",c);
    reqd[0] = atof(c);
    printf("\n\t\tVelocity Diagram required?\t1. Yes\t 2. No\n\t\t\t\t");
    scanf("%s",c);
    reqd[1] = atof(c);
    if( reqd[1]==1 ) {
      printf("\t\tAcceleration Diagram required?\t1. Yes\t 2. No\n\t\t\t\t");
      scanf("%s",c);
      reqd[2] = atof(c);
      if( reqd[2]==1 ) {
	printf("\t\tJerk Diagram required?\t1. Yes\t 2. No\n\t\t\t\t");
	scanf("%s",c);
	reqd[3] = atof(c);
      }
    }

    /*MECH1.x0 = 15;
      MECH1.y0 = 0;

      MECH1.l1 = 5;
      MECH1.l2 = 28;
      MECH1.l3.r = 19;

      reqd[0] = 2;
      reqd[1] = 2;
      reqd[2] = 2;
      reqd[3] = 2;

      MECH1.theta[0] = 0;
      MECH1.alpha[0] = 1;
      MECH1.phi[0] = 2;*/

    driver = getdriver( MECH1 );

    MECH1 = analyze( MECH1,driver,reqd );
    //writetofile(MECH1,"graphs.csv");
    displayptr = &display;
  }
  else {
    displayptr = &drawingpad;
  }
  takemechanism( MECH1,driver,reqd[0] );
  ///--END--OF--KINEMATICS---------------------///

  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowPosition( 600,35 );
  glutInitWindowSize( 700,700 );
  int main_window = glutCreateWindow( "Animation" );

  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passivemouse);
  glutKeyboardFunc( keyboard );
  glutReshapeFunc( reshape );
  glutIdleFunc( idle );
  glutDisplayFunc( mainloop_mainloop );
  //glutDisplayFunc( display );

  int s,r;
  s = glutCreateMenu( diagrams );
  glutAddMenuEntry( "Coupler Curve",0 );
  glutAddMenuEntry( "Velocity Diagram",1 );
  glutAddMenuEntry( "Acceleration Diagram",2 );
  glutAddMenuEntry( "Jerk Diagram",3 );
  glutAddMenuEntry( "All",4 );

  r = glutCreateMenu( choosedomain );
  glutAddMenuEntry( "0 to 360",1 );
  glutAddMenuEntry( "-180 to 180",2 );

  glutCreateMenu( choices );
  glutAddMenuEntry( "Restart",0 );
  glutAddMenuEntry( "Pause", 1);
  glutAddSubMenu( "Domain",r );
  glutAddSubMenu( "Diagrams",s );
  glutAddMenuEntry( "Save to File",2 );
  glutAddMenuEntry( "Exit",3 );
  glutAttachMenu( GLUT_RIGHT_BUTTON );

  glutMainLoop();

  return 0;
}
