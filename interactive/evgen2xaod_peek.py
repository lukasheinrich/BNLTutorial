#execute this with: python -i evgen2xaod_peek.py

#this program loops over an evgen file, converting it to xAOD on the fly, dresses the leptons (using the TruthDressingTool)
#and then builds the dilepton invariant mass
#see evgen2xaod_peek.C for the c++ equivalent

import ROOT

evt = ROOT.POOL.TEvent()
evt.readFrom("../evgen/my.evgen.root")
evt.getEntry(0) #initializes

from GaudiPython.Bindings import InterfaceCast

#Create instance of converter alg, configuring a property of it
#Note that this procedure will hopefully be improved soon
alg = ROOT.AthAnalysisHelper.createAlgorithm("xAODMaker::xAODTruthCnvAlg/myAlg")
myalg = InterfaceCast('IProperty').cast(alg)
myalg.setProperty( "AODContainerName" , "GEN_EVENT" )
alg.initialize()

tool = ROOT.AthAnalysisHelper.createTool("DerivationFramework::TruthDressingTool/TruthDressing")
#this is how to cast the tool in python so you have access to the addBranches method
tool = InterfaceCast('DerivationFramework::IAugmentationTool').cast(tool)
tool.initialize()

#these next two lines are needed for now, 
#until python bindings for retrieve method of TEvent is working
from AthenaPython import PyAthena
sg = PyAthena.py_svc("StoreGateSvc")

m_ll = [ ROOT.TH1D("m_ll","Dilepton mass",100,0,200) ];
m_ll += [ ROOT.TH1D("m_ll_dressed","Dressed Dilepton mass",100,0,200) ];

for i in range(0,evt.getEntries()):
  evt.getEntry(i)
  alg.execute() #converts HepMC into xAOD
  tool.addBranches() #decorates leptons with pt_dressed etc
  mc = sg['TruthParticles'] #retrieve objects via the sg storegate obj
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

#exit batch mode so can display canvas
ROOT.gROOT.SetBatch(False)
m_ll[0].Draw()
m_ll[1].SetLineColor(ROOT.kRed)
m_ll[1].Draw("same")

