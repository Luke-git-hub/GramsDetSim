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

// copy these into the main GramsDetSim routine, then all you should need to do is include code to read the ntuple
// charge_recombination performs the actual calculation of recombination (returning the fraction that survived, not the energy of that fraction)
// recombination_vector performs the charge_recombination calculation for a whole vector of inputs, and writes it to a new output vector

double charge_recombination(double dEdx)
// Takes input of the change in energy across the distance travelled in a single step by the particle
// source for this equation is eqn 2.4 in the 2013 paper "A study of electron recombination using highlyionizing particles in the ArgoNeuT Liquid ArgonTPC"
// this modified box model is used because it works for all ranges of dE/dx as well as not having the technical difficulties that arise when applying the birks model to highly ionizing particles
{
  double field; //electric field on the cloud of ionized particles, varies with position due to space charge, which we don't have a model for yet
  double new_dEdx; //since geant4's units gives dEdx as MeV/mm, we need to convert it to MeV/cm
  new_dEdx = dEdx * 10;
  double a = 0.930; //average of the a value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, unitless value
  double b = 0.212; // average of the B value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, units of (kV*g)/(MeV*cm^3)
  double rho = 1.3973; //density of liquid argon, from brookhaven LAr page, units of (g/cm^3)
  double sigma; //multiple calculations for this are presented in the paper, we will be using eqn 8.1 to calculate sigma as it removes some dependencies
  double total;
  field = 1.0; //since we can't model space charge, let's just set it to a constant value (units are kV/cm)
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  //Be very specific in where this equation is from (what paper), what it is finding, what it is talking about
  sigma = (b * dEdx) / (field * rho);
  total = log(a + sigma) / sigma;
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
//how to take in the TTree?
{
  std::string outputfile("gramsdetsim_recombination.root");
  //take in TTree and TFile from GramsDetSim
  ROOT::RDataFrame input_ntuple("GramsDetSim_ntuple", "NewOutput_GG4Level.root");
  auto output = 0; 
}


// Depreciated code below -----------------------------------------------------

/*
//here is the origonal recombination function, not generalizable as it asks for manual entry of values
double origonal_recombination()
// source for this equation is eqn 2.4 in the 2013 paper "A study of electron recombination using highlyionizing particles in the ArgoNeuT Liquid ArgonTPC"
// this modified box model is used because it works for all ranges of dE/dx as well as not having the technical difficulties that arise when applying the birks model to highly ionizing particles
{
  double field; //electric field on the cloud of ionized particles, varies with position
  double dEdx; //change in energy of the compton electron that produced the ionization field, over the path length of the electron
  double a = 0.930; //average of the a value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, unitless value
  double b = 0.212; // average of the B value for 20 to 90 degrees angle bins for proton sample detection in ArgoNeuT, units of (kV*g)/(MeV*cm^3)
  double rho = 1.3973; //density of liquid argon, from brookhaven LAr page, units of (g/cm^3)
  double sigma; //multiple calculations for this are presented in the paper, we will be using eqn 8.1 to calculate sigma as it removes some dependencies
  double total;
  //now we are going to find the electric field at the position of the compton electron
  std::cout << "Please enter the electric field in your LArTPC\n";
  std::cin >> field;
  std::cout << "Please enter the change in energy over the path length of the particle this step\n";
  std::cin >> dEdx;
  //use a different model? need to justify coefficients
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  //Be very specific in where this equation is from (what paper), what it is finding, what it is talking about
  sigma = (b * dEdx) / (field * rho);
  total = log(a + sigma) / sigma;
  //returns a decimal value representing the fraction that has survived recombination
  return total;
}


int old_main()
{
  double recomb_value;
  recomb_value = origonal_recombination();
  std::cout << "Your recombination value is " << recomb_value << std::endl;
}

double charge_absorption (double I_nought, double distance)
{
  //needs to be redone, currently looking at beta electrons and not ionization charges, disregard this function for now
  double beta_coefficient; //still hunting down how to calculcate this value
  double intensity;
  double power;
  beta_coefficient = 0.0;
  power = -1 * beta_coefficient * distance; //would need to keep track of the movement in each step. Momentum could work?
  intensity = I_nought * exp(power);
  return intensity;
}
*/
