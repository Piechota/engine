#pragma once

#ifdef _DEBUG
#include <assert.h>
#define ASSERT( condition ) assert( condition )
#define ASSERT_STR( condition, msg ) assert( condition && msg )
#define CT_ASSERT( condition ) static_assert( condition )
#else
#define ASSERT( condition ) 
#define ASSERT_STR( condition, msg ) 
#define CT_ASSERT( condition ) 
#endif

#define NOVTABLE __declspec(novtable)
#define FORCE_INLINE __forceinline
#define FLAG( f ) (1 << f)

#include "types.h"
#include "array.h"
#include "staticArray.h"
#include "math.h"
#include <string.h>
#include <cstdlib>
POD_TYPE(bool)
POD_TYPE(UINT16)
POD_TYPE(UINT32)
POD_TYPE(INT16)
POD_TYPE(INT32)
POD_TYPE(INT64)
POD_TYPE(unsigned char)
POD_TYPE(char)
POD_TYPE(float)
POD_TYPE(double)
POD_TYPE(Vec2)
POD_TYPE(Vec2i)
POD_TYPE(Vec3)
POD_TYPE(Vec3i)
POD_TYPE(Vec4)
POD_TYPE(Matrix3x3)
POD_TYPE(Matrix4x4)
POD_TYPE(Quaternion)
