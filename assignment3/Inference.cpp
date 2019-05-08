#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <stdlib.h>

using namespace std;

//initialize the probabilities
const float p0 = 0.2;//R0=true
const float p1 = 0.7;//Rt=true|Rt-1=true
const float p2 = 0.3;//Rt=true|Rt-1=false
const float p3 = 0.9;//Ut=true|Rt=true
const float p4 = 0.2;//Ut=true|Rt=false
const int ite = 20000;//iteration number

static vector<int> inputData;//the observed data
static vector<int> sample;//every day's weather of the random created new sample
static float pro = 0;//the probability of the sample
static float pt = 0;//the probability of RT=true
static float pf = 0;//the probability of RT=false

//initialize the probabilities for gibbs R0 and t>1
static float pg0 = p0*p1*p3/(p0*p1*p3+(1-p0)*p2*p4);//R0=true,R1=true,U0=true
static float pg1 = p0*p1*(1-p3)/(p0*p1*(1-p3)+(1-p0)*p2*(1-p4));//R0=true,R1=true,U0=false
static float pg2 = p0*(1-p1)*p3/(p0*(1-p1)*p3+(1-p0)*(1-p2)*p4);//R0=true,R1=false,U0=true
static float pg3 = p0*(1-p1)*(1-p3)/(p0*(1-p1)*(1-p3)+(1-p0)*(1-p2)*(1-p4));//R0=true,R1=false,U0=false

//initialize the probabilities for gibbs R0 and t=1
static float pg4 = p0*p3/(p0*p3+(1-p0)*p4);//R0=true,U0=true
static float pg5 = p0*(1-p3)/(p0*(1-p3)+(1-p0)*(1-p4));//R0=true,U0=true

//initialize the probabilities for gibbs Rt
static float pg6 = p1*p3/(p1*p3+(1-p1)*p4);//Rt=true,Rt-1=true,Ut=true
static float pg7 = p1*(1-p3)/(p1*(1-p3)+(1-p1)*(1-p4));//Rt=true,Rt-1=true,Ut=false
static float pg8 = p2*p3/(p2*p3+(1-p2)*p4);//Rt=true,Rt-1=false,Ut=true
static float pg9 = p2*(1-p3)/(p2*(1-p3)+(1-p2)*(1-p4));//Rt=true,Rt-1=false,Ut=flase

//initialize the probabilities for gibbs Rx(1<x<t)
static float pg10 = p1*p1*p3/(p1*p1*p3+(1-p1)*p2*p4);//Rx=true,Rx-1=true,Rx+1=true,Ux=true
static float pg11 = p1*(1-p1)*p3/(p1*(1-p1)*p3+(1-p1)*(1-p2)*p4);//Rx=true,Rx-1=true,Rx+1=false,Ux=true
static float pg12 = p2*p1*p3/(p2*p1*p3+(1-p2)*p2*p4);//Rx=true,Rx-1=false,Rx+1=true,Ux=true
static float pg13 = p2*(1-p1)*p3/(p2*(1-p1)*p3+(1-p2)*(1-p2)*p4);//Rx=true,Rx-1=false,Rx+1=false,Ux=true
static float pg14 = p1*p1*(1-p3)/(p1*p1*(1-p3)+(1-p1)*p2*(1-p4));//Rx=true,Rx-1=true,Rx+1=true,Ux=false
static float pg15 = p1*(1-p1)*(1-p3)/(p1*(1-p1)*(1-p3)+(1-p1)*(1-p2)*(1-p4));//Rx=true,Rx-1=true,Rx+1=false,Ux=false
static float pg16 = p2*p1*(1-p3)/(p2*p1*(1-p3)+(1-p2)*p2*(1-p4));//Rx=true,Rx-1=false,Rx+1=true,Ux=false
static float pg17 = p2*(1-p1)*(1-p3)/(p2*(1-p1)*(1-p3)+(1-p2)*(1-p2)*(1-p4));//Rx=true,Rx-1=false,Rx+1=false,Ux=false

//create a ramdom sample
void Sample() {
  sample.clear();
  pro = 1;
  float pro = 1;//the probability of meeting such weather
  float r = (((float)rand())/(float)RAND_MAX);
  if(r < p0) {//for R0
    sample.push_back(1);
    pro *= p0;
  }else {
    sample.push_back(0);
    pro *= 1-p0;
  }
  for(int i=1;i<inputData.size();i++) {//for the rest day
    if(sample[i-1] == 1) {
      r = (((float)rand())/(float)RAND_MAX);
      if(r < p1) {
        sample.push_back(1);
        pro *= p1;
      }else {
        sample.push_back(0);
        pro *= 1-p1;
      }
    }else {
      r = 1-(((float)rand())/(float)RAND_MAX);
      if(r < p2) {
        sample.push_back(1);
        pro *= p2;
      }else {
        sample.push_back(0);
        pro *= 1-p2;
      }
    }
  }
}	

