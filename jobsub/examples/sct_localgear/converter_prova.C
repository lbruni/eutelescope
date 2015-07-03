ifndef __CINT__ 

// lcio includes <.h>
//#include <LCIOTypes.h>
#include "IO/LCReader.h"
//#include <IO/LCWriter.h>
#include "IOIMPL/LCFactory.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCCollection.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/TrackImpl.h"
#include "IMPL/TrackerHitImpl.h"
//#include "IMPL/TrackerHitVecImpl.h"
#include "EVENT/LCEvent.h"
#include "UTIL/LCTOOLS.h"

#include "IMPL/LCTOOLS.h"
#include "EVENT/LCRunHeader.h"

#include "TString.h"
#include <vector>
#include <iostream>
#endif


/** Example script for testing the ROOT LCIO dictionary.
 * 
 *  jkConvert: 
 *  reads *.slcio file and writes track information
 *
 */
 
void converter_prova(const char* RunNum) {
  

  if (!TClassTable::GetDict("IMPL::ReconstructedParticleImpl")) {
    unsigned res ;
    
    res = gSystem->Load("/afs/cern.ch/user/l/lbruni/Eutelescope/v01-17-05/lcio/v02-04-03/lib/liblcio.so");
    res = gSystem->Load("/afs/cern.ch/user/l/lbruni/Eutelescope/v01-17-05/lcio/v02-04-03/lib/liblcioDict.so");
  }
  
  
  //--- create a ROOT file

  TFile* file = new TFile("Telescope00"+TString(RunNum)+".root","RECREATE"); //jk note suffix

  TTree* tree = new TTree( "Tracks" , "Tracks");
  
  int ntracks;
  //vector<int> miss;
  vector<double> x;
  vector<double> resid_y4;
  vector<double> resid_y5;

  TBranch* b_ntracks = tree->Branch("ntracks",&ntracks,"ntracks/I");
  TBranch* b_chi2 = tree->Branch("chi2", &chi2);
  TBranch* b_DoF = tree->Branch("DoF", &DoF);
  //  TBranch* b_miss = tree->Branch("miss", &miss);

  TBranch* b_x_0 = tree->Branch("x_0", &x_0);
  TBranch* b_x_1 = tree->Branch("x_1", &x_1);
  TBranch* b_x_2 = tree->Branch("x_2", &x_2);
  TBranch* b_x_3 = tree->Branch("x_3", &x_3);
  TBranch* b_x_4 = tree->Branch("x_4", &x_4);
  TBranch* b_x_5 = tree->Branch("x_5", &x_5);
  TBranch* b_x_8 = tree->Branch("x_8", &x_8);
  

  TBranch* b_y_0 = tree->Branch("y_0", &y_0);
  TBranch* b_y_1 = tree->Branch("y_1", &y_1);
  TBranch* b_y_2 = tree->Branch("y_2", &y_2);
  TBranch* b_y_3 = tree->Branch("y_3", &y_3);
  TBranch* b_y_4 = tree->Branch("y_4", &y_4);
  TBranch* b_y_5 = tree->Branch("y_5", &y_5);
    TBranch* b_y_8 = tree->Branch("y_8", &y_8);

  TBranch* b_resid_x0 = tree->Branch("resid_x0", &resid_x0);
  TBranch* b_resid_x1 = tree->Branch("resid_x1", &resid_x1);
  TBranch* b_resid_x2 = tree->Branch("resid_x2", &resid_x2);
  TBranch* b_resid_x3 = tree->Branch("resid_x3", &resid_x3);
  TBranch* b_resid_x4 = tree->Branch("resid_x4", &resid_x4);
  TBranch* b_resid_x5 = tree->Branch("resid_x5", &resid_x5);
  TBranch* b_resid_y0 = tree->Branch("resid_y0", &resid_y0);
  TBranch* b_resid_y1 = tree->Branch("resid_y1", &resid_y1);
  TBranch* b_resid_y2 = tree->Branch("resid_y2", &resid_y2);
  TBranch* b_resid_y3 = tree->Branch("resid_y3", &resid_y3);
  TBranch* b_resid_y4 = tree->Branch("resid_y4", &resid_y4);
  TBranch* b_resid_y5 = tree->Branch("resid_y5", &resid_y5);

  std::cout << " loaded LCIO library and dictionary ... " << std::endl ;
  
  int nEvents = 0  ;
  int maxEvt = 1000000 ;

  IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader() ;

  stringstream ss;
  ss << "output/lcio/run00"<< RunNum <<  "-missingcoordinate.slcio"; //jk: first/last4
  string stri = ss.str();
  char* cha = stri.c_str();

  lcReader->open( cha ) ;
  

  IMPL::LCCollectionVec emptyCol ;
  
  //----------- the event loop -----------
  while( (evt = lcReader->readNextEvent()) != 0  && nEvents < maxEvt ) {
    
    //UTIL::LCTOOLS::dumpEvent( evt ) ;
    if(nEvents % 100 == 0 || (nEvents % 10 == 0 && nEvents < 100) || nEvents < 10) cout<<"jk: on event "<<nEvents<<endl;
    nEvents ++ ;

    ntracks = 0;
    chi2.clear();
    DoF.clear();
    x_0.clear();
    x_1.clear();
    x_2.clear();
    x_3.clear();
    x_4.clear();
    x_5.clear();
    x_8.clear();
    
    y_0.clear();
    y_1.clear();
    y_2.clear();
    y_3.clear();
    y_4.clear();
    y_5.clear();
    y_8.clear();
  
    // miss.clear();

    resid_x0.clear();
    resid_x1.clear();
    resid_x2.clear();
    resid_x3.clear();
    resid_x4.clear();
    resid_x5.clear();
    resid_y0.clear();
    resid_y1.clear();
    resid_y2.clear();
    resid_y3.clear();
    resid_y4.clear();
    resid_y5.clear();

    std::vector<std::string>* ColNames = evt->getCollectionNames();

    //necessary because it crashes if there is no "track0"
    for(int y = 0; y != ColNames->size(); y++){
      if(ColNames->at(y) == "local_hit_new"){

	IMPL::LCCollectionVec* tracks = (IMPL::LCCollectionVec*) evt->getCollection("local_hit_new");
	
	ntracks = tracks->getNumberOfElements();
	
	//loop over tracks
	for(int i = 0; i < ntracks; i++){

	  IMPL::TrackImpl* tr = dynamic_cast<IMPL::TrackImpl*>( tracks->getElementAt( i ) );
	  chi2.push_back( (double)tr->getChi2() );
	  DoF.push_back( (double)tr->getNdf() );

	  EVENT::TrackerHitVec* hits = (EVENT::TrackerHitVec*)tr->getTrackerHits();
	  
	  int numhits = hits->size();

	  int count0 = 0;
	  int count1 = 0;
	  int count2 = 0;
	  int count3 = 0;
	  int count4 = 0;
	  int count5 = 0;
	  int count8 = 0;


	  for(int h = 0; h < numhits; h++){

	    IMPL::TrackerHitImpl* hit = dynamic_cast<IMPL::TrackerHitImpl*>( hits->at( h ) );
	    //cout<<"jk: x, y z of hit "<<h<<" = "<< hit->getPosition()[0]<<" "<< hit->getPosition()[1] <<" "<< hit->getPosition()[2] <<endl;

	    if( hit->getPosition()[2] == 0) count0++;
	    if( hit->getPosition()[2] == 150) count1++;
	    if( hit->getPosition()[2] == 300) count2++;
	    if( hit->getPosition()[2] == 127.45) count8++;
	    if( hit->getPosition()[2] == 450) count4++;
	    if( hit->getPosition()[2] == 600) count5++;
        if( hit->getPosition()[2] == 750) count5++;
	    //Positions on the track
	    if( hit->getPosition()[2] == 0 && count0 == 1 ){
	      x_0.push_back( hit->getPosition()[0] );
	      y_0.push_back( hit->getPosition()[1] );
	    }

	    if( hit->getPosition()[2] == 150 && count1 == 1 ){
	      x_1.push_back( hit->getPosition()[0] );
	      y_1.push_back( hit->getPosition()[1] );
	    }

	    if( hit->getPosition()[2] == 300 && count2 == 1 ){
	      x_2.push_back( hit->getPosition()[0] );
	      y_2.push_back( hit->getPosition()[1] );
	    }
          if( hit->getPosition()[2] == 127.45 && count8 == 1 ){
              x_8.push_back( hit->getPosition()[0] );
              y_8.push_back( hit->getPosition()[1] );
          }
	    if( hit->getPosition()[2] == 450 && count3 == 1 ){
	      x_3.push_back( hit->getPosition()[0] );
	      y_3.push_back( hit->getPosition()[1] );
	    }

	    if( hit->getPosition()[2] == 600 && count4 == 1 ){
	      x_4.push_back( hit->getPosition()[0] );
	      y_4.push_back( hit->getPosition()[1] );
	    }

	    if( hit->getPosition()[2] == 750 && count5 == 1 ){
	      x_5.push_back( hit->getPosition()[0] );
	      y_5.push_back( hit->getPosition()[1] );
	    }


	    //Positions of the hits themselves
	    if( hit->getPosition()[2] == 0 && count0 == 2 ){
	      resid_x0.push_back( hit->getPosition()[0] - x_0.at(i) );
	      resid_y0.push_back( hit->getPosition()[1] - y_0.at(i) );
	    }

	    if( hit->getPosition()[2] == 150 && count1 == 2 ){
	      resid_x1.push_back( hit->getPosition()[0] - x_1.at(i) );
	      resid_y1.push_back( hit->getPosition()[1] - y_1.at(i) );
	    }

	    if( hit->getPosition()[2] == 300 && count2 == 2 ){
	      resid_x2.push_back( hit->getPosition()[0] - x_2.at(i) );
	      resid_y2.push_back( hit->getPosition()[1] - y_2.at(i) );
	    }

	    if( hit->getPosition()[2] == 450 && count3 == 2 ){
	      resid_x3.push_back( hit->getPosition()[0] - x_3.at(i) );
	      resid_y3.push_back( hit->getPosition()[1] - y_3.at(i) );
	    }

	    if( hit->getPosition()[2] == 600 && count4 == 2 ){
	      resid_x4.push_back( hit->getPosition()[0] - x_4.at(i) );
	      resid_y4.push_back( hit->getPosition()[1] - y_4.at(i) );
	    }

	    if( hit->getPosition()[2] == 750 && count5 == 2 ){
	      resid_x5.push_back( hit->getPosition()[0] - x_5.at(i) );
	      resid_y5.push_back( hit->getPosition()[1] - y_5.at(i) );
	    }

	  }


	  //Calculate track position as it passes the DUT's
	/*  double xFCF = x_2.at(i)*(235./330.) + x_3.at(i)*(95./330.);
	  double yFCF = y_2.at(i)*(235./330.) + y_3.at(i)*(95./330.);
	  x_FCF.push_back( xFCF );
	  y_FCF.push_back( yFCF );

	  double xABC = x_2.at(i)*(66./330.) + x_3.at(i)*(264./330.);
	  double yABC = y_2.at(i)*(66./330.) + y_3.at(i)*(264./330.);
	  x_ABC.push_back( xABC );
	  y_ABC.push_back( yABC );


	  //If only requiring 5 hits, find out which was the missing one
	  
	  if(count0 == 1) miss.push_back(0);
	  else if(count1 == 1) miss.push_back(1);
	  else if(count2 == 1) miss.push_back(2);
	  else if(count8 == 1) miss.push_back(3);
	  else if(count3 == 1) miss.push_back(4);
	  else if(count4 == 1) miss.push_back(5);
      else if(count5 == 1) miss.push_back(6);

	  else { miss.push_back(-1); }	    */
	    }
		
	break;
      }

    }

    tree->Fill();

  }
  // -------- end of event loop -----------

  file->Write() ;
  file->Close() ;
  
  delete file ;


  std::cout << std::endl 
	    <<  "  " <<  nEvents 
	    << " events read from run #: " 
	    << RunNum << std::endl  ;
  
  
  lcReader->close() ;
  
  delete lcReader ;
}
