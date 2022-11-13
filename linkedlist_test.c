#include "common.h"
#include "linklist.h"

int main(void){
	make_new_line(0);
	printf("test4\n");
	clicked_on_line(0);

	write_char(0,'a');
	

	printf("test11\n");
	char** test = get_all_lines();
	printf("%s\n", test[0]);
	printf("test12\n");

}
