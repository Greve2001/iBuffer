#include "common.h"
#include "linklist.h"

int main(void){
	make_new_line(0);
	clicked_on_line(0);
	
	char** test = get_all_lines();
	int num_test = 0;
	
	printf("%s\n", test[0]);
	
	for(int i = 0; i < 5; i++){
		write_char(i,'a'+i);
	}
	delete_char(1);
	
	test = get_all_lines();

	printf("%s\n", test[0]);
	
	delete_char(6);

	test = get_all_lines();
	printf("%s\n", test[0]);
	
	delete_char(4);

	test = get_all_lines();
	printf("%s\n", test[0]);
	
	
	write_char(3,'\n');
	
	
	//clicked_on_line(1);
	
	test = get_all_lines();
	num_test = get_amount_of_lines();
	printf("%d\n", num_test);
	//printf("%s\n", test[0]);
	//printf("%s\n", test[1]);
	/*
	write_char(0,'a');
	
	test = get_all_lines();
	printf("%s\n", test[0]);
	if(test[1])
		printf("%s\n", test[1]);
	
	*/

}
