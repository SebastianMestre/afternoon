#pragma once

#include <assert.h>
#include <stdlib.h>

// stolen from Tsoding's nob.h
#define da_append(da, item) \
	do { \
		if ((da)->count >= (da)->capacity) { \
			(da)->capacity = (da)->capacity == 0 ? 8 : (da)->capacity * 2; \
			(da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
			assert((da)->items); \
		} \
		(da)->items[(da)->count++] = (item); \
	} while (0)
