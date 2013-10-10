///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_types.h
// 功能描述: 全局类型定义
// 最后修改: 2005-04-28
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_TYPES_H_
#define _ISE_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ise_stlport.h"

///////////////////////////////////////////////////////////////////////////////
// 类型定义

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

typedef uint8_t   byte;
typedef uint16_t  word;
typedef uint32_t  dword;
typedef uint32_t  uint;

typedef void* Pointer;

typedef vector<string> StringArray;
typedef vector<int> IntegerArray;
typedef set<int> IntegerSet;

///////////////////////////////////////////////////////////////////////////////

#endif // _ISE_TYPES_H_ 
