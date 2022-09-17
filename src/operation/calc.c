#include "calc.h"
#include "regex_type.h"

static Node *token_g;

extern bool make_regex(char *buf, Node **tokens);
extern int lexical_analysis(Node *tokens);
ret_val eval(Node *token, int begin, int end, int val_type);

int clac(char *expr, bool *success) {
    // token 分解
    if (!make_regex(expr, &token_g)) {
        dlist_clean(&token_g);
        *success = false;
        return -1;
    }
    // 分解打印检查
    printf("make_regex into the following tokens:\n");
	for(int i = 0; ; i++) {
		const Node *token = dlist_select(token_g, i);
		if (token == NULL) {
            break;
		}
        printf("%d: (type = %d) {expr = \"%s\"}\n", i, token->type, token->expr);
	}

    // 语法分析
    int type = lexical_analysis(token_g);
    // 分析打印检查
    printf("lexical_analysis into the following tokens:\n");
	for(int i = 0; ; i++) {
		const Node *token = dlist_select(token_g, i);
		if (token == NULL) {
            break;
		}
        printf("%d: (type = %d) {expr = \"%s\"}\n", i, token->type, token->expr);
	}

    //　语法分析, 计算求值
    ret_val result = eval(token_g->last, 0, token_g->id, type);
    // 分析打印检查
    if (type == TK_DECIMAL) {
        printf("eval result is (%lf):\n", result.double_val);
    } else {
        printf("eval result is (%ld):\n", result.int_val);
    }

    //清理
    dlist_clean(&token_g);

    *success = true;
    return 0;
}