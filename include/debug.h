/**
 * © jumbuna, 2021
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef DEBUG
#define D(x) 
#define PRIVATE static
#else
#define D(x) x
#define PRIVATE
#endif // DEBUG

#define RETURN_ON_ERROR_WITH_VALUE(...)   \
    if(last_error) return (__VA_ARGS__);\
    else

#define RETURN_ON_ERROR_VOID()  \
    if(last_error) return;\
    else

#define ERROR(x) (last_error = x)

#endif // _DEBUG_H
