/*
gcc -std=c11 -ffreestanding -fno-builtin -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes  -Wunused-parameter -Wfloat-conversion -c test_free.c -o test_free.o
Use the above compiler flags to test
 
*/


#define PCV_SRM_IMPLEMENT
#include "../pcv_srm.h"
#include "../pcv_normalize.h"
#include "../pcv_convert_gray.h"
#include "../pcv_convolution.h"
#include "../pcv_get_filters.h"
int main(void){
	return 0;
}
