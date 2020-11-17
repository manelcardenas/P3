/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include<math.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt> 
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    --umbral1 float   [default: 0.95] r[1]/r[0]
    --umbral2 float   [default: 0.48] r[lag]/r[0]
    --umbral3 float   [default: 4200] tasa de cruces por cero

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  //stof para obtener un valor real || args para extraerlo del docopt  || entre corchetes porque es opcional
  float umbral1 = stof(args["--umbral1"].asString());
  float umbral2 = stof(args["--umbral2"].asString());
  float umbral3 = stof(args["--umbral3"].asString());
  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  //PitchAnalyzer analyzer(umbral1,umbral2,umbral3,n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
    float max=0.0F;
    for(unsigned int k = 0; k< x.size(); k++){
      if(x[k]> max) max = x[k];
    }

    float llindar=0.01*max;
    for(unsigned int k = 0; k < x.size(); k++){
      if (fabs(x[k]) < llindar) x[k]=0;
      else if (x[k]> llindar) x[k]-=llindar;
      else if (x[k] < -llindar) x[k]+=llindar;
    }

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  float pot_max = -1000.0F;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
        float aux=0.0F;
        for (int y = 0; y < n_len; y++){
            aux += iX[y]*iX[y];
            if(aux==0.0F){
              aux = 1e-10; 
            }
        }
        float PdB=10*log10(aux/n_len); 
        if(PdB>pot_max){
          pot_max=PdB;
        }
         //cout << PdB << '\t' << pot_max << endl;
     }
  //Extra de la potencia
  PitchAnalyzer analyzer(umbral1,umbral2,umbral3,pot_max,n_len, rate, PitchAnalyzer::HAMMING, 50, 500);
  
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
float min1, min2, max1, max2;
  vector<float> filter;
  /*for(unsigned int i=0; i<f0.size()-1; i++){
    printf(" %f ",f0[i]);
  }*/
   filter = f0;

   min1=fmin(f0[0],f0[1]);
   max1= fmax(f0[0],f0[1]);
   filter[0]= f0[0]+f0[1]-min1-max1;

  for( unsigned int i=1; i < f0.size()-1 ; i++){

    min1=fmin(f0[i-1],f0[i]);
    min2=fmin(min1,f0[i+1]);
    max1= fmax(f0[i-1],f0[i]);
    max2 = fmax(max1,f0[i+1]);
    filter[i]= f0[i-1]+f0[i]+f0[i+1]-min2-max2; 
  }

  f0=filter;
  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
