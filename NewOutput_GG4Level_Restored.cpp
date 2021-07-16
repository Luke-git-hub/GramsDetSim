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

  typedef std::map< std::tuple< int , int , int, int >, std::string > trackMap_t;

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
				 ROOT::VecOps::RVec<char>process)
		    {
		      std::string processName;
		      for ( auto i = process.begin(); 
			    i!= process.end() && (*i) !=0; ++i){
			processName.push_back(*i);
		      }
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
		<< " process=" << value << std::endl;
    } // loop over track
  } // if debug

  // // Define a map that takes run, event, trackID and parentID and maps to a vector containing the rest of the primary information:
  // // Is this even necessary? See below... ********************************************************************************************
  // typedef std::map< std::tuple< int, int, int, int>, std::vector< std::string, //processName
  // 								  double, // energy
  // 								  double, // x
  // 								  double, // y
  // 								  double, // z 
  // 								  double, // t
  // 								  double, // px
  // 								  double, // py
  // 								  double // pz
  // 								  >> trackVectorMap_t;



  // // Create trackMapPri to hold the trackInfoPri information:
  // trackVectorMap_t trackMapPri;

  // // Create trackInfoPri, which contains the TrackInfo entries necessary for the pri_ variables
  // ROOT::RDataFrame trackInfoPri( "TrackInfo", filename);
  // trackInfoPri.Foreach(
  // [&trackMapPri]( int run, 
  // 		  int event, 
  // 		  int trackID, 
  // 		  int parentID,
  // 		  ROOT::VecOps::RVec<char> process, 
  // 		  double energy, 
  // 		  ROOT::VecOps::RVec<double> xPos, 
  // 		  ROOT::VecOps::RVec<double> yPos, 
  // 		  ROOT::VecOps::RVec<double> zPos, 
  // 		  ROOT::VecOps::RVec<double> time, 
  // 		  ROOT::VecOps::RVec<double> Px, 
  // 		  ROOT::VecOps::RVec<double> Py, 
  // 		  ROOT::VecOps::RVec<double> Pz);
  // {
  //   std::string processName;
  //   double x;
  //   double y;
  //   double z;
  //   double t;
  //   double px;
  //   double py;
  //   double px;
  //   for ( auto i = process.begin();
  // 	  i!= process.end() && (*i) != 0; ++i){
  //     processName.push_back(*i);
  //   }
  //   for ( auto i = xPos.begin();
  // 	  i! = xPos.end() && (*i) != 0; ++i){
  // 	    x.push_back(*i);
  // 	  }
  //   for ( auto i = yPos.begin();
  //         i! = yPos.end() && (*i) != 0; ++i){
  //     y.push_back(*i);
  //   }
  //   for ( auto i = zPos.begin();
  //         i! = zPos.end() && (*i) != 0; ++i){
  //     z.push_back(*i);
  //   }
  //   for ( auto i = time.begin();
  //         i! = time.end() && (*i) != 0; ++i){
  //     t.push_back(*i);
  //   }
  //   for ( auto i = Px.begin();
  //         i! = Px.end() && (*i) != 0; ++i){
  //     px.push_back(*i);
  //   }
  //   for ( auto i = Py.begin();
  //         i! = Py.end() && (*i) != 0; ++i){
  //     y.push_back(*i);
  //   }
  //   for ( auto i = Pz.begin();
  //         i! = Pz.end() && (*i) != 0; ++i){
  //     pz.push_back(*i);
  //   }

  //   trackMap[{run,event,trackID,parentID}] = (processName, energy, x, y, z, t, px, py, pz);
  // },
  // // need to figure out format here ****************************************************************************************

  //   {"Run", "Event", "TrackID", "ParentID", "ProcessName", "x", "y", "z", "t", "px", "py", "pz" }

  //);

  // Filter Primary rows from TrackInfo:
ROOT::RDataFrame trackInfoAll( "TrackInfo", filename );
 auto priInfo = trackInfoAll.Filter( 
   [&trackMap](int run, int event, int trackID)
   {
     return trackMap[{Run, Event, TrackID}] == "Primary";
   },
   {"Run", "Event", "TrackID" }
				     );

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
 [&trackMap](int run, int event, int trackID)
  {
    return trackMap[{Run, Event, TrackID}] == "ebrem";
  },
 { "Run", "Event", "TrackID"}
                              );
  
  // Define dEdx for each process: 
  // I don't think I have to name all the dxs different things but I'll clean it up later **********************************************
