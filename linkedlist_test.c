#include "common.h"
#include "linklist.h"
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
	

	make_new_line(0);
	clicked_on_line(0);
	
	char** test = get_all_lines();
	int num_test = 0;
	
	for(int i = 0; i < 5; i++){
		write_char(i,'a'+i);
	}
	delete_char(1);
	
	test = get_all_lines();
	assert_equal(test[0], "bcde");
	
	delete_char(6);

	test = get_all_lines();
	assert_equal(test[0], "bcde");
	
	delete_char(4);

	test = get_all_lines();
	assert_equal(test[0], "bcd");
	
	
	write_char(3,'\n');
	
	clicked_on_line(1);
	
	test = get_all_lines();
	assert_equal(test[0], "bcd");
	assert_equal(test[1], "");
	
	write_char(0,'k');
	
	test = get_all_lines();
	assert_equal(test[0], "bcd");
	assert_equal(test[1], "k");
	
	//clicked_on_line(0);

}
