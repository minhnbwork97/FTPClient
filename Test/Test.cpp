
#include "stdafx.h"
#include "winsock2.h"

int main()
{
	char str[80] = "minh.html";
	const char s[2] = ".";
	char *token;

	/* lay token dau tien */
	token = strtok(str, s);
	token = strtok(NULL, s);
	/* duyet qua cac token con lai */
	//while (token != NULL)
	//{
	//	printf(" %s\n", token);

	//	token = strtok(NULL, s);
	//}

	printf("%s", token);
	system("pause");
	return(0);
	
}