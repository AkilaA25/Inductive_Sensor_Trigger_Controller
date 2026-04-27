/*
 * filter.c
 *
 *  Created on: Dec 16, 2025
 *      Author: akila
 */

#include "filter.h"
#include <string.h>

//filter setup
void Filter_Init(MovingAverageFilter *filter, uint8_t size)
{
    if (filter == NULL) return;

    //keep the size valid
    if (size == 0) size = 1;
    if (size > FILTER_SIZE_MAX) size = FILTER_SIZE_MAX;

    filter->size = size;
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0;
    filter->initialized = true;

    // clear
    memset(filter->buffer, 0, sizeof(filter->buffer));
}



//filter
int16_t Filter_Update(MovingAverageFilter *filter, int16_t new_value)
{
    if (filter == NULL || !filter->initialized) {
        return new_value;  // wait till initializign
    }

    // rotate values through
    if (filter->count >= filter->size) {
        filter->sum -= filter->buffer[filter->index];
    } else {
        filter->count++;
    }

    // add new values
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;
    filter->index++;
    if (filter->index >= filter->size) {
        filter->index = 0;
    }

    // average
    return (int16_t)(filter->sum / filter->count);
}

