#include "object.h"

#include "da.h"
#include "shape.h"
#include "inline_cache.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>

struct object *object() {
	struct object *result = malloc(sizeof(*result));
	result->shape = &empty_shape;
	result->count = 0;
	result->capacity = 0;
	result->items = NULL;
	return result;
}

static int get_index(struct object* this, char* key, struct inline_cache* cache) {
	if (cache->shape != this->shape) {

		struct shape *it = this->shape;
		for (; it != NULL; it = it->parent) {
			if (it->last_key && strcmp(key, it->last_key) == 0) {
				break;
			}
		}

		cache->shape = this->shape;
		if (it == NULL) {
			cache->index = -1;
		} else {
			cache->index = it->key_count - 1;
		}
	}

	return cache->index;
}

struct value object_get(struct object* this, char* key, struct inline_cache* cache) {

	int index = get_index(this, key, cache);

	if (index == -1) {
		return value_none();
	} else {
		return this->items[index];
	}
}

void object_set(struct object *this, char *key, struct value value, struct inline_cache* cache) {
	int index = get_index(this, key, cache);

	if (index == -1) {
		struct shape *new_shape = extend_shape(this->shape, key);
		this->shape = new_shape;
		da_append(this, value);
	} else {
		this->items[index] = value;
	}

}
