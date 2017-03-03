# Displaced Photon Analysis Framework

Recipes below. Lots of analyzers, macros, many of the files/functions are duplicated. To run this code, we will need to keep up with the latest EGM smearing and VID:
* https://twiki.cern.ch/twiki/bin/viewauth/CMS/EGMSmearer
* https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2
* https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2

HLTDisplacedPhoton Trigger documentation: 
* https://twiki.cern.ch/twiki/bin/view/CMS/DisplacedPhotonTriggerRun2

------------------------------------

### Full Recipe for Analyzers and Rest of MC Production (step1-3)

Recipe to install of all the code necessary to run analyzers. Works so far on LXPLUS.

```
cmsrel CMSSW_8_0_26_patch2
cd CMSSW_8_0_26_patch2/src/
cmsenv
git cms-init

### EGM Smearing ###
git cms-merge-topic shervin86:Moriond17_23Jan
cd EgammaAnalysis/ElectronTools/data
git clone https://github.com/ECALELFS/ScalesSmearings.git
cd ../../../

### Electron VID modules ###
git cms-merge-topic ikrav:egm_id_80X_v2

### Photon VID modules ###
git cms-merge-topic ikrav:egm_id_80X_v3_photons

### Trigger tests --> Optional ###
git cms-addpkg HLTrigger/Configuration
git cms-addpkg GeneratorInterface/GenFilters
git cms-addpkg L1Trigger/L1TGlobal
git clone https://github.com/cms-data/L1Trigger-L1TGlobal.git L1Trigger/L1TGlobal/data
git clone git@github.com:cms-steam/RemovePileUpDominatedEvents.git RemovePileUpDominatedEvent
git cms-addpkg HLTrigger/HLTanalyzers
git clone git@github.com:cms-steam/HLTrigger temp
cp -r temp/* HLTrigger/
rm -fr temp/
cp /afs/cern.ch/user/n/ndaci/public/STEAM/L1Menus/L1Menu_Collisions2016_v9_m2.xml L1Trigger/L1TGlobal/data/Luminosity/startup/
cp /afs/cern.ch/user/n/ndaci/public/STEAM/Prescales/UGT_BASE_RS_FINOR_MASK_v91.xml L1Trigger/L1TGlobal/data/Luminosity/startup/
cp /afs/cern.ch/user/n/ndaci/public/STEAM/Prescales/UGT_BASE_RS_PRESCALES_v214.xml L1Trigger/L1TGlobal/data/Luminosity/startup/
cp /afs/cern.ch/user/n/ndaci/public/STEAM/Prescales/L1TGlobalPrescalesVetosESProducer.cc L1Trigger/L1TGlobal/plugins/

### Copy this directory for analyzers ###
git clone ssh://git@github.com/kmcdermo/Timing.git
cd Timing/
git remote rename origin kmcdermo
cd ../

### Copy this for custom timing reco ###
git cms-addpkg EgammaAnalysis/Configuration
cp Timing/TimingAnalyzer/python/RecoEcal_CustomiseTimeCleaning.py EgammaAnalysis/Configuration/python

scram b -j 16
```

From here, we can issue a set of cmsDriver commands to autogenerate python configs. You may need to get a proxy to run this, as it requires pinging confDB.  To run locally, we will need a GEN_SIM ROOT file from step0 as input into step1.  Make sure to test each autogenerated config locally with cmsRun before submitting to CRAB!

