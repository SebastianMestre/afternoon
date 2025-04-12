#pragma once

#include <stdint.h>

struct object;

enum value_tag {
	VALUE_NONE,
	VALUE_INT,
	VALUE_NUM,
	VALUE_OBJECT,
};

struct value {
	enum value_tag tag;
	union {
		int64_t as_int;
		double as_num;
		struct object *as_object;
	};
};

struct value value_none();

struct value value_int(int64_t value);

struct value value_num(double value);

struct value value_object();
