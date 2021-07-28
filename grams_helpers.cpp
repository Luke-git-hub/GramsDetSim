#include <cmath>       /* log */
#include <iostream>
#include <fstream>    /* read in data */
#include <algorithm>  
#include <string>
#include <iomanip>
#include <sstream>
#include <map>
#include <tuple>
#include <vector>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

// copy these into the main GramsDetSim routine, then all you should need to do is include code to read the ntupl

double charge_recombination(double dEdx)
// Takes input of the change in energy across the distance travelled in a single step by the particle
// source for this equation is eqn 2.4 in the 2013 paper "A study of electron recombination using highlyionizing particles in the ArgoNeuT Liquid ArgonTPC"
// this modified box model is used because it works for all ranges of dE/dx as well as not having the technical difficulties that arise when applying the birks model to highly ionizing particles
{
  double field; //electric field on the cloud of ionized particles, varies with position due to space charge, which we don't have a model for yet
  //since geant4's units gives dEdx as MeV/mm, we need to convert it to MeV/cm
  double new_dEdx = dEdx * 10
  double a = 0.930; //average of the a value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, unitless value
  double b = 0.212; // average of the B value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, units of (kV*g)/(MeV*cm^3)
  double rho = 1.3973; //density of liquid argon, from brookhaven LAr page, units of (g/cm^3)
  //multiple calculations for this are presented in the paper, we will be using eqn 8.1 to calculate sigma as it removes some variables
  field = 1.0; //since we can't model space charge, let's just set it to a constant value for now (units are kV/cm)
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  //Be very specific in where this equation is from (what paper), what it is finding, what it is talking about
  double sigma = (b * dEdx) / (field * rho);
  double total = log(a + sigma) / sigma; //using cmath's log function
  //returns a decimal value representing the fraction that has survived recombination
  return total;
}

typedef const ROOT::VecOps::RVec<double> inVec_t;
std::vector<double> recombination_vector(inVec_t dEdx)
{
  auto vectorSize = dEdx.size();
  std::vector<double> recomb_output( vectorSize );
  for (size_t i = 0; i != vectorSize, i++)
  {
    recomb_output[i] = charge_recombination(dEdx[i]);
  }
  return recomb_output;
}


int main()
//for testing recombination
//hopefully this will work
{
 inVec_t dEdx = <1,2,3,4,5>;
 std::vector<double> output = recombination_vector(dEdx);
 std::cout << output << std::endl;
}
