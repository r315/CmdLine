#include <stdio.h>
#include <stdint.h>

#include "strfunctions.h"

#define ASSERT_EQUAL(a,b) {if ( a != b){printf("Test fail %s:%s",__FILE__, __LINE__); exit(0);}}
// gcc test.c strfunctions.c -o test && ./test
// cl /I inc test.c strfunctions.c /link /out:test.exe 
 

void TEST_strtok_s(void){
char *res, *save, str[] = "Test String";

    res = strtok_s(str, ' ', sizeof(str), save);

    ASSERT_EQUAL(res, str);
    ASSERT_EQUAL(save, str + 6);
    
    printf("Pass!");
}

void TEST_pitoa(void){
    puts(pitoa(123,10,0));
    puts(pitoa(-3,-10,0));
    puts(pitoa(160,16,0));
    puts(pitoa(-160,-16,0));
    puts(pitoa(1234,-10,8));
    puts(pitoa(1234,-10,-8));

    puts(pftoa(3.1415926,10));
    puts(pftoa((1.0/4),6));
    puts(pitoa(1,10,-2));


    uint32_t value = 1<<0;
	for (uint8_t i = 0; i < 32; i++){
		printf("| %c", (value & (uint32_t)(1<<31))? '1' : '0');
        //printf("%08X\n", value & (uint32_t)(1<<31));
		value = (uint32_t)(value << 1);
	}

    printf("\nyatoi Test\n");
    printf("%d\n", ia2i(""));
    printf("%d\n", ia2i("1234"));
    printf("%d\n", ia2i("-1234"));
    printf("%d\n", ia2i("12w34"));

    printf("%d\n", ia2i("1234 dfs"));
    printf("%d\n", ia2i("1234\n"));

void *ptr = "1 2 3";
uint8_t args[4], i = 0;

    while( *((uint8_t*)ptr) != '\0'){
    args[i++] = nextInt((char**)&ptr);
  }

  for(int j=0 ; j < 4; j++){
      printf("%d ", args[j]);
  }
}


int main(int argc, char **argv){
char *tmp;
   /* if(stringSplit(NULL, ' ', sizeof(str)) != NULL){
        printf("Null test fail\n");
        return -1;
    }

    tmp = stringSplit(str, ' ',sizeof(str));
     if(tmp == NULL){
        printf("Error null returned\n");
        return -1;
    }else{
        if(tmp != str){
           printf("%s != %s\n", tmp,str);
            return -1; 
        }
    }


   tmp = stringSplit((char*)main, ' ',sizeof(str));
   
    if(tmp != NULL){
        printf("Invalid string fail");
        return -1;
    }
*/
    //printf("0x%s = %d\n", argv[1], ha2i(argv[1]));
//TEST_strtok_s();
TEST_pitoa();
    return 0;
}