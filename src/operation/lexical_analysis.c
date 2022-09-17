// 语法分析
#include "regex_type.h"
#include "calc.h"

#include <readline/readline.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dlist.h"

// // 计算的结果类型
// typedef union ret_val_{
//     int64_t int_val;
//     u_int64_t uint_val;
//     double double_val;
// } ret_val;

// 拆分运算符，根据优先级最低的进行拆分
Node *split_operator(Node *token, int begin, int end) {
	printf("\t\tsplit_operator  ==> %d ~ %d;\n", begin, end);
	Node *id = NULL;
	// int id = -1;
	int operator = UNDEFAULT;

	for (int i = begin; i <= end; i++) {
		printf("\t\tsplit_operator  ==> %d : %s; operator = %d\n", token->id, token->expr, operator);
		if (token->type == TK_LBRACK) {
			printf("\t\tsplit_operator  ==> ((\n");
			int flag = 0;
			for(; i <= end; i++) {
				printf("\t\t\tsplit_operator  ==> flag = %d\n", flag);
				if (token->type == TK_LBRACK) {
					flag++;
				} else if (token->type == TK_RBRACK) {
					flag--;
				}
				if (flag == 0) {
					break;
				}
				printf("\t\t\t\tsplit_operator break(%d: %s)!\n", token->id, token->expr);
				token = token->last;
			}
		} else if (token->type > operator && token->type < TK_DIGIT_SIGN_DIVISION) {
			// id = token->id;
			printf("\t\t\tsplit_operator operator ==> \'%s\' %d > %d;\n", token->expr, token->type, operator);
			id = token;
			operator = token->type;
		}
		token = token->last;
	}

	// while (token->id >= begin && token->id <= end) {
	// 	if (token->type > operator) {
	// 		// id = token->id;
	// 		id = token;
	// 		operator = token->type;
	// 	}
	// 	token = token->last;
	// }

	printf("\t\tsplit_operator  ==> return %d: %s;\n", id->id, id->expr);
	return id;
}

//括号检查
bool check_parentheses(Node *token, int begin, int end) {
	printf("\t\tcheck_parentheses  ==> %d ~ %d (begin = %s);\n", begin, end, token->expr);
	int flag = 0;
	if (token->type != TK_LBRACK) {
		return false;
	}
	
	for(token = token->last; begin + 1 < end; begin++) {
		if (token->type == TK_LBRACK) {
			flag++;
		} else if (token->type == TK_RBRACK) {
			flag--;
		}
		if (flag < 0) {
			return false;
		}
		token = token->last;
	}

	// while (token->id > begin && token->id < end) {
	// 	if (token->type == TK_LBRACK) {
	// 		flag++;
	// 	} else if (token->type == TK_RBRACK) {
	// 		flag--;
	// 	}
	// 	if (flag < 0) {
	// 		return false;
	// 	}
	// 	token = token->last;
	// }

	printf("\t\tcheck_parentheses  ==> %d ~ %d (begin = %s);\n", begin, end, token->expr);
	if (token->type != TK_RBRACK) {
		return false;
	}

	printf("\t\tcheck_parentheses  ==> flag = %d;\n", flag);
	if (flag == 0) {
		return true;
	}

	return false;
}

