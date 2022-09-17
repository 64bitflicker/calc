#ifndef __CALC_H__
#define __CALC_H__

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#include "common.h"
#include "dlist.h"


// 计算的结果类型
typedef union ret_val_{
    int64_t int_val;
    u_int64_t uint_val;
    double double_val;
} ret_val;

#endif //__CALC_H__