#include <stdio.h>
#include <limits.h>

int main(int argc, char const *argv[])
{
	unsigned int a = UINT_MAX;
	printf("a:%u\n",a);
	a = (int)(((unsigned int)a) + (unsigned int)1);
	printf("a:%u\n",a);
	return 0;
}