#include "value.h"

#include "object.h"

struct value value_none() {
	struct value result;
	result.tag = VALUE_NONE;
	return result;
}

struct value value_int(int64_t value) {
	struct value result;
	result.tag = VALUE_INT;
	result.as_int = value;
	return result;
}

struct value value_num(double value) {
	struct value result;
	result.tag = VALUE_NUM;
	result.as_num = value;
	return result;
}

struct value value_object() {
	struct value result;
	result.tag = VALUE_OBJECT;
	result.as_object = object();
	return result;
}
