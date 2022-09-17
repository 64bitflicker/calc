// 正则匹配，将每一个 符号/操作 分解存储在一个链表中
#include "regex_type.h"
#include "calc.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static struct rule {
	const char *regex;
	char token_type;
} rules[] = {
	{" +", UNDEFAULT},		// spaces 跳过空格
	{"\\{", TK_LBBRACK},	// {
	{"\\}", TK_RBBRACK},	// }
	{"\\[", TK_LSBRACK},	// [
	{"\\]", TK_RSBRACK},	// ]
	{"\\(", TK_LBRACK},		// (
	{"\\)", TK_RBRACK},		// )

	{"\\-", TK_NEGATIVE},	// 需要更新
	{"\\*", TK_GETVAL},		// 需要更新
	{"\\~", TK_BIT_NEGATE},	// 需要更新

	{"\\/", TK_DIV},	// 除
	{"\\%", TK_MODULO},	// 模

	{"\\+", TK_ADD},	// 加

	{"<<", TK_MVL},		// 左移
	{">>", TK_MVR},		// 右移

	{">=", TK_MOTEQ},	// 大于等于
	{">", TK_MOT},		// 大于
	{"<=", TK_LETQE},	// 小于等于
	{"<", TK_LET},		// 小于

	{"==", TK_EQ},		// 等于
	{"!=", TK_NEQ},		// 不等于
	{"\\!", TK_LOGIC_NEGATE},// 需要更新

	{"\\^", TK_BIT_XOR},		// logical or 按位或 ^

	{"&&", TK_LOGIC_AND},		// logical and 逻辑于 &&
	{"\\&", TK_GETADDR},		// 需要更新
	{"\\|\\|", TK_LOGIC_OR},	// logical or 逻辑或 ||
	{"\\|", TK_BIT_OR},			// logical or 按位或 |

	{"=", TK_ASSIGN},			// assign 赋值 =

	{"^[0-9]+\\.[0-9]+", TK_DECIMAL},		// 小数
	{"^0o?[0-7]+", TK_OCT_INTEGER},			// 八进制整数
	{"^0[xX][0-9a-fA-F]+", TK_HEX_INTEGER},	// 十六进制整数
	{"^0[bB][0-1]+", TK_BIN_INTEGER},		// 二进制整数
	{"^[0-9]+", TK_INTEGER},				// 整数

	{"^\\$[a-zA-Z0-9]+", TK_VARIABLE},	// 变量
};


#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))
static regex_t reg[NR_REGEX] = {};

