#include <stdio.h>
int c = 0;

int main(int argc, char *argv[]){
	int a = c;
	c=c+1;
	printf("%d",a);
	printf("%d",c);

	return 0;
}