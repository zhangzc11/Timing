#include "Skimmer.hh"
#include "TROOT.h"

#include <iostream>

Skimmer::Skimmer(const TString & indir, const TString & outdir, const TString & filename) : fInDir(indir), fOutDir(outdir), fFileName(filename)
{
  // because root is dumb?
  gROOT->ProcessLine("#include <vector>");

  // Get input
  const TString infilename = Form("%s/%s", fInDir.Data(), fFileName.Data());
  fInFile = TFile::Open(infilename.Data());
  CheckValidFile(fInFile,infilename);

  // Get input config tree
  const TString inconfigtreename = Form("%s%s",Config::rootdir.Data(),Config::configtreename.Data());
  fInConfigTree = (TTree*)fInFile->Get(inconfigtreename.Data());
  CheckValidTree(fInConfigTree,inconfigtreename,infilename);
  Skimmer::GetInConfig();

  // Get main input tree and initialize it
  const TString indisphotreename = Form("%s%s",Config::rootdir.Data(),Config::disphotreename.Data());
  fInTree = (TTree*)fInFile->Get(indisphotreename.Data());
  CheckValidTree(fInTree,indisphotreename,infilename);
  Skimmer::InitInTree();

  // Get the cut flow + event weight histogram
  const TString inh_cutflowname = Form("%s%s",Config::rootdir.Data(),Config::h_cutflowname.Data());
  fInCutFlow = (TH1F*)fInFile->Get(inh_cutflowname.Data());
  CheckValidTH1F(fInCutFlow,inh_cutflowname,infilename);

  // Set Output Stuff
  // Make the output file, make trees, then init them
  fOutFile = TFile::Open(Form("%s/%s", fOutDir.Data(), fFileName.Data()),"recreate");
  fOutFile->cd();
  
  fOutConfigTree = new TTree(Config::configtreename.Data(),Config::configtreename.Data());
  fOutTree = new TTree(Config::disphotreename.Data(),Config::disphotreename.Data());
  
  Skimmer::InitAndSetOutConfig();
  Skimmer::InitOutTree();
  Skimmer::InitOutCutFlow();
}

Skimmer::~Skimmer()
{
  delete fInCutFlow;
  delete fInTree;
  delete fInConfigTree;
  delete fInFile;

  delete fOutCutFlow;
  delete fOutTree;
  delete fOutConfigTree;
  delete fOutFile;
}

void Skimmer::EventLoop()
{
  // do loop over events, filling histos
  const UInt_t nEntries = fInTree->GetEntries();
  for (UInt_t entry = 0; entry < nEntries; entry++)
  {
    // read in tree
    fInTree->GetEntry(entry);

    // dump status check
    if (entry%Config::nEvCheck == 0 || entry == 0) std::cout << "Processing Entry: " << entry << " out of " << nEntries << std::endl;
    
    // get event weight
    const Float_t evtwgt = (fIsMC ? fInEvent.genwgt : 1.f);

    // perform skim
    if (fInEvent.njets < 5) continue;

    // fill cutflow
    fOutCutFlow->Fill((cutLabels["skim"]*1.f)-0.5f,evtwgt);
  
    // end of skim, now copy... dropping rechits
    if (fOutConfig.isGMSB) Skimmer::FillOutGMSBs();
    if (fOutConfig.isHVDS) Skimmer::FillOutHVDSs();
    Skimmer::FillOutEvent();
    Skimmer::FillOutJets();
    Skimmer::FillOutPhos();

    // fill the tree
    fOutTree->Fill();
  } // end loop over events

  // write out the output!
  fOutFile->cd();
  fOutCutFlow->Write();
  fOutConfigTree->Write();
  fOutTree->Write();
}

void Skimmer::FillOutGMSBs()
{
  for (Int_t igmsb = 0; igmsb < Config::nGMSBs; igmsb++)
  {
    const auto & ingmsb = fInGMSBs[igmsb];
    auto & outgmsb = fOutGMSBs[igmsb];

    outgmsb.genNmass = ingmsb.genNmass;
    outgmsb.genNE = ingmsb.genNE;
    outgmsb.genNpt = ingmsb.genNpt;
    outgmsb.genNphi = ingmsb.genNphi;
    outgmsb.genNeta = ingmsb.genNeta;
    outgmsb.genNprodvx = ingmsb.genNprodvx;
    outgmsb.genNprodvy = ingmsb.genNprodvy;
    outgmsb.genNprodvz = ingmsb.genNprodvz;
    outgmsb.genNdecayvx = ingmsb.genNdecayvx;
    outgmsb.genNdecayvy = ingmsb.genNdecayvy;
    outgmsb.genNdecayvz = ingmsb.genNdecayvz;
    outgmsb.genphE = ingmsb.genphE;
    outgmsb.genphpt = ingmsb.genphpt;
    outgmsb.genphphi = ingmsb.genphphi;
    outgmsb.genpheta = ingmsb.genpheta;
    outgmsb.genphmatch = ingmsb.genphmatch;
    outgmsb.gengrmass = ingmsb.gengrmass;
    outgmsb.gengrE = ingmsb.gengrE;
    outgmsb.gengrpt = ingmsb.gengrpt;
    outgmsb.gengrphi = ingmsb.gengrphi;
    outgmsb.gengreta = ingmsb.gengreta;
  }
}

