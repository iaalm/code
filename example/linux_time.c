#include<stdio.h>
#include<time.h>

int main()
{
	time_t t = time((time_t*) NULL);
	printf("%ld\n",t);
	t %= 86400;
	printf("%ld:%ld:%ld\n",t / 3600,t % 3600 / 60,t % 60);
	
	time_t a,b;
	a = time((time_t*) NULL);
	printf("��ʱ��ʼ");
	do
	{
		b = time((time_t*) NULL);
	}while(b <= a + 5);
	printf("\n��ʱ����");

	getchar();
	return 0;
}
