#ifndef EUTelOutputTTree_h__
#define EUTelOutputTTree_h__


#include "marlin/Processor.h"

// system includes <>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>

class output;
namespace eutelescope {


  class EUTelOutputTTree : public  marlin::Processor{

  public:
    virtual Processor*  newProcessor() { return new EUTelOutputTTree; }

    EUTelOutputTTree();
    virtual void init();
    virtual void processRunHeader(LCRunHeader* run);
    virtual void processEvent(LCEvent * evt);
    virtual void check(LCEvent * /*evt*/){ ; };
    virtual void end();

  protected:
    //TbTrack additions
    void prepareTree();
    void clear();

    bool readZsHits(std::string colName, LCEvent* event);
    bool readTracks(LCEvent* event);
    bool readHits(std::string hitColName, LCEvent* event);
    bool first;
    typedef std::map<std::string, output*> output_map_t;
    output_map_t m_out;
    TTree *m_tree;
    TFile *m_file;
    std::string path;
  private:




  };
}
#endif // EUTelOutputTTree_h__