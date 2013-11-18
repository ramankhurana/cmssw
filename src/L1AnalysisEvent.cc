#include "L1TriggerDPG/L1Ntuples/interface/L1AnalysisEvent.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
#include <sys/stat.h>

L1Analysis::L1AnalysisEvent::L1AnalysisEvent(std::string puMCFile, 
					     std::string puMCHist, 
					     std::string puDataFile, 
					     std::string puDataHist) :
  fillHLT_(true),
  doPUWeights_(false),
  lumiWeights_()
{
 
  // check PU files exists, and reweight if they do
  struct stat buf;
  if ((stat(puMCFile.c_str(), &buf) != -1) && (stat(puDataFile.c_str(), &buf) != -1)) {
    lumiWeights_ = edm::LumiReWeighting(puMCFile,
					puDataFile,
					puMCHist,
					puDataHist);
    doPUWeights_ = true;
  }
  else {
    edm::LogWarning("L1Prompt") << "No PU reweighting inputs - not going to calculate weights"<<std::endl;
  }


}

L1Analysis::L1AnalysisEvent::~L1AnalysisEvent()
{

}

void L1Analysis::L1AnalysisEvent::Set(const edm::Event& e, const edm::InputTag& hlt_)
{

  event_.run = e.id().run();
  event_.event = e.id().event();
  event_.time = e.time().value();
  event_.bx = e.bunchCrossing();   //overwritten by EVM info until fixed by fw
  event_.lumi = e.luminosityBlock();
  event_.orbit = e.orbitNumber();   //overwritten by EVM info until fixed by fw

  
  if (hlt_.label() != "none"){
    edm::Handle<edm::TriggerResults> hltresults;
    e.getByLabel(hlt_,hltresults);
    const edm::TriggerNames TrigNames_ = e.triggerNames(*hltresults);
    const int ntrigs = hltresults->size();
    
    for (int itr=0; itr<ntrigs; itr++){
      TString trigName=TrigNames_.triggerName(itr);
      if (!hltresults->accept(itr)) continue;
      event_.hlt.push_back(trigName);
    }
  }

  // do PU re-weighting for MC only
  double weight = 1.;

  if (doPUWeights_ && (! e.eventAuxiliary().isRealData())) {

    edm::Handle<std::vector< PileupSummaryInfo > >  puInfo;
    e.getByLabel(edm::InputTag("addPileupInfo"), puInfo);
    
    if (puInfo.isValid()) {
      std::vector<PileupSummaryInfo>::const_iterator pvi;
      
      float tnpv = -1;
      for(pvi = puInfo->begin(); pvi != puInfo->end(); ++pvi) {
	
	int bx = pvi->getBunchCrossing();
	
	if(bx == 0) { 
	  tnpv = pvi->getTrueNumInteractions();
	  continue;
	}
	
      }
      
      weight = lumiWeights_.weight( tnpv );
      
    }

  }

  event_.puWeight = weight;




}




