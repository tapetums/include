//
// stdfloat.h
//
//      Copyright (c) tapetums. All rights reserved.
//

#pragma once
#define _STDFLOAT

#ifdef __cplusplus

using float32_t = float;
using float64_t = double;

#else

typedef float  float32_t;
typedef double float64_t;

#endif // __cplusplus
