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


#if defined(__cplusplus)
#define __cvector_inline__ inline __attribute__((always_inline))
#else
#define __cvector_inline__ static __attribute__((always_inline))
#endif

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


#ifdef CVECTOR_FATPOINTER
#undef CVECTOR_FATPOINTER

#ifndef _CVECTOR_FATPOINTER_H_
#define _CVECTOR_FATPOINTER_H_

typedef struct {
	int cap;
	int len;
} cvector_header;

__cvector_inline__
cvector_header *cvector_get_header(void *vec) {
	return (cvector_header *)(vec - sizeof(cvector_header));
}
__cvector_inline__
int cvector_expand(void **vec, int item_size) {
	int cap = cvector_get_header(*vec)->cap;
	cap += cap == 0 ? 4 : ((cap & 0xff) & ~1);
	int size = sizeof(cvector_header) + cap * item_size;
	void *data = realloc(*vec - sizeof(cvector_header), size);
	if (data == NULL)
		return 1;
	*vec = data + sizeof(cvector_header);
	cvector_header *header = data;
	header->cap = cap;
	return 0;
}
__cvector_inline__
int cvector_shrink(void **vec, int item_size) {
	int cap = cvector_get_header(*vec)->cap / 2;
	int size = sizeof(cvector_header) + cap * item_size;
	void *data = realloc(*vec - sizeof(cvector_header), size);
	if (data == NULL)
		return 1;
	*vec = data + sizeof(cvector_header);
	cvector_header *header = data;
	header->cap = cap;
	return 0;
}

__cvector_inline__
int cvector_len(void *vec) {
	return cvector_get_header(vec)->len;
}

__cvector_inline__
int cvector_cap(void *vec) {
	return cvector_get_header(vec)->cap;
}

#define FOREACH(itemtype, varname, vec, block)							\
	for (int i = 0; i < cvector_get_header(vec)->len; i++) {			\
		itemtype *varname = &vec[i];									\
		block;															\
	}

#endif /* _CVECTOR_FATPOINTER_H_ */

#define CVECTOR_WITH_NAME(type, name)									\
	TYPEDEF(type*, name);												\
	__cvector_inline__													\
	name name##_with_capacity(int cap) {								\
		int size = sizeof(cvector_header) + cap * sizeof(type);			\
		cvector_header *header = calloc(size, 1);						\
		if (header == NULL)												\
			return NULL;												\
		header->cap = cap;												\
		header->len = 0;												\
		return (name)(header + 1);										\
	}																	\
	__cvector_inline__													\
	name name##_with_length(int len) {									\
		name vec = name##_with_capacity(len);							\
		cvector_get_header(vec)->len = len;								\
		return vec;														\
	}																	\
	__cvector_inline__													\
	name name##_with_fill(int len, type val) {							\
		name vec = name##_with_length(len);								\
		FOREACH(type, item, vec, {										\
			*item = val;												\
		});																\
		return vec;														\
	}																	\
	__cvector_inline__													\
	name name##_new() {													\
		return name##_with_capacity(0);									\
	}																	\
	__cvector_inline__													\
	int name##_cap(name vec) {											\
		return cvector_cap(vec);										\
	}																	\
	__cvector_inline__													\
	int name##_len(name vec) {											\
		return cvector_len(vec);										\
	}																	\
	__cvector_inline__													\
	int name##_expand_if_needed(name *vec) {							\
		return name##_len(*vec) >= name##_cap(*vec) ?					\
				cvector_expand((void **)vec, sizeof(type)) : 0;			\
	}																	\
	__cvector_inline__													\
	int name##_shrink_if_needed(name *vec) {							\
		return name##_len(*vec) * 3 <= name##_cap(*vec) ?				\
				cvector_shrink((void **)vec, sizeof(type)) : 0;			\
	}																	\
	__cvector_inline__													\
	type *name##_at(name vec, int idx) {								\
		return idx >= 0 && idx < name##_len(vec) ? &vec[idx] : NULL;	\
	}																	\
	__cvector_inline__													\
	CVECTOR_STATUS name##_set(name vec, int idx, type val) {			\
		if (idx < 0 || idx >= name##_len(vec))							\
			CVECTOR_RETURN(1);											\
		vec[idx] = val;													\
		CVECTOR_RETURN(0);												\
	}																	\
	__cvector_inline__													\
	CVECTOR_STATUS name##_push(name *vec, type val) {					\
		if (name##_expand_if_needed(vec))								\
			CVECTOR_RETURN(1);											\
		(*vec)[(cvector_get_header(*vec)->len)++] = val;				\
		CVECTOR_RETURN(0);												\
	}																	\
	__cvector_inline__													\
	type name##_pop(name *vec) {										\
		type val;														\
		memset(&val, 0, sizeof(type));									\
		if ((int)name##_len(*vec) == 0)									\
			return val;													\
		val = (*vec)[--(cvector_get_header(*vec)->len)];				\
		name##_shrink_if_needed(vec);									\
		return val;														\
	}																	\
	__cvector_inline__													\
	void name##_cleanup(name vec) {										\
		free((void *)vec - sizeof(cvector_header));						\
	}



#else

#ifndef _CVECTOR_H_
#define _CVECTOR_H_

#ifndef CVECTOR_ALIGNMENT
#define CVECTOR_ALIGNMENT (int)sizeof(size_t)
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
int       cvector_expand (cvector_t *);
int       cvector_shrink (cvector_t *);
cvector_t cvector_clone  (cvector_t *);
void      cvector_cleanup(cvector_t *);

