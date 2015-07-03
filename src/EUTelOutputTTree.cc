// eutelescope inlcudes
#include "EUTELESCOPE.h"
#include "EUTelEventImpl.h"
#include "EUTelExceptions.h"
#include "EUTelRunHeaderImpl.h"
#include "EUTelTrackerDataInterfacerImpl.h"

// eutelescope geometry
#include "EUTelGeometryTelescopeGeoDescription.h"
#include "EUTelGenericPixGeoDescr.h"

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/TrackImpl.h>
#include <UTIL/CellIDDecoder.h>

#include <algorithm>
#include "marlin/Processor.h"
#include <iostream>
#include "EUTelOutputTTree.h"
#include "EVENT/TrackerPulse.h"

using namespace eutelescope;
using namespace std;


TFile *gFile_ = NULL;
std::string gStupitNameForShittyROOTFile;
void print_vec(const std::vector<double>& vec, std::ostream& out){

  for (size_t i = 0; i < vec.size(); ++i)
  {
    out << vec[i];
    if (i != vec.size())
    {
      out << ", ";
    }
  }

}

class output{
public:
  virtual void pushCollection(const EVENT::LCCollection* col)=0;

  virtual void newEvent(int eventNR){}
  virtual void FinnishEvent() {};
  virtual void eventEnd(){
    
    FinnishEvent();
    if (m_tree){
      m_tree->Fill();
    }
    
  }

  virtual void eventStart(int eventNR) {
    m_event_nr = eventNR;
    m_id.clear();
    m_x.clear();
    m_y.clear();
    newEvent(eventNR);
  };
  output(const std::string& name, const std::string& type) :m_event_nr(0), m_name(name), m_type(type) {
    if (!gFile_)
    {
      std::cout << "create file with name: " << gStupitNameForShittyROOTFile<< std::endl;
      gFile_ = new TFile(gStupitNameForShittyROOTFile.c_str(), "RECREATE");
    }

    m_tree = new TTree(name.c_str(), name.c_str());
    m_tree->Branch("ID", &m_id);
    m_tree->Branch("x", &m_x);
    m_tree->Branch("y", &m_y);
    m_tree->Branch("event_nr", &m_event_nr);
  }
  virtual ~output(){
    if (m_tree)
    {
      m_tree->Write();
    }
  }
  int m_event_nr;
  TTree *m_tree;
  std::string m_name, m_type;
  std::vector<double> m_x, m_y, m_id;
};


class Tracks_output : public output{
public:
  typedef const EVENT::Track* data_t;
  static const char* TypeName(){
    return LCIO::TRACK;
  }
  Tracks_output(const std::string& name, const std::string& type) :output(name, type), warning(true) {}
  virtual void pushCollection(const EVENT::LCCollection* col) {

    if (warning)
    {
      std::cout << "not implemented type: " << TypeName() << std::endl;
      warning = false;
    }
  };
  bool warning;
};

class LCGenericObject_output : public output{
public:
  typedef const void* data_t;
  static const char* TypeName(){
    return "LCGenericObject";
  }
  LCGenericObject_output(const std::string& name, const std::string& type) :output(name, type), warning(true) {}
  virtual void pushCollection(const EVENT::LCCollection* col) {

    if (warning)
    {
      std::cout << "not implemented type: " << TypeName() << std::endl;
      warning = false;
    }
  };
  
  bool warning;
};
class Track_output :public output{
public:
  typedef const EVENT::Track data_t;
  static const char* TypeName(){
    return LCIO::TRACK;
  }
  virtual void newEvent(int eventNR){

    m_z.clear();
    m_D0.clear();
    m_phi.clear();
    m_omega.clear();
  }
  Track_output(const std::string& name, const std::string& type) :output(name, type) {
    m_tree->Branch("z", &m_z);
    m_tree->Branch("D0", &m_D0);
    m_tree->Branch("phi", &m_phi);
    m_tree->Branch("omega", &m_omega);

  }
  virtual void pushCollection(const EVENT::LCCollection* col) {

    for (int i = 0; i < col->getNumberOfElements(); ++i)
    {

      data_t*  trk = dynamic_cast<data_t*>(col->getElementAt(i));

      if (!trk)
      {
        std::cout << "unable to cast the pointer in " << m_type << ": " << m_name << std::endl;
        return;
      }

      m_id.push_back(0);
      m_x.push_back(trk->getReferencePoint()[0]);
      m_y.push_back(trk->getReferencePoint()[1]);
      m_z.push_back(trk->getReferencePoint()[2]);
      m_D0.push_back(trk->getD0());
      m_phi.push_back(trk->getPhi());
      m_omega.push_back(trk->getOmega());
    }
  }

  std::vector<double> m_z, m_D0, m_phi, m_omega;
};
class TrackerData_output :public output{
public:
  typedef const EVENT::TrackerData data_t;
  static const char* TypeName(){
    return LCIO::TRACKERDATA;
  }
  virtual void pushCollection(const EVENT::LCCollection* col) {
  
    for (size_t i = 0; i < col->getNumberOfElements(); ++i){



      data_t* hit = dynamic_cast<data_t*> (col->getElementAt(i));
      if (!hit)
      {
        std::cout << "unable to cast the pointer in " << m_type << ": " << m_name << std::endl;
        return;
      }

      for (size_t i = 0; i < hit->getChargeValues().size() / 4; i++)
      {
        UTIL::CellIDDecoder<data_t> hitDecoder(col);
        int sensorID = hitDecoder(hit)["sensorID"];
        m_id.push_back(sensorID);
        m_x.push_back(hit->getChargeValues().at(i * 4));
        m_y.push_back(hit->getChargeValues().at(i * 4 + 1));
      }

    }
  }

