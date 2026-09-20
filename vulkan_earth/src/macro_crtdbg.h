#ifdef _DEBUG 
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h> 
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__) 
#ifdef malloc
         #undef malloc
         #endif
         #define malloc(s) (_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
         #ifdef calloc
         #undef calloc
         #endif
         #define calloc(c, s) (_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
         #ifdef realloc
         #undef realloc
         #endif
         #define realloc(p, s) (_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
         #ifdef _expand
         #undef _expand
         #endif
         #define _expand(p, s) (_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__  ))
         #ifdef free
         #undef free
         #endif
         #define free(p) (_free_dbg(p, _NORMAL_BLOCK))
         #ifdef _msize
         #undef _msize
         #endif
         #define _msize(p) (_msize_dbg(p, _NORMAL_BLOCK))
#endif
