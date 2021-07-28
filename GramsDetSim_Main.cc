// GramsDetLevelSim_Main.cpp
// 23-Jul-2021 Luke Zerrer <luke.zerrer@columbia.edu>

// This program reads in GramsG4 Level information in ntuple format 
// and returns GramsDetSim Level ntuples. For the ntuple format reference
// the ntuple_outline.tex colocated with this file in the git repository.

// The program will automatically be compiled during the cmake/make 
// process for GramsG4. However, it can be compiled stand-alone 
// assuming that ROOT has been set up with:

// g++ <program-name>.cc `root-config --clfags --libs` -o <program-name>

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

// Recombination takes the input of change in ionized energy (E), electric field (F),
// and Argon density rho (rho), and returnes the amount of ionized energy
// which does not undergo recombination.
double recombination(double rho, 
        double F, 
        double Eion)
{
    double value;
    double numer;
    double total;

//The following calculations are based off of the modified box model used in the ICARUS 
// experiment, with constant values taken from the brookhaven page on liquid argon TPCs.

    value = (0.930 + (0.212/(F * rho)) * (Eion));
    numer = std::log (value);
    total = numer / (0.212 / ( F * rho));
    
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
        recombinationResult[i] = recombination ( 1.396 , .1 , Eion[i] );
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
   auto updatedNtuple = inputNtuple.Define( "recombination", recombinationVector, {
       "Eion"} 
    );

    // 
    updatedNtuple.Snapshot("recombinationNtuple" , outputfile );

}

/* Next steps:
1) Change the inputNtuple to only read in the values E_ion, t, x, y, z
2) E_ion will become E, x, y will remain the same. What will differ is that t_final will equat t_n + z*drift speed. 
Explanation: The ionization energy (in a perfect system) will remain the same from what the GG4 Level determines it will output.
The x position and the y position will also remain the same. The time, however, will differ. This is because the detector will not
read out the energy reading the wires immediately upon emission. Rather the energy will only be deposited after the electrons
drift across the LArTPC.
3) Update names "energy_after_recombination" & "DetSimNtuple"
4) Drift speed: 2.2 mm/microsec
5) XML integration