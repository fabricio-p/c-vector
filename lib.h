#include <string.h>
#include <malloc.h>
#include <stddef.h>

#ifdef __cvector_inline__
#undef __cvector_inline__
#endif
#ifdef CVECTOR_RETURN
#undef CVECTOR_RETURN
#endif
#ifdef CVECTOR_STATUS
#undef CVECTOR_STATUS
#endif
#ifdef TYPEDEF
#undef TYPEDEF
#endif
#ifdef CVECTOR_WITH_NAME
#undef CVECTOR_WITH_NAME
#endif
#ifdef CVECTOR_DECLARE_WITH_NAME
#undef CVECTOR_DECLARE_WITH_NAME
#endif
#ifdef CVECTOR_DECLARE
#undef CVECTOR_DECLARE
#endif
#ifdef CVECTOR
#undef CVECTOR
#endif
#ifdef CVECTOR_HEAP_STRUCT_GEN
#undef CVECTOR_HEAP_STRUCT_GEN
#endif
#ifdef CVECTOR_POINTERMODE_COMPAT_GEN
#undef CVECTOR_POINTERMODE_COMPAT_GEN
#endif
#ifndef VOIDP
#ifdef __cplusplus
#define VOIDP char *
#else
#define VOIDP void *
#endif /* __cplusplus */
#endif /* VOIDP */

#define __cvector_inline__                                              \
  static inline __attribute__((always_inline)) __attribute__((unused))

#ifndef CVECTOR_SILENT
#define CVECTOR_RETURN(expr) { return expr; }
#define CVECTOR_STATUS int
#else
#define CVECTOR_RETURN(expr) { expr; return; }
#define CVECTOR_STATUS void
#endif

#ifdef CVECTOR_NO_TYPEDEF
#define TYPEDEF(_, __)
#undef CVECTOR_NO_TYPEDEF
#else
#define TYPEDEF(type, name) typedef type name
#endif

#ifdef CVECTOR_HEAP_STRUCT
#undef CVECTOR_HEAP_STRUCT
#define CVECTOR_HEAP_STRUCT_GEN(type, name)                       \
  __cvector_inline__                                              \
  name *name##_new() {                                            \
    name *vec = malloc(sizeof(name));                             \
    return vec == NULL || name##_init(vec) ? NULL : vec;          \
  }                                                               \
  __cvector_inline__                                              \
  name *name##_with_capacity(int cap) {                           \
    name *vec = malloc(sizeof(name));                             \
    return vec == NULL || name##_init_with_capacity(vec, cap) ?   \
           NULL : vec;                                            \
  }                                                               \
  __cvector_inline__                                              \
  name *name##_with_length(int len) {                             \
    name *vec = malloc(sizeof(name));                             \
    if (vec == NULL) return NULL;                                 \
    return vec == NULL || name##_init_with_length(vec, len) ?     \
           NULL : vec;                                            \
  }                                                               \
  name *name##_with_fill(int len, type val) {                     \
    name *vec = malloc(sizeof(name));                             \
    if (vec == NULL) return NULL;                                 \
    return vec == NULL || name##_init_with_fill(vec, len, val) ?  \
           NULL : vec;                                            \
  }
#else
#define CVECTOR_HEAP_STRUCT_GEN(type, name)
#endif /* CVECTOR_HEAP_STRUCT */

