#include "common.h"
#include "linklist.h"

int main(void){
	make_new_line(0);
	clicked_on_line(0);

	
	for(int i = 0; i < 5; i++){
		write_char(i,'a'+i);
	}
	delete_char(1);

	char** test = get_all_lines();
	printf("%s\n", test[0]);
	
	delete_char(6);

	test = get_all_lines();
	printf("%s\n", test[0]);
	
	delete_char(4);

	test = get_all_lines();
	printf("%s\n", test[0]);
	

}
