#include <cmath>       /* log */
#include <iostream>
#include <fstream>    /* read in data */
#include <algorithm>  
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

typedef std::vector<double_t> vector_t; // vector of doubles
typedef std::vector<vector_t> nested_t; // vector of vectors of doubles

float recombination (float E, float F, float rho)
//Takes input of change in ionized energy (E), electric field (F), and argon density (rho)
{
  float value;
  float numer;
  float total;
  //The following calculations are based off of the modified box model used in the ICARUS experiment, with constant values taken from the brookhaven page on liquid argon TPCs
  value = (0.930 + (0.212/(F * rho))*(E));
  numer = log(value);
  total = numer / (0.212 / ( F * rho));
  //returns the amount of ionized energy that survived recombination
  return total;
}


//Main GramsDetSim

int main () {
//This version of the program will require the user to externally save and prepare the output from Grams G4 Sim. Future versions will imbed this functionality.
//Grams G4 Sim ntuple output must be saved as GramsG4Data.txt
   
  std::ifstream testFile("GramsG4Data.txt");    
  std::string line;

  while(getline(testFile, line)){
//Declare all variables as their types. In its current format all columns are declared as floats. This will change. 
      
// Edit LR 6/25/2021 changed from float to float_t ("flexible" integer), double_t ("flexible" double), vector_t and nested_t (see top)

      float_t Line; // LR 6/25/2021 changing this to float_t from float will save a(n albeit miniscule) amount of space (int = 2 bytes, float =4)
      double_t pri_E;
      double_t pri_xPos; // LR 6/25/2021 these might end up being G4ThreeVectors... planning on asking Bill Tues.
      double_t pri_yPos;
      double_t pri_zPos;
      double_t pri_t;
      double_t pri_px;
      double_t pri_py;
      double_t pri_pz;
      float_t compt_p;
      // vector_t compt_E;
      // vector_t compt_xPos;

      // LR 6/25/2021 I would recommed for the rest of these: list = vector_t, list of arrays = nested_t, everything else either float_t or double_t

//Not entirely sure what the next lines do but something along the lines of splitting each line by asterisks, the assigning them to ss
      std::replace(line.begin(), line.end(), '*', ' ');

      std::stringstream ss(line);

      ss >> Line;
      ss >> pri_E;
      ss >> pri_xPos;
      ss >> pri_yPos;
      ss >> pri_zPos;
      ss >> pri_t;
      ss >> pri_px;
      ss >> pri_py;
      ss >> pri_pz;
      ss >> compt_p;
      //ss >> compt_E;
      //ss >> compt_xPos;

      double_t var1 = 1;
      // compt_E.push_back(var1);
      // compt_xPos.push_back(var1);


//Prints out each position with a label. This will change in the future and take each variable rut it through the corresponding function. 
//Bug: This works up until compt_E. But compt_E is a list and this program cannot recognize that.
      // Input from Simulator
      std::cout << "Line: "        << Line << " ";
      std::cout << "pri_E: "       << pri_E << " ";
      std::cout << "pri_xPos: "    << pri_xPos << " ";
      std::cout << "pri_yPos: "    << pri_yPos << " ";
      std::cout << "pri_zPos: "    << pri_zPos << " ";
      std::cout << "pri_t: "       << pri_t << " ";
      std::cout << "pri_px: "      << pri_px << " ";
      std::cout << "pri_py: "      << pri_py << " ";
      std::cout << "pri_pz: "      << pri_pz << " ";
      std::cout << "compt_p: "     << compt_p << " ";
      // std::cout << "compt_E: "     << compt_E << " ";
      // std::cout << "compt_xPos: "  << compt_xPos << " ";
      //
      //Functions called to replicate what the detector will be recieving 
      std::cout << "\n";
      std::cout << "ionized energy after recombination: " << recombination(pri_E, .1, 1.396) << "MeV";
      // std::cout << "compt_E: "     << compt_E << " ";
      // std::cout << "compt_xPos: "  << compt_xPos << " ";

      // compt_E.clear();
      // compt_xPos.clear();

      std::cout << '\n';
  }   
}
//END
