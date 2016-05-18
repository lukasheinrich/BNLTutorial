{
  POOL::TEvent evt;
  evt.readFrom("$TestArea/evgen/my.evgen.root");

  //create an instance of the xAODTruthCnv algorithm
  IAlgorithm* myAlg = AthAnalysisHelper::createAlgorithm("xAODMaker::xAODTruthCnvAlg/myAlg");
  AthAnalysisHelper::setProperty( myAlg , "AODContainerName", "GEN_EVENT" );
  myAlg->initialize();

  IAlgTool* tool = AthAnalysisHelper::createTool("DerivationFramework::TruthDressingTool/TruthDressing");
  tool->initialize();


  //plot dilepton invariant mass
  //form all possible l+l- pairs and plot masses
  TH1D* m_ll[2];
  m_ll[0] = new TH1D("m_ll","Dilepton mass",100,0,200);
  m_ll[1] =  new TH1D("m_ll_dressed","Dressed Dilepton mass",100,0,200);

  for(int i=0;i<evt.getEntries();i++) {
    evt.getEntry(i);
    myAlg->execute(); //executes the algorithm to create the xAOD objects
    dynamic_cast<DerivationFramework::IAugmentationTool*>(tool)->addBranches(); //main method of tool to do truth dressing
    

    //retrieve the truth
    const xAOD::TruthParticleContainer_v1* mc = 0; //sad that we cannot use unversioned
    evt.retrieve( mc );

    const McEventCollection* dummy = 0; //without this, ROOT prompt doesn't load libraries properly??


    
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
      for(auto& elm : elminus[i]) for(auto& elp : elplus[i])	m_ll[i]->Fill((elm+elp).M()/1000.);
      for(auto& mum : muminus[i]) for(auto& mup : muplus[i])	m_ll[i]->Fill((mum+mup).M()/1000.);
    }

  }
  m_ll[0]->Draw();
  m_ll[1]->SetLineColor(kRed);
  m_ll[1]->Draw("same");
}
