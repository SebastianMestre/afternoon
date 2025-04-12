#include "shape.h"

#include "da.h"

#include <stdlib.h>
#include <string.h>

struct shape empty_shape;

struct shape *extend_shape(struct shape *shape, char* key) {
	for (int i = 0; i < shape->count; ++i) {
		if (strcmp(key, shape->items[i]->last_key) == 0) {
			return shape->items[i];
		}
	}

	struct shape *new_shape = malloc(sizeof(*new_shape));
	new_shape->last_key = strcpy(malloc(strlen(key)+1), key);
	new_shape->key_count = shape->key_count + 1;
	new_shape->parent = shape;

	new_shape->count = 0;
	new_shape->capacity = 0;
	new_shape->items = NULL;

	da_append(shape, new_shape);

	return new_shape;
}
