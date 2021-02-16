#include "simplegaussian.h"
#include <cmath>
#include <cassert>
#include "wavefunction.h"
#include "../system.h"
#include "../particle.h"
#include "../sampler.h"

#include <iostream>

#include "gradientdecent.h"


using namespace std;

SimpleGaussian::SimpleGaussian(System* system, double alpha, double beta) :
        WaveFunction(system) {
    assert(alpha >= 0);
    m_numberOfParameters = 1;
    m_parameters.reserve(2);
    m_parameters.push_back(alpha);
    m_parameters.push_back(beta);

    //Add a beta value also and make the r_tot depend on beta and set beta=0
    //to get harmonic thing
}

double SimpleGaussian::evaluate(std::vector<class Particle*> particles) {
    /* You need to implement a Gaussian wave function here. The positions of
     * the particles are accessible through the particle[i].getPosition()
     * function.
     *
     * For the actual expression, use exp(-alpha * r^2), with alpha being the
     * (only) variational parameter.
     */

     //Calculating a simple gaussian function, looping over each particle
     //in each dimension.

// Newest

    //double r_tot=0.0, psi=1.0,
    double g_func=1;
    std::vector<double> r_pos;

    double x_part, y_part, z_part;
    std::vector<int> dimensions_length(m_system->getNumberOfDimensions());
    std::iota(dimensions_length.begin(), dimensions_length.end(), 0);
/*
    for(int i=0; i<m_system->getNumberOfParticles(); i++){
      r_pos=particles[i]->getPosition();
      for (int dim=0; dim<m_system->getNumberOfDimensions(); dim++){
        if (dim==2){
          r_tot+=m_parameters[1]*r_pos[dim]*r_pos[dim];
        }
        else{
        r_tot+=r_pos[dim]*r_pos[dim];
        }
      }
      g_func*=exp(-m_parameters[0]*r_tot);
    }


    */

    for(int i=0; i<m_system->getNumberOfParticles(); i++){
      r_pos=particles[i]->getPosition();

      if (dimensions_length.size()==3){
        x_part=r_pos[0]*r_pos[0];
        y_part=r_pos[1]*r_pos[1];
        z_part=m_parameters[1]*r_pos[2]*r_pos[2];
      }
      else if(dimensions_length.size()==2){
        x_part=r_pos[0]*r_pos[0];
        y_part=r_pos[1]*r_pos[1];
        z_part=0;
      }
      else {
        x_part=r_pos[0]*r_pos[0];
        y_part=0;
        z_part=0;
      }

      g_func*=exp(-m_parameters[0]*(x_part+y_part+z_part));
    }

    return g_func;
}

//Analytical differentiation
double SimpleGaussian::computeDoubleDerivative(std::vector<class Particle*> particles) {
    /* All wave functions need to implement this function, so you need to
     * find the double derivative analytically. Note that by double derivative,
     * we actually mean the sum of the Laplacians with respect to the
     * coordinates of each particle.
     *
     * This quantity is needed to compute the (local) energy (consider the
     * Schrödinger equation to see how the two are related).
     */

     //Non interacting, probably have to add a longer bit of code and an
     //if statement when adding the interacting case.
     //This is analytically and only works on simple gaussion functions

     // For non-interacting particles
     std::vector<double> r_pos;
     double wf=evaluate(particles);
     double derivate2=0;
     double x_lap, y_lap, z_lap;
     std::vector<int> dimensions_length(m_system->getNumberOfDimensions());
     std::iota(dimensions_length.begin(), dimensions_length.end(), 0);

     //Analytically differentiatian
     //if(m_system->getNumeric()==false){

       for(int i=0; i<m_system->getNumberOfParticles(); i++){
         r_pos=particles[i]->getPosition();

         if (dimensions_length.size()==3){
           x_lap=(2*m_parameters[0]*r_pos[0]*r_pos[0]-1);
           y_lap=(2*m_parameters[0]*r_pos[1]*r_pos[1]-1);
           z_lap=m_parameters[1]*(2*m_parameters[0]*m_parameters[1]*r_pos[2]*r_pos[2]-1);
         }
         else if(dimensions_length.size()==2){
           x_lap=(2*m_parameters[0]*r_pos[0]*r_pos[0]-1);
           y_lap=(2*m_parameters[0]*r_pos[1]*r_pos[1]-1);
           z_lap=0;
         }
         else {
           x_lap=(2*m_parameters[0]*r_pos[0]*r_pos[0]-1);
           y_lap=0;
           z_lap=0;
         }

         derivate2+=(x_lap+y_lap+z_lap);
       }

      return -derivate2*wf*m_parameters[0];
    //}

}


std::vector<double> SimpleGaussian::computeQuantumForce (std::vector<double> particles){
    //Returns a vector consisting of up to 3 particles
    //Only for spherical without correlation
    //double QF=0;

    double x_i=particles[0];
    double y_i=particles[1];
    double z_i=particles[2];

    std::vector<double> QF_vec =  std::vector<double>();
    std::vector<int> dimensions_length(m_system->getNumberOfDimensions());
    std::iota(dimensions_length.begin(), dimensions_length.end(), 0);

    if (dimensions_length.size()==3){
      QF_vec.push_back(-4*x_i*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
      QF_vec.push_back(-4*y_i*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
      QF_vec.push_back(-4*z_i*m_parameters[1]*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
    }
    else if(dimensions_length.size()==2){
      QF_vec.push_back(-4*x_i*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
      QF_vec.push_back(-4*y_i*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
      z_i=0;
    }
    else {
      QF_vec.push_back(-4*x_i*m_parameters[0]*exp(m_parameters[0]*(x_i+y_i+m_parameters[1]*z_i)));
      y_i=0;
      z_i=0;
    }

    return QF_vec;
}

/*
double SimpleGaussian::E_LDerivative(double alpha_n){
    m_parameters[0]=alpha_n;
    m_system->runMetropolisSteps(1000);
    double expectEnerg, expectE_L_deri, expectderi_dot_EL;
    std::vector<double> grad_values =  std::vector<double>();

    grad_values =m_sampler->getGradientDecentValues();

    expectEnerg=grad_values[0];
    expectE_L_deri=grad_values[1];
    expectderi_dot_EL=grad_values[2];
    double expectEnergy    = expectEnerg/(m_system->getNumberOfMetropolisSteps());//getEquilibrationFraction());
    double expectE_L_deriv     = expectE_L_deri/(m_system->getNumberOfMetropolisSteps());//getEquilibrationFraction());
    double expectderiv_dot_EL = expectderi_dot_EL/(m_system->getNumberOfMetropolisSteps());//getEquilibrationFraction());

    double E_L_derive=2*(expectderiv_dot_EL - expectEnergy*expectE_L_deriv)/m_system->getEquilibrationFraction();
    return  E_L_derive;
}
*/
