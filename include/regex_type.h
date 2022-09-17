#ifndef __REGEX_H__
#define __REGEX_H__

#include "dlist.h"

// test {} [] () - * ~ / % + << >> >= <= > < == != ! ^ && & || | = 0x123 0123 0b101 999 0 3.14 $eip1
enum {	
	UNDEFAULT = 0,	// 未定义类型
	// 从左到右
	TK_LBBRACK = 1, // left big brackets {
	TK_RBBRACK, 	// right big brackets }

	TK_LSBRACK, 	// left square brackets [
	TK_RSBRACK, 	// right square brackets ]

	TK_LBRACK, 		// left brackets (
	TK_RBRACK, 		// right brackets )

	TK_BRACK_SIGN_DIVISION,	// brack sign division, 括号和符号的分割

	// 从右到左 匹配后默认值，检查后会更新
	TK_NEGATIVE, 	// negative 负号 -
	TK_GETVAL,		// get value 取值 *
	TK_GETADDR,		// get addr 取址 &
	TK_LOGIC_NEGATE,// logical negate 取反 !
	TK_BIT_NEGATE,	// bit negate 取反 ~

	// 从左到右
	TK_DIV, 		// division 除 /
	TK_MUL, 		// multiplication 乘 *
	TK_MODULO, 		// modulo 模 %

	TK_ADD,			// addition 加 +
	TK_SUB, 		// subtraction 减 -

	TK_MVL,			// move left 左移 <<
	TK_MVR,			// move right 右移 >>

	TK_MOT,			// more than 大于 >
	TK_MOTEQ, 		// more than equal 大于等于 >=
	TK_LET,			// less than 小于 <
	TK_LETQE, 		// less than equal 小于等于 <=

	TK_EQ,			// equal 等于 ==
	TK_NEQ,			// not equal 不等于 !=

	TK_BIT_AND,		// logical and 按位于 &
	TK_BIT_XOR,		// logical or 按位或 ^
	TK_BIT_OR,		// logical or 按位或 |

	TK_LOGIC_AND,	// logical and 逻辑于 &&
	TK_LOGIC_OR,	// logical or 逻辑或 ||

	TK_ASSIGN,		// assign 赋值 =

	TK_DIGIT_SIGN_DIVISION,	// digit sign division, 括号和符号的分割

	// 值
	TK_HEX_INTEGER,	// hex integer 十六进制整数 0x
	TK_OCT_INTEGER,	// octal integer 八进制整数 0
	TK_BIN_INTEGER,	// binary integer 二进制整数 0b
	TK_NEINTEGER,	// 负数
	TK_INTEGER,	// hex integer 整数  [0-9]+
	TK_DECIMAL,	// decimal 小数 [0-9]+\.[0-9]+

	// 变量
	TK_VARIABLE, // variable 变量	\$[0-9a-zA-Z]
};


// extern bool make_regex(char *buf, Node **tokens);
extern int token_check(Node *token, int sum);

#endif // __REGEX_H__