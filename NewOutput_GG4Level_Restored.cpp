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
			      {"Run", "Event", "TrackID","ParentID", "ProcessName"} );

  // Copy TrackInfo's info to trackMap: 

  trackInfo.Foreach(
		    [&trackMap]( int run, 
				 int event, 
				 int trackID,
				 int parentID, 
				 ROOT::VecOps::RVec<char>process)
		    {
		      std::string processName;
		      for ( auto i = process.begin(); 
			    i!= process.end() && (*i) !=0; ++i)
			processName.push_back(*i);
		      trackMap[{run,event,trackID,parentID}] = processName;
		    },
		     
		    {"Run", "Event", "TrackID", "ParentID", "ProcessName" }
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

  // Create trackMapPri to hold the trackInfoPri information:
  trackMap_t trackMapPri;

  // Create trackInfoPri, which contains the TrackInfo entries necessary for the pri_ variables
  ROOT::RDataFrame trackInfoPri( "TrackInfo", filename);
  [&trackMapPri]( int run, 
		  int event, 
		  int trackID, 
		  int parentID,
		  ROOT::VecOps::RVec<char> process, 
		  double energy, 
		  ROOT::VecOps::RVec<double> x, 
		  ROOT::VecOps::RVec<double> y, 
		  ROOT::VecOps::RVec<double> z, 
		  ROOT::VecOps::RVec<double> t, 
		  ROOT::VecOps::RVec<double> px, 
		  ROOT::VecOps::RVec<double> py, 
		  ROOT::VecOps::RVec<double>  pz)


  // Filter Compton rows from LArHits: 

  ROOT::RDataFrame larHits( "LArHits", filename );
  auto comptonHits = larHits.Filter(
    [&trackMap](int run, int event,  int trackID, int parentID)
     { 
       return trackMap[{Run, Event, TrackID, ParentID}]  == "compt";
     },
    { "Run", "Event", "TrackID", "ParentID" }
				    );

  // Filter photoabsorption rows from LArHits:

  ROOT::RDataFrame larHits( "LArHits", filename );
  auto photHits = larHits.Filter(
      [&trackMap](int run, int event, int trackID, int parentID)
       {
	 return trackMap[{Run, Event, TrackID, ParentID}] == "phot";
       },
      { "Run", "Event", "TrackID", "ParentID"}
			           );

  // Filter pair production rows from LArHits:

  ROOT::RDataFrame larHits( "LarHits", filename);
  auto pairHits = larHits.Filter(
     [&trackMap](int run, int event, int trackID, int parentID)
    {
      return trackMap[{Run, Event, TrackID}] == "pair";
    },
     { "Run", "Event", "TrackID", "ParentID" }
                                );

// Filter Bremsstrahlung rows from LArHits:

  ROOT::RDataFrame larHits( "LArHits", filename);
  auto ebremHits = larHits.Filter(
 [&trackMap](int run, int event, int trackID, int parentID )
  {
    return trackMap[{Run, Event, TrackID, ParentID}] == "ebrem";
  },
 { "Run", "Event", "TrackID", "ParentID"}
                              );

       // Define hitVectors:

typedef struct hitVectors
       {
	 std::vector<int>    numPhotons;
	 std::vector<double> energy;
	 std::vector<double> tStart;
	 std::vector<double> xStart;
	 std::vector<double> yStart;
	 std::vector<double> zStart;
	 // need energies here *************************************************
	 // need to add ParentID (make it part of the map above!)  ***********************************************
} hitInfo;

typedef trackVectors
{
  doubl
    }

    typedef  std::map< std::tuple< int, int, int, int >, hitInfo > hitMap;

 hitMap pri_hitMap;
 hitMap compt_hitMap;
 hitMap phot_hitMap;
 hitMap pair_hitMap;
 hitMap ebrem_hitMap;

typedef std::vector< hitMap, hitMap, hitMap, hitMap, hitMap > hitMapVect;

 hitMapVect combinedVector;



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

 // Add each hitMap to combinedVector:

 combinedVector.push_back( pri_hitMap );
 combinedVector.push_back( compt_hitMap );
 combinedVector.push_back( phot_hitMap );
 combinedVector.push_back( pair_hitMap );
 combinedVector.push_back( ebrem_hitMap );


 // Create the new output ntuple: 

 TFile* output = new TFile(outputfile.c_str(), "RECREATE");
 TTree* ntuple = new TTree("comptonNtuple", "Energy deposited in Compton scattering");

 // Define the variables to be accessed: 

 int Run;
 int Event;
 double pri_xStart;
 double pri_yStart;
 double pri_zStart;
 double pri_tStart;
 double pri_px;
 double pri_py;
 double pri_pz;


 int compt_Run;
 int compt_Event; 
 int compt_TrackID;
 int compt_ParentID;
 int compt_quantity;
 std::vector<int> compt_numPhotons;
 std::vector<double> compt_energy;
 std::vector<double> compt_tStart;
 std::vector<double> compt_xStart;
 std::vector<double> compt_yStart;
 std::vector<double> compt_zStart;
 

 int phot_Run;
 int phot_Event;
 int phot_TrackID;
 int phot_ParentID;
 int phot_quantity;
 std::vector<int> phot_numPhotons;
 std::vector<double> phot_energy;
 std::vector<double> phot_tStart;
 std::vector<double> phot_xStart;
 std::vector<double> phot_yStart;
 std::vector<double> phot_zStart;


 int pair_Run;
 int pair_Event;
 int pair_TrackID;
 int pair_ParentID;
 int pair_quantity;
 std::vector<int> pair_numPhotons;
 std::vector<double> pair_energy;
 std::vector<double> pair_tStart;
 std::vector<double> pair_xStart;
 std::vector<double> pair_yStart;
 std::vector<double> pair_zStart;


 int ebrem_Run;
 int ebrem_Event;
 int ebrem_TrackID;
 int ebrem_ParentID;
 int ebrem_quantity;
 std::vector<int> ebrem_numPhotons;
 std::vector<double> ebrem_energy;
 std::vector<double> ebrem_tStart;
 std::vector<double> ebrem_xStart;
 std::vector<double> ebrem_yStart;
 std::vector<double> ebrem_zStart;


 // Assign each variable to its own branch:

 ntuple->Branch("Run", &Run, "Run/I");
 ntuple->Branch("Event", &Event, "Event/I");
 ntuple->Branch("pri_xStart", &pri_xStart, "pri_xStart/D");
 ntuple->Branch("pri_yStart", &pri_yStart, "pri_yStart/D");
 ntuple->Branch("pri_zStart", &pri_zStart, "pri_zStart/D");
 ntuple->Branch("pri_tStart", &pri_tStart, "pri_tStart/D");
 ntuple->Branch("pri_px", &pri_px, "pri_px/D");
 ntuple->Branch("pri_py", &pri_py, "pri_py/D");
 ntuple->Branch("pri_pz", &pri_pz, "pri_pz/D");

 ntuple->Branch("compt_Run", &compt_Run, "compt_Run/I");
 ntuple->Branch("compt_Event", &compt_Event, "compt_Event/I");
 ntuple->Branch("compt_TrackID", &compt_TrackID, "compt_TrackID/I");
 ntuple->Branch("compt_ParentID", &compt_ParentID, "compt_ParentID/I");
 ntuple->Branch("compt_quantity", &compt_quantity, "compt_quantity/I");
 ntuple->Branch("compt_numPhotons", &compt_numPhotons);
 ntuple->Branch("compt_energy", &compt_energy);
 ntuple->Branch("compt_tStart", &compt_tStart);
 ntuple->Branch("compt_xStart", &compt_xStart);
 ntuple->Branch("compt_yStart", &compt_yStart);
 ntuple->Branch("compt_zStart", &compt_zStart);


 ntuple->Branch("phot_Run", &phot_Run, "phot_Run/I");
 ntuple->Branch("phot_Event", &phot_Event, "phot_Event/I");
 ntuple->Branch("phot_TrackID", &phot_TrackID, "phot_TrackID/I");
 ntuple->Branch("phot_ParentID", &phot_ParentID, "phot_ParentID/I");
 ntuple->Branch("phot_quantity", &phot_quantity, "phot_quantity/I");
 ntuple->Branch("phot_numPhotons", &phot_numPhotons);
 ntuple->Branch("phot_energy", &phot_energy);
 ntuple->Branch("phot_tStart", &phot_tStart);
 ntuple->Branch("phot_xStart", &phot_xStart);
 ntuple->Branch("phot_yStart", &phot_yStart);
 ntuple->Branch("phot_zStart", &phot_zStart);


 ntuple->Branch("pair_Run", &pair_Run, "pair_Run/I");
 ntuple->Branch("pair_Event", &pair_Event, "pair_Event/I");
 ntuple->Branch("pair_TrackID", &pair_TrackID, "pair_TrackID/I");
 ntuple->Branch("pair_ParentID", &pair_ParentID, "pair_ParentID/I");
 ntuple->Branch("pair_quantity", &pair_quantity, "pair_quantity/I");
 ntuple->Branch("pair_numPhotons", &pair_numPhotons);
 ntuple->Branch("pair_energy", &pair_energy);
 ntuple->Branch("pair_tStart", &pair_tStart);
 ntuple->Branch("pair_xStart", &pair_xStart);
 ntuple->Branch("pair_yStart", &pair_yStart);
 ntuple->Branch("pair_zStart", &pair_zStart);


 ntuple->Branch("ebrem_Run", &ebrem_Run, "ebrem_Run/I");
 ntuple->Branch("ebrem_Event", &ebrem_Event, "ebrem_Event/I");
 ntuple->Branch("ebrem_TrackID", &ebrem_TrackID, "ebrem_TrackID/I");
 ntuple->Branch("ebrem_ParentID", &ebrem_ParentID, "ebrem_ParentID/I");
 ntuple->Branch("ebrem_quantity", &ebrem_quantity, "ebrem_quantity/I");
 ntuple->Branch("ebrem_numPhotons", &ebrem_numPhotons);
 ntuple->Branch("ebrem_energy", &ebrem_energy);
 ntuple->Branch("ebrem_tStart", &ebrem_tStart);
 ntuple->Branch("ebrem_xStart", &ebrem_xStart);
 ntuple->Branch("ebrem_yStart", &ebrem_yStart);
 ntuple->Branch("ebrem_zStart", &ebrem_zStart);

 // For each entry in combinedVector... 

 for ( auto j = combinedVector.begin(); j!= combinedVector.end(); ++j)

   {
 
 // Loop over map, write each entry to output ntuple. 

     if ( *j = 0 ) {
       // primary info write-in goes here
     }

     if ( *j = 1 ) {
 for ( auto i = compt_hitMap.begin(); i!= compt_hitMap.end(); ++i )
   {
     auto key = (*i).first;
     compt_Run = std::get<0>(key);
     compt_Event = std::get<1>(key);
     compt_TrackID = std::get<2>(key);
     compt_ParentID = std::get<3>(key);

     auto &vectors = (*i).second;
     
     compt_quantity = vectors.energy.size();
     compt_numPhotons = vectors.numPhotons;
     compt_energy = vectors.energy;
     compt_tStart = vectors.tStart;
     compt_xStart = vectors.xStart;
     compt_yStart = vectors.yStart;
     compt_zStart = vectors.zStart;

    
   }
     }

     if ( *j = 2 ) {

 for ( auto i = phot_hitMap.begin(); i!= phot_hitMap.end(); ++i )
   {
     auto key = (*i).first;
     phot_Run = std::get<0>(key);
     phot_Event = std::get<1>(key);
     phot_TrackID = std::get<2>(key);
     phot_ParentID = std::get<3>(key);

     auto &vectors = (*i).second;

     phot_quantity = vectors.energy.size();
     phot_numPhotons = vectors.numPhotons;
     phot_energy = vectors.energy;
     phot_tStart = vectors.tStart;
     phot_xStart = vectors.xStart;
     phot_yStart = vectors.yStart;
     phot_zStart = vectors.zStart;


   }
     }
     if ( *j = 3 ) {
 for ( auto i = pair_hitMap.begin(); i!= pair_hitMap.end(); ++i )
   {
     auto key = (*i).first;
     pair_Run = std::get<0>(key);
     pair_Event = std::get<1>(key);
     pair_TrackID = std::get<2>(key);
     pair_ParentID = std::get<3>(key);

     auto &vectors = (*i).second;

     pair_quantity = vectors.energy.size();
     pair_numPhotons = vectors.numPhotons;
     pair_energy = vectors.energy;
     pair_tStart = vectors.tStart;
     pair_xStart = vectors.xStart;
     pair_yStart = vectors.yStart;
     pair_zStart = vectors.zStart;

     
   }
     }

     if ( *j = 4 ) {

 for ( auto i = ebrem_hitMap.begin(); i!= ebrem_hitMap.end(); ++i )
   {
     auto key = (*i).first;
     ebrem_Run = std::get<0>(key);
     ebrem_Event = std::get<1>(key);
     ebrem_TrackID = std::get<2>(key);
     ebrem_ParentID = std::get<3>(key);

     auto &vectors = (*i).second;

     ebrem_quantity = vectors.energy.size();
     ebrem_numPhotons = vectors.numPhotons;
     ebrem_energy = vectors.energy;
     ebrem_tStart = vectors.tStart;
     ebrem_xStart = vectors.xStart;
     ebrem_yStart = vectors.yStart;
     ebrem_zStart = vectors.zStart;

   }
     }

 ntuple->Fill();
}

 ntuple->Write();
 output->Close();
}
