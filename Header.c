#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <Header.h>

typedef double type;
/* typedef struct{ type r,*s;}extensible; */

int N; ///Sampling domain from 0 to N

type dfu( type (*f)( type* p ), type *parameter, int n )///parameter array has all the parameters in proper order
{
  type h = pow(10,-3),ret;

  parameter[n] += h;
  ret = f( parameter );
  parameter[n] -= 2*h;
  ret = (ret - f(parameter) )/(2*h);
  parameter[n] += h;

  return ret;
}

type loopcl1( type* parameter )
{
  type l1 = parameter[0],l2=parameter[1],l3r=parameter[2],l3s=parameter[3];
  type theta = parameter[4],alpha=parameter[5],phi=parameter[6],x0=parameter[7];
  type ret;
  ret = l1*cos(theta) + l2*cos(alpha) - (l3r+l3s)*cos(phi) - x0;
  return ret;
}

type loopcl2( type* parameter )
{
  type l1 = parameter[0],l2=parameter[1],l3r=parameter[2],l3s=parameter[3];
  type theta = parameter[4],alpha=parameter[5],phi=parameter[6],y0=parameter[8];
  type ret;
  ret = l1*sin(theta) + l2*sin(alpha) - (l3r+l3s)*sin(phi) - y0;
  return ret;
}

type velcl1( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ )
    ret += dfu( loopcl1,parameter,i )*parameter[9+i];
  return ret;
}

type velcl2( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ )
    ret+= dfu( loopcl2,parameter,i )*parameter[9+i];
  return ret;
}

type accelcl1( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ ) {
    ret += dfu( velcl1,parameter,i )*parameter[9+i];
    ret += dfu( velcl1,parameter,i+9 )*parameter[18+i];
  }
  return ret;
}

type accelcl2( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ ) {
    ret += dfu( velcl2,parameter,i )*parameter[9+i];
    ret += dfu( velcl2,parameter,i+9 )*parameter[18+i];
  }
  return ret;
}

type jerkcl1( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ ) {
    ret += dfu( accelcl1,parameter,i )*parameter[9+i];
    ret += dfu( accelcl1,parameter,i+9 )*parameter[18+i];
    ret += dfu( accelcl1,parameter,i+18 )*parameter[27+i];
  }
  return ret;
}

type jerkcl2( type* parameter )
{
  int i;
  type ret = 0;
  for( i=4;i<7;i++ ) {
    ret += dfu( accelcl2,parameter,i )*parameter[9+i];
    ret += dfu( accelcl2,parameter,i+9 )*parameter[18+i];
    ret += dfu( accelcl2,parameter,i+18 )*parameter[27+i];
  }
  return ret;
}

void Newton_Raphson( type (*f)(  type* p ), type (*g)( type* p ), type *parameter, int x, int y )
{
  type e = 0.0001;
  type F,G, fx, fy, gx, gy;
  type det;
  F = f( parameter );
  G = g( parameter );
  int iters=-1;
  do {
    iters++;
    fx = dfu( f,parameter,x );
    fy = dfu( f,parameter,y );
    gx = dfu( g,parameter,x );
    gy = dfu( g,parameter,y );
    det = fx*gy - gx*fy;

    parameter[x] = parameter[x] - ( gy*F - fy*G )/det;
    parameter[y] = parameter[y] - (-gx*F + fx*G )/det;

    F = f( parameter );
    G = g( parameter );
  }while( pow( pow(F,2) + pow(G,2),0.5 )>e && iters<20 );

  if( iters==20 ) {
    parameter[x] = -15;
    parameter[y] = -15;
  }
}

