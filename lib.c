#include "lib.h"

#define ALIGN(s) (((s) + (CVECTOR_ALIGNMENT) - 1) & \
					~((CVECTOR_ALIGNMENT) - 1))

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

int cvector_expand(cvector_t *vec) {
	int cap = vec->cap ? (vec->cap + (vec->cap & 0xff)) :
			      CVECTOR_ALIGNMENT;
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

cvector_t cvector_clone(cvector_t *vec) {
	cvector_t new_vec;
	cvector_init(&new_vec, vec->item_size, vec->cap);
	new_vec.print_item = vec->print_item;
	new_vec.len = vec->len;
	memcpy(new_vec.data, vec->data, new_vec.size);
	return new_vec;
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
