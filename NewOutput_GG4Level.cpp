// Created 7-7-2021 LR
// This should restructure the TrackInfo and LArHits output into the
// GramsG4-level part of the new output. 

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TFile.h>
#include <TTree.h>

#include <vector>
#include <map>
#include <cstring>
#include <iostream>

// Debug flag:
static const bool debug = false;

// Main routine: 

int main( int argc, char** argv ) {
 
  // Input file name:
  std::string filename("gramsg4.root");
  if ( argc > 1 )
    filename = std::string( argv[1] );

  // Output file name:
  std::string outputfile("NewOutput_GG4Level.cpp");
  if ( argc > 2 )
    outputfile = std::string( argv[2] );

  // Define map:

  typedef std::map< std::tuple< int , int , int >, std::string > trackMap_t;

  // Define trackMap:

  trackMap_t trackMap; 

  // Read TrackInfo ntuple:

  ROOT::RDataFrame trackInfo( "TrackInfo", filename,
			      {"Run", "Event", "TrackID", "ParentID", 
			       "PDGCode", "ProcessName", "t", "x", "y", "z",
			       "Etot", "px", "py", "pz", "identifier"} );

  // Copy TrackInfo's info to trackMap: 

  trackInfo.Foreach(
		    [&trackMap]( int Run, 
				 int Event, 
				 int TrackID, 
				 int ParentID, 
				 int PDGCode, 
				 ROOT::VecOps::RVec<char>ProcessName,
				 ROOT::VecOps::RVec<double>t,
				 ROOT::VecOps::RVec<double>x,
				 ROOT::VecOps::RVec<double>y,
				 ROOT::VecOps::RVec<double>z,
				 double Etot,
				 ROOT::VecOps::RVec<double>px,
				 ROOT::VecOps::RVec<double>py,
				 ROOT::VecOps::RVec<double>pz,
				 int identifier )
// Check if ID int**********************************************************
		    {
		      std::string ProcessName;
		      for ( auto i = process.begin(); 
			    i!= process.end() && (*i) !=0; ++i)
			processName.push_back(*i);
		      trackMap[{Run,Event,TrackID,ParentID}] = ProcessName;
		    },
		    {"Run", "Event", "TrackID", "ParentID", "ProcessName", 
		     "t", "x", "y", "z", "Etot", "px", "py", "pz",
		     "identifier" }
		    );


  if (debug) {
    for ( auto i = trackMap.begin(); i != trackMap.end(); ++i ){
      auto key = (*i).first;
      auto value = (*i).second;
      std::cout << " Run=" << std::get<0>(key)
		<< " Event=" << std::get<1>(key)
		<< " TrackID=" << std::get<2>(key)
		<< " ParentID=" << std::get<3>(key)
		<< " ProcessName=" << value 
		<< " t=" << std::get<5>(key)
		<< " x=" << std::get<6>(key)
		<< " y=" << std::get<7>(key)
		<< " z=" << std::get<8>(key)
		<< " Etot=" << std::get<9>(key)
		<< " px=" << std::get<10>(key)
		<< " py=" << std::get<11>(key)
		<< " pz=" << std::get<12>(key)
		<< " identifier=" << std::get<13>(key)
	//check key vs value for 5-13 **************************************
    } // loop over track
  } // if debug

  // Filter Compton rows from LArHits: 

  ROOT::RDataFrame larHits( "LArHits", filename );
  auto comptonHits = larHits.Filter(
       [&trackMap](int Run, int Event,  int TrackID, int ParentID)
     { 
       return trackMap[{Run, Event, TrackID}]  == "compt";
       //check this ^^ *****************************************************
     },
       { "Run", "Event", "TrackID", "ParentID" }
				    );
       // Define hitVectors:

typedef struct hitVectors
       {
	 std::vector<int>    PDGCode;
	 std::vector<int>    numPhotons;
	 std::vector<double> energy;
	 std::vector<double> tStart;
	 std::vector<double> xStart;
	 std::vector<double> yStart;
	 std::vector<double> zStart;
	 std::vector<double> tEnd;
	 std::vector<double> xEnd;
	 std::vector<double> yEnd;
	 std::vector<double> zEnd;
	 std::vector<int>    identifier;
} hitInfo;

 std::map< std::tuple< int, int, int >, hitInfo > hitMap;

 // Include Compton-induced hits in hitMap:

 comptonHits.Foreach{
   [&hitMap]( int Run,
	      int Event, 
	      int TrackID, 
	      int PDGCode, 
	      int numPhotons, 
	      double energy, 
	      double tStart, 
	      double xStart, 
	      double yStart, 
	      double zStart, 
	      double tEnd, 
	      double xEnd, 
	      double yEnd, 
	      double zEnd, 
	      int identifier)
     { // Copy the address of the struct. pointed to by run/event/trackID
       auto &hit = hitMap[{Run, Event, TrackID}];
				   
