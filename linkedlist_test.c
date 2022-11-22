#include "common.h"
#include <string.h>

void assert_equal(char* string1, char* string2)
{
	if(!strcmp(string1,string2))
	{
		printf("test passed\n");
		return;
	}
	printf("test failed - reason: ");
	printf("tryed to compare %s - %s\n", string1, string2);
}

int main(void){


    init();
    for(int i = 0; i < 10; i++){
        write_char('a'+i, i, 0);
    }

    printf("%s", get_line(0));

    make_new_line();

    write_char('a',0,1);

    printf("\n%s", get_line(1));

    delete_char(4,0);
    delete_char(1,0);

    printf("\n%s", get_line(0));


    return 0;

}
