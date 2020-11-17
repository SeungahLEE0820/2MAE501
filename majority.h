#ifndef MAJORITY_H_
#define MAJORITY_H_
#define LEN 5       /* half of number of means to compute */

float mean(float* vec, int size);

float mean_weight(float* sensors, int size, float start, float threshold);

float voting(float* sensors, int size, float low, float high, float threshold);

float merge (float* sensors, int size, float low, float high, float threshold);
#endif
