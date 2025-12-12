#include <math.h>
#include <float.h>

#define F32_PI              3.14159265358979f
#define F32_TAU             6.28318530717959f
#define F32_DEG_TO_RAD      (TAU / 360.0f)
#define F32_RAD_TO_DEG      (360.0f / TAU)
#define F32_MAX             3.402823466e+38F
#define F32_EPSILON         FLT_EPSILON

typedef struct
{
    f32 x, y;
    
} vector2;

typedef struct
{
    union
    {
        struct
        {
            f32 x, y, z;
        };
        vector2 xy;
    };

} vector3;

typedef struct
{
    union
    {
        struct
        {
            f32 x, y, z, w;
        };
        vector3 xyz;
    };

} vector4;