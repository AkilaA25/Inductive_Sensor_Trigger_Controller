/*
 * filter.h
 *
 *  Created on: Dec 16, 2025
 *      Author: akila
 */

#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_

#include <stdint.h>
#include <stdbool.h>

/* Filter Configuration */
#define FILTER_SIZE_DEFAULT     8       // Default filter size
#define FILTER_SIZE_MAX         32      // max filter

/* Filter structure */
typedef struct {
    int16_t buffer[FILTER_SIZE_MAX];    // Circular buffer for samples
    uint8_t index;                       // Current write index
    uint8_t size;                        // Active filter size
    uint8_t count;                       // Number of valid samples
    int32_t sum;                         // Running sum for efficiency
    bool initialized;                    // Filter state
} MovingAverageFilter;

/* Filter Functions */
void Filter_Init(MovingAverageFilter *filter, uint8_t size);
int16_t Filter_Update(MovingAverageFilter *filter, int16_t new_value);

#endif /* CODE_FILTER_H_ */
