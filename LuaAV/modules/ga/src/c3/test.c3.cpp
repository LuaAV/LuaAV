#include <stdio.h>
#include "c3ga.h"


int main(int argc, const char *argv[]) {
	c3ga::e1_t _e1;
	c3ga::e2_t _e2;
	c3ga::e3_t _e3;
	c3ga::no_t _no;
	c3ga::ni_t _ni;
	
	// c3ga::exp bug
	c3ga::mv res1 = _e1^_e2*3.14159;
	c3ga::mv res2 = _e3^_ni;
	c3ga::mv res3 = res1-res2;
	c3ga::mv res4 = c3ga::exp(res3);
	
	printf("--------------------\n");
	printf("c3ga::exp bug:\n");
	printf("%s\n", res1.toString_f().c_str());
	printf("%s\n", res2.toString_f().c_str());
	printf("%s\n", res3.toString_f().c_str());
	printf("%s\n", res4.toString_f().c_str());
	
	return 0;
}