#include <stdio.h>

int main()
{
	char *str="add";
	printf("%X\n",str);
	printf("%X\n",str+3);
	*(str+2)='a';   //����������Ϊwritten
	//*(str+10)='\0';
	printf("%s\n",*str);
}