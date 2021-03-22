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

#endif // _DEBUG_H
