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


  double time1[5]={0};
  double time2[5]={0};
  double temp1[5]={0};
  double temp2[5]={0};
  double p1[5]={0};
  double p2[5]={0};
  double h1[5]={0};
  double h2[5]={0};
  int n=5;
  
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
    
  
double *swip(double X[], double m){
  for (int i = 0; i < 5; i++) {
      if(i==4){
       X[i]=m;}
      else{
      X[i]=X[i+1];}

      }
return X;
}
struct send_data f(struct send_data a){
  double Temp1_p;
  double pressure1_p;
  double Temp2_p;
  double pressure2_p;
  double h1_p;
  double h2_p;
  double c0, c1, cov00, cov01, cov11, sumsq;
  struct send_data pred;
  pred.fac=a.fac; 
  pred.timeStamp=0;
   
  if(a.fac==1){
  swip(time1,a.timeStamp);
  
  swip(temp1,a.temperature);
  gsl_fit_linear(time1,1,temp1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  Temp1_p=c0+c1*(time1[4]+1);
  pred.temperature=Temp1_p; 
  
  swip(p1,a.pressure);
  gsl_fit_linear(time1,1,p1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  pressure1_p=c0+c1*(time1[4]+1);
  pred.pressure=pressure1_p; 
  
  swip(h1,a.humidity);
  gsl_fit_linear(time1,1,h1,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  h1_p=c0+c1*(time1[4]+1);
  pred.humidity = h1_p; 
  }
  
  else if(a.fac==2){
  swip(time2,a.timeStamp);
  
  swip(temp2,a.temperature);
  gsl_fit_linear(time2,1,temp2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  Temp2_p=c0+c1*(time2[4]+1);
  pred.temperature=Temp2_p;
	 
  swip(p2,a.pressure);
  gsl_fit_linear(time2,1,p2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  pressure2_p=c0+c1*(time2[4]+1);
  pred.pressure=pressure2_p; 
  
  swip(h2,a.humidity);
  gsl_fit_linear(time2,1,h2,1,n,&c0,&c1,&cov00,&cov01,&cov11,&sumsq);
  h2_p=c0+c1*(time2[4]+1);
  pred.humidity=h2_p; 

   }
   
   return pred; 
}
 

int main(void){
  
  struct send_data s;
  s.fac=1;
  s.timeStamp=5;
  s.temperature=6;
  s.pressure=7;
  s.humidity=8;
  
  struct send_data pred = f(s); 
  
  printf("prediction temperature for factory %i is %f degrees \n",pred.fac, pred.temperature);
  printf("prediction pressure for factory %i is %f bars \n",pred.fac, pred.pressure);
  printf("prediction humidity for factory %i is %f g/m^3 \n",pred.fac, pred.humidity);
  
  return 0;


}
