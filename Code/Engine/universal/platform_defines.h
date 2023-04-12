#pragma once

#define IN_USE      &&
#define NOT_IN_USE  &&!
#define USE_IF( x ) &&( ( x ) ? 1 : 0 )&&
#define USING( x )  ( 1 x 1 )

#if defined( _DEBUG ) || defined( DDEBUG )
#define TEST_BUILD  IN_USE
#define FINAL_BUILD NOT_IN_USE
#else
#define TEST_BUILD  NOT_IN_USE
#define FINAL_BUILD IN_USE
#endif

// force test build
#undef TEST_BUILD
#undef FINAL_BUILD
#define TEST_BUILD  IN_USE
#define FINAL_BUILD NOT_IN_USE

#if USING( TEST_BUILD )
#define IF_TEST( ... ) ( __VA_ARGS__ )
#else
#define IF_TEST( ... ) ( (void)0 )
#endif
#if USING( FINAL_BUILD )
#define IF_FINAL( ... ) ( __VA_ARGS__ )
#else
#define IF_FINAL( ... ) ( (void)0 )
#endif
