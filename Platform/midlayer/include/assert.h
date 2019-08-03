/*********************************************
* @文件: assert.h
* @作者: wuhuiskt
* @版本: v1.0.1
* @时间: 2017-11-14
* @概要: 判断数据的有效性，并返
                   回相应的值。
*********************************************/
#ifndef __ASSERT_H
#define __ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif//end __cplusplus

#include "debug.h"

//arg1 == arg2 return
#define ASSERT_EQUAL_RETURN(arg1, arg2)\
    do{\
        if(arg1 == arg2)\
        {\
            print_err("equal failed!\n");\
            return;\
        }\
    }while(0)
	
//arg1 == arg2 return val
#define ASSERT_EQUAL_RETVAL(arg1, arg2, val)\
    do{\
        if(arg1 == arg2)\
        {\
            print_err("equal failed!\n");\
            return val;\
        }\
    }while(0)

//arg1 != arg2 return val
#define ASSERT_NE_RETVAL(arg1, arg2, val)\
    do{\
        if(arg1 != arg2)\
        {\
            printf("no equal failed!\n");\
            return val;\
        }\
    }while(0);

//arg1 > arg2 return val
#define ASSERT_GT_RETVAL(arg1, arg2, val)\
    do{\
        if(arg1 > arg2)\
        {\
            print_err("no equal failed!\n");\
            return val;\
        }\
    }while(0)

//arg1 >= arg2 return val
#define ASSERT_GE_RETVAL(arg1, arg2, val)\
    do{\
        if(arg1 >= arg2)\
        {\
            print_err("no equal failed!\n");\
            return val;\
        }\
    }while(0)

//arg1 != arg2 return
#define ASSERT_NE_RETURN(arg1, arg2)\
    do{\
        if(arg1 != arg2)\
        {\
            print_err("no equal failed!\n");\
            return;\
        }\
    }while(0);

//arg1 > arg2 return
#define ASSERT_GT_RETURN(arg1, arg2)\
    do{\
        if(arg1 > arg2)\
        {\
            print_err("no equal failed!\n");\
            return;\
        }\
    }while(0)

//arg1 >= arg2 return
#define ASSERT_GE_RETURN(arg1, arg2)\
    do{\
        if(arg1 >= arg2)\
        {\
            print_err("no equal failed!\n");\
            return;\
        }\
    }while(0)


//arg1 <= arg2 return
#define ASSERT_NGT_RETURN(arg1, arg2)\
    do{\
        if(arg1 <= arg2)\
        {\
            print_err("no equal failed!\n");\
            return;\
        }\
    }while(0)

//execute function return value  !=  ret return val
#define ASSERT_FUN_FAILED_RETVAL(fun, ret, val)\
    do{\
        if(ret != fun)\
        {\
            print_err("function return failed!\n");\
            return val;\
        }\
    }while(0)

#ifdef __cplusplus
}
#endif//end __cplusplus

#endif //end __ASSERT_H
