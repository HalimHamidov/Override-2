#include <stdio.h>
#include <string.h>

int crack(int num)
{
	char decoded[17];
	char *encoded = "Q}|u`sfg~sf{}|a3";
	char *to_find = "Congratulations!";

	decoded[16] = '\0';

	for (int i = 0; i < 16; i++)
	{
		decoded[i] = encoded[i] ^ num;
	}
	return (strcmp(decoded, to_find) == 0 ? 1 : 0);
}

int main(void)
{
	int base = 0x1337d00d;
	int password;

	for(int i = 0; i <= 22; i++)
	{
		password = base - i;
		if (crack(i))
		{
			printf("Password is : %d\n", password);
		}
	}
	return 0;
}
