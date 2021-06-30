#include <cmath>       /* log */
#include <iostream>
#include <fstream>    /* read in data */
#include <algorithm>  
#include <string>
#include <iomanip>
#include <sstream>


double recombination (double E, double F, double rho)
//Takes input of change in ionized energy (E), electric field (F), and argon density (rho)
{
  double value;
  double numer;
  double total;
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  value = (0.930 + (0.212/(F * rho))*(E));
  numer = log(value);
  total = numer / (0.212 / ( F * rho));
  //returns the amount of ionized energy that survived recombination
  return total;
}

double absorption (double I_nought, double distance)
{
  //georgia said I should be looking at absorption of ionization, isn't that what recombination is though?
  //seems like this eqn works for absorbtion of beta electrons though so we're good ig
  double beta_coefficient; //still hunting down how to calculcate this value
  double intensity;
  double power;
  beta_coefficient = 0.0;
  power = -1 * beta_coefficient * distance;
  intensity = I_nought * exp(power);
  return intensity;
}


double pair_production (double photon_energy)
{
  //find the probability of pair production occurring
  //minimum energy of 1.022 MeV, this equation might function better at higher energies? unsure
  //has the advantage of not requiring the thickness of the absorber
  double const avogadro=6.022e+23;
  double atomic_mass;
  double radiation_length;
  double energy; //bnl says this should be the incident particle energy, but that's the photon which is accounted for above
  double energy_transfer;
  double value;
  atomic_mass = 39.948; //using average mass, should double check later
  radiation_length = 14.0; //current units are cm, taken from bnl documentation
  energy_transfer = energy / photon_energy;
  value = atomic_mass / (avogadro * radiation_length) * (1 - 4 * energy_transfer / 3) * (1 - energy_transfer);
  return value;


}

double pp_energy(double photon_energy)
{
  //calculating the energies of the two particles produced, given the energy of the photon that underwent pair production
  double MeC_squared;
  //combined rest masses of the electron and positron
  double energy_transfer;
  //on average, the energy will be split evenly, so this value will be the energy transferred to both the positron and the electron
  MeC_squared = 2 * .5110; //units of MeV
  energy_transfer = .5 * (photon_energy - MeC_squared);
  return energy_transfer;
  
}

/* double bremsstrahlung_energy (double nu)
{
    //note that this returns the energy of the brems photon
    //it does NOT return the probability of it occuring
    // do we even need this? should see the energy drop when it occurs
  double MeC_squared;
  double brems_energy;
  double h;
  //all units in MeV
  MeC_squared = 0.5109989; //check sig figs
  brems_energy = h * nu / MeC_squared;
  return brems_energy;
}
*/

double bremsstrahlung_probability (double e, double particle_mass)
{
  //Equation for emission probability from Leo book (page 38)
  //gonna be honest, I don't know what e is
  double numerator;
  double denominator;
  double probability;
  numerator = pow(e,2);
  denominator = particle_mass * pow(299792458, 2);
  probability = pow(numerator/denominator, 2);
  return probability;
}

int main()
{
    return 0;
}