__cvector_inline__
int cvector_expand_if_needed(cvector_t *vec) {
	if (vec->len >= vec->cap)
		return cvector_expand(vec);
	return 0;
}
__cvector_inline__
int cvector_shrink_if_needed(cvector_t *vec) {
	if (vec->len * 3 <= vec->cap && vec->cap > CVECTOR_ALIGNMENT)
		return cvector_shrink(vec);
	return 0;
}
__cvector_inline__
void *cvector_get(cvector_t *vec, int idx) {
	return vec->data + idx * vec->item_size;
}
__cvector_inline__
void *cvector_at(cvector_t *vec, int idx) {
	if (idx < 0)
		idx = vec->len + idx;
	return idx < 0 || idx >= vec->len ? cvector_get(vec, idx) : NULL;
}

#endif /* _CVECTOR_H_ */

#define CVECTOR_DECLARE_WITH_NAME(type, name)						\
	typedef cvector_t name;											\
	__cvector_inline__												\
	CVECTOR_STATUS name##_init(name *vec) {							\
		CVECTOR_RETURN(cvector_init(vec, sizeof(type), 0));			\
	}																\
	__cvector_inline__												\
	CVECTOR_STATUS name##_init_with_capacity(name *vec, int cap) {	\
		CVECTOR_RETURN(cvector_init(vec, sizeof(type), cap));		\
	}																\
	__cvector_inline__												\
	CVECTOR_STATUS name##_init_with_length(name *vec, int len) {	\
		if (name##_init_with_capacity(vec, len))					\
			CVECTOR_RETURN(1);										\
		vec->len = len;												\
		CVECTOR_RETURN(0);											\
	}																\
	CVECTOR_STATUS name##_init_with_fill(name *, int, type);		\
	__cvector_inline__												\
	type *name##_at(name *vec, int idx) {							\
		return (type *)cvector_at(vec, idx);						\
	}																\
	__cvector_inline__												\
	type *name##_get(name *vec, int idx) {							\
		return (type *)cvector_get(vec, idx);						\
	}																\
	__cvector_inline__												\
	name name##_clone(name *vec) {									\
		return cvector_clone(vec);									\
	}																\
	__cvector_inline__												\
	void name##_set(name *vec, int idx, type val) {					\
		memcpy(vec->data + idx * vec->item_size, &val,				\
				sizeof(type));										\
	}																\
	CVECTOR_STATUS name##_set_at(name *, int, type);				\
	CVECTOR_STATUS name##_push(name *, type);						\
	type name##_pop(name *);										\
	name name##_deep_clone(name *, type (*cloner)(type *));			\
	void name##_print(name *);										\
	__cvector_inline__												\
	void name##_cleanup(name *vec) { cvector_cleanup(vec); }

#define CVECTOR_WITH_NAME(type, name)								\
	CVECTOR_STATUS name##_init_with_fill(name *vec, int len,		\
			type val) {												\
		if (name##_init_with_length(vec, len))						\
			return 1;												\
		for (int i = 0; i < len; i++) {								\
			name##_set(vec, i, val);								\
		}															\
		CVECTOR_RETURN(0);											\
	}																\
	CVECTOR_STATUS name##_set_at(name *vec, int idx, type val) {	\
		if (idx < 0 || idx >= vec->len)								\
			CVECTOR_RETURN(1);										\
		name##_set(vec, idx, val);									\
		CVECTOR_RETURN(0);											\
	}																\
	CVECTOR_STATUS name##_push(name *vec, type item) {				\
		if (cvector_expand_if_needed(vec) != 0)						\
			CVECTOR_RETURN(1);										\
		memcpy(vec->data + ((vec->len)++) * vec->item_size,			\
				&item, sizeof(type));								\
		CVECTOR_RETURN(0);											\
	}																\
	type name##_pop(name *vec) {									\
		type val;													\
		memset(&val, 0, sizeof(type));								\
		if (vec->len <= 0)											\
		return val;													\
		type *ptr = (type *)(vec->data +							\
				(--(vec->len)) * sizeof(type));						\
		memcpy(&val, ptr, sizeof(type));							\
		cvector_shrink_if_needed(vec);								\
		return val;													\
	}																\
	name name##_deep_clone(name *vec, type (*cloner)(type *)) {		\
		name new_vec;												\
		name##_init_with_capacity(&new_vec, vec->cap);				\
		for (int i = 0; i < vec->len; i++) {						\
			name##_push(&new_vec, cloner(name##_get(vec, i)));		\
		}															\
		return new_vec;												\
	}																\
	void name##_print(name *vec) {									\
		printf("[" #name "][cvector_t(" #type ")] {\n"				\
			   "  .len: %d\n"										\
			   "  .cap: %d\n"										\
			   "  .data: %p\n"										\
			   "  .items: [",										\
			   vec->len, vec->cap, vec->data);						\
		if (vec->print_item == NULL) {								\
			printf(" -- No item print function "					\
					"available --");								\
		} else {													\
			for (int i = 0; i < vec->len; i++) {					\
				type *item = name##_get(vec, i);					\
				if (i == 0) {										\
					vec->print_item(item);							\
				} else {											\
					printf(", ");									\
					vec->print_item(item);							\
				}													\
			}														\
			printf("]\n}\n");										\
		}															\
	}

#define CVECTOR_DECLARE(type)						\
	CVECTOR_DECLARE_WITH_NAME(type, Vector_##name)


#endif /* CVECTOR_FATPOINTER */
#define CVECTOR(type)								\
	CVECTOR_WITH_NAME(type, Vector_##type);
