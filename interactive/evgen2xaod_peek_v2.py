#execute this with: athena -i evgen2xaod_peek.py

#this program loops over an EVNT file, converts it to xAOD on the fly, dresses the leptons
#and then builds the dilepton invariant mass
#see evgen2xaod_peek.C for the c++ equivalent

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections=["../evgen/my.evgen.root"]

algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.xAODMaker__xAODTruthCnvAlg("myAlg",AODContainerName="GEN_EVENT")


import ROOT
tool = ROOT.AthAnalysisHelper.createTool("DerivationFramework::TruthDressingTool/TruthDressing")
theApp.initialize()

evt = PyAthena.py_svc("StoreGateSvc")

from GaudiPython.Bindings import InterfaceCast

tool = InterfaceCast('DerivationFramework::IAugmentationTool').cast(tool)
tool.initialize()

m_ll = [ ROOT.TH1D("m_ll","Dilepton mass",100,0,200) ];
m_ll += [ ROOT.TH1D("m_ll_dressed","Dressed Dilepton mass",100,0,200) ];

for i in range(0,theApp.size()):
  theApp.nextEvent()
  tool.addBranches() #does the lepton dressing
  mc = evt['TruthParticles']
  elplus = [[],[]]
  elminus = [[],[]]
  muplus = [[],[]]
  muminus = [[],[]]
  for p in mc:
    if p.status() != 1: continue
    if abs(p.pdgId()) != 11 and abs(p.pdgId()) != 13: continue
    v = p.p4();
    v2 = ROOT.TLorentzVector()
    v2.SetPtEtaPhiE( p.auxdata("float")("pt_dressed") , p.auxdata("float")("eta_dressed"), p.auxdata("float")("phi_dressed"), p.auxdata("float")("e_dressed"))
    if(p.pdgId()==11) : elplus[0]+=[v];elplus[1]+=[v2]
    elif(p.pdgId()==-11) : elminus[0]+=[v];elminus[1]+=[v2]
    elif(p.pdgId()==13) : muplus[0]+=[v];elplus[1]+=[v2]
    elif(p.pdgId()==-13) : muminus[0]+=[v];elminus[1]+=[v2]
  for j in range(0,2):
    for elp in elplus[j]: 
      for elm in elminus[j]: 
        m_ll[j].Fill( (elp+elm).M()/1000. )
    for mup in muplus[j]: 
      for mum in muminus[j]: 
        m_ll[j].Fill( (mup+mum).M()/1000. )

f = ROOT.TFile("myFile.root","RECREATE")
m_ll[0].SetDirectory(f); m_ll[0].Write()
m_ll[1].SetLineColor(ROOT.kRed)
m_ll[1].SetDirectory(f); m_ll[1].Write()
f.Close()
