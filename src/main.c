#include <stdio.h>
// #include <regex.h>

extern int regex_unit_test(char * buf);
extern int init_regex();
extern int interact();

void Welcome() {
	printf("********************************\n");
	printf("*  Welcome to Calc Tools! (%s)  *\n", "R");
	printf("********************************\n");
}

void Byebye() {
	printf("********************************\n");
	printf("* bye~ , thanks for using! (%s) *\n", "R");
	printf("********************************\n");
}

int main(int argc, char **argv) {
	Welcome();

	// 初始化部分
	// 初始化正则匹配表
	init_regex();

	// 进入交互ui
	interact();

	Byebye();
	return 0;
}