//likelihood weighted sampling method
void LWS() {
  Sample();
  for(int i=0;i<sample.size();i++) {//calculate the weight
    if(sample[i] == 1) {
      if(inputData[i] == 1) {
        pro *= p3;
      }else {
        pro *= 1-p3;
      }
    }else {
      if(inputData[i] == 1) {
        pro *= p4;
      }else {
        pro *= 1-p4;
      }
    }
  }
  if(sample.back() == 1) {
    pt += pro;
  }else {
    pf += pro;
  }
}

//Gibbs sampling method
void Gibbs() {
  float d0;//the probability of day0 is rain
  float dt;//the probability of day t is rain
  float dx;//the probability of day x(1<x<t) is rain
  float r;//random float
  
  //decide the probability of day0
  if(sample.size()==1) {//for t=1
    r = (((float)rand())/(float)RAND_MAX);
    if(inputData[0]==1){
      if(r < pg4) {
        sample[0] = 1;
      }else {
        sample[0] = 0;
      }
    }else {
      if(r < pg5) {
        sample[0] = 1;
      }else {
        sample[0] = 0;
      }
    }
    return;
  }else {//for t>1
    if(sample[1] == 1) {
      if(inputData[0] == 1) {
        d0 = pg0;
      }else {
        d0 = pg1;
      }
    }else {
      if(inputData[0] == 1) {
        d0 = pg2;
      }else {
        d0 = pg3;
      }
    }
    r = (((float)rand())/(float)RAND_MAX);
    if(r < d0) {//change R0
      sample[0] = 1;
    }else {
      sample[0] = 0;
    }
  }
  
  //decide the probability of Rx(1<x<t)
  if(sample.size()>2) {
    for(int i=1;i<sample.size()-1;i++) {
      if(inputData[i] == 1) {
        if(sample[i-1] == 1) {
          if(sample[i+1] == 1) {
            dx = pg10;
          }else {
            dx = pg11;
          }
        }else {
          if(inputData[0] == 1) {
            dx = pg12;
          }else {
            dx = pg13;
          }
        }
      }else {
        if(sample[i-1] == 1) {
          if(sample[i+1] == 1) {
            dx = pg14;
          }else {
            dx = pg15;
          }
        }else {
          if(inputData[0] == 1) {
            dx = pg16;
          }else {
            dx = pg17;
          }
        }
      }
      r = (((float)rand())/(float)RAND_MAX);
      if(r < dx) {//change Rx
        sample[i] = 1;
      }else {
        sample[i] = 0;
      }
    }
  }
  
  //decide the probability of getting Rt
  if(sample[sample.size()-2] == 1) {
    if(inputData.back() == 1) {
      dt = pg6;
    }else {
      dt = pg7;
    }
  }else {
    if(inputData.back() == 1) {
      dt = pg8;
    }else {
      dt = pg9;
    }
  }
  
  r = (((float)rand())/(float)RAND_MAX);
  if(r < dt) {//change Rt
    sample[sample.size()-1] = 1;
  }else {
    sample[sample.size()-1] = 0;
  }
}

int main(int argc, char* argv[]) {
  
  //read input data
  ifstream inputFile(argv[1]);
  if (inputFile.good()) {
    int num = 0;
    while (inputFile >> num) {
      inputData.push_back(num);
    }
    inputFile.close();
  }else {
    cout << "Error! Can't read this file.";
    return 0;
  }
  
  //likelihood weighted sampling method
  for(int i=0;i<ite;i++) {
    LWS();
  }
  cout << fixed << setprecision(3) << pt/(pt+pf) << " " << fixed << setprecision(3) << pf/(pt+pf) << " Likelihood" << endl;
  pt = 0;
  pf = 0;
  
  /*
   for(int i=100;i<20000;i++) {
   for(int f=0;f<i;f++) {
   LWS();
   }
   cout << fixed << setprecision(3) << pt/(pt+pf) << "," << fixed << setprecision(3) << pf/(pt+pf)<< endl;
   pt = 0;
   pf = 0;
   }
  */
  
  Sample();
  for(int i=0;i<ite;i++) {
    Gibbs();
    if(i>ite/2 && i%5==0) {
      if(sample.back()==1) {
        pt++;
      }else {
        pf++;
      }
    }
  }
  cout << fixed << setprecision(3) << pt/(pt+pf) << " " << fixed << setprecision(3) << pf/(pt+pf) << " Gibbs" << endl;
  pt = 0;
  pf = 0;
  
  /*
  for(int i=100;i<20000;i++) {
    for(int f=0;f<i;f++) {
      Gibbs();
      if(f>i/2 && f%5==0) {
        if(sample.back()==1) {
          pt++;
        }else {
          pf++;
        }
      }
    }
    cout << fixed << setprecision(3) << pt/(pt+pf) << "," << fixed << setprecision(3) << pf/(pt+pf)<< endl;
    pt = 0;
    pf = 0;
  }
  */
  
  return 0;
}