  TrackerData_output(const std::string& name, const std::string& type) :output(name,type){ }

 
};

class TrackerPulse_Output :public output{
public:
  typedef const EVENT::TrackerPulse data_t;
  static const char* TypeName(){
    return LCIO::TRACKERPULSE;
  }

  virtual void pushCollection(const EVENT::LCCollection* col){

    for (size_t i = 0; i < col->getNumberOfElements(); ++i){



      data_t* hit = dynamic_cast<data_t*> (col->getElementAt(i));
      if (!hit)
      {
        std::cout << "unable to cast the pointer" << std::endl;
        return;
      }

      for (size_t i = 0; i < hit->getTrackerData()->getChargeValues().size() / 4; i++)
      {
        UTIL::CellIDDecoder<data_t> hitDecoder(col);
        int sensorID = hitDecoder(hit)["sensorID"];
        m_id.push_back(sensorID);
        m_x.push_back(hit->getTrackerData()->getChargeValues().at(i * 4));
        m_y.push_back(hit->getTrackerData()->getChargeValues().at(i * 4 + 1));
      }

    }
  }
  TrackerPulse_Output(const std::string& name, const std::string& type):output(name,type){
  }
  virtual ~TrackerPulse_Output(){}

};

class TrackerHit_output :public output{
public:
  typedef const EVENT::TrackerHit data_t;
  static const char* TypeName(){
    return LCIO::TRACKERHIT;
  }

  virtual void pushCollection(const EVENT::LCCollection* col){

    data_t* hit = NULL;
    for (size_t i = 0; i < col->getNumberOfElements(); ++i){

      hit = dynamic_cast<data_t*>(col->getElementAt(i));
      if (!hit){
        std::cout << "unable to cast the pointer" << std::endl;
        continue;

      }
      UTIL::CellIDDecoder<data_t> hitDecoder(col);
      int sensorID = hitDecoder(hit)["sensorID"];
      m_id.push_back(sensorID);
      m_x.push_back(hit->getPosition()[0]);
      m_y.push_back(hit->getPosition()[1]);

    }
  }

  TrackerHit_output(const std::string& name, const std::string& type) :output(name, type){

  }
};
output* createOutput(const std::string& name, const std::string& type){
  if (type == Track_output::TypeName()){
    return new Track_output(name, type);
  }

  if (type == TrackerPulse_Output::TypeName()){

    return new TrackerPulse_Output(name, type);
  }
  if (type == TrackerData_output::TypeName())
  {
    return new  TrackerData_output(name, type);
  }
  if (type==TrackerHit_output::TypeName())
  {
    return new TrackerHit_output(name, type);
  }
  if (type==LCGenericObject_output::TypeName())
  {
    return new LCGenericObject_output(name, type);
  }
  if (type == TrackerHit_output::TypeName())
  {
    return new TrackerHit_output(name, type);
  }

  std::cout << "unsupported type  " << type << std::endl;
  return NULL;
}


EUTelOutputTTree::EUTelOutputTTree() :Processor("EUTelOutputTTree"), m_file(NULL), m_tree(NULL){

  
  registerProcessorParameter("OutputPath", "Path/File where root-file should be stored",
    path, std::string("NTuple.root"));
}
void EUTelOutputTTree::init()
{
 
  std::string name("test.root");
  geo::gGeometry().initializeTGeoDescription(name, true);
  first = true;






  




  gStupitNameForShittyROOTFile = path;




  std::cout << "EUTelOutputTTree::init" << std::endl;
}

void EUTelOutputTTree::processRunHeader(LCRunHeader* run)
{

  std::cout << "EUTelOutputTTree::processRunHeader" << std::endl;
}

void EUTelOutputTTree::processEvent(LCEvent * evt)
{
  const std::vector<std::string>*  names = evt->getCollectionNames();
  if (first)
  {
  //  std::cout << "<event id=\"" << evt->getEventNumber() << "\">\n";
    std::vector< std::string >::const_iterator name;
    for (name = names->begin(); name != names->end(); name++){

      LCCollection* col = evt->getCollection(*name);
      if (!col)
      {
        continue;
      }
      output_map_t::const_iterator  it = m_out.find(*name);
      if (it == m_out.end()){
        m_out[*name] = createOutput(*name, col->getTypeName());

      }
      m_out[*name]->eventStart(evt->getEventNumber());

      m_out[*name]->pushCollection(col);

    }

   // first = false;
    for (output_map_t::const_iterator it = m_out.begin(); it != m_out.end(); ++it){
      it->second->eventEnd();
    }
  //  std::cout << "</event>\n";
  }
}

void EUTelOutputTTree::end()
{
  for (output_map_t::const_iterator it = m_out.begin(); it != m_out.end(); ++it){
    delete it->second;
  }
  if (gFile_)
  {
    std::cout << "closing  file " << std::endl;
    gFile_->Write();
    gFile_->Close();
    delete gFile_;
    gFile_ = NULL;
  }
  std::cout << "EUTelOutputTTree::end" << std::endl;
}


EUTelOutputTTree gEUTelOutputTTree;