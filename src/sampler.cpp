#include <iostream>
#include <cmath>
#include <vector>
#include "sampler.h"
#include "system.h"
#include "particle.h"
#include "Hamiltonians/hamiltonian.h"
#include "WaveFunctions/wavefunction.h"

//Write to file modules
#include <fstream>
//Modulesto meassure the cpu time
#include <ctime>
#include <ratio>
#include <chrono>

#include <string>


using namespace std::chrono;
using std::cout;
using std::endl;
using namespace std;


Sampler::Sampler(System* system) {
    m_system = system;
    m_stepNumber = 0;
}

void Sampler::setNumberOfMetropolisSteps(int steps) {
    m_numberOfMetropolisSteps = steps;
}

void Sampler::sample(bool acceptedStep) {
    // Make sure the sampling variable(s) are initialized at the first step.
    if (m_stepNumber == 0) {
        m_cumulativeEnergy = 0;
        time_sec =0;

    }
    //Starting the clock
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    /* Here you should sample all the interesting things you want to measure.
     * Note that there are (way) more than the single one here currently.
     */
    double localEnergy = m_system->getHamiltonian()->
                         computeLocalEnergy(m_system->getParticles());

   //Stopping the clock, adding the time together for each energy cycle
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    time_sec += time_span.count();

    m_cumulativeEnergy  += localEnergy;
    m_stepNumber++;

    if (acceptedStep){
        m_acceptedSteps++;
    }

}

void Sampler::printOutputToTerminal() {
    int     np = m_system->getNumberOfParticles();
    int     nd = m_system->getNumberOfDimensions();
    int     ms = m_system->getNumberOfMetropolisSteps();
    int     p  = m_system->getWaveFunction()->getNumberOfParameters();
    double  ef = m_system->getEquilibrationFraction();
    std::vector<double> pa = m_system->getWaveFunction()->getParameters();

    cout << endl;
    cout << "  -- System info -- " << endl;
    cout << " Number of particles  : " << np << endl;
    cout << " Number of dimensions : " << nd << endl;
    cout << " Number of Metropolis steps run : 10^" << std::log10(ms) << endl;
    cout << " Number of equilibration steps  : 10^" << std::log10(std::round(ms*ef)) << endl;
    cout << endl;
    cout << "  -- Wave function parameters -- " << endl;
    cout << " Number of parameters : " << p << endl;
    for (int i=0; i < p; i++) {
        cout << " Parameter " << i+1 << " : " << pa.at(i) << endl;
    }
    cout << endl;
    cout << "  -- Results -- " << endl;
    cout << " CPU time: " << time_sec << " s" << endl;
    cout << " Energy : " << m_energy << endl;
    cout << " Variance : " << m_variance << endl;
    cout << " Accepted step ratio : " << m_acceptRatio << endl;
    cout << endl;

    //Casually setting the mood if the code works for 1 particle in 1 dimension
    /*
    double analytical_answer_1D_N_1=0.5;
    if (m_energy=analytical_answer_1D_N_1){
      system("open https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    }
    */
}

void Sampler::computeAverages() {
    /* Compute the averages of the sampled quantities. You need to think
     * thoroughly through what is written here currently; is this correct?
     */
    double steps_min_eq=(m_system->getNumberOfMetropolisSteps()*(1-m_system->getEquilibrationFraction()));
    m_energy = m_cumulativeEnergy / steps_min_eq;
    //minus 1?
    //m_stddeviation =sqrt( /m_system->getNumberOfMetropolisSteps()-1);
    //These two are wrong?
    m_variance = m_cumulativeEnergy*m_energy-m_energy*m_energy;
    m_acceptRatio = m_acceptedSteps / steps_min_eq;

}

void Sampler::writeToFile(){
  ofstream myfile;
  string folderpart1, folderpart2;

  bool numeric2=false;
  bool bruteforce=true;

  if (bruteforce==true){
    folderpart1="datadump/bruteforce/";
  }
  else {
    folderpart1 ="datadump/importancesampling/";
  }
  if (numeric2==true){
    folderpart2="numeric/setalpha/";
  }
  else {
    folderpart2 ="analytic/setalpha/";
  }
  int parti= m_system->getNumberOfParticles();
  int dimen= m_system->getNumberOfDimensions();
  //fix to make alpha and numeric global
  double alph= 0.5;
  //std::setprecision(2);
  //cout<<folder;
  //cout<<std::setprecision(2)<<alph;
  std::string filename=folderpart1+folderpart2+"N="+std::to_string(parti)+"Dim="+std::to_string(dimen);
  //<<filename;
  //filename='Data/Processing';
  myfile.open(filename);
  myfile<< "Particles= " << parti << endl;
  myfile<< "Dimensions= "<<dimen<<endl;
  myfile<< "Energy= "<<m_energy<<endl;
  myfile<< "Alpha= "<<alph<<endl;

  myfile.close();


/*

  cout << "  -- System info -- " << endl;
  cout << " Number of particles  : " << np << endl;
  cout << " Number of dimensions : " << nd << endl;
  cout << " Number of Metropolis steps run : 10^" << std::log10(ms) << endl;
  cout << " Number of equilibration steps  : 10^" << std::log10(std::round(ms*ef)) << endl;
  cout << endl;
  cout << "  -- Wave function parameters -- " << endl;
  cout << " Number of parameters : " << p << endl;
  for (int i=0; i < p; i++) {
      cout << " Parameter " << i+1 << " : " << pa.at(i) << endl;
  }
  cout << endl;
  cout << "  -- Results -- " << endl;
  cout << " CPU time: " << time_sec << " s" << endl;
  cout << " Energy : " << m_energy << endl;
  cout << " Variance : " << m_variance << endl;
  cout << " Accepted step ratio : " << m_acceptRatio << endl;

*/

/*
  char name2;
  name2=m_energy;
  ofstream myfile;
  myfile.open("datadump/test"+ name2+".txt");
  myfile << m_energy;
  myfile.close();
*/
}
