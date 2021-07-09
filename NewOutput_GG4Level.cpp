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
		    [&trackMap]( int run, 
				 int event, 
				 int trackID, 
				 int parentID, 
				 int PDGCode, 
				 ROOT::VecOps::RVec<char>process,
				 ROOT::VecOps::RVec<double>time,
				 ROOT::VecOps::RVec<double>xpos,
				 ROOT::VecOps::RVec<double>ypos,
				 ROOT::VecOps::RVec<double>zpos,
				 double Etot,
				 ROOT::VecOps::RVec<double>Px,
				 ROOT::VecOps::RVec<double>Py,
				 ROOT::VecOps::RVec<double>Pz,
				 int identifier )
		    // Check aliases ***************************************
// Check if ID int**********************************************************
		    {
		      std::string processName;
		      for ( auto i = process.begin(); 
			    i!= process.end() && (*i) !=0; ++i)
			processName.push_back(*i);
		      trackMap[{run,event,trackID}] = processName;
		    },
		    // check these vv ***************************************
		    {
		      std::double t;
		      for ( auto i = time.begin();
			    i!=time.end() && (*i) !=0; ++i)
			t.push_back(*i);
		    }
		    {
		      std::double x;
		      for ( auto i = xpos.begin();
			    i!=xpos.end() && (*i) !=0; ++i)
			x.push_back(*i);
		    }
		    {
		      std::double y;
		      for ( auto i = ypos.begin();
			   i!=ypos.end() && (*i) !=0; ++i)
			y.push_back(*i);
		    }
		    {
		      std::double z;
		      for ( auto i = zpos.begin();
			   i!=zpos.end() && (*i) !=0; ++i)
			z.push_back(*i);
		    }
		      std::double px;
		      for ( auto i = Px.begin();
			    i!=Px.end() && (*i) !=0; ++i)
		        px.push_back(*i);
		    }
                    {
		      std::double py;
		      for ( auto i = Py.begin();
			    i!=Py.end() && (*i) !=0; ++i)
			py.push_back(*i);
		    }
                    {
		      std::double pz;
		      for ( auto i = Pz.begin();
			    i!=Pz.end() && (*i) !=0; ++i)
			pz.push_back(*i);
		    }
                    {
		      std::double 
		    {"Run", "Event", "TrackID", "ParentID", "ProcessName", 
		     "t", "x", "y", "z", "Etot", "px", "py", "pz",
		     "identifier" }
		    );
  //figure out how to do this ^^ for rest of vectors************************

  if (debug) {
    for ( auto i = trackMap.begin(); i != trackMap.end(); ++i ){
      auto key = (*i).first;
      auto value = (*i).second;
      std::cout << " run=" << std::get<0>(key)
		<< " event=" << std::get<1>(key)
		<< " trackID=" << std::get<2>(key)
		<< " parentID=" << std::get<3>(key)
		<< " process=" << value 
		<< " time=" << std::get<5>(key)
		<< " xpos=" << std::get<6>(key)
		<< " ypos=" << std::get<7>(key)
		<< " zpos=" << std::get<8>(key)
		<< " Etot=" << std::get<9>(key)
		<< " Px=" << std::get<10>(key)
		<< " Py=" << std::get<11>(key)
		<< " Pz=" << std::get<12>(key)
		<< " identifier=" << std::get<13>(key)
	//check key vs value for 5-13 **************************************
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
   [&hitMap]( int run,
	      int event, 
	      int trackID, 
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
       auto &hit = hitMap[{run, event, rrackID}];
				   
       // Append all the info in this ntuple row to the hitInfo vectors:
       hit.PDGCode.push_back( PDGCode );
       hit.numPhotons.push_back( numPhotons );
       hit.energy.push_back( energy );
       hit.tStart.push_back( tStart );
       hit.xStart.push_back( xStart );
       hit.yStart.push_back( yStart );
       hit.zStart.push_back( zStart );
       hit.tEnd.push_back( tEnd );
       hit.xEnd.push_back( xEnd );
       hit.yEnd.push_back( yEnd );
       hit.zEnd.push_back( zEnd );
       hit.identifier.push_back( identifier );
     },

     // List ntuple columns: 
     {   "Run",
         "Event",
	 "TrackID",
	 "PDGCode", 
	 "numPhotons", 
	 "energy", 
	 "tStart", 
	 "xStart", 
	 "yStart", 
	 "zStart", 
	 "tEnd", 
	 "xEnd", 
	 "yEnd", 
	 "zEnd", 
	 "identifier"}
     );

 // Create the new output ntuple: 

 TFile* output = new TFile(outputfile.c_str(), "RECREATE");
 TTree* ntuple = new TTree("comptonNtuple", "Energy deposited in Compton scattering");

 // Define the variables to be accessed: 

 int Run;
 int Event; 
 int TrackID;
 std::vector<int> PDGCode;
 std::vector<int> numPhotons;
 std::vector<double> energy;
 std::vector<double> tStart;
 std::vector<double> xStart;
 std::vector<double> yStart;
 std::vector<double> zStart;
 std::vector<double> tEnd;
 std::vector<double> xEnd;
 std::vector<double> yEnd;
 std::vector<double> zEnd;
 std::vector<int> identifier;


 // Assign each variable to its own branch:

 ntuple->Branch("Run", &Run, "Run/I");
 ntuple->Branch("Event", &Event, "Event/I");
 ntuple->Branch("TrackID", &TrackID, "TrackID/I");
 ntuple->Branch("PDGCode", &PDGCode);
 ntuple->Branch("numPhotons", &numPhotons);
 ntuple->Branch("energy", &energy);
 ntuple->Branch("tStart", &tStart);
 ntuple->Branch("xStart", &xStart);
 ntuple->Branch("yStart", &yStart);
 ntuple->Branch("zStart", &zStart);
 ntuple->Branch("tEnd", &tEnd);
 ntuple->Branch("xEnd", &xEnd);
 ntuple->Branch("yEnd", &yEnd);
 ntuple->Branch("zEnd", &zEnd);
 ntuple->Branch("identifier", &identifier);

 // Loop over map, write each entry to output ntuple. 

 for ( auto i = hitMap.begin(); i!= hitMap.end(); ++i )
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
