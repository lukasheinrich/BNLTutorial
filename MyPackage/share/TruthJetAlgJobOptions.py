#Skeleton joboption for a simple analysis job

theApp.EvtMax=10                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
svcMgr.EventSelector.InputCollections=[os.environ['TestArea']+"/derivation/DAOD_TRUTH0.my.daod.root"]   #insert your list of input files here

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.TruthJetAlg(DoCppConfig=True)                                 #adds an instance of your alg to it


if not algseq.TruthJetAlg.DoCppConfig:
  #python config instead
  #1. Configure MCTruthClassifier, actually just using default properties
  ToolSvc += CfgMgr.MCTruthClassifier("JetMCTruthClassifier")


  #2. configure a CopyTruthJetParticles tool
  ToolSvc += CfgMgr.CopyTruthJetParticles("TruthParticleSelector",
    OutputName="SelectedTruthParticles",
    MCTruthClassifier = ToolSvc.JetMCTruthClassifier,
    IncludeNeutrinos=False, #change to True for 'WZ' truth jets
    IncludeMuons=False,     #change to True for 'WZ' truth jets
    IncludeWZLeptons=True,  #change to False for 'WZ' truth jets
    IncludeTauLeptons=True, 
    MaxAbsEta=5
  )

  #3. configure a PseudoJetGetter to make pseudojets from selected particles
  ToolSvc += CfgMgr.PseudoJetGetter("truthget",
    Label = "Truth",
    InputContainer = ToolSvc.TruthParticleSelector.OutputName,
    OutputContainer = "PseudoJetTruth",
    GhostScale = 0.0,
    SkipNegativeEnergy = True
  )

  #4. configure a jet builder, which will make xAOD::Jet from a PseudoJet
  #   Use all default values here
  ToolSvc += CfgMgr.JetFromPseudojet("jetbuild")

  #5. configure a jet finder, which actually does the AntiKt on the PseudoJets, 
  #   but also then use the builder to make the xAOD::Jet
  ToolSvc += CfgMgr.JetFinder("jetfind", 
    JetAlgorithm="AntiKt", 
    JetRadius = 0.4, 
    PtMin=5000., 
    JetBuilder=ToolSvc.jetbuild
  )

  #6. Configure JetRecTool to actually execute the JetFinder on the given PseudoJets
  ToolSvc += CfgMgr.JetRecTool("MyJetRecTool",
    OutputContainer="MyTruthJets",
    PseudoJetGetters=[ToolSvc.truthget], 
    JetFinder=ToolSvc.jetfind 
  )