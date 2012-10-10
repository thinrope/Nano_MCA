
#ifndef __Fixed_h
#define __Fixed_h

typedef s32 fixed;		// fixed decimal scaled by 1000, in format xx,xxx.xxx
#define FIXED_SCALE 1000
#define fixed_from_int(a)((fixed) (a) * FIXED_SCALE)
#define fixed_to_int(a) ((int) ((a) / FIXED_SCALE))
#define fixed_from_uint(a)((fixed) (a) * FIXED_SCALE)
#define fixed_to_uint(a) ((unsigned int) ((a) / FIXED_SCALE))
#define fixed_mul(a, b) (((a) * (b)) / FIXED_SCALE)
#define fixed_div(a, b) (((a) * FIXED_SCALE) /(b))
#define FIXED_ONE	1000
#define FIXED_HALF 500
#define FIXED_DIGITS 10	// xx,xxx.xxx
#define FIXED_BCD 8


#endif
/******************************** END OF FILE *********************************/
