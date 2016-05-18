{
  POOL::TEvent::Init("POOLRootAccess/basicxAOD.opts")
  POOL::TEvent evt;
  evt.readFrom("../derivation/DAOD_TRUTH0.my.daod.root");
  evt.getEntry(0);
  std::cout << evt.evtStore()->dump() << std::endl; //list the contents of the storegate
  const xAOD::EventInfo_v1* evtInfo = 0;
  evt.retrieve( evtInfo );
  AthAnalysisHelper::printAuxElement( *evtInfo );
  
}