#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_fit.h>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include<iomanip>
#include <algorithm> 
#include <chrono> 
#include<vector> 
#include <queue>
using namespace std; 
using namespace std::chrono; 
double *swip(double X[], double m){
  for (int i = 0; i < 5; i++) {
      if(i==4){
       X[i]=m;}
      else{
      X[i]=X[i+1];}

      }
return X;
}

int main(void){
  struct send_data {
	/*! Identifier */
	int fac;
	/*! Time of data */
	long long int timeStamp; 
	/*! Compensated pressure */
	double pressure;
	/*! Compensated temperature */
	double temperature;
	/*! Compensated humidity */
	double humidity;
	/*! Alarm ON/OFF */
	int alarmON : 1;
	/*! Fan ON/OFF */
	int fanON : 1;
    }; 
  struct send_data s;
  double time1[5]={0};
  double time2[5]={0};
  double temp1[5]={0};
  double temp2[5]={0};
  double p1[5]={0};
  double p2[5]={0};
  double h1[5]={0};
  double h2[5]={0};
  double Temp1_p;
  double pressure1_p;
  double Temp2_p;
  double pressure2_p;
  double h1_p;
  double h2_p;
  double c0, c1, cov00, cov01, cov11, sumsq;
  int n=5;
  s.fac=1;
  s.timeStamp=5;
  s.temperature=6;
  s.pressure=7;
  s.humidity=8;
  if(s.fac==1){
  swip(time1,s.timeStamp);
  
  swip(temp1,s.temperature);
  gsl_fit_linear(time1,1,temp1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  Temp1_p=c0+c1*(time1[4]+1);
  printf("prediction temp for factiry 1  %f \n",Temp1_p);
  swip(p1,s.pressure);
 gsl_fit_linear(time1,1,p1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  pressure1_p=c0+c1*(time1[4]+1);
  printf("prediction pressure for factiry 1  %f \n",pressure1_p);
  swip(h1,s.humidity);
gsl_fit_linear(time1,1,h1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  h1_p=c0+c1*(time1[4]+1);
  printf("prediction humidity for factiry 1  %f \n",h1_p);

   }
  else if(s.fac==2){
  swip(time2,s.timeStamp);
  
  swip(temp2,s.temperature);
 gsl_fit_linear(time2,1,temp2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  Temp2_p=c0+c1*(time2[4]+1);
  printf("prediction temp for factiry 2  %f \n",Temp2_p);

  swip(p2,s.pressure);
 gsl_fit_linear(time2,1,p2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  
  pressure2_p=c0+c1*(time2[4]+1);
  printf("prediction pressure for factiry 2  %f \n",pressure2_p);
  swip(h2,s.humidity);
gsl_fit_linear(time2,1,h2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  h2_p=c0+c1*(time2[4]+1);
  printf("prediction humidity for factiry 2  %f \n",h2_p);

   }
 
  
  return 0;


}