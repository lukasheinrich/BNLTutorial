// MyPackage includes
#include "TruthJetAlg.h"



TruthJetAlg::TruthJetAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  declareProperty( "DoCppConfig", m_doCppConfig=false, "Set to True to do Config in initialize method" );

}


TruthJetAlg::~TruthJetAlg() {}


StatusCode TruthJetAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if(m_doCppConfig) {
    m_classifier.setTypeAndName("MCTruthClassifier/JetMCTruthClassifier");
  
    m_selectorTool.setTypeAndName("CopyTruthJetParticles/TruthParticleSelector");
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "MCTruthClassifier", m_classifier ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "OutputName", "SelectedTruthParticles" ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "IncludeNeutrinos", false ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "IncludeMuons", false ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "IncludeWZLeptons", true ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "IncludeTauLeptons", true ) );
    CHECK( AthAnalysisHelper::setProperty( m_selectorTool, "MaxAbsEta", 5.0 ) );
    CHECK( m_selectorTool.retrieve() );
  
    m_pseudoGetter.setTypeAndName("PseudoJetGetter/truthget");
    CHECK( AthAnalysisHelper::setProperty( m_pseudoGetter, "Label", "Truth" ) );
    CHECK( AthAnalysisHelper::setProperty( m_pseudoGetter, "InputContainer", "SelectedTruthParticles" ) );
    CHECK( AthAnalysisHelper::setProperty( m_pseudoGetter, "OutputContainer", "PseudoJetTruth" ) );
    CHECK( AthAnalysisHelper::setProperty( m_pseudoGetter, "GhostScale", 0.0 ) );
    CHECK( AthAnalysisHelper::setProperty( m_pseudoGetter, "SkipNegativeEnergy", true ) );
    CHECK( m_pseudoGetter.retrieve() );
  
    //m_jetBuilder.setTypeAndName("JetFromPseudojet/jetbuild"); ... not actually needed
  
    m_jetFinder.setTypeAndName("JetFinder/jetfind");
    CHECK( AthAnalysisHelper::setProperty( m_jetFinder, "JetAlgorithm", "AntiKt" ) );
    CHECK( AthAnalysisHelper::setProperty( m_jetFinder, "JetRadius", 0.4 ) );
    CHECK( AthAnalysisHelper::setProperty( m_jetFinder, "PtMin", 5000. ) );
    CHECK( AthAnalysisHelper::setProperty( m_jetFinder, "JetBuilder", "JetFromPseudojet/jetbuild" /*m_jetFinder.typeAndName()*/ ) );
    CHECK( m_jetFinder.retrieve() );
  
  
    ToolHandleArray<asg::IAsgTool> getters; getters.push_back(m_pseudoGetter);
  
    m_jetRecTool.setTypeAndName("JetRecTool/MyJetRecTool");
    CHECK( AthAnalysisHelper::setProperty( m_jetRecTool, "OutputContainer", "MyTruthJets" ) );
    CHECK( AthAnalysisHelper::setProperty( m_jetRecTool, "PseudoJetGetters", getters ) );
    CHECK( AthAnalysisHelper::setProperty( m_jetRecTool, "JetFinder", m_jetFinder ) );
    CHECK( m_jetRecTool.retrieve() );
  } else {
    //just need to retrieve the executable tools 
    m_selectorTool.setTypeAndName("CopyTruthJetParticles/TruthParticleSelector");
    m_jetRecTool.setTypeAndName("JetRecTool/MyJetRecTool");
    CHECK( m_selectorTool.retrieve() );
    CHECK( m_jetRecTool.retrieve() );
  }


  return StatusCode::SUCCESS;
}

StatusCode TruthJetAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TruthJetAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //execute the tools
  m_selectorTool->execute();
  m_jetRecTool->execute();

  //dump the storegate to check our collection of jets called 'MyTruthJets' is there
  std::cout << evtStore()->dump() << std::endl;



  return StatusCode::SUCCESS;
}


