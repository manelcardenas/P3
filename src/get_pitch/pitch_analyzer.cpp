/// @file


#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
     //FILE *autocorr = fopen("autocorr.txt","a");
    for (unsigned int l = 0; l < r.size(); ++l) {
  		// \TODO Compute the autocorrelation r[l]
      r[l] = 0;
        for(unsigned int n = 0; n < x.size() - l; n++){
          r[l]= x[n] * x[n+l] + r[l];
        }
        r[l]=(1.0F/x.size())*r[l];
        //fprintf(autocorr,"%f\n",r[l]);
       
      /// \DONE 
    }
     //fclose(autocorr); 
    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
     case HAMMING:{
      /// \TODO Implement the Hamming window
      // w(n)=0.54 - 0.46*cos2pin/N
      float k0 = 0.54F;
      float k1 = 0.46F;
        for(unsigned int i=0;i<frameLen;i++){
          window[i]=k0-k1*cos((2*M_PI*i)/(frameLen-1));
        }
      /// \DONE
    } 
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm,float tasa) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    bool unvoiced = true;
    float potnorm=pot/potmaxima;
    //cout << potnorm << '\t' << pot << '\t' << potmaxima << endl;
    if(r1norm > umb1 || rmaxnorm > umb2 /*|| tasa < umb3*/){        //0.95 0.48   91,26%
      unvoiced = false;
    }
    if(tasa > umb3){   // 4200 91,26%
      unvoiced = true;
    }
    if(potnorm<2.5 && tasa<2450){      //2.5  2500   91,53
      unvoiced = false;
    }
    return unvoiced;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.

    while (*iR > 0 && iR < r.end()){
      iR++;
    }

    if(iR < r.begin() + npitch_min){ 
      iR = r.begin() + npitch_min;
    }

    iRMax = iR;
     while(iR!=r.end()){
      if(*iR>*iRMax){
        iRMax=iR;
      }
      ++iR;
    }

  
  /// \DONE
  
    unsigned int lag = iRMax - r.begin();

    float pot = 10 * log10(r[0]);
    //float potnorm=pot/potmax;
    float tasa = compute_zcr(x, frameLen, samplingFreq);
    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
    //FILE *potencia = fopen("pot.txt","a");
    //FILE *r1norm = fopen("r1norm.txt","a");
    //FILE *rmaxnorm = fopen("rmaxnorm.txt","a");
    //fprintf(potencia,"%f\n",pot);
    //fprintf(r1norm,"%f\n",r[1]/r[0]);
    //fprintf(rmaxnorm, "%f\n",r[lag]/r[0]);
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] <<'\t' << tasa << endl;
#endif
    //fclose(potencia);
    //fclose(r1norm);
    //fclose(rmaxnorm); 
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0],tasa))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
  //float compute_zcr(const float *x, unsigned int N, float fm) {
  float PitchAnalyzer::compute_zcr(vector<float> & x, unsigned int N, unsigned int fm) const {
    float d = 0.0;
    for(unsigned int i=1;i<N;i++){  
        if((x[i]>0 && x[i-1]>0)||(x[i]<0 && x[i-1]<0)){
            d += 0;
        }
        else d += 1;
    }
    float tasa=(fm/2)*d/(N-1);
    return tasa;
 }

 //float compute_power(const float *x, unsigned int N) {
 /* float PitchAnalyzer::compute_power(vector<float> & x, unsigned int N){
    float aux=0.0F;
    for(unsigned int i=0; i<N;i++){
        aux += x[i]*x[i];
    }
    //float aux2=aux/N;
    float PdB=10*log10(aux/N); 
    return PdB;
  }*/
}