MECHANISM analyze( MECHANISM M, int driver, int reqd[4] )
{
  {///dynamic memory allocation
    M.thetadot = (type*)calloc(N,sizeof(type));
    M.alphadot = (type*)calloc(N,sizeof(type));
    M.phidot = (type*)calloc(N,sizeof(type));
    M.thetadotdot = (type*)calloc(N,sizeof(type));
    M.alphadotdot = (type*)calloc(N,sizeof(type));
    M.phidotdot = (type*)calloc(N,sizeof(type));
    M.thetadotdotdot = (type*)calloc(N,sizeof(type));
    M.alphadotdotdot = (type*)calloc(N,sizeof(type));
    M.phidotdotdot = (type*)calloc(N,sizeof(type));
  }
  driver--;
  //    printf("\n%d,%d,%d\n",4+(driver)%3,4+(driver+1)%3,4+(driver+2)%3);
  int i,j;///Loop variables
  type ulim = (reqd[0]==1)?2*pi:pi;
  type llim = (reqd[0]==1)?0:-pi;
  //printf("%lf to %lf\n\n",llim,ulim);
  type* parameter = (type*)calloc( 36,sizeof(type) );
  parameter[0] = M.l1;
  parameter[1] = M.l2;
  parameter[2] = M.l3.r;
  parameter[7] = M.x0;
  parameter[8] = M.y0;
  type omega = 10;///in rpm
  type a = 0;///in rad p s^2
  printf("\n\t\tCalculating...\n\t\tProgress\t: ");
  int initialdriver=0;
  parameter[4] = M.theta[0];
  parameter[5] = M.alpha[0];
  parameter[6] = M.phi[0];

  initialdriver = parameter[4+(driver)%3]*180/pi;

  for( i=0;i<N;i++ ) {
    parameter[3] = M.l3.s[i];
    printf("%2d%c\b\b\b",100*(i+1)/N,37);
    ///DISPLACEMENT----------------------------------------///
    parameter[4+(driver)%3] = ((initialdriver+i*360/N)%360)*pi/180;
    parameter[4+(driver+1)%3] = parameter[4+(driver+1)%3];
    parameter[4+(driver+2)%3] = parameter[4+(driver+2)%3];

    Newton_Raphson( loopcl1,loopcl2,parameter,4+(driver+1)%3,4+(driver+2)%3 );

    for( j=1;j<3;j++ ) {
      if( parameter[4+(driver+j)%3]<llim )
	{parameter[4+(driver+j)%3] += 2*pi;j--;}
      else if( parameter[4+(driver+j)%3]>ulim )
	{parameter[4+(driver+j)%3] -= 2*pi;j--;}
    }
    M.theta[i] = parameter[4];
    M.alpha[i] = parameter[5];
    M.phi[i] = parameter[6];
    if( parameter[4+(driver+1)%3]!=-15 ) {
      ///VELOCITY--------------------------------------------///
      parameter[4+(driver)%3+9] = omega*2*pi/60;
      parameter[4+(driver+1)%3+9] = (i==0)?1:parameter[4+(driver+1)%3+9];
      parameter[4+(driver+2)%3+9] = (i==0)?2:parameter[4+(driver+2)%3+9];

      Newton_Raphson( velcl1,velcl2,parameter,4+(driver+1)%3+9,4+(driver+2)%3+9 );
      M.thetadot[i] = parameter[13];
      M.alphadot[i] = parameter[14];
      M.phidot[i] = parameter[15];
      ///ACCELERATION----------------------------------------///
      parameter[4+(driver)%3+18] = a;
      parameter[4+(driver+1)%3+18] = (i==0)?1:parameter[4+(driver+1)%3+18];
      parameter[4+(driver+2)%3+18] = (i==0)?2:parameter[4+(driver+2)%3+18];

      Newton_Raphson( accelcl1,accelcl2,parameter,4+(driver+1)%3+18,4+(driver+2)%3+18 );
      M.thetadotdot[i] = parameter[22];
      M.alphadotdot[i] =  parameter[23];
      M.phidotdot[i] = parameter[24];
      ///JERK------------------------------------------------///
      parameter[4+(driver)%3+27] = 0;
      parameter[4+(driver+1)%3+27] = (i==0)?1:parameter[4+(driver+1)%3+27];
      parameter[4+(driver+2)%3+27] = (i==0)?2:parameter[4+(driver+2)%3+27];

      Newton_Raphson( jerkcl1,jerkcl2,parameter,4+(driver+1)%3+27,4+(driver+2)%3+27 );
      M.thetadotdotdot[i] = parameter[31];
      M.alphadotdotdot[i] =  parameter[32];
      M.phidotdotdot[i] = parameter[33];
    }
  }

  int c;
  for( i=0;i<N;i++ ) {
    parameter[4] = M.theta[i];
    parameter[5] = M.alpha[i];
    parameter[6] = M.phi[i];
    c = (i+1)%N;
    while( parameter[4+(driver+1)%3]==-15 && c!=i ) {
      M.theta[i] = M.theta[c];
      M.alpha[i] = M.alpha[c];
      M.phi[i] = M.phi[c];

      M.thetadot[i] = M.thetadot[c];
      M.alphadot[i] = M.alphadot[c];
      M.phidot[i] = M.phidot[c];

      M.thetadotdot[i] = M.thetadotdot[c];
      M.alphadotdot[i] = M.alphadotdot[c];
      M.phidotdot[i] = M.phidotdot[c];

      M.thetadotdotdot[i] = M.thetadotdotdot[c];
      M.alphadotdotdot[i] = M.alphadotdotdot[c];
      M.phidotdotdot[i] = M.phidotdotdot[c];
      c = (c+1)%N;
    }
  }

  printf("\n\t\tDone!\n");

  return M;
}
