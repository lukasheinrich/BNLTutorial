
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MyPackageAlg.h"

DECLARE_ALGORITHM_FACTORY( MyPackageAlg )


#include "../TruthJetAlg.h"
DECLARE_ALGORITHM_FACTORY( TruthJetAlg )

DECLARE_FACTORY_ENTRIES( MyPackage ) 
{
  DECLARE_ALGORITHM( TruthJetAlg );
  DECLARE_ALGORITHM( MyPackageAlg );
}
