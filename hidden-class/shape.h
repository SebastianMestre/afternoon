#pragma once

struct shape {
	char* last_key;
	int key_count;
	struct shape* parent;

	int count;
	int capacity;
	struct shape** items;
};

extern struct shape empty_shape;

struct shape *extend_shape(struct shape *shape, char* key);
