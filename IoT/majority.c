#include "majority.h"
#include <stdio.h>

/* Returns the mean of a vector */
float mean(float* vec, int size) {
    float sum = 0.0;
    for(int i=1; i<size; i++) {
        sum += vec[i];
    }
    return sum / size;
}

/* Performs a simple clustering of the input vector */
float mean_weight(float* sensors, int size, float start, float threshold) {
    float mean = start;
    float old_mean = mean + threshold * 10;
    
    float sum = 0.0;
    float weight = 0.0;
    while ((mean - old_mean)*(mean - old_mean) > threshold) {
        old_mean = mean;
        mean = 0.0;
        sum = 0.0;
        for(int i=0; i<size; i++) {
            weight = 1 / ((old_mean - sensors[i])*(old_mean - sensors[i])+1);     // To reduce the influence of further points, each one has a weight of 1/(1+x^2)
            sum = sum + weight;                                                   // with x the current distance from the cluster position 
            mean = mean + sensors[i] * weight;
        }
        mean = mean / sum;
    }
    return mean;
}

/* Performs a nested clustering by repetedly calling the mean_weight function */
float voting(float* sensors, int size, float low, float high, float threshold) {

    float means[LEN*2], sensor_minus[size]; // The input sensor is multiplied by -1 to improve symmetry, as this algorithm has a tendency to select lower values
    
    for(int i=0; i<size; i++) {                 
        sensor_minus[i] = sensors[i] * -1.0;
    }
    
    for(int i=0; i<LEN; i++) {
        means[i] = mean_weight(sensor_minus, size, (low - high)/(LEN-1)*i - low, threshold) * -1.0; // LEN clusters with equally distributed starting points are found in the inverted input
    }
    
    for(int i=0; i<LEN; i++) {
        means[i + LEN] = mean_weight(sensors, size, (high - low)/(LEN-1)*i + low, threshold);       // And in the normal one
    }
    float mean_mean = mean(means,LEN*2);
    return mean_weight(means, LEN*2, mean_mean, threshold);     // Finally a clustering of clusters is computed from their normal mean
}

/* Selects the most suitable consensus strategy */ 
float merge (float* sensors, int size, float low, float high, float threshold){

    if (size == 1){return sensors[0];}
    else if (size == 2){return .5*(sensors[0] + sensors[1]);}
    else {return voting(sensors, size, low, high, threshold);}

}
