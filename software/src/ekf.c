#include "ekf.h"
#include "sys.h"




void ekf_pos(float *x, float *xdot, float *xddot,float z,float *R,float Q,float dt)
{
	float covtemp[4];							//先验协方差
	static float cov_pre[4]={1.0f,0.0f,0.0f,1.0f};	//上一个协方差		

	float cov_z;
	float z_cor;
	float k[2];
	float ctemp;
	
	//先验状态
	*x += *xdot*dt+0.5f*(*xddot)*dt*dt;
	*xdot += (*xddot)*dt;
	
	//先验协方差
	ctemp = cov_pre[1] + cov_pre[3] * dt;
	covtemp[0] = cov_pre[0]  + cov_pre[2]*dt + ctemp*dt + R[0];
	covtemp[1] = ctemp;
	covtemp[2] = cov_pre[2] + cov_pre[3]*dt;
	covtemp[3] = cov_pre[3] + R[1];

	//计算卡尔曼增益
	cov_z = covtemp[0] + Q;
	k[0] = covtemp[0] / cov_z;
	k[1] = covtemp[2] / cov_z;

	//融合测量结果
	z_cor = z - *x;
	*x += k[0] * z_cor;
	*xdot += k[1] * z_cor;
	//更新状态协方差
	cov_pre[0] = (1 - k[0])*covtemp[0];
	cov_pre[1] = (1 - k[0])*covtemp[1];
	cov_pre[2] = covtemp[2] - k[1] * covtemp[0];
	cov_pre[3] = covtemp[3] - k[1] * covtemp[1];
}

