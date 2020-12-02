#ifndef MAJORITY_H_
#define MAJORITY_H_
#define LEN 5       /* half of number of means to compute */

/* Returns the mean of a vector */
float mean(float* vec, int size);

/* Performs a simple clustering of the input vector */ 
float mean_weight(float* sensors, int size, float start, float threshold);

/* Performs a nested clustering by repetedly calling the mean_weight function */
float voting(float* sensors, int size, float low, float high, float threshold);

/* Selects the most suitable consensus strategy */ 
float merge (float* sensors, int size, float low, float high, float threshold);
#endif
