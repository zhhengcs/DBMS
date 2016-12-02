#include "stdio.h"

void trim(char *input_string)
{
	int i=0, j=0;
	while (input_string[i] != '\0')
	{
		if (input_string[i] != ' ')
		{
			input_string[j++]=input_string[i];
		}
		i++;
	}
	input_string[j] = '\0';
}

/*
void main()
{
	char s[1000] = " hello       world  ";
	trim(s);
	puts(s); s
}*/