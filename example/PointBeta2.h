#include "vector.h"

/*class environment
{
      public:
            virtual vector GetF(point*a);
            environment();
            virtual ~environment();
};
*/
class point//:public environment
{
public:
	float OutQ();
	double OutM();
	vector OutP();
	vector OutF();
	vector OutV();
	void move();
	point(float tdt,bool ts,vector tp,vector tv,double tm,float tq);
	virtual ~point();
private:
	static unsigned int i; //��Ա������ 
	static float dt;
	static point *pp[127];  //��Աָ��  
	unsigned int id;
	bool s;                //�Ƿ�ֹ
	vector p;              //���� 
	vector f;              //�� 
	vector v;              //�ٶ� 
	float q;               //�������
	double m;               //����

}
