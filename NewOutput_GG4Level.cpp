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
  std::string outputfile("NewOutput_GG4Level.root");
  if ( argc > 2 )
    outputfile = std::string( argv[2] );

  // Define map:

  typedef std::map< std::tuple< int , int , int >, std::string > trackMap_t;

  // Define trackMap:

  trackMap_t trackMap; 

  // Read TrackInfo ntuple:

  ROOT::RDataFrame trackInfo( "TrackInfo", filename,
			      {"Run", "Event", "TrackID", "ProcessName"} );

  // Copy TrackInfo's info to trackMap: 

  trackInfo.Foreach(
		    [&trackMap]( int run, 
				 int event, 
				 int trackID, 
				 ROOT::VecOps::RVec<char>process)
		    {
		      std::string processName;
		      for ( auto i = process.begin(); 
			    i!= process.end() && (*i) !=0; ++i)
			processName.push_back(*i);
		      trackMap[{run,event,trackID}] = processName;
		    },
		     
		    {"Run", "Event", "TrackID", "ProcessName" }
		    );
  if (debug) {
    for ( auto i = trackMap.begin(); i != trackMap.end(); ++i ){
      auto key = (*i).first;
      auto value = (*i).second;
      std::cout << " run=" << std::get<0>(key)
		<< " event=" << std::get<1>(key)
		<< " trackID=" << std::get<2>(key)
		<< " parentID=" << std::get<3>(key)
		<< " process=" << value << std::endl;
    } // loop over track
  } // if debug

  // Filter Compton rows from LArHits: 

  ROOT::RDataFrame larHits( "LArHits", filename );
  auto comptonHits = larHits.Filter(
       [&trackMap](int run, int event,  int trackID)
     { 
       return trackMap[{Run, Event, TrackID}]  == "compt";
     },
       { "Run", "Event", "TrackID" }
				    );

  // Filter photoabsorption rows from LArHits:

  ROOT::RDataFrame larHits( "LArHits", filename );
  auto photHits = larHits.Filter(
       [&trackMap](int run, int event, int trackID)
       {
	 return trackMap[{Run, Event, TrackID}] == "phot";
       },
       { "Run", "Event", "TrackID"}
			           );

  // Filter pair production rows from LArHits:

  ROOT::RDataFrame larHits( "LarHits", filename);
  auto pairHits = larHits.Filter(
       [&trackMap](int run, int event, int trackID)
    {
      return trackMap[{Run, Event, TrackID}] == "pair";
    },
    { "Run", "Event", "TrackID"}
                                );

// Filter Bremsstrahlung rows from LArHits:

  ROOT::RDataFrame larHits( "LArHits", filename);
  auto ebremHits = larHits.Filter(
       [&trackMap](int run, int event, int trackID )
  {
    return trackMap[{Run, Event, TrackID}] == "ebrem";
  },
  { "Run", "Event", "TrackID"}
                              );

       // Define hitVectors:

typedef struct hitVectors
       {
	 // need to figure out how to add quantity ******************************
	 std::vector<int>    numPhotons;
	 std::vector<double> energy;
	 std::vector<double> tStart;
	 std::vector<double> xStart;
	 std::vector<double> yStart;
	 std::vector<double> zStart;
	 // need energies here *************************************************
	 // need to add ParentID ***********************************************
} hitInfo;

