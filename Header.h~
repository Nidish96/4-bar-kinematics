#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<strings.h>
#define pi acos(-1)
#define nparams 26 ///TOTAL 26 parameters

typedef double type;
typedef struct{ type r,*s;}extensible;

typedef struct{
                type l1,l2;
                extensible l3;
                type x0,y0;
                type *theta,*alpha,*phi;
                type *thetadot,*alphadot,*phidot;
                type *thetadotdot,*alphadotdot,*phidotdot;
                type *thetadotdotdot,*alphadotdotdot,*phidotdotdot;}MECHANISM;
///------STRUCTURE OF PARAMETER ARRAY--------///
/// (9+i)th value of any quantity represents ///
/// its time derivative                      ///
///------------------------------------------///
///_____1 to 8 -> DISPLACEMENT PARAMETERS____///
///parameter[0] = l1                         ///
///parameter[1] = l2                         ///
///parameter[2] = l3r                        ///
///parameter[3] = l3s                        ///
///parameter[4] = theta                      ///
///parameter[5] = alpha                      ///
///parameter[6] = phi                        ///
///parameter[7] = x0                         ///
///parameter[8] = y0                         ///
///_____9 to 17 -> VELOCITY PARAMETERS_______///
///parameter[9] = l1dot                      ///
///parameter[10] = l2dot                     ///
///parameter[11] = l3rdot                    ///
///parameter[12] = l3sdot                    ///
///parameter[13] = thetadot                  ///
///parameter[14] = alphadot                  ///
///parameter[15] = phidot                    ///
///parameter[16] = x0dot                     ///
///parameter[17] = y0dot                     ///
///___18 to 26 -> ACCELERATION PARAMETERS____///
///parameter[18] = l1dotdot                  ///
///parameter[19] = l2dotdot                  ///
///parameter[20] = l3rdotdot                 ///
///parameter[21] = l3sdotdot                 ///
///parameter[22] = thetadotdot               ///
///parameter[23] = alphadotdot               ///
///parameter[24] = phidotdot                 ///
///parameter[25] = x0dotdot                  ///
///parameter[26] = y0dotdot                  ///
///___27 to 35 -> JERK PARAMETERS____________///
///parameter[27] = l1dotdotdot               ///
///parameter[28] = l2dotdotdot               ///
///parameter[29] = l3rdotdotdot              ///
///parameter[30] = l3sdotdotdot              ///
///parameter[31] = thetadotdotdot            ///
///parameter[32] = alphadotdotdot            ///
///parameter[33] = phidotdotdot              ///
///parameter[34] = x0dotdotdot               ///
///parameter[35] = y0dotdotdot               ///
///------------------------------------------///
extern int N; ///Sampling domain from 0 to N
type dfu( type (*f)( type* p ), type *parameter, int n ); ///parameter array has all the parameters in proper order
type loopcl1( type* parameter );
type loopcl2( type* parameter );
type velcl1( type* parameter );
type velcl2( type* parameter );
type accelcl1( type* parameter );
type accelcl2( type* parameter );
type jerkcl1( type* parameter );
type jerkcl2( type* parameter );
void Newton_Raphson( type (*f)(  type* p ), type (*g)( type* p ), type *parameter, int x, int y );
MECHANISM analyze( MECHANISM M, int driver, int reqd[4] );

#endif // HEADER_H_INCLUDED
