#ifndef _EKF_H
#define _EKF_H
#include "sys.h"

//float kalman_translation(float *x, float *xdot, float *xddot, float z, float *cov, float *R, float *Q, float dt);
void ekf_pos(float *x, float *xdot, float *xddot,float z,float *R,float Q,float dt);
#endif
