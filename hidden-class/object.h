#pragma once

struct object {
	struct shape* shape;

	int count;
	int capacity;
	struct value* items;
};

struct object *object();

struct inline_cache;
struct value object_get(struct object* this, char* key, struct inline_cache* cache);
void object_set(struct object *this, char *key, struct value value, struct inline_cache* cache);
