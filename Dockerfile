FROM lukasheinrich/atlas-athanalysisbase-2.4.8
ADD StandalonePackage/trunk /analysis/StandalonePackage
WORKDIR /analysis
RUN source $AtlasSetup/scripts/asetup.sh AthAnalysisBase,2.4.8,here && cmt find_packages && cmt compile
RUN echo 'asetup AthAnalysisBase,2.4.8,here' >> ~/.bashrc