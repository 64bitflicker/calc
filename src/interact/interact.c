#include "calc.h"

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>


// readline 读取一行输入
static char *rl_gets() {
	static char *line_read = NULL;
	
	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(calc): ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

extern int clac(char *expr, bool *success);

// 交互接口，通过readline获取用户输入

int interact() {
	bool success = true;
	for (char *str; ( str = rl_gets()) != NULL; ) {
		if (strcmp(str, "quit") == 0 || strcmp(str, "exit") == 0) {
			return 0;
		}
		if (strcmp(str, "q")== 0) {
			return 0;
		}
		if (strlen(str) == 0) {
			continue;
		}
		// 拿到用户输入的 str
		// int val = 
		clac(str, &success);
		// if (val != 0 && success == true) {
		// 	// printf("%s = %d\n", str, val);
		// 	val = 0;
		// }
	}
	return 0;
}

