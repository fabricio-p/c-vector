#ifndef _CVECTOR_H_
#define _CVECTOR_H_
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#ifndef ALIGNMENT
#define ALIGNMENT (int)sizeof(size_t)
#endif

#if defined(__cplusplus)
#define __inline__ inline
#elif defined(_MSC_VER)
#define __inline__ __attribute__((always_inline))
#else
#define __inline__ static
#endif

typedef struct {
	int  item_size;
	int  cap;
	int  len;
	int  size;
	void *data;
	void (* print_item)(void *);
} cvector_t;

int       cvector_init   (cvector_t *, int, int);
void      *cvector_get   (cvector_t *, int);
int       cvector_expand (cvector_t *);
int       cvector_shrink (cvector_t *);
cvector_t cvector_clone  (cvector_t *);
void      cvector_cleanup(cvector_t *);

__inline__
int cvector_expand_if_needed(cvector_t *vec) {
	if (vec->len >= vec->cap)
		return cvector_expand(vec);
	return 0;
}
__inline__
int cvector_shrink_if_needed(cvector_t *vec) {
	if (vec->len <= vec->cap / 2 && vec->cap > ALIGNMENT)
		return cvector_shrink(vec);
	return 0;
}

#define CVECTOR_DECLARE_WITH_NAME(type, name)					\
	typedef cvector_t name;										\
	__inline__													\
	int name##_init(name *vec) {								\
		return cvector_init(vec, sizeof(type), 0);				\
	}															\
	__inline__													\
	int name##_init_with_capacity(name *vec, int cap) {			\
		return cvector_init(vec, sizeof(type), cap);			\
	}															\
	int name##_init_with_fill(name *, int, type);				\
	__inline__													\
	type *name##_get(name *vec, int idx) {						\
		return (type *)cvector_get((cvector_t *)vec, idx);		\
	}															\
	__inline__													\
	name name##_clone(name *vec) {								\
		return cvector_clone(vec);								\
	}															\
	int name##_set(name *, int, type);							\
	int name##_push(name *, type);								\
	type name##_pop(name *);									\
	name name##_deep_clone(name *, type (*cloner)(type *));		\
	void name##_print(name *);									\
	__inline__													\
	void name##_cleanup(name *vec) { cvector_cleanup(vec); }

#define CVECTOR_WITH_NAME(type, name)							\
	int name##_init_with_fill(name *vec, int len, type val) {	\
		if (name##_init(vec))									\
		return 1;												\
		for (int left = len; left > 0; left--) {				\
			if (name##_push(vec, val))							\
			return 1;											\
		}														\
		return 0;												\
	}															\
	int name##_set(name *vec, int idx, type val) {				\
		if (idx >= vec->len)									\
		return 1;												\
		memcpy((type *)vec->data + idx, &val,					\
				sizeof(type));									\
		return 0;												\
	}															\
	int name##_push(name *vec, type item) {						\
		if (cvector_expand_if_needed(vec) != 0) {				\
			return 1;											\
		}														\
		memcpy(vec->data + ((vec->len)++) * vec->item_size,		\
				&item, sizeof(type));							\
		return 0;												\
	}															\
	type name##_pop(name *vec) {								\
		type val;												\
		memset(&val, 0, sizeof(type));							\
		if (vec->len <= 0)										\
		return val;												\
		type *ptr = (type *)(vec->data +						\
				(--(vec->len)) * sizeof(type));					\
		memcpy(&val, ptr, sizeof(type));						\
		cvector_shrink_if_needed(vec);							\
		return val;												\
	}															\
	name name##_deep_clone(name *vec, type (*cloner)(type *)) {	\
		name new_vec;											\
		name##_init_with_capacity(&new_vec, vec->cap);			\
		for (int i = 0; i < vec->len; i++) {					\
			name##_push(&new_vec, cloner(name##_get(vec, i)));	\
		}														\
		return new_vec;											\
	}															\
	void name##_print(name *vec) {								\
		printf("[" #name "][cvector_t(" #type ")] {\n");		\
		printf("  .len: %d\n", vec->len);						\
		printf("  .cap: %d\n", vec->cap);						\
		printf("  .data: %p\n", vec->data);						\
		printf("  .items: [");															\
		if (vec->print_item == NULL) {							\
			printf(" -- No item print function "				\
					"available --");							\
		} else {												\
			for (int i = 0; i < vec->len; i++) {				\
				type *item = name##_get(vec, i);				\
				if (i == 0) {									\
					vec->print_item(item);						\
				} else {										\
					printf(", ");								\
					vec->print_item(item);						\
				}												\
			}													\
			printf("]\n}\n");									\
		}														\
	}


#endif /* _CVECTOR_H_ */