auto compt_dEdx = comptonHits
  .Define("compt_dx","sqrt(pow(xStart-xEnd,2) + pow(yStart-yEnd,2) + pow(zStart-zEnd,2))")
  .Define("compt_dEdx","energy / compt_dx");

auto phot_dEdx = photHits
  .Define("phot_dx","sqrt(pow(xStart-xEnd,2) + pow(yStart-yEnd,2) + pow(zStart-zEnd,2))")
  .Define("phot_dEdx","energy / phot_dx");

auto pair_dEdx = pairHits
  .Define("pair_dx","sqrt(pow(xStart-xEnd,2) + pow(yStart-yEnd,2) + pow(zStart-zEnd,2))")
  .Define("pair_dEdx","energy / pair_dx");

auto ebrem_dEdx = ebremHits
  .Define("ebrem_dx","sqrt(pow(xStart-xEnd,2) + pow(yStart-yEnd,2) + pow(zStart-zEnd,2))")
  .Define("ebrem_dEdx","energy / ebrem_dx");

// Define photon energy:

auto compt_Ephot = comptonHits
  .Define(// figure out what to write here ************************************************************************************************


// Define ionization energy:

auto compt_Eion = comptonHits
.Define("compt_Eion", "(energy / compt_dx) - compt_Ephot");

auto phot_Eion = photHits
.Define("phot_Eion", "(energy / phot_dx) - phot_Ephot");

auto pair_Eion = pairHits
.Define("pair_Eion", "(energy / pair_dx) - pair_Ephot");

auto ebrem_Eion = ebremHits
.Define("ebrem_Eion", "(energy / ebrem_dx) - ebrem_Ephot");



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

} hitInfo;

// I don't think these actually have to be vectors but I don't want to rock the boat for now -- I'll streamline it later ***********************
// trackVectors has a different structure: 

typedef struct trackVectors
{
  std::vector<double> energy;
  std::vector<double> x;
  std::vector<double> y; 
  std::vector<double> z;
  std::vector<double> t;
  std::vector<double> px;
  std::vector<double> py;
  std::vector<double> pz;
      
} trackInfoVect;

    typedef  std::map< std::tuple< int, int, int, int >, hitInfo > hitMap;
    typedef  std::map< std::tuple< int, int, int, int >, trackInfoVect > priHitMap;

 priHitMap pri_hitMap;
 hitMap compt_hitMap;
 hitMap phot_hitMap;
 hitMap pair_hitMap;
 hitMap ebrem_hitMap;

