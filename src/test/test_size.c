#include <stdio.h>

#define printSize(TYPE) printf("Size of "#TYPE":\t %ld\n", sizeof(TYPE))
void main()
{
	printSize(short);
	printSize(int);
	printSize(long);
	printSize(long long);
	printSize(float);
	printSize(double);
	printSize(char);
	printSize(char *);
}
