// GramsDetLevelSim_Main.cpp
// 23-Jul-2021 Luke Zerrer <luke.zerrer@columbia.edu>

// This program reads in GramsG4 Level information in ntuple format 
// and returns GramsDetSim Level ntuples. For the ntuple format reference
// the ntuple_outline.tex colocated with this file in the git repository.

// The program will automatically be compiled during the cmake/make 
// process for GramsG4. However, it can be compiled stand-alone 
// assuming that ROOT has been set up with:

// g++ <program-name>.cc `root-config --cflags --libs` -o <program-name>

// ROOT includes.
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

// C++ includes.
#include <vector>
#include <iostream>
#include <cstring>
#include <cmath>

// Debug flag.
static const bool debug = false;

// ooooOOOOooooOOOOooooOOOOooooOOOOooooOOOO
// Begin function definitions:

double charge_recombination(double dEdx)
// Takes input of the change in energy across the distance travelled in a single step by the particle
// source for this equation is eqn 2.4 in the 2013 paper "A study of electron recombination using highlyionizing particles in the ArgoNeuT Liquid ArgonTPC"
// this modified box model is used because it works for all ranges of dE/dx as well as not having the technical difficulties that arise when applying the birks model to highly ionizing particles
{
  double field; //electric field on the cloud of ionized particles, varies with position due to space charge, which we don't have a model for yet
  //since geant4's units gives dEdx as MeV/mm, we need to convert it to MeV/cm
  double new_dEdx = dEdx * 10;
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

// ____----____----____----____----____----____----____----____----

// Working with ntuple columns with entire vectors this function reads
// and returns entire vectors
typedef const ROOT::VecOps::RVec<double> inVec_t;

std::vector<double> recombinationVector (
    inVec_t Eion)
{
    auto vectorSize = Eion.size();

    std::vector<double> recombinationResult ( vectorSize );

    for ( size_t i = 0; i != vectorSize; ++i ){
        recombinationResult[i] = charge_recombination ( Eion[i] );
    }

    return recombinationResult;

}

// End function definitions.
// ooooOOOOooooOOOOooooOOOOooooOOOOooooOOOO


// C++ main routine.
int main ( int argc, char** argv ) {
    
    // Determine the name of the input file. 
    std::string filename("NewOutput_GG4Level.root");
    if ( argc > 1 )
        filename = std::string( argv[1] );

    // Determine the name of the output file.
    std::string outputfile("GramsDetSim_Level.root");
    if ( argc > 2 )
    outputfile = std::string( argv[2] );

    // Commented out as we do not want multi-threading with current data
    /*
    if ( ! debug ) { 
        ROOT::EnableImplicitMT();
    }
    */

   // The input dataframe. 
   ROOT::RDataFrame inputNtuple( "Geant4Ntuple", filename );

    // Builds and appends a new column onto inputNtuple after applying function (argument 2) and the arguments necessary for the 
    // function to execute (argument 3).
   auto updatedNtuple = inputNtuple.Define( "energy_after_recombination", recombinationVector, {
       "Eion"} 
    );

    // 
    updatedNtuple.Snapshot("DetSimNtuple" , outputfile );

}

/* Next steps:
1) Change the inputNtuple to only read in the values E_ion, t, x, y, z
2) E_ion will become E, x, y will remain the same. What will differ is that t_final will equat t_n + z*drift speed. 
Explanation: The ionization energy (in a perfect system) will remain the same from what the GG4 Level determines it will output.
The x position and the y position will also remain the same. The time, however, will differ. This is because the detector will not
read out the energy reading the wires immediately upon emission. Rather the energy will only be deposited after the electrons
drift across the LArTPC.
4) Drift speed: 2.2 mm/microsec
5) XML integration  */