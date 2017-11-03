#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define AMOUNT 128 
int main(int argc, char* argv[]) {


uint8_t random_number = 0;
uint8_t random_number2 = 0;
//srand(1234);
srand(time(NULL));

printf("static int8_t dither_random_table[%d] = {\n    ",AMOUNT);

for (int i=0;i<AMOUNT;i++) {
    
random_number = rand() % 4;
random_number2 = rand() % 4;

    int8_t dither = (random_number + random_number2) - 4;
    printf("%3d", dither);
    if ( i < AMOUNT) { printf(","); }
    if ( (i+1) % 16 == 0) { printf("\n    "); }

    //fprintf(stderr,"%d\n", dither);
}

printf("\n};\n");

return 0;
}
