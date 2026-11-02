/*
 *          Copyright 2026, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "aca_batch.h"

// cross module

typedef struct {
	const char *name;
	const char *vendor;
	char *data;
	uint32_t major, minor, patch;
} aca_module_t;

typedef struct {
	aca_module_t **modules;
	char *err_str;
	uint32_t modules_len, index, err1, err2;
	int32_t pass;
} aca_batch_run_t;

// private

typedef void (*batch_task_func_t)(aca_batch_run_t *batch_run);

// public

void aca_batch_alloc(void ***const data, const uint32_t data_len) {
	const size_t size = (size_t)data_len*2*sizeof(void*);
	void *const data_new = malloc(size);
	if (data_new) {
		memset(data_new, 0, size);
		*data = (void**)data_new;
	}
}

void aca_batch_run(void **const data, const uint32_t data_len, const int32_t passes, uint32_t *const err1, uint32_t *const err2, uint32_t *const err_idx, char **const err_str) {
	aca_batch_run_t batch_run = {(aca_module_t**)&data[data_len], NULL, data_len, 0, 0, 0, 0};
	// main
	while (batch_run.pass < passes) {
		// forward
		for (batch_run.index = 0; batch_run.index < data_len && !batch_run.err1; batch_run.index++) {
			const batch_task_func_t batch_task = (batch_task_func_t)data[batch_run.index];
			if (batch_task) {
				*err_idx = batch_run.index;
				batch_task(&batch_run);
			}
		}
		// backwards
		if (!batch_run.err1 && ++batch_run.pass < passes) {
			batch_run.index = data_len - 1;
			while (!batch_run.err1) {
				const batch_task_func_t batch_task = (batch_task_func_t)data[batch_run.index];
				if (batch_task) {
					*err_idx = batch_run.index;
					batch_task(&batch_run);
				}
				if (batch_run.index > 0)
					batch_run.index--;
				else
					break;
			}
		}
		if (!batch_run.err1)
			batch_run.pass++;
		else
			break;
	}
	// error handling
	if (batch_run.err1) {
		batch_run.pass = -(batch_run.pass + 1);
		if (batch_run.pass == -1)
			batch_run.index = *err_idx;
		else
			batch_run.index = data_len - 1;
		while (true) {
			const batch_task_func_t batch_task = (batch_task_func_t)data[batch_run.index];
			if (batch_task) {
				batch_task(&batch_run);
			}
			if (batch_run.index > 0)
				batch_run.index--;
			else
				break;
		}
		*err1 = batch_run.err1;
		*err2 = batch_run.err2;
		*err_str = batch_run.err_str;
	}
}

void aca_batch_free(void **const data) {
	free((void*)data);
}