typedef  std::map< std::tuple< int, int, int >, hitInfo > hitMap;

 hitMap compt_hitMap;
 hitMap phot_hitMap;
 hitMap pair_hitMap;
 hitMap ebrem_hitMap;

 // Include Compton-induced hits in compt_hitMap:

 comptonHits.Foreach(
    [&compt_hitMap](
	      int run, 
	      int event,
	      int trackID,
	      int numPhotons, 
	      double energy, 
	      double tStart, 
	      double xStart, 
	      double yStart, 
	      double zStart 
)
     { // Copy the address of the struct. pointed to by run/event/trackID
       auto &hit = compt_hitMap[{run, event, trackID}];
       // "Define the address of hit as hitMap[{run, event, trackID}] 	    

	   
       // Append all the info in this ntuple row to the hitInfo vectors:
       hit.numPhotons.push_back( numPhotons );
       hit.energy.push_back( energy );
       hit.tStart.push_back( tStart );
       hit.xStart.push_back( xStart );
       hit.yStart.push_back( yStart );
       hit.zStart.push_back( zStart );
     },


     // List ntuple columns: 
    {   "run",
	"event", 
	"trackID", 
	 "numPhotons", 
	 "energy", 
	 "tStart", 
	 "xStart", 
	 "yStart", 
	 "zStart"
       
	 }
     );

 photHits.Foreach(
   [&phot_hitMap](
	      int run,
	      int event,
	      int trackID,  
	      int numPhotons,
      	      double energy,
	      double tStart
	      double xStart,
	      double yStart,
	      double zStart
	      )

{ // Copy the address of the struct. pointed to by run/event/trackID
  auto &hit = phot_hitMap[{run, event, trackID}];
 // "Define the address of hit as hitMap[{run, event, trackID}]

 // Append all the info in this ntuple row to the hitInfo vectors:
  hit.numPhotons.push_back( numPhotons );
  hit.energy.push_back( energy );
  hit.tStart.push_back( tStart );
  hit.xStart.push_back( xStart );
  hit.yStart.push_back( yStart );
  hit.zStart.push_back( zStart );
 },


    // List ntuple columns:
    {   "Run",
	"Event",
	"TrackID",
	"numPhotons",
	"energy",
	"tStart",
	"xStart",
	"yStart",
	"zStart"
	}
		  );

 pairHits.Foreach(
  [&pair_hitMap]( int run,
		  int event,
	     	  int trackID,
	      	  int numPhotons,
      		  double energy,
                  double tStart
	       	  double xStart,
		  double yStart,
		  double zStart
	          )
 
 { // Copy the address of the struct. pointed to by run/event/trackID
   auto &hit = pair_hitMap[{run, event, trackID}];
 // "Define the address of hit as hitMap[{run, event, trackID}]

 // Append all the info in this ntuple row to the hitInfo vectors:
   hit.numPhotons.push_back( numPhotons );
   hit.energy.push_back( energy );
   hit.tStart.push_back( tStart );
   hit.xStart.push_back( xStart );
   hit.yStart.push_back( yStart );
   hit.zStart.push_back( zStart );
 },


  // List ntuple columns:
  {   "Run",
      "Event",
      "TrackID",
      "numPhotons",
      "energy",
      "tStart",
      "xStart",
      "yStart",
      "zStart"
      }
		  );

 pairHits.Foreach(
  [&ebrem_hitMap](int run,
    		  int event,
      		  int trackID,
	       	  int numPhotons,
	       	  double energy,
                  double tStart
	          double xStart,
		  double yStart,
	          double zStart
		  )


  { // Copy the address of the struct. pointed to by run/event/trackID
    auto &hit = ebrem_hitMap[{run, event, trackID}];

    // "Define the address of hit as hitMap[{run, event, trackID}]

    // Append all the info in this ntuple row to the hitInfo vectors:
   
    hit.numPhotons.push_back( numPhotons );
    hit.energy.push_back( energy );
    hit.tStart.push_back( tStart );
    hit.xStart.push_back( xStart );
    hit.yStart.push_back( yStart );
    hit.zStart.push_back( zStart );
  },


  // List ntuple columns:
  {   "Run",
      "Event",
      "TrackID",
      "numPhotons",
      "energy",
      "tStart",
      "xStart",
      "yStart",
      "zStart"
      }
                  );


 // Create the new output ntuple: 

 TFile* output = new TFile(outputfile.c_str(), "RECREATE");
 TTree* ntuple = new TTree("comptonNtuple", "Energy deposited in Compton scattering");

 // Define the variables to be accessed: 

 int compt_Run;
 int compt_Event; 
 int compt_TrackID;
 std::vector<int> compt_numPhotons;
 std::vector<double> compt_energy;
 std::vector<double> compt_tStart;
 std::vector<double> compt_xStart;
 std::vector<double> compt_yStart;
 std::vector<double> compt_zStart;
 

 int phot_Run;
 int phot_Event;
 int phot_TrackID;
 std::vector<int> phot_numPhotons;
 std::vector<double> phot_energy;
 std::vector<double> phot_tStart;
 std::vector<double> phot_xStart;
 std::vector<double> phot_yStart;
 std::vector<double> phot_zStart;


 int pair_Run;
 int pair_Event;
 int pair_TrackID;
 std::vector<int> pair_numPhotons;
 std::vector<double> pair_energy;
 std::vector<double> pair_tStart;
 std::vector<double> pair_xStart;
 std::vector<double> pair_yStart;
 std::vector<double> pair_zStart;


 int ebrem_Run;
 int ebrem_Event;
 int ebrem_TrackID;
 std::vector<int> ebrem_numPhotons;
 std::vector<double> ebrem_energy;
 std::vector<double> ebrem_tStart;
 std::vector<double> ebrem_xStart;
 std::vector<double> ebrem_yStart;
 std::vector<double> ebrem_zStart;


 // Assign each variable to its own branch:

 ntuple->Branch("compt_Run", &compt_Run, "compt_Run/I");
 ntuple->Branch("compt_Event", &compt_Event, "compt_Event/I");
 ntuple->Branch("compt_TrackID", &compt_TrackID, "compt_TrackID/I");
 ntuple->Branch("compt_numPhotons", &compt_numPhotons);
 ntuple->Branch("compt_energy", &compt_energy);
 ntuple->Branch("compt_tStart", &compt_tStart);
 ntuple->Branch("compt_xStart", &compt_xStart);
 ntuple->Branch("compt_yStart", &compt_yStart);
 ntuple->Branch("compt_zStart", &compt_zStart);


 ntuple->Branch("phot_Run", &phot_Run, "phot_Run/I");
 ntuple->Branch("phot_Event", &phot_Event, "phot_Event/I");
 ntuple->Branch("phot_TrackID", &phot_TrackID, "phot_TrackID/I");
 ntuple->Branch("phot_numPhotons", &phot_numPhotons);
 ntuple->Branch("phot_energy", &phot_energy);
 ntuple->Branch("phot_tStart", &phot_tStart);
 ntuple->Branch("phot_xStart", &phot_xStart);
 ntuple->Branch("phot_yStart", &phot_yStart);
 ntuple->Branch("phot_zStart", &phot_zStart);


 ntuple->Branch("pair_Run", &pair_Run, "pair_Run/I");
 ntuple->Branch("pair_Event", &pair_Event, "pair_Event/I");
 ntuple->Branch("pair_TrackID", &pair_TrackID, "pair_TrackID/I");
 ntuple->Branch("pair_numPhotons", &pair_numPhotons);
 ntuple->Branch("pair_energy", &pair_energy);
 ntuple->Branch("pair_tStart", &pair_tStart);
 ntuple->Branch("pair_xStart", &pair_xStart);
 ntuple->Branch("pair_yStart", &pair_yStart);
 ntuple->Branch("pair_zStart", &pair_zStart);


 ntuple->Branch("ebrem_Run", &ebrem_Run, "ebrem_Run/I");
 ntuple->Branch("ebrem_Event", &ebrem_Event, "ebrem_Event/I");
 ntuple->Branch("ebrem_TrackID", &ebrem_TrackID, "ebrem_TrackID/I");
 ntuple->Branch("ebrem_numPhotons", &ebrem_numPhotons);
 ntuple->Branch("ebrem_energy", &ebrem_energy);
 ntuple->Branch("ebrem_tStart", &ebrem_tStart);
 ntuple->Branch("ebrem_xStart", &ebrem_xStart);
 ntuple->Branch("ebrem_yStart", &ebrem_yStart);
 ntuple->Branch("ebrem_zStart", &ebrem_zStart);



 // Loop over map, write each entry to output ntuple. 

 for ( auto i = compt_hitMap.begin(); i!= compt_hitMap.end(); ++i )
   {
     auto key = (*i).first;
     Run = std::get<0>(key);
     Event = std::get<1>(key);
     TrackID = std::get<2>(key);

     auto &vectors = (*i).second;
     PDGCode = vectors.PDGCode;
     numPhotons = vectors.numPhotons;
     energy = vectors.energy;
     tStart = vectors.tStart;
     xStart = vectors.xStart;
     yStart = vectors.yStart;
     zStart = vectors.zStart;
     tEnd = vectors.tEnd;
     xEnd = vectors.xEnd;
     yEnd = vectors.yEnd;
     zEnd = vectors.zEnd;
     identifier = vectors.identifier;

     ntuple->Fill();
   }

 ntuple->Write();
 output->Close();
}
