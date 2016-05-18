// MyPackage includes
#include "MyPackageAlg.h"

//#include "xAODEventInfo/EventInfo.h"

//uncomment the line below to use the HistSvc for outputting trees and histograms
#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"
#include "xAODTruth/TruthParticleContainer.h"


MyPackageAlg::MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty ); //example property declaration

}


MyPackageAlg::~MyPackageAlg() {}


StatusCode MyPackageAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //
  tool.setTypeAndName("DerivationFramework::TruthDressingTool/TruthDressing");
  AthAnalysisHelper::setProperty( tool, "dressingConeSize", 0.2 );
  CHECK( tool.retrieve() );



  m_ll[0] = new TH1D("m_ll","Dilepton mass",100,0,200);
  m_ll[1] =  new TH1D("m_ll_dressed","Dressed Dilepton mass",100,0,200);


  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc->regHist("m_ll", m_ll[0]) );
  CHECK( histSvc->regHist("m_ll_dressed", m_ll[1]) );
  

  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //

  m_ll[1]->SetLineColor(kRed);

  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed



  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //


  //HERE IS AN EXAMPLE
  //const xAOD::EventInfo* evtInfo = 0;
  //CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  //ATH_MSG_INFO("eventNumber=" << evtInfo->eventNumber() );


    CHECK( tool->addBranches() );
    
    //retrieve the truth
    const xAOD::TruthParticleContainer* mc = 0;
    CHECK( evtStore()->retrieve( mc ) );

    std::vector<TLorentzVector> elplus[2];
    std::vector<TLorentzVector> muplus[2];
    std::vector<TLorentzVector> elminus[2];
    std::vector<TLorentzVector> muminus[2];
    
    //loop over truth particles
    for(auto itr = mc->begin(); itr != mc->end(); ++itr) {
      if((*itr)->status()!=1) continue; //must be stable
      if(fabs((*itr)->pdgId()) != 11 && (fabs((*itr)->pdgId()) != 13)) continue; //must be a lepton

      TLorentzVector v((*itr)->p4());
      TLorentzVector v2;v2.SetPtEtaPhiE((*itr)->auxdata<float>("pt_dressed"),(*itr)->auxdata<float>("eta_dressed"),(*itr)->auxdata<float>("phi_dressed"),(*itr)->auxdata<float>("e_dressed"));

      switch( (*itr)->pdgId() ) {
        case 11: elminus[0].push_back(v);elminus[1].push_back(v2);break;
        case -11: elplus[0].push_back(v);elplus[1].push_back(v2);break;
        case 13: muminus[0].push_back(v);muminus[1].push_back(v2);break;
        case -13: muplus[0].push_back(v);muplus[1].push_back(v2);break;
      }
    }
    //form dilepton masses from available leptons
    for(int i=0;i<2;i++) {
      for(auto& elm : elminus[i]) {for(auto& elp : elplus[i]) {m_ll[i]->Fill((elm+elp).M()/1000.);}}
      for(auto& mum : muminus[i]) {for(auto& mup : muplus[i]) {m_ll[i]->Fill((mum+mup).M()/1000.);}}
    }
  



  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


