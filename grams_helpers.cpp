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

// recombination needs more geometry, nrg and field are function of x y, z and t (thanks LArHits)
// specify where in the step it occurs
// electric field table? doesn't need super fine resolution
// finish up recombination, run an ntuple through it, make plots to show there is a difference between recomb on or off, and it makes sense

double field_lookup (double x, double y, double z, double t)
{
  return 0 //replace with actual field function when possible
}

double energy_calculation(double x, double y, double z, double t)
{
 return 0 //replace with actual energy function if possible
}

double charge_recombination(double x, double y, double z, double t)
// Takes input of x, y, and z location of particle in the detector, as well as the time
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
  field = field_lookup(x, y, z, t);
  dEdx = energy_calculation(x,y,z,t) ;//georgia says we need this, can we just use Bill's code?
  //use a different model? need to justify coefficients
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  //Be very specific in where this equation is from (what paper), what it is finding, what it is talking about
  sigma = (b * dEdx) / (field * rho);
  total = log(a + sigma) / sigma;
  //returns a decimal value representing the fraction that has survived recombination
  return total;
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

int main()
//for testing recombination
{
  std::string filename("gramsdetsim.root");
  std::string outputfile("gramsdetsim_recombination.root");
    //go through from ntuple.start to ntuple.end
    double output;
    auto input;
    //need to process ntuple, pull out what I need
    output = charge_recombination(input);
    std::cout << output << std::endl;
}