```
cd CMSSW_8_0_26_patch2/src/
cmsenv
voms-proxy-init --voms cms --valid 168:00

### Step1: L1 and HLT simulation. ### 
### Since we are developing our own trigger, we have a bit of customization in the HLT step ###
### Exact config found in Timing/GEN_SIM/GMSB/step1_userHLT_DIGI_L1_DIGI2RAW_HLT_PU-RAWSIM.py ###

cmsDriver.py step1_userHLT --mc --era Run2_2016 --conditions 80X_mcRun2_asymptotic_2016_TrancheIV_v6 --eventcontent RAWSIM --datatier GEN-SIM-RAW --step DIGI,L1,DIGI2RAW,HLT:hltdev:/users/kmcdermo/TestDev/DisplacedPhoton_8_0_24/V1 --pileup 2016_25ns_Moriond17MC_PoissonOOTPU --pileup_input dbs:/MinBias_TuneCUETP8M1_13TeV-pythia8/RunIISummer15GS-MCRUN2_71_V1_ext1-v1/GEN-SIM --filein file:GEN_SIM-RAWSIM.root --fileout file:DIGI_L1_DIGI2RAW_HLT_PU-RAWSIM.root --no_exec

mv step1_userHLT_DIGI_L1_DIGI2RAW_HLT_PU.py step1_userHLT_DIGI_L1_DIGI2RAW_HLT_PU-RAWSIM.py 
cmsRun step1_userHLT_DIGI_L1_DIGI2RAW_HLT_PU-RAWSIM.py 

### Step2: Full RECO simulation, store AODSIM. ###
### Again, since we are interested in OOT recHits as seeds, need some more customization compared to the standard workflow ###
### Exact config found in Timing/GEN_SIM/GMSB/step2_customiseTiming_RAW2DIGI_L1Reco_RECO-AODSIM.py ###

cmsDriver.py step2_customise --customise RecoEcal/Configuration/RecoEcal_CustomiseTimeCleaning.customiseTimeCleaning --mc --eventcontent AODSIM --runUnscheduled --datatier AODSIM --conditions 80X_mcRun2_asymptotic_2016_TrancheIV_v6 --step RAW2DIGI,L1Reco,RECO --era Run2_2016 --filein file:DIGI_L1_DIGI2RAW_HLT_PU-RAWSIM.root --fileout file:RAW2DIGI_L1Reco_RECO-AODSIM.root --no_exec

mv step2_customiseTiming_RAW2DIGI_L1Reco_RECO.py step2_customiseTiming_RAW2DIGI_L1Reco_RECO-AODSIM.py
cmsRun step2_customiseTiming_RAW2DIGI_L1Reco_RECO-AODSIM.py

### Step3: PAT, i.e. MINIAOD format. ###
### Exact config found in Timing/GEN_SIM/GMSB/step3_PAT-MINIAODSIM.root ###

cmsDriver.py step3 --mc --eventcontent MINIAODSIM --runUnscheduled --datatier MINIAODSIM --conditions 80X_mcRun2_asymptotic_2016_TrancheIV_v6 --step PAT --era Run2_2016 --filein file:RAW2DIGI_L1Reco_RECO-AODSIM.root --fileout file:PAT-MINIAODSIM.root --no_exec

mv step3_PAT.root step3_PAT-MINIAODSIM.root
cmsRun step3_PAT-MINIAODSIM.root
```

Further Documentation:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVMCcampaignRunIISummer16DR80
 * https://twiki.cern.ch/twiki/bin/view/CMS/PdmVMCcampaignRunIISummer16MiniAODv2

#### Running the general displaced photon analyzers and macros

The general displaced photon analyzer for now is Timing/TimingAnalyzer/plugins/PhotonDump.cc[h]. It has a number of unique branches for each MC samples (GMSB, HVDS, and background).  The gen info stored is specific to each sample.  The python config to run this plugin is Timing/TimingAnalyzer/test/photondump-*.py. Make sure to checkout each one and modify it accordingly for new samples.  One thing to be aware of is the fact that there is a moderate level of configuration via the command line, so make sure to check out the available options.  

```
cd CMSSW_8_0_26_patch2/src/Timing/TimingAnalyzer
cmsenv
cmsRun test/photondump-gmsb.py

### We now need ROOT5 for this next part... non-ideal, I realize as the default is ROOT6 with cmsenv for CMSSW_8_0_X+. ###
### ROOT5 is actually the default of lxplus without setting the CMS environment. ###
### Or one can copy the output of the analyzer to a laptop with ROOT5 (my personal favorite). ###
### I will soon make this macro into a compiled code, like the Zee timing resolution tools (i.e. plugins/ZeeTree.cc[h] and work/) ###

cd macros/
root -l -b -j runPhotonPlots.C

### Few things worth mentioning... there is again another huge set of configurables, so make sure to set them properly! ###
### This code compiles PlotPhotons.cc[hh]. ### 
### This code is in flux, as some sections are commented out, so be sure to comment them in if you want certain plots to be made. ###  

### Also, pay attention to the line containing: ###
### if (((fIsGMSB || fIsHVDS) && (*phmatch)[iph] <= 0) || (fIsBkg && (*phisMatched)[iph] != 0) ) continue; (at L632 at the moment) ###  ### Comment this out if you do NOT want gen-level matching (for signals and gamma+jets) or gen-level matching veto (for QCD). ###
```

------------------------------------

### Recipe for GEN-SIM ONLY (step0)  

```
cmsrel CMSSW_7_1_25
cd CMSSW_7_1_25/src/
cmsenv
git cms-init

git clone ssh://git@github.com/kmcdermo/Timing.git
mv Timing/GEN_SIM/Configuration . 
```
cmsDriver.py for step0 must be launched from CMSSW_7_1_25/src/ in order to properly pickup Configuration/.  See https://github.com/kmcdermo/Timing/blob/80X/GEN_SIM/GMSB/step0_GMSB_L180_Ctau6000_Pythia8_13TeV_GEN_SIM-RAWSIM.py#L5 for an example of the cmsDriver.py command for generating MC with SLHA files.

Further Documentation:
 * https://github.com/lsoffi/GMSB-MC2016-PRODUCTION
