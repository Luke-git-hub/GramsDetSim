#include <cmath>       /* log */
#include <iostream>
#include <fstream>    /* read in data */
#include <algorithm>  
#include <string>
#include <iomanip>
#include <sstream>

// recombination needs more geometry, nrg and field are function of x y, z and t (thanks LArHits)
// specify where in the step it occurs
// electric field table? doesn't need super fine resolution
// finish up recombination, run an ntuple through it, make plots to show there is a difference between recomb on or off, and it makes sense

double charge_recombination (double E, double F, double rho)
//Takes input of ionization energy deposited by a particle (E), electric field (F), and argon density (rho)
//units of MeV, kV/cm, and g/cm^3 respectively
{
  double value;
  double numer;
  double total;
  //use a different model? need to justify coefficients
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  //Be very specific in where this equation is from (what paper), what it is finding, what it is talking about
  value = (0.930 + (0.212/(F * rho))*(E));
  numer = log(value);
  total = numer / (0.212 / ( F * rho) * E);
  //returns the ratio of ionization charge that survived recombination (expand on this)
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
