#ifndef MYPACKAGE_MYPACKAGEALG_H
#define MYPACKAGE_MYPACKAGEALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "TH1D.h"

class MyPackageAlg: public ::AthAnalysisAlgorithm { 
 public: 
  MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MyPackageAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
      ToolHandle<DerivationFramework::IAugmentationTool> tool;
  
      TH1D* m_ll[2];

}; 

#endif //> !MYPACKAGE_MYPACKAGEALG_H
