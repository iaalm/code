#include<stdio.h>

int main()
{
	char c;
	while((c=getchar())!='\n')
		putchar(c);//printf("%c",c);��ͬcout,cerr����ͬ���������뻺��
	return 0;
}
