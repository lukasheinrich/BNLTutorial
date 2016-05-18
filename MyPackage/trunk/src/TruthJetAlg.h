#ifndef MYPACKAGE_TRUTHJETALG_H
#define MYPACKAGE_TRUTHJETALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!


#include "AsgTools/IAsgTool.h"
#include "JetInterface/IJetExecuteTool.h"

//This example requires: ParticleJetTools-00-03-41 or higher

class TruthJetAlg: public ::AthAnalysisAlgorithm { 
 public: 
  TruthJetAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TruthJetAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  bool m_doCppConfig=false;
  ToolHandle<asg::IAsgTool> m_classifier; //just use IAsgTool interface if all we do is set properties
  ToolHandle<IJetExecuteTool> m_selectorTool;
  ToolHandle<asg::IAsgTool> m_pseudoGetter;
  ToolHandle<asg::IAsgTool> m_jetBuilder;
  ToolHandle<asg::IAsgTool> m_jetFinder;
  ToolHandle<IJetExecuteTool> m_jetRecTool;

}; 

#endif //> !MYPACKAGE_TRUTHJETALG_H