#ifdef CVECTOR_POINTERMODE_COMPAT
#undef CVECTOR_POINTERMODE_COMPAT
#define CVECTOR_POINTERMODE_COMPAT_GEN(type, name)                      \
  __cvector_inline__                                                    \
  CVECTOR_STATUS name##_init(name *vec) {                               \
    CVECTOR_RETURN((*name = name##_new()) ? 0 : 1);                     \
  }                                                                     \
  __cvector_inline__                                                    \
  CVECTOR_STATUS name##_init_with_capacity(name *vec, int cap) {        \
    CVECTOR_RETURN((*name = name##_with_capacity(cap)) ? 0 : 1);        \
  }                                                                     \
  __cvector_inline__                                                    \
  CVECTOR_STATUS name##_init_with_length(name *vec, int len) {          \
    CVECTOR_RETURN((*name = name##_with_length(len)) ? 0 : 1);          \
  }                                                                     \
  __cvector_inline__                                                    \
  CVECTOR_STATUS name##_init_with_fill(name *vec, int len, type val) {  \
    CVECTOR_RETURN((*name = name##_with_fill(len, val)) ? 0 : 1);       \
  }
#else
#define CVECTOR_POINTERMODE_COMPAT_GEN(type, name)
#endif /* CVECTOR_POINTERMODE_COMPAT */

#ifdef CVECTOR_POINTERMODE
#undef CVECTOR_POINTERMODE

#ifndef _CVECTOR_POINTERMODE_H_
#define _CVECTOR_POINTERMODE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  int cap;
  int len;
} cvector_header;

__cvector_inline__
cvector_header *cvector_get_header(VOIDP vec) {
  return (cvector_header *)(vec - sizeof(cvector_header));
}
__cvector_inline__
int cvectorp_len(VOIDP vec) {
  return cvector_get_header(vec)->len;
}
__cvector_inline__
int cvectorp_cap(VOIDP vec) {
  return cvector_get_header(vec)->cap;
}
__cvector_inline__
int cvectorp_expand(VOIDP *vec, int item_size) {
  int cap = cvector_get_header(*vec)->cap;
  cap = cap ? cap << 1 : 4;
  if (cap < cvectorp_cap(*vec)) return 2;
  int size = sizeof(cvector_header) + cap * item_size;
  VOIDP data = realloc(*vec - sizeof(cvector_header), size);
  if (data == NULL)
    return 1;
  *vec = data + sizeof(cvector_header);
  cvector_header *header = data;
  header->cap = cap;
  return 0;
}
__cvector_inline__
int cvectorp_shrink(VOIDP *vec, int item_size) {
  int cap = cvector_get_header(*vec)->cap / 2;
  int size = sizeof(cvector_header) + cap * item_size;
  VOIDP data = realloc(*vec - sizeof(cvector_header), size);
  if (data == NULL)
    return 1;
  *vec = data + sizeof(cvector_header);
  cvector_header *header = data;
  header->cap = cap;
  return 0;
}

#ifdef __cplusplus
};
#endif

#define FOREACH(itemtype, varname, vec, block)    \
  for (int i = 0; i < cvectorp_len(vec); i++) {   \
    itemtype *varname = &vec[i];                  \
    block;                                        \
  }

#endif /* _CVECTOR_POINTERMODE_H_ */

#define CVECTOR_WITH_NAME(type, name)                             \
  TYPEDEF(type*, name);                                           \
  __cvector_inline__                                              \
  name name##_with_capacity(int cap) {                            \
    int size = sizeof(cvector_header) + cap * sizeof(type);       \
    cvector_header *header = calloc(size, 1);                     \
    if (header == NULL)                                           \
      return NULL;                                                \
    header->cap = cap;                                            \
    header->len = 0;                                              \
    return (name)(header + 1);                                    \
  }                                                               \
  __cvector_inline__                                              \
  name name##_with_length(int len) {                              \
    name vec = name##_with_capacity(len);                         \
    cvector_get_header(vec)->len = len;                           \
    return vec;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  name name##_with_fill(int len, type val) {                      \
    name vec = name##_with_length(len);                           \
    FOREACH(type, item, vec, {                                    \
      *item = val;                                                \
    });                                                           \
    return vec;                                                   \
  }                                                               \
  CVECTOR_POINTERMODE_COMPAT_GEN(type, name)                      \
  __cvector_inline__                                              \
  name name##_new() {                                             \
    return name##_with_capacity(0);                               \
  }                                                               \
  __cvector_inline__                                              \
  int name##_cap(name vec) {                                      \
    return cvectorp_cap(vec);                                     \
  }                                                               \
  __cvector_inline__                                              \
  int name##_len(name vec) {                                      \
    return cvectorp_len(vec);                                     \
  }                                                               \
  __cvector_inline__                                              \
  int name##_expand_if_needed(name *vec) {                        \
    return name##_len(*vec) >= name##_cap(*vec) ?                 \
        cvectorp_expand((VOIDP *)vec, sizeof(type)) : 0;          \
  }                                                               \
  __cvector_inline__                                              \
  int name##_shrink_if_needed(name *vec) {                        \
    return name##_len(*vec) * 3 <= name##_cap(*vec) ?             \
        cvectorp_shrink((VOIDP *)vec, sizeof(type)) : 0;          \
  }                                                               \
  __cvector_inline__                                              \
  type *name##_at(name vec, int idx) {                            \
    return idx >= 0 && idx < name##_len(vec) ? &vec[idx] : NULL;  \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_set(name vec, int idx, type val) {        \
    if (idx < 0 || idx >= name##_len(vec))                        \
      CVECTOR_RETURN(1);                                          \
    vec[idx] = val;                                               \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  type *name##_push_empty(name *vec) {                            \
    return (name##_expand_if_needed(vec)) ?                       \
            NULL : &((*vec)[(cvector_get_header(*vec)->len)++]);  \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_pushp(name *vec, type *val_p) {           \
    type *p;                                                      \
    if ((p = name##_push_empty(vec)) == NULL)                     \
      CVECTOR_RETURN(1);                                          \
    (sizeof(type) > 256) ? (void)memcpy(p, val_p, sizeof(type)) : \
                           (void)(*p = *val_p);                   \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_push(name *vec, type val) {               \
    type *p;                                                      \
    if ((p = name##_push_empty(vec)) == NULL)                     \
      CVECTOR_RETURN(1);                                          \
    *p = val;                                                     \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop(name vec) {                                      \
    if (cvector_get_header(vec)->len > 0) {                       \
      int *len_p = &(cvector_get_header(vec)->len);               \
      memset(&(vec[*len_p]), 0, sizeof(type));                    \
      return --(*len_p);                                          \
    }                                                             \
    return -1;                                                    \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop_get_into(name vec, type *val_p) {                \
    if (name##_len(vec) != 0) {                                   \
      type *p = &(vec[--(cvector_get_header(vec)->len)]);         \
      if (sizeof(type) > 256) {                                   \
        memcpy(val_p, p, sizeof(type));                           \
      } else {                                                    \
        *val_p = *p;                                              \
      }                                                           \
      return name##_len(vec);                                     \
    } else {                                                      \
      memset(val_p, '\0', sizeof(type));                          \
      return -1;                                                  \
    }                                                             \
  }                                                               \
  __cvector_inline__                                              \
  type name##_pop_get(name vec) {                                 \
    type val;                                                     \
    name##_pop_get_into(vec, &val);                               \
    return val;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop_shrink_get_into(name *vec, type *val_p) {        \
    int len = name##_pop_get_into(*vec, val_p);                   \
    name##_shrink_if_needed(vec);                                 \
    return len;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  type name##_pop_shrink_get(name *vec) {                         \
    type val;                                                     \
    name##_pop_shrink_get_into(vec, &val);                        \
    return val;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  void name##_clear(name vec) {                                   \
    memset(vec, '\0', sizeof(type) *name##_len(vec));             \
    cvector_get_header(vec)->len = 0;                             \
  }                                                               \
  __cvector_inline__                                              \
  name name##_clone(name vec) {                                   \
    name clone = name##_with_capacity(name##_cap(vec));           \
    if (clone != NULL) {                                          \
      cvector_get_header(clone)->len = name##_len(vec);           \
      memcpy(clone, vec, name##_len(vec) * sizeof(type));         \
    }                                                             \
    return clone;                                                 \
  }                                                               \
  __cvector_inline__                                              \
  name name##_deep_clone(                                         \
      name vec, void (*cloner)(type const *, type *)              \
  ) {                                                             \
    name clone = name##_with_capacity(name##_cap(vec));           \
    if (clone != NULL) {                                          \
      cvector_get_header(clone)->len = name##_len(vec);           \
      for (int i = 0; i < name##_len(vec); ++i) {                 \
        cloner(&(vec[i]), &(clone[i]));                           \
      }                                                           \
    }                                                             \
    return clone;                                                 \
  }                                                               \
  __cvector_inline__                                              \
  void name##_cleanup(name vec) {                                 \
    free((VOIDP)vec - sizeof(cvector_header));                    \
  }



#else

#ifndef _CVECTOR_H_
#define _CVECTOR_H_

#ifndef CVECTOR_ALIGNMENT
#define CVECTOR_ALIGNMENT ((int)sizeof(size_t))
#endif
#define CVECTOR_ALIGN(s)                                          \
  (((s) + (CVECTOR_ALIGNMENT) - 1) & ~((CVECTOR_ALIGNMENT) - 1))

#ifdef __cplusplus
extern "C" {
#endif

#define CVECTOR_STRUCT(type)  \
  struct {                    \
    int   c;                  \
    int   l;                  \
    type *d;                  \
  }

typedef CVECTOR_STRUCT(void) cvector_t;

__cvector_inline__
int cvector_init(cvector_t *vec, int item_size, int cap) {
  vec->l = 0;
  vec->c = cap;
  int size = CVECTOR_ALIGN(item_size * cap);
  if (cap != 0) {
    if ((vec->d = malloc(size)) == NULL)
      return 1;
    memset(vec->d, 0, size);
  } else {
    vec->d = NULL;
  }
  return 0;
}
__cvector_inline__
int cvector_expand(cvector_t *vec, int item_size) {
  int cap = vec->c ? vec->c << 1 : CVECTOR_ALIGNMENT;
  int size = CVECTOR_ALIGN(cap * item_size);
  VOIDP data = realloc(vec->d, size);
  if (data == NULL)
    return 1;
  vec->c = cap;
  vec->d = data;
  return 0;
}
__cvector_inline__
int cvector_shrink(cvector_t *vec, int item_size) {
  int cap = vec->c >> 1;
  int size = CVECTOR_ALIGN(cap * item_size);
  VOIDP data = realloc(vec->d, size);
  if (data == NULL)
    return 1;
  vec->c = cap;
  vec->d = data;
  return 0;
}
__cvector_inline__
cvector_t cvector_clone(cvector_t *vec, int item_size) {
  cvector_t new_vec;
  cvector_init(&new_vec, item_size, vec->c);
  new_vec.l = vec->l;
  memcpy(new_vec.d, vec->d, new_vec.l * item_size);
  return new_vec;
}
__cvector_inline__
void cvector_cleanup(cvector_t *vec) {
  if (vec != NULL) {
    if (vec->d != NULL)
      free(vec->d);
    vec->l = vec->c = 0;
    vec->d = NULL;
  }
}

__cvector_inline__
int cvector_expand_if_needed(cvector_t *vec, int item_size) {
  if (vec->l >= vec->c)
    return cvector_expand(vec, item_size);
  return 0;
}
__cvector_inline__
int cvector_shrink_if_needed(cvector_t *vec, int item_size) {
  if (vec->l * 3 <= vec->c && vec->c > CVECTOR_ALIGNMENT)
    return cvector_shrink(vec, item_size);
  return 0;
}
__cvector_inline__
VOIDP cvector_get(cvector_t *vec, int idx, int item_size) {
  return vec->d + idx * item_size;
}
__cvector_inline__
VOIDP cvector_at(cvector_t *vec, int idx, int item_size) {
  if (idx < 0)
    idx = vec->l + idx;
  return idx >= 0 && idx < vec->l ?
          cvector_get(vec, idx, item_size) : NULL;
}

#ifdef __cplusplus
};
#endif

#endif /* _CVECTOR_H_ */

#define CVECTOR_WITH_NAME(type, name)                             \
  typedef CVECTOR_STRUCT(type) name;                              \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_init(name *vec) {                         \
    CVECTOR_RETURN(                                               \
        cvector_init((cvector_t *)vec, sizeof(type), 0)           \
    );                                                            \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_init_with_capacity(name *vec, int cap) {  \
    CVECTOR_RETURN(                                               \
        cvector_init((cvector_t *)vec, sizeof(type), cap)         \
    );                                                            \
  }                                                               \
  __cvector_inline__                                              \
  void name##_set(name *vec, int idx, type val) {                 \
    /* TODO: DEREF */                                             \
    memcpy(&(vec->d[idx]), &val, sizeof(type));                   \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_init_with_length(name *vec, int len) {    \
    if (name##_init_with_capacity(vec, len))                      \
      CVECTOR_RETURN(1);                                          \
    vec->l = len;                                                 \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_init_with_fill(name *vec, int len,        \
                                       type val) {                \
    if (name##_init_with_length(vec, len))                        \
      return 1;                                                   \
    for (int i = 0; i < len; i++) {                               \
      name##_set(vec, i, val);                                    \
    }                                                             \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  CVECTOR_HEAP_STRUCT_GEN(type, name)                             \
  __cvector_inline__                                              \
  type *name##_at(name *vec, int idx) {                           \
    return (type *)cvector_at(                                    \
        (cvector_t *)vec, idx, sizeof(type)                       \
    );                                                            \
  }                                                               \
  __cvector_inline__                                              \
  type *name##_get(name *vec, int idx) {                          \
    return (type *)cvector_get(                                   \
        (cvector_t *)vec, idx, sizeof(type)                       \
    );                                                            \
  }                                                               \
  __cvector_inline__                                              \
  name name##_clone(name *vec) {                                  \
    union { cvector_t cv; name v; } u;                            \
    u.cv = cvector_clone((cvector_t *)vec, sizeof(type));         \
    return u.v;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_set_at(name *vec, int idx, type val) {    \
    if (idx < 0 || idx >= vec->l)                                 \
      CVECTOR_RETURN(1);                                          \
    name##_set(vec, idx, val);                                    \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  type *name##_push_empty(name *vec) {                            \
    return cvector_expand_if_needed(                              \
        (cvector_t *)vec, sizeof(type)                            \
    ) ? NULL :                                                    \
        cvector_get((cvector_t *)vec, (vec->l)++, sizeof(type));  \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_push(name *vec, type item) {              \
    type *p;                                                      \
    if ((p = name##_push_empty(vec)) != NULL) {                   \
      if (sizeof(type) > 256) memcpy(p, &item, sizeof(type));     \
      else                    *p = item;                          \
      CVECTOR_RETURN(0);                                          \
    }                                                             \
    CVECTOR_RETURN(1);                                            \
  }                                                               \
  __cvector_inline__                                              \
  CVECTOR_STATUS name##_pushp(name *vec, type *val_p) {           \
    type *p;                                                      \
    if ((p = name##_push_empty(vec)) == NULL)                     \
      CVECTOR_RETURN(1);                                          \
    (sizeof(type) > 256) ? (void)memcpy(p, val_p, sizeof(type)) : \
                           (void)(*p = *val_p);                   \
    CVECTOR_RETURN(0);                                            \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop(name *vec) {                                     \
    if (vec->l != 0) {                                            \
      --(vec->l);                                                 \
      memset(                                                     \
          vec->d + vec->l * sizeof(type), '\0', sizeof(type)      \
      );                                                          \
      return vec->l;                                              \
    }                                                             \
    return -1;                                                    \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop_get_into(name *vec, type *val_p) {               \
    if (vec->l != 0) {                                            \
      type *p = &(vec->d[--(vec->l)]);                            \
      if (sizeof(type) > 256) {                                   \
        memcpy(val_p, p, sizeof(type));                           \
      } else {                                                    \
        *val_p = *p;                                              \
      }                                                           \
      return vec->l;                                              \
    } else {                                                      \
      memset(val_p, '\0', sizeof(type));                          \
      return -1;                                                  \
    }                                                             \
  }                                                               \
  __cvector_inline__                                              \
  type name##_pop_get(name *vec) {                                \
    type val;                                                     \
    name##_pop_get_into(vec, &val);                               \
    return val;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  int name##_pop_shrink_get_into(name *vec, type *val_p) {        \
    int len = name##_pop_get_into(vec, val_p);                    \
    cvector_shrink_if_needed((cvector_t *)vec, sizeof(type));     \
    return len;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  type name##_pop_shrink_get(name *vec) {                         \
    type val;                                                     \
    name##_pop_shrink_get_into(vec, &val);                        \
    return val;                                                   \
  }                                                               \
  __cvector_inline__                                              \
  void name##_clear(name *vec) {                                  \
    vec->l = 0;                                                   \
    memset(vec->d, '\0', vec->l * sizeof(type));                  \
  }                                                               \
  __cvector_inline__                                              \
  name name##_deep_clone(                                         \
      name *vec, void (*cloner)(type const *, type *)             \
  ) {                                                             \
    name new_vec;                                                 \
    if (name##_init_with_capacity(&new_vec, vec->c) == 0) {       \
      new_vec.l = vec->l;                                         \
      for (int i = 0; i < vec->l; ++i) {                          \
        cloner(&(vec->d[i]), &(new_vec.d[i]));                    \
      }                                                           \
    }                                                             \
    return new_vec;                                               \
  }                                                               \
  __cvector_inline__                                              \
  void name##_print(name *vec, void (*print_item)(type *)) {      \
    printf("[" #name "][cvector_t(" #type ")] {\n"                \
         "  .len: %d\n"                                           \
         "  .cap: %d\n"                                           \
         "  .data: %p\n"                                          \
         "  .items: [",                                           \
         vec->l, vec->c, vec->d);                                 \
    if (print_item == NULL) {                                     \
      printf(" -- No item print function "                        \
             "available --");                                     \
    } else {                                                      \
      for (int i = 0; i < vec->l; i++) {                          \
        type *item = name##_get(vec, i);                          \
        if (i == 0) {                                             \
          print_item(item);                                       \
        } else {                                                  \
          printf(", ");                                           \
          print_item(item);                                       \
        }                                                         \
      }                                                           \
      printf("]  \n}\n");                                         \
    }                                                             \
  }                                                               \
  __cvector_inline__                                              \
  void name##_cleanup(name *vec) {                                \
    cvector_cleanup((cvector_t *)vec);                            \
  }

#endif /* CVECTOR_POINTERMODE */
#define CVECTOR(type) CVECTOR_WITH_NAME(type, Vector_##type)