void Skimmer::FillOutHVDSs()
{
  for (Int_t ihvds = 0; ihvds < Config::nHVDSs; ihvds++)
  {
    const auto & inhvds = fInHVDSs[ihvds];
    auto & outhvds = fOutHVDSs[ihvds];

    outhvds.genvPionmass = inhvds.genvPionmass;
    outhvds.genvPionE = inhvds.genvPionE;
    outhvds.genvPionpt = inhvds.genvPionpt;
    outhvds.genvPionphi = inhvds.genvPionphi;
    outhvds.genvPioneta = inhvds.genvPioneta;
    outhvds.genvPionprodvx = inhvds.genvPionprodvx;
    outhvds.genvPionprodvy = inhvds.genvPionprodvy;
    outhvds.genvPionprodvz = inhvds.genvPionprodvz;
    outhvds.genvPiondecayvx = inhvds.genvPiondecayvx;
    outhvds.genvPiondecayvy = inhvds.genvPiondecayvy;
    outhvds.genvPiondecayvz = inhvds.genvPiondecayvz;
    outhvds.genHVph0E = inhvds.genHVph0E;
    outhvds.genHVph0pt = inhvds.genHVph0pt;
    outhvds.genHVph0phi = inhvds.genHVph0phi;
    outhvds.genHVph0eta = inhvds.genHVph0eta;
    outhvds.genHVph0match = inhvds.genHVph0match;
    outhvds.genHVph1E = inhvds.genHVph1E;
    outhvds.genHVph1pt = inhvds.genHVph1pt;
    outhvds.genHVph1phi = inhvds.genHVph1phi;
    outhvds.genHVph1eta = inhvds.genHVph1eta;
    outhvds.genHVph1match = inhvds.genHVph1match;
  }
}

void Skimmer::FillOutEvent()
{
  fOutEvent.run = fOutEvent.run;
  fOutEvent.lumi = fOutEvent.lumi;
  fOutEvent.event = fOutEvent.event;
  fOutEvent.hltSignal = fOutEvent.hltSignal;
  fOutEvent.hltRefPhoID = fOutEvent.hltRefPhoID;
  fOutEvent.hltRefDispID = fOutEvent.hltRefDispID;
  fOutEvent.hltRefHT = fOutEvent.hltRefHT;
  fOutEvent.hltPho50 = fOutEvent.hltPho50;
  fOutEvent.nvtx = fOutEvent.nvtx;
  fOutEvent.vtxX = fOutEvent.vtxX;
  fOutEvent.vtxY = fOutEvent.vtxY;
  fOutEvent.vtxZ = fOutEvent.vtxZ;
  fOutEvent.rho = fOutEvent.rho;
  fOutEvent.t1pfMETpt = fOutEvent.t1pfMETpt;
  fOutEvent.t1pfMETphi = fOutEvent.t1pfMETphi;
  fOutEvent.t1pfMETsumEt = fOutEvent.t1pfMETsumEt;
  fOutEvent.jetHT = fOutEvent.jetHT;
  fOutEvent.njets = fOutEvent.njets;
  fOutEvent.nrechits = fOutEvent.nrechits;
  fOutEvent.nphotons = fOutEvent.nphotons;
    
  if (fIsMC)
  {
    fOutEvent.genwgt = fOutEvent.genwgt;
    fOutEvent.genpuobs = fOutEvent.genpuobs;
    fOutEvent.genputrue = fOutEvent.genputrue;
    if (fOutConfig.isGMSB)
    {
      fOutEvent.nNeutoPhGr = fOutEvent.nNeutoPhGr;
    }
    if (fOutConfig.isHVDS)
    {
      fOutEvent.nvPions = fOutEvent.nvPions;
    }
  }
}

void Skimmer::FillOutJets()
{
  // Jets Info
  for (Int_t ijet = 0; ijet < Config::nJets; ijet++) 
  {
    const auto & injet = fInJets[ijet];
    auto & outjet = fOutJets[ijet];
    
    outjet.E = injet.E;
    outjet.pt = injet.pt;
    outjet.eta = injet.eta;
    outjet.phi = injet.phi;
  }
}

void Skimmer::FillOutPhos()
{  
  // Photon Info
  for (Int_t ipho = 0; ipho < Config::nPhotons; ipho++) 
  {
    const auto & inpho = fInPhos[ipho];
    auto & outpho = fOutPhos[ipho];
    
    outpho.E = inpho.E;
    outpho.pt = inpho.pt;
    outpho.eta = inpho.eta;
    outpho.phi = inpho.phi;
    outpho.scE = inpho.scE;
    outpho.sceta = inpho.sceta;
    outpho.scphi = inpho.scphi;
    outpho.HoE = inpho.HoE;
    outpho.r9 = inpho.r9;
    outpho.ChgHadIso = inpho.ChgHadIso;
    outpho.NeuHadIso = inpho.NeuHadIso;
    outpho.PhoIso = inpho.PhoIso;
    outpho.EcalPFClIso = inpho.EcalPFClIso;
    outpho.HcalPFClIso = inpho.HcalPFClIso;
    outpho.TrkIso = inpho.TrkIso;
    outpho.sieie = inpho.sieie;
    outpho.sipip = inpho.sipip;
    outpho.sieip = inpho.sieip;
    outpho.smaj = inpho.smaj;
    outpho.smin = inpho.smin;
    outpho.alpha = inpho.alpha;
    outpho.isOOT = inpho.isOOT;
    outpho.isEB = inpho.isEB;
    outpho.isHLT = inpho.isHLT;
    outpho.isTrk = inpho.isTrk;
    outpho.ID = inpho.ID;

    if (fInConfig.storeRecHits)
    {
      if (inpho.seed >= 0)
      {
	outpho.seedtime = (*fInRecHits.time)[inpho.seed];
	outpho.seedE    = (*fInRecHits.E)   [inpho.seed];
	outpho.seedID   = (*fInRecHits.ID)  [inpho.seed];
      }
      else 
      {
	outpho.seedtime = -9999.f;
	outpho.seedE    = -9999.f;
	outpho.seedID   = 0;
      }
    }
    else
    {
      outpho.seedtime = inpho.seedtime;
      outpho.seedE    = inpho.seedE;
      outpho.seedID   = inpho.seedID;
    }
    
    if (fIsMC)
    {
      outpho.isGen = inpho.isGen;
      if (fOutConfig.isGMSB || fOutConfig.isHVDS)
      {
	outpho.isSignal = inpho.isSignal;
      }
    }
  }
}

