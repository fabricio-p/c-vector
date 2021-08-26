#include "cvector.h"

#define ALIGN(s) (((s) + (ALIGNMENT) - 1) & ~((ALIGNMENT) - 1))

int cvector_init(cvector_t *vec, int item_size, int cap) {
	vec->len = 0;
	vec->cap = cap;
	vec->item_size = item_size;
	vec->size = ALIGN(item_size * cap);
	if (cap != 0) {
		if ((vec->data = malloc(vec->size)) == NULL)
			return 1;
		memset(vec->data, 0, vec->size);
	} else {
		vec->data = NULL;
	}
	return 0;
}

void *cvector_get(cvector_t *vec, int idx) {
	if (idx >= vec->len)
		return NULL;
	return vec->data + idx * vec->item_size;
}

int cvector_expand(cvector_t *vec) {
	int cap = vec->cap ? (vec->cap + (vec->cap & 0xff)) :
			      ALIGNMENT;
	int size = ALIGN(cap * vec->item_size);
	void *data = realloc(vec->data, size);
	if (data == NULL)
		return 1;
	vec->cap = cap;
	vec->size = size;
	vec->data = data;
	return 0;
}

int cvector_shrink(cvector_t *vec) {
	int cap = vec->cap >> 1;
	int size = ALIGN(cap * vec->item_size);
	void *data = realloc(vec->data, size);
	if (data == NULL)
		return 1;
	vec->cap = cap;
	vec->size = size;
	vec->data = data;
	return 0;
}

void cvector_cleanup(cvector_t *vec) {
	if (vec != NULL) {
		if (vec->data != NULL)
			free(vec->data);
		vec->len = vec->cap = vec->item_size = 0;
		vec->data = NULL;
		vec->print_item = NULL;
	}
}
