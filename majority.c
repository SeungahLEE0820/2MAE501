#include "majority.h"
#include <stdio.h>

float mean(float* vec, int size) {
    float sum = 0.0;
    for(int i=1; i<size; i++) {
        sum += vec[i];
    }
    return sum / size;
}

float mean_weight(float* sensors, int size, float start, float threshold) {
    float mean = start;
    float old_mean = mean + threshold * 10;
    
    float sum = 0.0;
    float temp = 0.0;
    while ((mean - old_mean)*(mean - old_mean) > threshold) {
        old_mean = mean;
        mean = 0.0;
        sum = 0.0;
        for(int i=0; i<size; i++) {
            temp = 1 / ((old_mean - sensors[i])*(old_mean - sensors[i])+1);
            sum = sum + temp;
            mean = mean + sensors[i] * temp;
        }
        mean = mean / sum;
    }
    return mean;
}

float voting(float* sensors, int size, float low, float high, float threshold) {

    float means[LEN*2], sensor_minus[size];
    
    for(int i=0; i<size; i++) {
        sensor_minus[i] = sensors[i] * -1.0;
    }
    
    for(int i=0; i<LEN; i++) {
        means[i] = mean_weight(sensor_minus, size, (low - high)/(LEN-1)*i - low, threshold) * -1.0;
    }
    
    for(int i=0; i<LEN; i++) {
        means[i + LEN] = mean_weight(sensors, size, (high - low)/(LEN-1)*i + low, threshold);
    }
    float mean_mean = mean(means,LEN*2);
    return mean_weight(means, LEN*2, mean_mean, threshold);
}


float merge (float* sensors, int size, float low, float high, float threshold){

    if (size == 1){return sensors[0];}
    else if (size == 2){return .5*(sensors[0] + sensors[1]);}
    else {return voting(sensors, size, low, high, threshold);}

}