void Skimmer::GetInConfig()
{
  // Get Input Config
  Skimmer::InitInConfigStrings();
  Skimmer::InitInConfigBranches();

  // read in first entry (will be the same for all entries in a given file
  fInConfigTree->GetEntry(0);

  // set isMC
  fIsMC = (fInConfig.isGMSB || fInConfig.isHVDS || fInConfig.isBkgd);
}

void Skimmer::InitInConfigStrings()
{
  fInConfig.phIDmin = 0;
  fInConfig.phgoodIDmin = 0;
  fInConfig.inputPaths = 0;
  fInConfig.inputFilters = 0;
}

void Skimmer::InitInConfigBranches()
{
  fInConfigTree->SetBranchAddress(fInConfig.s_blindSF.c_str(), &fInConfig.blindSF);
  fInConfigTree->SetBranchAddress(fInConfig.s_applyBlindSF.c_str(), &fInConfig.applyBlindSF);
  fInConfigTree->SetBranchAddress(fInConfig.s_blindMET.c_str(), &fInConfig.blindMET);
  fInConfigTree->SetBranchAddress(fInConfig.s_applyBlindMET.c_str(), &fInConfig.applyBlindMET);
  fInConfigTree->SetBranchAddress(fInConfig.s_jetpTmin.c_str(), &fInConfig.jetpTmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_jetIDmin.c_str(), &fInConfig.jetIDmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_rhEmin.c_str(), &fInConfig.rhEmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_phpTmin.c_str(), &fInConfig.phpTmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_phIDmin.c_str(), &fInConfig.phIDmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_seedTimemin.c_str(), &fInConfig.seedTimemin);
  fInConfigTree->SetBranchAddress(fInConfig.s_splitPho.c_str(), &fInConfig.splitPho);
  fInConfigTree->SetBranchAddress(fInConfig.s_onlyGED.c_str(), &fInConfig.onlyGED);
  fInConfigTree->SetBranchAddress(fInConfig.s_onlyOOT.c_str(), &fInConfig.onlyOOT);
  fInConfigTree->SetBranchAddress(fInConfig.s_storeRecHits.c_str(), &fInConfig.storeRecHits);
  fInConfigTree->SetBranchAddress(fInConfig.s_applyTrigger.c_str(), &fInConfig.applyTrigger);
  fInConfigTree->SetBranchAddress(fInConfig.s_minHT.c_str(), &fInConfig.minHT);
  fInConfigTree->SetBranchAddress(fInConfig.s_applyHT.c_str(), &fInConfig.applyHT);
  fInConfigTree->SetBranchAddress(fInConfig.s_phgoodpTmin.c_str(), &fInConfig.phgoodpTmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_phgoodIDmin.c_str(), &fInConfig.phgoodIDmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_applyPhGood.c_str(), &fInConfig.applyPhGood);
  fInConfigTree->SetBranchAddress(fInConfig.s_dRmin.c_str(), &fInConfig.dRmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_pTres.c_str(), &fInConfig.pTres);
  fInConfigTree->SetBranchAddress(fInConfig.s_trackdRmin.c_str(), &fInConfig.trackdRmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_trackpTmin.c_str(), &fInConfig.trackpTmin);
  fInConfigTree->SetBranchAddress(fInConfig.s_inputPaths.c_str(), &fInConfig.inputPaths);
  fInConfigTree->SetBranchAddress(fInConfig.s_inputFilters.c_str(), &fInConfig.inputFilters);
  fInConfigTree->SetBranchAddress(fInConfig.s_isGMSB.c_str(), &fInConfig.isGMSB);
  fInConfigTree->SetBranchAddress(fInConfig.s_isHVDS.c_str(), &fInConfig.isHVDS);
  fInConfigTree->SetBranchAddress(fInConfig.s_isBkgd.c_str(), &fInConfig.isBkgd);
  fInConfigTree->SetBranchAddress(fInConfig.s_xsec.c_str(), &fInConfig.xsec);
  fInConfigTree->SetBranchAddress(fInConfig.s_filterEff.c_str(), &fInConfig.filterEff);
  fInConfigTree->SetBranchAddress(fInConfig.s_BR.c_str(), &fInConfig.BR);
}

void Skimmer::InitInTree() 
{
  Skimmer::InitInStructs();
  Skimmer::InitInBranchVecs();
  Skimmer::InitInBranches();
}

void Skimmer::InitInStructs()
{
  if (fIsMC)
  {
    if (fInConfig.isGMSB)
    {
      fInGMSBs.clear(); 
      fInGMSBs.resize(Config::nGMSBs);
    }
    if (fInConfig.isHVDS)
    {
      fInHVDSs.clear(); 
      fInHVDSs.resize(Config::nHVDSs);
    }
  }

  fInJets.clear();
  fInJets.resize(Config::nJets);

  fInPhos.clear();
  fInPhos.resize(Config::nPhotons);
}

void Skimmer::InitInBranchVecs()
{
  if (fInConfig.storeRecHits) 
  {
    fInRecHits.eta = 0;
    fInRecHits.phi = 0;
    fInRecHits.E = 0;
    fInRecHits.time = 0;
    fInRecHits.OOT = 0;
    fInRecHits.ID = 0;

    for (Int_t ipho = 0; ipho < Config::nPhotons; ipho++) 
    {
      fInPhos[ipho].recHits = 0;
    }  
  }
}

