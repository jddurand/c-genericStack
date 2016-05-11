#ifndef GENERICSTACK_H
#define GENERICSTACK_H

#include <stdlib.h>       /* For malloc, free */
#include <string.h>       /* For memcpy */
#include <stddef.h>       /* For size_t */

/* =============== */
/* C generic stack */
/* =============== */

/* Most of the generic stack implementations either assume that every  */
/* element is of the same size, or bypass type checking going through  */
/* a C layer. This version does not have these two constraints.        */
/* By default it is restricted to ANSI-C data type, nevertheless       */
/* adding others is as trivial as looking into e.g. long long below.   */
/* Please note that, if the data can typecast to the stack, no warning */

/* Define GENERICSTACK_C99 to have C99 data type */

#ifdef GENERICSTACK_C99
#  undef GENERICSTACK_HAVE_LONG_LONG
#  define GENERICSTACK_HAVE_LONG_LONG 1
#  undef GENERICSTACK_HAVE__BOOL
#  define GENERICSTACK_HAVE__BOOL     1
#  undef GENERICSTACK_HAVE__COMPLEX
#  define GENERICSTACK_HAVE__COMPLEX  1
#else
#  ifndef GENERICSTACK_HAVE_LONG_LONG
#    define GENERICSTACK_HAVE_LONG_LONG 0
#  endif
#  ifndef GENERICSTACK_HAVE__BOOL
#    define GENERICSTACK_HAVE__BOOL     0
#  endif
#  ifndef GENERICSTACK_HAVE__COMPLEX
#    define GENERICSTACK_HAVE__COMPLEX  0
#  endif
#endif

typedef struct genericStackItemAny {
  void *p;
  size_t size;
  void *(*malloc)(size_t size);
  void *(*memcpy)(void *dst, void *src, size_t size);
  void *(*free)(void *p);
} genericStackItemAny_t;

typedef enum genericStackItemType {
  GENERICSTACKITEMTYPE_CHAR,
  GENERICSTACKITEMTYPE_SHORT,
  GENERICSTACKITEMTYPE_INT,
  GENERICSTACKITEMTYPE_LONG,
  GENERICSTACKITEMTYPE_FLOAT,
  GENERICSTACKITEMTYPE_DOUBLE,
  GENERICSTACKITEMTYPE_PTR,
#if GENERICSTACK_HAVE_LONG_LONG > 0
  GENERICSTACKITEMTYPE_LONG_LONG,
#endif
#if GENERICSTACK_HAVE__BOOL > 0
  GENERICSTACKITEMTYPE__BOOL,
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
  GENERICSTACKITEMTYPE_FLOAT__COMPLEX,
  GENERICSTACKITEMTYPE_DOUBLE__COMPLEX,
  GENERICSTACKITEMTYPE_LONG_DOUBLE__COMPLEX,
#endif
  GENERICSTACKITEMTYPE_ANY
} genericStackItemType_t;

typedef struct genericStackItem {
  genericStackItemType_t type;
  union {
    char c;
    short s;
    int i;
    long l;
    float f;
    double d;
    void *p;
#if GENERICSTACK_HAVE_LONG_LONG > 0
    long long ll;
#endif
#if GENERICSTACK_HAVE__BOOL > 0
    _Bool b;
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
    float _Complex fc;
    double _Complex dc;
    long double _Complex ldc;
#endif
    genericStackItemAny_t any;
  } u;
} genericStackItem_t;

typedef struct genericStack {
  size_t size;
  size_t used;
  genericStackItem_t *items;
} genericStack_t;

#define GENERICSTACK_DECL(stackName) genericStack_t * stackName

#define GENERICSTACK_NEW(stackName) do {				\
    stackName = malloc(sizeof(genericStack_t));                         \
    stackName->size = 0;                                                \
    stackName->used = 0;                                                \
    stackName->items = NULL;                                            \
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, size) do {			\
    size_t _size = (size);						\
    GENERICSTACK_NEW(stackName);					\
    stackName->items = malloc(sizeof(genericStackItem_t) * (stackName->size = _size)); \
  } while (0)

#define GENERICSTACK_EXTEND(stackName) do {                             \
    if (stackName->used > stackName->size) {                            \
      stackName->items = (stackName->items != NULL) ? realloc(stackName->items, sizeof(genericStackItem_t) * stackName->used) : malloc(sizeof(genericStackItem_t) * stackName->used); \
      stackName->size = stackName->used;                                \
    }									\
  } while (0)

#define GENERICSTACK_PUSH_BASIC_TYPE(stackName, varType, var, itemType, dst) do { \
    size_t _i = stackName->used++;                                      \
									\
    GENERICSTACK_EXTEND(stackName);                                     \
    stackName->items[_i].type = (itemType);                             \
    stackName->items[_i].u.dst = (varType) (var);                       \
  } while (0)