typedef std::vector< hitMap, hitMap, hitMap, hitMap, hitMap > hitMapVect;

 hitMapVect combinedVector;
 // I need to figure out some way to get the parentID in here ***************************************************************
 // Include Primary TrackInfo in pri_hitMap:
 // Need to check if I can add Ephot and Eion directly to the hitmap here in the same way as the other variables ************
 priInfo.Foreach(
    [&pri_hitMap](
     	       int run,
	       int event, 
	       int trackID, 
	       int parentID, 
	       double energy, 
	       double x, 
	       double y, 
	       double z, 
	       double t, 
	       double px, 
	       double py, 
	       double pz
		  )
    {
      auto &hit= pri_hitMap[{run, event, trackID, parentID}];

      hit.energy.push_back( energy );
      hit.x.push_back( x );
      hit.y.push_back( y );
      hit.z.push_back( z );
      hit.t.push_back( t );
      hit.px.push_back( px );
      hit.py.push_back( py );
      hit.pz.push_back( pz );

    },
    // list ntuple columns:

    {   "run", 
	"event", 
	"trackID", 
	"parentID", 
	"energy", 
	"x", 
	"y", 
	"z", 
	"t", 
	"px", 
	"py", 
	"pz"
	}
		 );

 // Include Compton-induced hits in compt_hitMap:

 comptonHits.Foreach(
    [&compt_hitMap](
	      int run, 
	      int event,
	      int trackID,
	      int parentID,
	      int numPhotons, 
	      double energy, 
	      double tStart, 
	      double xStart, 
	      double yStart, 
	      double zStart, 
	      double Eion, 
	      double Ephot 
)
     { // Copy the address of the struct. pointed to by run/event/trackID
       auto &hit = compt_hitMap[{run, event, trackID, parentID}];
       // "Define the address of hit as hitMap[{run, event, trackID, parentID}] 	    

	   
       // Append all the info in this ntuple row to the hitInfo vectors:
       hit.numPhotons.push_back( numPhotons );
       hit.energy.push_back( energy );
       hit.tStart.push_back( tStart );
       hit.xStart.push_back( xStart );
       hit.yStart.push_back( yStart );
       hit.zStart.push_back( zStart );
       hit.Eion.push_back( compt_Eion );
       hit.Ephot.push_back( compt_Ephot );
     },


     // List ntuple columns: 
    {   "run",
	"event", 
	"trackID", 
	"parentID",
        "numPhotons", 
        "energy", 
        "tStart", 
	"xStart", 
	"yStart", 
	"zStart", 
	"Eion", 
	"Ephot"
       
	 }
     );

 photHits.Foreach(
   [&phot_hitMap](
	      int run,
	      int event,
	      int trackID,  
	      int parentID, 	      
	      int numPhotons,
      	      double energy,
	      double tStart
	      double xStart,
	      double yStart,
	      double zStart,
	      double Eion, 
	      double Ephot
	      )

{ // Copy the address of the struct. pointed to by run/event/trackID
  auto &hit = phot_hitMap[{run, event, trackID, parentID}];
 // "Define the address of hit as hitMap[{run, event, trackID, parentID}]

 // Append all the info in this ntuple row to the hitInfo vectors:
  hit.numPhotons.push_back( numPhotons );
  hit.energy.push_back( energy );
  hit.tStart.push_back( tStart );
  hit.xStart.push_back( xStart );
  hit.yStart.push_back( yStart );
  hit.zStart.push_back( zStart );
  hit.Eion.push_back( phot_Eion );
  hit.Ephot.push_back( phot_Ephot ); 
},


    // List ntuple columns:
    {   "Run",
	"Event",
	"TrackID",
	"ParentID",
	"numPhotons",
	"energy",
	"tStart",
	"xStart",
	"yStart",
	"zStart",
	"Eion", 
	"Ephot"
	}
		  );

 pairHits.Foreach(
  [&pair_hitMap]( int run,
		  int event,
	     	  int trackID,
		  int parentID, 
	      	  int numPhotons,
      		  double energy,
                  double tStart
	       	  double xStart,
		  double yStart,
		  double zStart,
		  double Eion, 
		  double Ephot, 	       
   )
 
 { // Copy the address of the struct. pointed to by run/event/trackID
   auto &hit = pair_hitMap[{run, event, trackID, parentID}];
 // "Define the address of hit as hitMap[{run, event, trackID, parentID}]

 // Append all the info in this ntuple row to the hitInfo vectors:
   hit.numPhotons.push_back( numPhotons );
   hit.energy.push_back( energy );
   hit.tStart.push_back( tStart );
   hit.xStart.push_back( xStart );
   hit.yStart.push_back( yStart );
   hit.zStart.push_back( zStart );
   hit.Eion.push_back( Eion );
   hit.Ephot.push_back( Ephot );
 },


  // List ntuple columns:
  {   "Run",
      "Event",
      "TrackID",
      "ParentID",
      "numPhotons",
      "energy",
      "tStart",
      "xStart",
      "yStart",
      "zStart",
      "Eion", 
      "Ephot"
      }
		  );

 pairHits.Foreach(
  [&ebrem_hitMap](int run,
    		  int event,
      		  int trackID,
		  int parentID, 
	       	  int numPhotons,
	       	  double energy,
                  double tStart
	          double xStart,
		  double yStart,
	          double zStart,
		  double Eion, 
		  double Ephot
		  )


  { // Copy the address of the struct. pointed to by run/event/trackID
    auto &hit = ebrem_hitMap[{run, event, trackID, parentID}];

    // "Define the address of hit as hitMap[{run, event, trackID, parentID}]

    // Append all the info in this ntuple row to the hitInfo vectors:
   
    hit.numPhotons.push_back( numPhotons );
    hit.energy.push_back( energy );
    hit.tStart.push_back( tStart );
    hit.xStart.push_back( xStart );
    hit.yStart.push_back( yStart );
    hit.zStart.push_back( zStart );
    hit.Eion.push_back( Eion );
    hit.Ephot.push_back( Ephot );
  },


  // List ntuple columns:
  {   "Run",
      "Event",
      "TrackID",
      "ParentID", 
      "numPhotons",
      "energy",
      "tStart",
      "xStart",
      "yStart",
      "zStart", 
      "Eion", 
      "Ephot"
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
 // Again, don't think the pri ones need to be vectors but I will address that later
 // Also, I may change some of the variable names to make things easier
 int pri_Run;
 int pri_Event;
 int pri_TrackID;
 int pri_ParentID;
 std::vector<double> pri_energy;
 std::vector<double> pri_x;
 std::vector<double> pri_y;
 std::vector<double> pri_z;
 std::vector<double> pri_t;
 std::vector<double> pri_px;
 std::vector<double> pri_py;
 std::vector<double> pri_pz;


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
 std::vector<double> compt_Eion;
 std::vector<double> compt_Ephot;

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
 std::vector<double> phot_Eion;
 std::vector<double> phot_Ephot;

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
 std::vector<double> pair_Eion;
 std::vector<double> pair_Ephot;

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
 std::vector<double> ebrem_Eion;
 std::vector<double> ebrem_Ephot;


 // Assign each variable to its own branch:
 // Again-again, I will change the pri stuff once I get this to work and may change some variable names
 ntuple->Branch("pri_Run", &pri_Run, "pri_Run/I");
 ntuple->Branch("pri_Event", &pri_Event, "pri_Event/I");
 ntuple->Branch("pri_TrackID", &pri_TrackID, "pri_TrackID/I");
 ntuple->Branch("pri_ParentID", &pri_ParentID, "pri_ParentID/I");
 ntuple->Branch("pri_energy", &pri_energy);
 ntuple->Branch("pri_x", &pri_x);
 ntuple->Branch("pri_y", &pri_y);
 ntuple->Branch("pri_z", &pri_z);
 ntuple->Branch("pri_t", &pri_t);
 ntuple->Branch("pri_px", &pri_px);
 ntuple->Branch("pri_py", &pri_py);
 ntuple->Branch("pri_pz", &pri_pz);

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
 ntuple->Branch("compt_Eion", &compt_Eion);
 ntuple->Branch("compt_Ephot", &compt_Ephot);


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
 ntuple->Branch("phot_Eion", &phot_Eion);
 ntuple->Branch("phot_Ephot", &phot_Ephot);


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
 ntuple->Branch("pair_Eion", &pair_Eion);
 ntuple->Branch("pair_Ephot", &pair_Ephot);


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
 ntuple->Branch("ebrem_Eion", &ebrem_Eion);
 ntuple->Branch("ebrem_Ephot", &ebrem_Ephot);


 // For each entry in combinedVector... 

 for ( auto j = combinedVector.begin(); j!= combinedVector.end(); ++j)

   {
 
 // Loop over map, write each entry to output ntuple. 

     if ( *j = 0 ) {
       for ( auto i = pri_hitMap.begin(); i!= pri_hitMap.end(); ++i ) 
	 {
	   auto key = (*i).first;
	   pri_Run = std::get<0>(key);
	   pri_Event = std::get<1>(key);
	   pri_TrackID = std::get<2>(key);
	   pri_ParentID = std::get<3>(key);
       
	   auto &vectors = (*i).second;

	   pri_energy = vectors.energy;
	   pri_x = vectors.x;
	   pri_y = vectors.y;
	   pri_z = vectors.z;
	   pri_t = vectors.t;
	   pri_px = vectors.px;
	   pri_py = vectors.py;
	   pri_pz = vectors.pz;
     }
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
     compt_Eion = vectors.Eion; 
     compt_Ephot = vectors.Ephot;
    
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
     phot_Eion = vectors.Eion;
     phot_Ephot = vectors.Ephot;

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
     pair_Eion = vectors.Eion;
     pair_Ephot = vectors.Ephot;

     
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
     ebrem_Eion = vectors.Eion;
     ebrem_Ephot = vectors.Ephot;

   }
     }

 ntuple->Fill();
}

 ntuple->Write();
 output->Close();
}