void Skimmer::InitInBranches()
{
  if (fIsMC)
  {
    fInTree->SetBranchAddress(fInEvent.s_genwgt.c_str(), &fInEvent.genwgt);
    fInTree->SetBranchAddress(fInEvent.s_genpuobs.c_str(), &fInEvent.genpuobs);
    fInTree->SetBranchAddress(fInEvent.s_genputrue.c_str(), &fInEvent.genputrue);
    
    if (fInConfig.isGMSB)
    {
      fInTree->SetBranchAddress(fInEvent.s_nNeutoPhGr.c_str(), &fInEvent.nNeutoPhGr);
      for (Int_t igmsb = 0; igmsb < Config::nGMSBs; igmsb++) 
      {
	auto & gmsb = fInGMSBs[igmsb];
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNmass.c_str(),igmsb), &gmsb.genNmass);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNE.c_str(),igmsb), &gmsb.genNE);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNpt.c_str(),igmsb), &gmsb.genNpt);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNphi.c_str(),igmsb), &gmsb.genNphi);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNeta.c_str(),igmsb), &gmsb.genNeta);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNprodvx.c_str(),igmsb), &gmsb.genNprodvx);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNprodvy.c_str(),igmsb), &gmsb.genNprodvy);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNprodvz.c_str(),igmsb), &gmsb.genNprodvz);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNdecayvx.c_str(),igmsb), &gmsb.genNdecayvx);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNdecayvy.c_str(),igmsb), &gmsb.genNdecayvy);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genNdecayvz.c_str(),igmsb), &gmsb.genNdecayvz);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genphE.c_str(),igmsb), &gmsb.genphE);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genphpt.c_str(),igmsb), &gmsb.genphpt);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genphphi.c_str(),igmsb), &gmsb.genphphi);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genpheta.c_str(),igmsb), &gmsb.genpheta);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_genphmatch.c_str(),igmsb), &gmsb.genphmatch);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_gengrmass.c_str(),igmsb), &gmsb.gengrmass);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_gengrE.c_str(),igmsb), &gmsb.gengrE);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_gengrpt.c_str(),igmsb), &gmsb.gengrpt);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_gengrphi.c_str(),igmsb), &gmsb.gengrphi);
	fInTree->SetBranchAddress(Form("%s_%i",gmsb.s_gengreta.c_str(),igmsb), &gmsb.gengreta);
      } // end loop over neutralinos
    } // end block over gmsb

    if (fInConfig.isHVDS)
    {
      fInTree->SetBranchAddress(fInEvent.s_nvPions.c_str(), &fInEvent.nvPions);
      for (Int_t ihvds = 0; ihvds < Config::nHVDSs; ihvds++) 
      {
	auto & hvds = fInHVDSs[ihvds]; 
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionmass.c_str(),ihvds), &hvds.genvPionmass);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionE.c_str(),ihvds), &hvds.genvPionE);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionpt.c_str(),ihvds), &hvds.genvPionpt);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionphi.c_str(),ihvds), &hvds.genvPionphi);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPioneta.c_str(),ihvds), &hvds.genvPioneta);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionprodvx.c_str(),ihvds), &hvds.genvPionprodvx);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionprodvy.c_str(),ihvds), &hvds.genvPionprodvy);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPionprodvz.c_str(),ihvds), &hvds.genvPionprodvz);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPiondecayvx.c_str(),ihvds), &hvds.genvPiondecayvx);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPiondecayvy.c_str(),ihvds), &hvds.genvPiondecayvy);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genvPiondecayvz.c_str(),ihvds), &hvds.genvPiondecayvz);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph0E.c_str(),ihvds), &hvds.genHVph0E);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph0pt.c_str(),ihvds), &hvds.genHVph0pt);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph0phi.c_str(),ihvds), &hvds.genHVph0phi);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph0eta.c_str(),ihvds), &hvds.genHVph0eta);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph0match.c_str(),ihvds), &hvds.genHVph0match);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph1E.c_str(),ihvds), &hvds.genHVph1E);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph1pt.c_str(),ihvds), &hvds.genHVph1pt);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph1phi.c_str(),ihvds), &hvds.genHVph1phi);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph1eta.c_str(),ihvds), &hvds.genHVph1eta);
	fInTree->SetBranchAddress(Form("%s_%i",hvds.s_genHVph1match.c_str(),ihvds), &hvds.genHVph1match);
      } // end loop over nvpions 
    } // end block over hvds
  } // end block over isMC

  fInTree->SetBranchAddress(fInEvent.s_run.c_str(), &fInEvent.run);
  fInTree->SetBranchAddress(fInEvent.s_lumi.c_str(), &fInEvent.lumi);
  fInTree->SetBranchAddress(fInEvent.s_event.c_str(), &fInEvent.event);
  fInTree->SetBranchAddress(fInEvent.s_hltSignal.c_str(), &fInEvent.hltSignal);
  fInTree->SetBranchAddress(fInEvent.s_hltRefPhoID.c_str(), &fInEvent.hltRefPhoID);
  fInTree->SetBranchAddress(fInEvent.s_hltRefDispID.c_str(), &fInEvent.hltRefDispID);
  fInTree->SetBranchAddress(fInEvent.s_hltPho50.c_str(), &fInEvent.hltPho50);
  fInTree->SetBranchAddress(fInEvent.s_nvtx.c_str(), &fInEvent.nvtx);
  fInTree->SetBranchAddress(fInEvent.s_vtxX.c_str(), &fInEvent.vtxX);
  fInTree->SetBranchAddress(fInEvent.s_vtxY.c_str(), &fInEvent.vtxY);
  fInTree->SetBranchAddress(fInEvent.s_vtxZ.c_str(), &fInEvent.vtxZ);
  fInTree->SetBranchAddress(fInEvent.s_rho.c_str(), &fInEvent.rho);
  fInTree->SetBranchAddress(fInEvent.s_t1pfMETpt.c_str(), &fInEvent.t1pfMETpt);
  fInTree->SetBranchAddress(fInEvent.s_t1pfMETphi.c_str(), &fInEvent.t1pfMETphi);
  fInTree->SetBranchAddress(fInEvent.s_t1pfMETsumEt.c_str(), &fInEvent.t1pfMETsumEt);
  fInTree->SetBranchAddress(fInEvent.s_jetHT.c_str(), &fInEvent.jetHT);

  fInTree->SetBranchAddress(fInEvent.s_njets.c_str(), &fInEvent.njets);
  for (Int_t ijet = 0; ijet < Config::nJets; ijet++) 
  {
    auto & jet = fInJets[ijet];
    fInTree->SetBranchAddress(Form("%s_%i",jet.s_E.c_str(),ijet), &jet.E);    
    fInTree->SetBranchAddress(Form("%s_%i",jet.s_pt.c_str(),ijet), &jet.pt);    
    fInTree->SetBranchAddress(Form("%s_%i",jet.s_phi.c_str(),ijet), &jet.phi);    
    fInTree->SetBranchAddress(Form("%s_%i",jet.s_eta.c_str(),ijet), &jet.eta);    
  }

  fInTree->SetBranchAddress(fInEvent.s_nrechits.c_str(), &fInEvent.nrechits);
  if (fInConfig.storeRecHits)
  {
    fInTree->SetBranchAddress(fInRecHits.s_eta.c_str(), &fInRecHits.eta);
    fInTree->SetBranchAddress(fInRecHits.s_phi.c_str(), &fInRecHits.phi);
    fInTree->SetBranchAddress(fInRecHits.s_E.c_str(), &fInRecHits.E);
    fInTree->SetBranchAddress(fInRecHits.s_time.c_str(), &fInRecHits.time);
    fInTree->SetBranchAddress(fInRecHits.s_OOT.c_str(), &fInRecHits.OOT);
    fInTree->SetBranchAddress(fInRecHits.s_ID.c_str(), &fInRecHits.ID);
  }

  fInTree->SetBranchAddress(fInEvent.s_nphotons.c_str(), &fInEvent.nphotons);
  for (Int_t ipho = 0; ipho < Config::nPhotons; ipho++) 
  {
    auto & pho = fInPhos[ipho];
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_E.c_str(),ipho), &pho.E);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_pt.c_str(),ipho), &pho.pt);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_eta.c_str(),ipho), &pho.eta);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_phi.c_str(),ipho), &pho.phi);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_scE.c_str(),ipho), &pho.scE);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_sceta.c_str(),ipho), &pho.sceta);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_scphi.c_str(),ipho), &pho.scphi);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_HoE.c_str(),ipho), &pho.HoE);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_r9.c_str(),ipho), &pho.r9);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_ChgHadIso.c_str(),ipho), &pho.ChgHadIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_NeuHadIso.c_str(),ipho), &pho.NeuHadIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_PhoIso.c_str(),ipho), &pho.PhoIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_EcalPFClIso.c_str(),ipho), &pho.EcalPFClIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_HcalPFClIso.c_str(),ipho), &pho.HcalPFClIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_TrkIso.c_str(),ipho), &pho.TrkIso);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_sieie.c_str(),ipho), &pho.sieie);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_sipip.c_str(),ipho), &pho.sipip);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_sieip.c_str(),ipho), &pho.sieip);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_smaj.c_str(),ipho), &pho.smaj);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_smin.c_str(),ipho), &pho.smin);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_alpha.c_str(),ipho), &pho.alpha);
    if (fInConfig.storeRecHits)
    {
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_seed.c_str(),ipho), &pho.seed);
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_recHits.c_str(),ipho), &pho.recHits);
    }
    else
    {
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_seedtime.c_str(),ipho), &pho.seedtime);
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_seedE.c_str(),ipho), &pho.seedE);
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_seedID.c_str(),ipho), &pho.seedID);;
    }
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_isOOT.c_str(),ipho), &pho.isOOT);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_isEB.c_str(),ipho), &pho.isEB);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_isHLT.c_str(),ipho), &pho.isHLT);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_isTrk.c_str(),ipho), &pho.isTrk);
    fInTree->SetBranchAddress(Form("%s_%i",pho.s_ID.c_str(),ipho), &pho.ID);
    
    if (fIsMC)
    {
      fInTree->SetBranchAddress(Form("%s_%i",pho.s_isGen.c_str(),ipho), &pho.isGen);
      if (fInConfig.isGMSB || fInConfig.isHVDS)
      {
	fInTree->SetBranchAddress(Form("%s_%i",pho.s_isSignal.c_str(),ipho), &pho.isSignal);
      }
    }
  }
}