#define GENERICSTACK_PUSH_CHAR(stackName, var)   GENERICSTACK_PUSH_BASIC_TYPE(stackName, char,   var, GENERICSTACKITEMTYPE_CHAR, c)
#define GENERICSTACK_PUSH_SHORT(stackName, var)  GENERICSTACK_PUSH_BASIC_TYPE(stackName, short,  var, GENERICSTACKITEMTYPE_SHORT, s)
#define GENERICSTACK_PUSH_INT(stackName, var)    GENERICSTACK_PUSH_BASIC_TYPE(stackName, int,    var, GENERICSTACKITEMTYPE_INT, i)
#define GENERICSTACK_PUSH_LONG(stackName, var)   GENERICSTACK_PUSH_BASIC_TYPE(stackName, long,   var, GENERICSTACKITEMTYPE_LONG, l)
#define GENERICSTACK_PUSH_FLOAT(stackName, var)  GENERICSTACK_PUSH_BASIC_TYPE(stackName, float,  var, GENERICSTACKITEMTYPE_FLOAT, f)
#define GENERICSTACK_PUSH_DOUBLE(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, double, var, GENERICSTACKITEMTYPE_DOUBLE, d)
#define GENERICSTACK_PUSH_PTR(stackName, var)    GENERICSTACK_PUSH_BASIC_TYPE(stackName, void *, var, GENERICSTACKITEMTYPE_PTR, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_PUSH_LONG_LONG(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, long long, var, GENERICSTACKITEMTYPE_LONG_LONG, ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_PUSH__BOOL(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, _Bool, var, GENERICSTACKITEMTYPE_LONG_LONG, b)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_PUSH_FLOAT__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, float _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, fc)
#define GENERICSTACK_PUSH_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, dc)
#define GENERICSTACK_PUSH_LONG_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, long double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, ldc)
#endif

#define GENERICSTACK_PUSH_ANY(stackName, type, var, mallocp, memcpyp, freep) do { \
    size_t _i = stackName->used++;                                      \
    size_t _size = sizeof(type);					\
    void *_p;								\
    void *_malloc = (void (*)()) (mallocp);				\
    void *_memcpy = (void (*)()) (memcpyp);				\
    void *_free = (void (*)()) (freefp);				\
    									\
    GENERICSTACK_EXTEND(stackName);                                     \
    p = (mallocp == NULL) ? malloc(_size) : mallocp(_size);		\
    if (memcpyp == NULL) {                                              \
      memcpy(p, var, _size);                                            \
    } else {                                                            \
      memcpyp(p, var, _size);                                           \
    }                                                                   \
    stackName->items[_i].type = GENERICSTACKITEMTYPE_ANY;               \
    stackName->items[_i].u.any.p = p;                                   \
    stackName->items[_i].u.any.size = _size;                            \
    stackName->items[_i].u.any.malloc = mallocp;                        \
    stackName->items[_i].u.any.memcpy = memcpyp;                        \
    stackName->items[_i].u.any.free = freep;                            \
									\
  } while (0)

#define GENERICSTACK_POP_BASIC_TYPE(stackName, src) stackName->items[--stackName->used].u.src

#define GENERICSTACK_POP_CHAR(stackName)   GENERICSTACK_POP_BASIC_TYPE(stackName, c)
#define GENERICSTACK_POP_SHORT(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, s)
#define GENERICSTACK_POP_INT(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, i)
#define GENERICSTACK_POP_LONG(stackName)   GENERICSTACK_POP_BASIC_TYPE(stackName, l)
#define GENERICSTACK_POP_FLOAT(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, f)
#define GENERICSTACK_POP_DOUBLE(stackName) GENERICSTACK_POP_BASIC_TYPE(stackName, d)
#define GENERICSTACK_POP_PTR(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_POP_LONG_LONG(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_POP__BOOL(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, b)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_POP_FLOAT__COMPLEX(stackName)       GENERICSTACK_POP_BASIC_TYPE(stackName, fc)
#define GENERICSTACK_POP_DOUBLE__COMPLEX(stackName)      GENERICSTACK_POP_BASIC_TYPE(stackName, dc)
#define GENERICSTACK_POP_LONG_DOUBLE__COMPLEX(stackName) GENERICSTACK_POP_BASIC_TYPE(stackName, ldc)
#endif

#define GENERICSTACK_POP_ANY(stackName, type, var) do {                 \
    void *p;								\
    genericStackItemAny_t any = stackName->items[--stackName->used].u.any; \
									\
    p = (any.malloc == NULL) ? malloc(any.size) : any.malloc(any.size); \
    if (any.memcpy == NULL) {                                           \
      memcpy(p, any.p, any.size);                                       \
    } else {                                                            \
      any.memcpy(p, any.p, any.size);                                   \
    }                                                                   \
    if (any.free == NULL) {                                             \
      free(any.p);                                                      \
    } else {                                                            \
      any.free(any.p);                                                  \
    }                                                                   \
    (var) = (type) p;							\
  } while (0)

#define GENERICSTACK_FREE(stackName) do {				\
    if (stackName->size > 0) {                                          \
      while (stackName->used > 0) {                                     \
        if (stackName->items[stackName->used - 1].type == GENERICSTACKITEMTYPE_ANY) { \
          void *_p;                                                     \
          GENERICSTACK_POP_ANY(stackName, void *, _p);			\
        }                                                               \
      }									\
      free(stackName->items);                                           \
    }                                                                   \
    free(stackName);                                                    \
  } while (0);

#endif /* GENERICSTACK_H */