// 语法分析
ret_val eval(Node *token, int begin, int end, int val_type) {
	printf("eval begin ==> %d: %s ; %d --> %d; type = %d\n", token->id, token->expr, begin, end, val_type);
	if (begin > end) {
		printf("error : begin > end\n");
		exit(-1);
	} else if (begin == end) {
		// int64_t val = 0;
		// double dval = 0.0;
		ret_val val;
		switch (token->type) {
			case TK_BIN_INTEGER:
				val.int_val = strtoll(token->expr + 2, NULL, 2);
			// val = strtoll(token->expr + 2, NULL, 2);
			break;
			case TK_OCT_INTEGER:
				if ((token->expr)[1] == 'o' || (token->expr)[1] == 'O') {
					val.int_val = strtoll(token->expr + 2, NULL, 8);
					// val = strtoll(token->expr + 2, NULL, 8);
				} else {
					val.int_val = strtoll(token->expr + 1, NULL, 8);
					// val = strtoll(token->expr + 1, NULL, 8);
				}
			break;
			case TK_HEX_INTEGER:
				val.int_val = strtoll(token->expr + 2, NULL, 16);
				// val = strtoll(token->expr + 2, NULL, 16);
			break;
			case TK_INTEGER:
				val.int_val = strtoll(token->expr, NULL, 10);
				// val = strtoll(token->expr, NULL, 10);
			break;
			// case TK_NEINTEGER:
			// 	val.int_val = -strtoll(token->last->expr, NULL, 10);
			// 	// val = -strtoll(token->expr, NULL, 16);
			// break;
			case TK_DECIMAL:
				sscanf(token->expr, "%lf", &(val.double_val));
				// dval = sscanf(token->expr, "%lf", &dval)// strtoll(token->expr + 2, NULL, 16);
			break;
			case TK_VARIABLE:
			break;
		}

		if (val_type == TK_DECIMAL && token->type != TK_DECIMAL) {
			val.double_val = val.int_val;
			printf("\t\teval double val ==> %lf;\n", val.double_val);
		} else {
			printf("\t\t\teval double val ==> %ld;\n", val.int_val);
		}

		return val;
	} else if (check_parentheses(token, begin, end) == true) {
		printf("\teval check_parentheses ==> %d: %d ;\n",  begin, end);
		return eval(token->last, begin + 1, end - 1, val_type);
	} else {
		Node * op_token = split_operator(token, begin, end);
		int op = op_token->id;
		printf("\teval split_operator ==> %d: %s ;\n", op, op_token->expr);
		// Node * op_token = dlist_select(token, op);
		ret_val val1;
		ret_val val2;
		ret_val ret;
		// 若单目运算符，则只算val2,不算val1
		val1.int_val = 0;
		ret.int_val = 0;
		switch (op_token->type) {
			case TK_NEGATIVE: 	// negative 负号 -
			case TK_GETVAL:		// get value 取值 *
			case TK_GETADDR:		// get addr 取址 &
			case TK_LOGIC_NEGATE:// logical negate 取反 !
			case TK_BIT_NEGATE:	// bit negate 取反 ~
			break;
			default:
				val1 = eval(token, begin, op - 1, val_type);
		}
		val2 = eval(op_token->last, op + 1, end, val_type);

		if (val_type == TK_DECIMAL) {
			printf("\t\teval double val1 ==> %lf;\n", val1.double_val);
			printf("\t\teval double val2 ==> %lf;\n", val2.double_val);
		} else {
			printf("\t\teval int val1 ==> %ld;\n", val1.int_val);
			printf("\t\teval int val2 ==> %ld;\n", val2.int_val);
		}

		switch (op_token->type) {
			case TK_NEGATIVE: 	// negative 负号 -
				ret.int_val = -val2.int_val;
			break;
			case TK_GETVAL:		// get value 取值 *
				ret.int_val =  0;
			break;
			case TK_GETADDR:		// get addr 取址 &
				ret.int_val =  0;
			break;
			case TK_LOGIC_NEGATE:// logical negate 取反 !
				ret.int_val =  !val2.int_val;
			break;
			case TK_BIT_NEGATE:	// bit negate 取反 ~
				ret.int_val =  ~val2.int_val;
			break;
			case TK_DIV: 		// division 除 /
				if (val_type == TK_DECIMAL) {
					ret.double_val =  val1.double_val / val2.double_val;
				} else {
					ret.int_val =  val1.int_val / val2.int_val;
				}
			break;
			case TK_MUL: 		// multiplication 乘 *
				if (val_type == TK_DECIMAL) {
					ret.double_val =  val1.double_val * val2.double_val;
				} else {
					ret.int_val =  val1.int_val * val2.int_val;
				}
			break;
			case TK_MODULO: 		// modulo 模 %
				ret.int_val =  val1.int_val % val2.int_val;
			break;
			case TK_ADD:			// addition 加 +
				printf("\t\t\t add;\n");
				if (val_type == TK_DECIMAL) {
					ret.double_val =  val1.double_val + val2.double_val;
				} else {
					ret.int_val =  val1.int_val + val2.int_val;
				}
			break;
			case TK_SUB: 		// subtraction 减 -
				if (val_type == TK_DECIMAL) {
					ret.double_val =  val1.double_val - val2.double_val;
				} else {
					ret.int_val =  val1.int_val - val2.int_val;
				}
			break;
			case TK_MVL:			// move left 左移 <<
				ret.int_val =  val1.int_val << val2.int_val;
			break;
			case TK_MVR:			// move right 右移 >>
				ret.int_val =  val1.int_val >> val2.int_val;
			break;
			case TK_MOT:			// more than 大于 >
				ret.int_val =  val1.int_val > val2.int_val;
			break;
			case TK_MOTEQ: 		// more than equal 大于等于 >=
				ret.int_val =  val1.int_val >= val2.int_val;
			break;
			case TK_LET:			// less than 小于 <
				ret.int_val =  val1.int_val < val2.int_val;
			break;
			case TK_LETQE: 		// less than equal 小于等于 <=
				ret.int_val =  val1.int_val <= val2.int_val;
			break;
			case TK_EQ:			// equal 等于 ==
				ret.int_val =  val1.int_val == val2.int_val;
			break;
			case TK_NEQ:			// not equal 不等于 !=
				ret.int_val =  val1.int_val != val2.int_val;
			break;
			case TK_BIT_AND:		// logical and 按位于 &
				ret.int_val =  val1.int_val & val2.int_val;
			break;
			case TK_BIT_XOR:		// logical or 按位或 ^
				ret.int_val =  val1.int_val ^ val2.int_val;
			break;
			case TK_BIT_OR:		// logical or 按位或 |
				ret.int_val =  val1.int_val | val2.int_val;
			break;
			case TK_LOGIC_AND:	// logical and 逻辑于 &&
				ret.int_val =  val1.int_val && val2.int_val;
			break;
			case TK_LOGIC_OR:	// logical or 逻辑或 ||
				ret.int_val =  val1.int_val || val2.int_val;
			break;
			case TK_ASSIGN:		// assign 赋值 =
				ret.int_val =  0;
			break;
		}

		return ret;
	}
}

// 将输入传入进行匹配拆分成token
// 通过token进行语法分析
int lexical_analysis(Node *tokens) {
	Node *token = tokens->last;
	int tokens_num = tokens->id;
	printf("%s: tokens num = %d\n",__func__, tokens_num);
	// token 检查
	int val_type = token_check(token, tokens_num);
	
	// do {

	// 	token = token->next;

	// } while(tokens != NULL && tokens != token);

	// fa

	return val_type;
}


