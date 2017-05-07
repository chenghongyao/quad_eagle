#include "ekf.h"
#include "sys.h"




void ekf_pos(float *x, float *xdot, float *xddot,float z,float *R,float Q,float dt)
{
	float covtemp[4];							//����Э����
	static float cov_pre[4]={1.0f,0.0f,0.0f,1.0f};	//��һ��Э����		

	float cov_z;
	float z_cor;
	float k[2];
	float ctemp;
	
	//����״̬
	*x += *xdot*dt+0.5f*(*xddot)*dt*dt;
	*xdot += (*xddot)*dt;
	
	//����Э����
	ctemp = cov_pre[1] + cov_pre[3] * dt;
	covtemp[0] = cov_pre[0]  + cov_pre[2]*dt + ctemp*dt + R[0];
	covtemp[1] = ctemp;
	covtemp[2] = cov_pre[2] + cov_pre[3]*dt;
	covtemp[3] = cov_pre[3] + R[1];

	//���㿨��������
	cov_z = covtemp[0] + Q;
	k[0] = covtemp[0] / cov_z;
	k[1] = covtemp[2] / cov_z;

	//�ںϲ������
	z_cor = z - *x;
	*x += k[0] * z_cor;
	*xdot += k[1] * z_cor;
	//����״̬Э����
	cov_pre[0] = (1 - k[0])*covtemp[0];
	cov_pre[1] = (1 - k[0])*covtemp[1];
	cov_pre[2] = covtemp[2] - k[1] * covtemp[0];
	cov_pre[3] = covtemp[3] - k[1] * covtemp[1];
}

