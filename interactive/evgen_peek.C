{
  POOL::TEvent evt;
  evt.readFrom("$TestArea/evgen/my.evgen.root");

  //plot dilepton invariant mass
  //form all possible l+l- pairs and plot masses
  TH1D* m_ll = new TH1D("m_ll","Dilepton mass",100,0,200);

  for(int i=0;i<evt.getEntries();i++) {
    evt.getEntry(i);
    
    //retrieve the truth
    const McEventCollection* mc = 0;
    evt.retrieve( mc );

    std::vector<TLorentzVector> elplus;
    std::vector<TLorentzVector> muplus;
    std::vector<TLorentzVector> elminus;
    std::vector<TLorentzVector> muminus;

    //loop over truth particles
    for(auto itr = mc->at(0)->particles_begin(); itr != mc->at(0)->particles_end(); ++itr) {
      if((*itr)->status()!=1) continue; //must be stable
      if(fabs((*itr)->pdg_id()) != 11 && (fabs((*itr)->pdg_id()) != 13)) continue; //must be a lepton

      TLorentzVector v((*itr)->momentum().px(),(*itr)->momentum().py(),(*itr)->momentum().pz(),(*itr)->momentum().e());

      switch( (*itr)->pdg_id() ) {
      case 11: elminus.push_back(v); break;
      case -11: elplus.push_back(v); break;
      case 13: muminus.push_back(v); break;
      case -13: muplus.push_back(v); break;
      }
    }
    //form dilepton masses from available leptons
    for(auto& elm : elminus) for(auto& elp : elplus)	m_ll->Fill((elm+elp).M()/1000.);
    for(auto& mum : muminus) for(auto& mup : muplus)	m_ll->Fill((mum+mup).M()/1000.);
    

  }
  m_ll->Draw();

}
