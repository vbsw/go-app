/*
 *          Copyright 2026, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef ACA_BATCH_H
#define ACA_BATCH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void aca_batch_alloc(void ***data, uint32_t data_len);
void aca_batch_run(void **data, uint32_t data_len, int32_t passes, uint32_t *err1, uint32_t *err2, uint32_t *err_idx, char **err_str);
void aca_batch_free(void **data);

#ifdef __cplusplus
}
#endif

#endif /* ACA_BATCH_H */
