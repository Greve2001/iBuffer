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
	write_char('a', 0,0);
	write_char('b',1,0);
	
	printf(get_line(0));
	
	make_new_line();
	
	printf("\n");
	write_char('c',0,1);
	
	printf(get_line(1));
	
	return 0;

}