void Skimmer::InitAndSetOutConfig()
{
  fOutConfigTree->Branch(fOutConfig.s_blindSF.c_str(), &fOutConfig.blindSF);
  fOutConfigTree->Branch(fOutConfig.s_applyBlindSF.c_str(), &fOutConfig.applyBlindSF);
  fOutConfigTree->Branch(fOutConfig.s_blindMET.c_str(), &fOutConfig.blindMET);
  fOutConfigTree->Branch(fOutConfig.s_applyBlindMET.c_str(), &fOutConfig.applyBlindMET);
  fOutConfigTree->Branch(fOutConfig.s_jetpTmin.c_str(), &fOutConfig.jetpTmin);
  fOutConfigTree->Branch(fOutConfig.s_jetIDmin.c_str(), &fOutConfig.jetIDmin);
  fOutConfigTree->Branch(fOutConfig.s_rhEmin.c_str(), &fOutConfig.rhEmin);
  fOutConfigTree->Branch(fOutConfig.s_phpTmin.c_str(), &fOutConfig.phpTmin);
  fOutConfigTree->Branch(fOutConfig.s_phIDmin.c_str(), &fOutConfig.phIDmin_s);
  fOutConfigTree->Branch(fOutConfig.s_seedTimemin.c_str(), &fOutConfig.seedTimemin);
  fOutConfigTree->Branch(fOutConfig.s_splitPho.c_str(), &fOutConfig.splitPho);
  fOutConfigTree->Branch(fOutConfig.s_onlyGED.c_str(), &fOutConfig.onlyGED);
  fOutConfigTree->Branch(fOutConfig.s_onlyOOT.c_str(), &fOutConfig.onlyOOT);
  fOutConfigTree->Branch(fOutConfig.s_storeRecHits.c_str(), &fOutConfig.storeRecHits);
  fOutConfigTree->Branch(fOutConfig.s_applyTrigger.c_str(), &fOutConfig.applyTrigger);
  fOutConfigTree->Branch(fOutConfig.s_minHT.c_str(), &fOutConfig.minHT);
  fOutConfigTree->Branch(fOutConfig.s_applyHT.c_str(), &fOutConfig.applyHT);
  fOutConfigTree->Branch(fOutConfig.s_phgoodpTmin.c_str(), &fOutConfig.phgoodpTmin);
  fOutConfigTree->Branch(fOutConfig.s_phgoodIDmin.c_str(), &fOutConfig.phgoodIDmin_s);
  fOutConfigTree->Branch(fOutConfig.s_applyPhGood.c_str(), &fOutConfig.applyPhGood);
  fOutConfigTree->Branch(fOutConfig.s_dRmin.c_str(), &fOutConfig.dRmin);
  fOutConfigTree->Branch(fOutConfig.s_pTres.c_str(), &fOutConfig.pTres);
  fOutConfigTree->Branch(fOutConfig.s_trackdRmin.c_str(), &fOutConfig.trackdRmin);
  fOutConfigTree->Branch(fOutConfig.s_trackpTmin.c_str(), &fOutConfig.trackpTmin);
  fOutConfigTree->Branch(fOutConfig.s_inputPaths.c_str(), &fOutConfig.inputPaths_s);
  fOutConfigTree->Branch(fOutConfig.s_inputFilters.c_str(), &fOutConfig.inputFilters_s);
  fOutConfigTree->Branch(fOutConfig.s_isGMSB.c_str(), &fOutConfig.isGMSB);
  fOutConfigTree->Branch(fOutConfig.s_isHVDS.c_str(), &fOutConfig.isHVDS);
  fOutConfigTree->Branch(fOutConfig.s_isBkgd.c_str(), &fOutConfig.isBkgd);
  fOutConfigTree->Branch(fOutConfig.s_xsec.c_str(), &fOutConfig.xsec);
  fOutConfigTree->Branch(fOutConfig.s_filterEff.c_str(), &fOutConfig.filterEff);
  fOutConfigTree->Branch(fOutConfig.s_BR.c_str(), &fOutConfig.BR);
  
  fOutConfig.blindSF = fInConfig.blindSF;
  fOutConfig.applyBlindSF = fInConfig.applyBlindSF;
  fOutConfig.blindMET = fInConfig.blindMET;
  fOutConfig.applyBlindMET = fInConfig.applyBlindMET;
  fOutConfig.jetpTmin = fInConfig.jetpTmin;
  fOutConfig.jetIDmin = fInConfig.jetIDmin;
  fOutConfig.rhEmin = fInConfig.rhEmin;
  fOutConfig.phpTmin = fInConfig.phpTmin;
  fOutConfig.phIDmin_s = fInConfig.phIDmin->c_str();
  fOutConfig.seedTimemin = fInConfig.seedTimemin;
  fOutConfig.splitPho = fInConfig.splitPho;
  fOutConfig.onlyGED = fInConfig.onlyGED;
  fOutConfig.onlyOOT = fInConfig.onlyOOT;
  fOutConfig.storeRecHits = false; // drop these now!
  fOutConfig.applyTrigger = fInConfig.applyTrigger;
  fOutConfig.minHT = fInConfig.minHT;
  fOutConfig.applyHT = fInConfig.applyHT;
  fOutConfig.phgoodpTmin = fInConfig.phgoodpTmin;
  fOutConfig.phgoodIDmin_s = fInConfig.phgoodIDmin->c_str();
  fOutConfig.applyPhGood = fInConfig.applyPhGood;
  fOutConfig.dRmin = fInConfig.dRmin;
  fOutConfig.pTres = fInConfig.pTres;
  fOutConfig.trackdRmin = fInConfig.trackdRmin;
  fOutConfig.trackpTmin = fInConfig.trackpTmin;
  fOutConfig.inputPaths_s = fInConfig.inputPaths->c_str();
  fOutConfig.inputFilters_s = fInConfig.inputFilters->c_str();
  fOutConfig.isGMSB = fInConfig.isGMSB;
  fOutConfig.isHVDS = fInConfig.isHVDS;
  fOutConfig.isBkgd = fInConfig.isBkgd;
  fOutConfig.xsec = fInConfig.xsec;
  fOutConfig.filterEff = fInConfig.filterEff;
  fOutConfig.BR = fInConfig.BR;
  
  fOutConfigTree->Fill();
}

