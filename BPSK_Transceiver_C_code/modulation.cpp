#include <iostream>
#include <random>
#include <ctime>
#include <time.h>
#include <cmath>
#include <math.h>
#include <array>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

int L = 16;
float carrierFrequency = 1000000000000; //carrier frequency , for Ultrasound 1MHz
float amplitudeGain = 1; // amplitude gain of the carrier frequency 
float samplingFrequency = L*carrierFrequency; // sampling rate of carrier generator 


// BPSK - Transmitter
// -----------------------------------------------------------------

vector<int> MultiplyArrayByTwo(vector<int> arr){ // level converter
    for(int i = 0; i < arr.size(); i++) {
        arr[i] = arr[i]==1? 1: -1; // if == 1 true then 1 else -1
    }
     return arr; 
}

vector<int> upsample(vector<int> arr, int L){ // upsampling
    vector<int> temp;
    for(int i = 0; i<arr.size(); i++){
        for(int j = 0; j<L ; j++){
            temp.push_back(arr[i]);
        }
    }
    return temp;
}

/*vector<int> time(int num, int L){
    vector<int> temp;
    for(int i = 0; i<num*L; i++){
        temp.push_back(i);
        }
    return temp;
}
*/

vector<double> signal(vector<int> arr){
    vector<double> s;
    for(int i = 0; i<arr.size(); i++){
        s.push_back(arr[i]*amplitudeGain*cos(2*M_PI*(carrierFrequency*i/samplingFrequency)));
    }
    return s;
}

// -----------------------------------------------------------------



// BPSK - Receiver
// -----------------------------------------------------------------

vector<double> MultiplyByCos(vector<double> arr){
    vector<double> a;
    for(int i = 0; i < arr.size(); i++) {
        a.push_back(arr[i]*amplitudeGain*cos(2*M_PI*(carrierFrequency*i/samplingFrequency)));
    }
    return a;
}

vector<double> MultiplyBySin(vector<double> arr){
    vector<double> a;
    for(int i = 0; i < arr.size(); i++) {
        a.push_back(arr[i]*amplitudeGain*sin(2*M_PI*(carrierFrequency*i/samplingFrequency)));
    }
    return a;
}

vector<double> Add(vector<double> arr, vector<double> arra){
    vector<double> sum;
    for(int i = 0; i<arr.size(); i++){
        sum.push_back(arr[i] + arra[i]);
    }
    return sum;
}

// https://stackoverflow.com/questions/24518989/how-to-perform-1-dimensional-valid-convolution

template<typename T>
std::vector<T>
conv(std::vector<T> const &f, std::vector<T> const &g) {
  int const nf = f.size();
  int const ng = g.size();
  int const n  = nf + ng - 1;
  std::vector<T> out(n, T());
  for(auto i(0); i < n; ++i) {
    int const jmn = (i >= ng - 1)? i - (ng - 1) : 0;
    int const jmx = (i <  nf - 1)? i            : nf - 1;
    for(auto j(jmn); j <= jmx; ++j) {
      out[i] += (f[j] * g[i - j]);
    }
  }
  return out; 
}


vector<int> sample(vector<double> arr,int L){
    vector<int> tmp;
    for(int i = L-1; i<arr.size(); i = i+L){
        tmp.push_back(arr[i]);
    }
    return tmp;
}  


vector<int> transform(vector<int> arr){ // transform back
    for(int i = 0; i < arr.size(); i++) {
        arr[i] = arr[i]>0? 1: 0; // if > 0 true then 1 else 0
    }
     return arr; 
}

// -----------------------------------------------------------------


// generate vector for convolution

vector<double> vec(int L){
    vector<double> v;
    int o = 1;
    for(int i = 0; i<L; i++){
        v.push_back(o);
    }
    return v;
}

int main()
{   
    ofstream outputsignal2("outputsignal_test.csv");
    ofstream s("s_bbtest.csv");
    ofstream demod2("MultipliedCostest.csv");
    ofstream demod_signal2("demodsignaltest.csv");
    ofstream sin("sinus.csv");
    ofstream summ("summe.csv");
    const int num = 10; // Number of bits
    vector<int> arr;
    
    cout << "array for convolution: ";
    vector<double> t = vec(L);
    for (int i = 0; i < t.size(); i++){
        cout << t[i] << " ";
    }
    
    cout << "\n";
    // -----------------------------------------------------------------

    

    //cout << "Generierter Array:" << endl;
    // random array generator
    /*
    srand(static_cast<unsigned int>(time(NULL)));
    for (int i = 0; i < num; i++){
        arr.push_back(rand() % 2);
        cout << arr[i] << " ";
    }
    */
    
    

    // create own vector 
    // -----------------------------------------------------------------
    
    int myints[] = {1,1,1,0,0,0,1,1,1,0,1};
    vector<int> vect (myints, myints + sizeof(myints) / sizeof(int) );
    cout << "The contents of vector are:";
    for (vector<int>::iterator it = vect.begin(); it != vect.end(); ++it)
    cout << ' ' << *it;
    cout << '\n';
    

    // call functions
    // -----------------------------------------------------------------
    
    //modulation
    vector<int> sbb = MultiplyArrayByTwo(vect);
    vector<int> upsampled = upsample(sbb,L);
    vector<double> out = signal(upsampled);
    
     
    for(int i = 0; i < upsampled.size(); i++){
         //cout << arr2[i] << " ";
         //cout << s[i] << " ";
         s << i << " ;" << upsampled[i] << endl;
         outputsignal2 << i << " ;" << out[i] << endl;
    }
 
    //demodulation  
    vector<double> d = MultiplyByCos(out); 
     for(int i = 0; i < d.size(); i++){
        demod2 << i << " ;" << d[i] << endl;
    }

    vector<double> h = MultiplyBySin(out); 
     for(int i = 0; i < h.size(); i++){
       sin << i << " ;" << h[i] << endl;
    }

    vector<double> summe = Add(d,h);
    for(int i = 0; i < summe.size(); i++){
       summ << i << " ;" << summe[i] << endl;
    }


    
    //vector<double> f = summe; 
    vector<double> f = d; 
    vector<double> b = conv(f,t); 

    cout << "Signal after convolution: ";
    for(int i = 0; i < b.size(); i++){
        cout << b[i] << " ";
    }

    cout << "\n";
    
    cout << "Signal after sampling: ";
    vector<int> e = sample(b,L);  
    for(int i = 0; i < e.size(); i++){
        cout << e[i] << " ";
    }

    cout << "\n";
    cout << "Demodulated Signal at receiver end: ";

    vector<int> dsignal = transform(e);
    for(int i = 0; i < dsignal.size(); i++){
        cout << dsignal[i] << " ";
    }
    
    cout << "\n";
    cout << "Upsampled for plot :";

    vector<int> r_t = upsample(dsignal,L);
    for(int i = 0; i < r_t.size(); i++){
        cout << r_t[i] << " ";
        demod_signal2 << i << " ;" << r_t[i] << endl;
    }    

    // -----------------------------------------------------------------
    
    return 0;
}


    //vector<double> g{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
   /* 
    auto v1 = conv(f, g);
    for(auto i : v1){ 
    cout << i << " ";
    demod_signal << i << endl;
    cout << endl;
    }
    */