//ʧ�ܣ��޷���֤˫��ƽ������ڰ���͹�����ᵥ��ƽ����޷�����ѭ��
#include <stdio.h>
#include <math.h>

double y(double x);                                      //ʾ������

double SearchX(double (*f)(double n),double min,double max,double er);   //��Ҫ�Ķ��ֲ��Һ���

int main()
{
	printf("%lf",SearchX(y,1.5,2.5,0.0001));
	return 0;
}

double y(double x)
{
	return x*x-5*x+6;
}

double SearchX(double (*f)(double n),double min,double max,double er)
{
	double a=(*f)(min),b=(*f)(max),half;
	if(a*b<0)             //�˴��ɸĽ�Ϊ�÷��ź����ж�
	{
		do
		{
			a=(*f)(min),b=(*f)(max);
			half=fabs(a/(a-b))*(max-min)+min;
			if((*f)(half)*a<0)
				max=half;
			else
				min=half;
		}while(max-min>er);
		return max;
	}
	else
	{
		printf("SearchX Error! f(min)*f(max)>0\n");
		return 0;
	}
}