// 初始化正则匹配
int init_regex() {
	int i = 0;
	int ret = 0;
	char error_msg[128];

	for (i = 0; i < NR_REGEX; i++) {
		ret = regcomp(&reg[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0) {
			regerror(ret, &reg[i], error_msg, 128);
			printf("regex compilation failed: %s\n%s\n", error_msg, rules[i].regex);
			return -1;
		}
	}
	return true;
}

bool make_regex(char *buf, Node **tokens) {
	int i = 0;
	int position = 0;
	regmatch_t pmatch;

	printf("cars = '%s';\n", buf);
	while (buf[position] != '\0') {
		// printf("car = '%s';\n", buf + position);
		for (i = 0; i < NR_REGEX; i++) {
			if (regexec(&reg[i], buf + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = buf + position;
        		int substr_len = pmatch.rm_eo;
				// printf("match rules[%d] = \"%s\" at position %d with len %d: %.*s\n", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				if (rules[i].token_type == UNDEFAULT) {
					break;
				}
				// 存储数据
				Node *token = dlist_inster(tokens);
				token->type = rules[i].token_type;
				token->expr = (char *)malloc(substr_len + 1);
				memset(token->expr, 0, substr_len + 1);
				strncpy(token->expr, substr_start, substr_len);

				// printf("inster token %d: (type = %d) \"expr = %s\"\n", token->id, token->type, token->expr);

				break;
			}
		}
		// printf("\n");
		// position++;
		if (i == NR_REGEX) {
			return false;
		}
	}
	return true;
}

int token_check(Node *token, int num) {
	int val_type = TK_INTEGER;
	for (int i = 0; i <= num; i++) {
		printf("%s: num = %d ; id = %d\n",__func__, num, token->id);
		switch (token->type) {
			case TK_NEGATIVE:
				if (token->id > 0 && token->next->type == TK_NEGATIVE) {
					// 负号优先级高，做负号收缩，负负得正
					dlist_delete(token, token->id);
					dlist_delete(token->next, token->next->id);
				} else if (token->id == 0 ||

					token->next->type == TK_LBRACK ||
					token->next->type == TK_DIV ||
					token->next->type == TK_MUL ||
					token->next->type == TK_MODULO ||
					token->next->type == TK_ADD ||
					token->next->type == TK_SUB) {
					printf("%s: fu shu\n",__func__);
					token->type = TK_NEGATIVE;
				} else if (token->last->type == TK_DECIMAL ||
					token->next->type == TK_OCT_INTEGER ||
					token->next->type == TK_HEX_INTEGER ||
					token->next->type == TK_BIN_INTEGER ||
					token->next->type == TK_INTEGER ||
					token->next->type == TK_NEINTEGER ||
					token->next->type == TK_VARIABLE ||
					token->next->type == TK_RBRACK) {
					printf("%s: jian\n",__func__);
					token->type = TK_SUB;
				}
			break;
			case TK_GETVAL:
				token->type = TK_MUL;
			break;
			case TK_GETADDR:
				token->type = TK_BIT_AND;
			break;
			case TK_DECIMAL:
				val_type = TK_DECIMAL;
			break;
		}
		token = token->last;
	}
	return val_type;
}

int regex_unit_test(char * buf) {
	if (init_regex() != 0) {
		return -1;
	}

	if (!make_regex(buf, NULL)) {
		return -1;
	}

	return 0;
}


// 				// printf("%s ", rules[i].regex);
// 				switch (rules[i].token_type) {
// 					case UNDEFAULT:
// // printf("NULL");
// continue;
// 					break;
// 					case TK_LBBRACK:
// printf("{");
// 					break;
// 					case TK_RBBRACK:
// printf("}");
// 					break;
// 					case TK_LSBRACK:
// printf("[");
// 					break;
// 					case TK_RSBRACK:
// printf("]");
// 					break;
// 					case TK_LBRACK:
// printf("(");
// 					break;
// 					case TK_RBRACK:
// printf(")");
// 					break;
// 					case TK_NEGATIVE:
// printf("negative -");
// 					break;
// 					case TK_GETVAL:
// printf("get val *");
// 					break;
// 					case TK_BIT_NEGATE:
// printf("bit negate ~");
// 					break;
// 					case TK_DIV:
// printf("div /");
// 					break;
// 					case TK_MODULO:
// printf("modulo %c", '%');
// 					break;
// 					case TK_ADD:
// printf("add +");
// 					break;
// 					case TK_MVL:
// printf("left move <<");
// 					break;
// 					case TK_MVR:
// printf("right move >>");
// 					break;
// 					case TK_MOTEQ:
// printf("more than eq >=");
// 					break;
// 					case TK_MOT:
// printf("more than >");
// 					break;
// 					case TK_LETQE:
// printf("less than eq <=");
// 					break;
// 					case TK_LET:
// printf("less than <");
// 					break;
// 					case TK_EQ:
// printf("eq ==");
// 					break;
// 					case TK_NEQ:
// printf("not eq !=");
// 					break;
// 					case TK_LOGIC_NEGATE:
// printf("logic negate !");
// 					break;
// 					case TK_BIT_XOR:
// printf("bit xor ^");
// 					break;
// 					case TK_LOGIC_AND:
// printf("logic and &&");
// 					break;
// 					case TK_GETADDR:
// printf("getaddr &");
// 					break;
// 					case TK_LOGIC_OR:
// printf("logic or ||");
// 					break;
// 					case TK_BIT_OR:
// printf("bit or |");
// 					break;
// 					case TK_ASSIGN:
// printf("assign =");
// 					break;
// 					case TK_HEX_INTEGER:
// printf("hex integer 0x");
// 					break;
// 					case TK_OCT_INTEGER:
// printf("oct integer 0o");
// 					break;
// 					case TK_BIN_INTEGER:
// printf("bin integer 0b");
// 					break;
// 					case TK_INTEGER:
// printf("integer 123");
// 					break;
// 					case TK_DECIMAL:
// printf("decimal 3.14");
// 					break;
// 					case TK_VARIABLE:
// printf("variable $eip");
// 					break;
// 					default:
// printf("???");
// 				}
// printf("\n");
// 				break;










// 	switch (token->type) {
// 		case UNDEFAULT:
// 		break;
// 		case TK_LBBRACK:

// 		break;
// 		case TK_RBBRACK:

// 		break;
// 		case TK_LSBRACK:

// 		break;
// 		case TK_RSBRACK:

// 		break;
// 		case TK_LBRACK:
// 		break;
// 		case TK_RBRACK:
// 		break;
// 		case TK_NEGATIVE:
// printf("negative -");
// 			if ((token->id == 0 ||
// 				token->last.type == TK_NEGATIVE ||
// 				token->last->type == TK_LBRACK) ||
// 				(token->last->type != TK_DECIMAL &&
// 				token->last->type != TK_OCT_INTEGER &&
// 				token->last->type != TK_HEX_INTEGER &&
// 				token->last->type != TK_BIN_INTEGER &&
// 				token->last->type != TK_INTEGER &&
// 				token->last->type != TK_NEINTEGER &&
// 				token->last->type != TK_VARIABLE &&
// 				token->last->type != TK_RBRACK) {
// 				token->type = TK_NEGATIVE;
// 			} else {
// 				token->type = TK_SUB;
// 			}
// 		break;
// 		case TK_GETVAL:
// printf("get val *");
// 		break;
// 		case TK_BIT_NEGATE:
// printf("bit negate ~");
// 		break;
// 		case TK_DIV:
// printf("div /");
// 		break;
// 		case TK_MODULO:
// printf("modulo %c", '%');
// 		break;
// 		case TK_ADD:
// printf("add +");
// 		break;
// 		case TK_MVL:
// printf("left move <<");
// 		break;
// 		case TK_MVR:
// printf("right move >>");
// 		break;
// 		case TK_MOTEQ:
// printf("more than eq >=");
// 		break;
// 		case TK_MOT:
// printf("more than >");
// 		break;
// 		case TK_LETQE:
// printf("less than eq <=");
// 		break;
// 		case TK_LET:
// printf("less than <");
// 		break;
// 		case TK_EQ:
// printf("eq ==");
// 		break;
// 		case TK_NEQ:
// printf("not eq !=");
// 		break;
// 		case TK_LOGIC_NEGATE:
// printf("logic negate !");
// 		break;
// 		case TK_BIT_XOR:
// printf("bit xor ^");
// 		break;
// 		case TK_LOGIC_AND:
// printf("logic and &&");
// 		break;
// 		case TK_GETADDR:
// printf("getaddr &");
// 		break;
// 		case TK_LOGIC_OR:
// printf("logic or ||");
// 		break;
// 		case TK_BIT_OR:
// printf("bit or |");
// 		break;
// 		case TK_ASSIGN:
// printf("assign =");
// 		break;
// 		case TK_HEX_INTEGER:
// printf("hex integer 0x");
// 		break;
// 		case TK_OCT_INTEGER:
// printf("oct integer 0o");
// 		break;
// 		case TK_BIN_INTEGER:
// printf("bin integer 0b");
// 		break;
// 		case TK_INTEGER:
// printf("integer 123");
// 		break;
// 		case TK_DECIMAL:
// printf("decimal 3.14");
// 		break;
// 		case TK_VARIABLE:
// printf("variable $eip");
// 		break;
// 		default:
// printf("???");
// 				}
// printf("\n");
// 				break;
