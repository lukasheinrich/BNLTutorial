

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "POOLRootAccess/TEvent.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "TLorentzVector.h"
#include "TH1D.h"
#include "TFile.h"

#include "TApplication.h"

int main() {
  TApplication theApp("App",0,0);

  POOL::TEvent evt;
  evt.readFrom("$TestArea/evgen/my.evgen.root");

  //create an instance of the xAODTruthCnv algorithm
  //IAlgorithm* myAlg = AthAnalysisHelper::createAlgorithm("xAODMaker::xAODTruthCnvAlg/myAlg");
  //AthAnalysisHelper::setProperty( myAlg , "AODContainerName", "GEN_EVENT" );
  //myAlg->initialize();
  
  //Instead of the above ... we configure a property of the EventLoop called 'TopAlg' 
  //This will mean the algorithm will be executed automatically
  AthAnalysisHelper::setProperty( evt.evtLoop() , "TopAlg", std::vector<std::string>({"xAODMaker::xAODTruthCnvAlg/myAlg"}) );
  AthAnalysisHelper::addPropertyToCatalogue( "myAlg", "AODContainerName", "GEN_EVENT" ); //adding a property for something that doesn't exist yet

  //IAlgTool* tool = AthAnalysisHelper::createTool("DerivationFramework::TruthDressingTool/TruthDressing");
  //AthAnalysisHelper::setProperty( tool, "dressingConeSize", 0.2 );
  //tool->initialize();

  //Instead of the above ... use a ToolHandle ...
  ToolHandle<DerivationFramework::IAugmentationTool> tool("DerivationFramework::TruthDressingTool/TruthDressing");
  AthAnalysisHelper::setProperty( tool, "dressingConeSize", 0.2 );
  tool.retrieve(); //retrieve does the initialization


  //plot dilepton invariant mass
  //form all possible l+l- pairs and plot masses
  TH1D* m_ll[2];
  m_ll[0] = new TH1D("m_ll","Dilepton mass",100,0,200);
  m_ll[1] =  new TH1D("m_ll_dressed","Dressed Dilepton mass",100,0,200);

  for(int i=0;i<evt.getEntries();i++) {
    evt.getEntry(i);
    //myAlg->execute(); - don't need this, the EventLoop will execute it for us
    tool->addBranches();
    
    //retrieve the truth
    const xAOD::TruthParticleContainer* mc = 0;
    evt.retrieve( mc );

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
  }

        
  

  m_ll[1]->SetLineColor(kRed);
  m_ll[0]->Draw();m_ll[1]->Draw("same");

  theApp.Run(kTRUE); //waits
  return 0;
}