void Skimmer::InitOutTree()
{
  if (fIsMC)
  {
    fOutTree->Branch(fOutEvent.s_genwgt.c_str(), &fOutEvent.genwgt);
    fOutTree->Branch(fOutEvent.s_genpuobs.c_str(), &fOutEvent.genpuobs);
    fOutTree->Branch(fOutEvent.s_genputrue.c_str(), &fOutEvent.genputrue);
    
    if (fOutConfig.isGMSB)
    {
      fOutTree->Branch(fOutEvent.s_nNeutoPhGr.c_str(), &fOutEvent.nNeutoPhGr);
      fOutGMSBs.resize(Config::nGMSBs);
      for (Int_t igmsb = 0; igmsb < Config::nGMSBs; igmsb++) 
      {
	auto & gmsb = fOutGMSBs[igmsb];
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNmass.c_str(),igmsb), &gmsb.genNmass);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNE.c_str(),igmsb), &gmsb.genNE);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNpt.c_str(),igmsb), &gmsb.genNpt);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNphi.c_str(),igmsb), &gmsb.genNphi);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNeta.c_str(),igmsb), &gmsb.genNeta);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNprodvx.c_str(),igmsb), &gmsb.genNprodvx);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNprodvy.c_str(),igmsb), &gmsb.genNprodvy);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNprodvz.c_str(),igmsb), &gmsb.genNprodvz);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNdecayvx.c_str(),igmsb), &gmsb.genNdecayvx);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNdecayvy.c_str(),igmsb), &gmsb.genNdecayvy);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genNdecayvz.c_str(),igmsb), &gmsb.genNdecayvz);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genphE.c_str(),igmsb), &gmsb.genphE);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genphpt.c_str(),igmsb), &gmsb.genphpt);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genphphi.c_str(),igmsb), &gmsb.genphphi);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genpheta.c_str(),igmsb), &gmsb.genpheta);
	fOutTree->Branch(Form("%s_%i",gmsb.s_genphmatch.c_str(),igmsb), &gmsb.genphmatch);
	fOutTree->Branch(Form("%s_%i",gmsb.s_gengrmass.c_str(),igmsb), &gmsb.gengrmass);
	fOutTree->Branch(Form("%s_%i",gmsb.s_gengrE.c_str(),igmsb), &gmsb.gengrE);
	fOutTree->Branch(Form("%s_%i",gmsb.s_gengrpt.c_str(),igmsb), &gmsb.gengrpt);
	fOutTree->Branch(Form("%s_%i",gmsb.s_gengrphi.c_str(),igmsb), &gmsb.gengrphi);
	fOutTree->Branch(Form("%s_%i",gmsb.s_gengreta.c_str(),igmsb), &gmsb.gengreta);
      } // end loop over neutralinos
    } // end block over gmsb

    if (fOutConfig.isHVDS)
    {
      fOutTree->Branch(fOutEvent.s_nvPions.c_str(), &fOutEvent.nvPions);
      fOutHVDSs.resize(Config::nHVDSs);
      for (Int_t ihvds = 0; ihvds < Config::nHVDSs; ihvds++) 
      {
	auto & hvds = fOutHVDSs[ihvds]; 
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionmass.c_str(),ihvds), &hvds.genvPionmass);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionE.c_str(),ihvds), &hvds.genvPionE);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionpt.c_str(),ihvds), &hvds.genvPionpt);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionphi.c_str(),ihvds), &hvds.genvPionphi);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPioneta.c_str(),ihvds), &hvds.genvPioneta);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionprodvx.c_str(),ihvds), &hvds.genvPionprodvx);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionprodvy.c_str(),ihvds), &hvds.genvPionprodvy);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPionprodvz.c_str(),ihvds), &hvds.genvPionprodvz);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPiondecayvx.c_str(),ihvds), &hvds.genvPiondecayvx);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPiondecayvy.c_str(),ihvds), &hvds.genvPiondecayvy);
	fOutTree->Branch(Form("%s_%i",hvds.s_genvPiondecayvz.c_str(),ihvds), &hvds.genvPiondecayvz);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph0E.c_str(),ihvds), &hvds.genHVph0E);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph0pt.c_str(),ihvds), &hvds.genHVph0pt);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph0phi.c_str(),ihvds), &hvds.genHVph0phi);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph0eta.c_str(),ihvds), &hvds.genHVph0eta);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph0match.c_str(),ihvds), &hvds.genHVph0match);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph1E.c_str(),ihvds), &hvds.genHVph1E);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph1pt.c_str(),ihvds), &hvds.genHVph1pt);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph1phi.c_str(),ihvds), &hvds.genHVph1phi);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph1eta.c_str(),ihvds), &hvds.genHVph1eta);
	fOutTree->Branch(Form("%s_%i",hvds.s_genHVph1match.c_str(),ihvds), &hvds.genHVph1match);
      } // end loop over nvpions 
    } // end block over hvds
  } // end block over isMC

  fOutTree->Branch(fOutEvent.s_run.c_str(), &fOutEvent.run);
  fOutTree->Branch(fOutEvent.s_lumi.c_str(), &fOutEvent.lumi);
  fOutTree->Branch(fOutEvent.s_event.c_str(), &fOutEvent.event);
  fOutTree->Branch(fOutEvent.s_hltSignal.c_str(), &fOutEvent.hltSignal);
  fOutTree->Branch(fOutEvent.s_hltRefPhoID.c_str(), &fOutEvent.hltRefPhoID);
  fOutTree->Branch(fOutEvent.s_hltRefDispID.c_str(), &fOutEvent.hltRefDispID);
  fOutTree->Branch(fOutEvent.s_hltPho50.c_str(), &fOutEvent.hltPho50);
  fOutTree->Branch(fOutEvent.s_nvtx.c_str(), &fOutEvent.nvtx);
  fOutTree->Branch(fOutEvent.s_vtxX.c_str(), &fOutEvent.vtxX);
  fOutTree->Branch(fOutEvent.s_vtxY.c_str(), &fOutEvent.vtxY);
  fOutTree->Branch(fOutEvent.s_vtxZ.c_str(), &fOutEvent.vtxZ);
  fOutTree->Branch(fOutEvent.s_rho.c_str(), &fOutEvent.rho);
  fOutTree->Branch(fOutEvent.s_t1pfMETpt.c_str(), &fOutEvent.t1pfMETpt);
  fOutTree->Branch(fOutEvent.s_t1pfMETphi.c_str(), &fOutEvent.t1pfMETphi);
  fOutTree->Branch(fOutEvent.s_t1pfMETsumEt.c_str(), &fOutEvent.t1pfMETsumEt);
  fOutTree->Branch(fOutEvent.s_jetHT.c_str(), &fOutEvent.jetHT);

  fOutTree->Branch(fOutEvent.s_njets.c_str(), &fOutEvent.njets);
  fOutJets.resize(Config::nJets);
  for (Int_t ijet = 0; ijet < Config::nJets; ijet++) 
  {
    auto & jet = fOutJets[ijet];
    fOutTree->Branch(Form("%s_%i",jet.s_E.c_str(),ijet), &jet.E);    
    fOutTree->Branch(Form("%s_%i",jet.s_pt.c_str(),ijet), &jet.pt);    
    fOutTree->Branch(Form("%s_%i",jet.s_phi.c_str(),ijet), &jet.phi);    
    fOutTree->Branch(Form("%s_%i",jet.s_eta.c_str(),ijet), &jet.eta);    
  }

  fOutTree->Branch(fOutEvent.s_nrechits.c_str(), &fOutEvent.nrechits);

  fOutTree->Branch(fOutEvent.s_nphotons.c_str(), &fOutEvent.nphotons);
  fOutPhos.resize(Config::nPhotons);
  for (Int_t ipho = 0; ipho < Config::nPhotons; ipho++) 
  {
    auto & pho = fOutPhos[ipho];
    fOutTree->Branch(Form("%s_%i",pho.s_E.c_str(),ipho), &pho.E);
    fOutTree->Branch(Form("%s_%i",pho.s_pt.c_str(),ipho), &pho.pt);
    fOutTree->Branch(Form("%s_%i",pho.s_eta.c_str(),ipho), &pho.eta);
    fOutTree->Branch(Form("%s_%i",pho.s_phi.c_str(),ipho), &pho.phi);
    fOutTree->Branch(Form("%s_%i",pho.s_scE.c_str(),ipho), &pho.scE);
    fOutTree->Branch(Form("%s_%i",pho.s_sceta.c_str(),ipho), &pho.sceta);
    fOutTree->Branch(Form("%s_%i",pho.s_scphi.c_str(),ipho), &pho.scphi);
    fOutTree->Branch(Form("%s_%i",pho.s_HoE.c_str(),ipho), &pho.HoE);
    fOutTree->Branch(Form("%s_%i",pho.s_r9.c_str(),ipho), &pho.r9);
    fOutTree->Branch(Form("%s_%i",pho.s_ChgHadIso.c_str(),ipho), &pho.ChgHadIso);
    fOutTree->Branch(Form("%s_%i",pho.s_NeuHadIso.c_str(),ipho), &pho.NeuHadIso);
    fOutTree->Branch(Form("%s_%i",pho.s_PhoIso.c_str(),ipho), &pho.PhoIso);
    fOutTree->Branch(Form("%s_%i",pho.s_EcalPFClIso.c_str(),ipho), &pho.EcalPFClIso);
    fOutTree->Branch(Form("%s_%i",pho.s_HcalPFClIso.c_str(),ipho), &pho.HcalPFClIso);
    fOutTree->Branch(Form("%s_%i",pho.s_TrkIso.c_str(),ipho), &pho.TrkIso);
    fOutTree->Branch(Form("%s_%i",pho.s_sieie.c_str(),ipho), &pho.sieie);
    fOutTree->Branch(Form("%s_%i",pho.s_sipip.c_str(),ipho), &pho.sipip);
    fOutTree->Branch(Form("%s_%i",pho.s_sieip.c_str(),ipho), &pho.sieip);
    fOutTree->Branch(Form("%s_%i",pho.s_smaj.c_str(),ipho), &pho.smaj);
    fOutTree->Branch(Form("%s_%i",pho.s_smin.c_str(),ipho), &pho.smin);
    fOutTree->Branch(Form("%s_%i",pho.s_alpha.c_str(),ipho), &pho.alpha);
    fOutTree->Branch(Form("%s_%i",pho.s_seedtime.c_str(),ipho), &pho.seedtime);
    fOutTree->Branch(Form("%s_%i",pho.s_seedE.c_str(),ipho), &pho.seedE);
    fOutTree->Branch(Form("%s_%i",pho.s_seedID.c_str(),ipho), &pho.seedID);;
    fOutTree->Branch(Form("%s_%i",pho.s_isOOT.c_str(),ipho), &pho.isOOT);
    fOutTree->Branch(Form("%s_%i",pho.s_isEB.c_str(),ipho), &pho.isEB);
    fOutTree->Branch(Form("%s_%i",pho.s_isHLT.c_str(),ipho), &pho.isHLT);
    fOutTree->Branch(Form("%s_%i",pho.s_isTrk.c_str(),ipho), &pho.isTrk);
    fOutTree->Branch(Form("%s_%i",pho.s_ID.c_str(),ipho), &pho.ID);

    if (fIsMC)
    {
      fOutTree->Branch(Form("%s_%i",pho.s_isGen.c_str(),ipho), &pho.isGen);
      if (fOutConfig.isGMSB || fOutConfig.isHVDS)
      {
	fOutTree->Branch(Form("%s_%i",pho.s_isSignal.c_str(),ipho), &pho.isSignal);
      }
    }
  }
} 

void Skimmer::InitOutCutFlow()
{
  // get cut flow labels
  const Int_t inNbinsX = fInCutFlow->GetNbinsX();
  for (Int_t ibin = 1; ibin <= inNbinsX; ibin++)
  {
    cutLabels[fInCutFlow->GetXaxis()->GetBinLabel(ibin)] = ibin;
  }
  cutLabels["skim"] = inNbinsX+1;

  // make new cut flow
  fOutCutFlow = new TH1F(Config::h_cutflowname.Data(),fInCutFlow->GetTitle(),cutLabels.size(),0,cutLabels.size());
  fOutCutFlow->Sumw2();

  for (const auto & cutlabel : cutLabels)
  {
    const Int_t ibin = cutlabel.second;

    fOutCutFlow->GetXaxis()->SetBinLabel(ibin,cutlabel.first.c_str());

    if (ibin > inNbinsX) continue;

    fOutCutFlow->SetBinContent(ibin,fInCutFlow->GetBinContent(ibin));
    fOutCutFlow->SetBinError(ibin,fInCutFlow->GetBinError(ibin));
  }
  fOutCutFlow->GetYaxis()->SetTitle(fInCutFlow->GetYaxis()->GetTitle());
}
