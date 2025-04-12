#include "da.h"
#include "inline_cache.h"
#include "object.h"
#include "shape.h"
#include "value.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



struct value object_get_uncached(struct object* this, char *key) {
	struct inline_cache cache = {NULL, 0};
	return object_get(this, key, &cache);
}

void object_set_uncached(struct object* this, char *key, struct value value) {
	struct inline_cache cache = {NULL, 0};
	object_set(this, key, value, &cache);
}

int main() {

	empty_shape = (struct shape) {
		.last_key = NULL,
		.key_count = 0,
		.parent = NULL,

		.count = 0,
		.capacity = 0,
		.items = NULL,
	};

	struct object *obj1 = object();
	struct object *obj2 = object();

	for (int i = 0; i < 20; ++i) {
		char c[2] = {'A' + i, 0};
		object_set_uncached(obj1, c, value_none());
		object_set_uncached(obj2, c, value_none());
	}

	clock_t t0 = clock();

	struct inline_cache obj1_A_cache = {NULL, -1};
	struct inline_cache obj2_A_cache = {NULL, -1};

	for (int i = 0; i < 10000000; ++i) {
#ifdef USE_CACHE
		object_set(obj1, "A", value_int(rand()), &obj1_A_cache);
		object_set(obj2, "A", value_int(rand()), &obj2_A_cache);
#else
		object_set_uncached(obj1, "A", value_int(rand()));
		object_set_uncached(obj2, "A", value_int(rand()));
#endif
	}

	clock_t t1 = clock();

	printf("elapsed: %.2lfms\n", (t1 - t0) * 1000ll / (double)CLOCKS_PER_SEC);

}
