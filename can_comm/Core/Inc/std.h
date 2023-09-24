/* @filename 	: 	std.h
 * @brief		:	Holds standard typedefs/macros/etc. and helping macros
 * @author		:	Hossam Elwahsh
 *
 * */

typedef unsigned char boolean;

#define STATIC static
#define BOOLEAN boolean

#define TRUE 	(1)
#define FALSE 	(0)
#define ZERO	(0)
#define NULL_PTR ((void *)0)
#define MOD(val, mod_with) (val % mod_with)
#define BYTE_MAX_VAL 0xFF
#define ARR_SIZE(arr) ( sizeof(arr) / sizeof(arr[0]) )
