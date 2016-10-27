#include "PlotPhotons.hh"
#include "TSystem.h"
#include "TCanvas.h"
#include "TObject.h" 

#include <iostream>

PlotPhotons::PlotPhotons(TString filename, Bool_t isMC, TString outdir,
			 Bool_t applyjetptcut, Float_t jetptcut, Bool_t applyphptcut, Float_t phptcut,
			 Bool_t applyphvidcut, TString phvid, Bool_t applyrhecut, Float_t rhEcut,
			 Bool_t applyecalacceptcut) :
  fOutDir(outdir), fIsMC(isMC),
  fApplyJetPtCut(applyjetptcut), fJetPtCut(jetptcut), fApplyPhPtCut(applyphptcut), fPhPtCut(phptcut),
  fApplyPhVIDCut(applyphvidcut), fPhVID(phvid), fApplyrhECut(applyrhecut), frhECut(rhEcut),
  fApplyECALAcceptCut(applyecalacceptcut)
{
  // input
  fInFile = TFile::Open(filename.Data());
  fInTree = (TTree*)fInFile->Get("tree/tree");

  // initialize tree
  PlotPhotons::InitTree();

  // in routine initialization
  fNEvCheck = 1000;
  // make the vid maps!
  fPhVIDMap["loose"]  = 1;
  fPhVIDMap["medium"] = 2;
  fPhVIDMap["tight"]  = 3;

  // output
  // setup outdir name
  fOutDump = fOutDir; // --> simple directory for dump of plots

  if (!fApplyJetPtCut && !applyphptcut && !applyphvidcut && !applyrhecut)
  { 
    fOutDir += "/Inclusive";
  }
  else 
  {
    fOutDir += "/cuts";
    if (fApplyJetPtCut)      fOutDir += Form("_jetpt%3.1f",fJetPtCut);
    if (fApplyPhPtCut)       fOutDir += Form("_phpt%3.1f" ,fPhPtCut);
    if (fApplyPhVIDCut)      fOutDir += Form("_phVID%s"   ,fPhVID.Data());
    if (fApplyrhECut)        fOutDir += Form("_rhE%2.1f"  ,frhECut);
    if (fApplyECALAcceptCut) fOutDir += Form("_ecalaccept");
  }

  FileStat_t dummyFileStat; 
  if (gSystem->GetPathInfo(fOutDir.Data(), dummyFileStat) == 1)
  {
    TString mkDir = Form("mkdir -p %s",fOutDir.Data());
    gSystem->Exec(mkDir.Data());
  }

  fOutFile = new TFile(Form("%s/plots.root",fOutDir.Data()),"UPDATE");
}

PlotPhotons::~PlotPhotons()
{
  delete fInTree;
  delete fInFile;

  delete fOutFile;
}

void PlotPhotons::DoPlots()
{
  PlotPhotons::SetupPlots();
  PlotPhotons::EventLoop();
  PlotPhotons::MakeSubDirs();
  PlotPhotons::OutputTH1Fs();
  PlotPhotons::OutputTH2Fs();
}

void PlotPhotons::SetupPlots()
{
  if (fIsMC)
  {
    PlotPhotons::SetupGenInfo();
    PlotPhotons::SetupGenParticles();
    PlotPhotons::SetupGenJets();
  }
  PlotPhotons::SetupObjectCounts();
  PlotPhotons::SetupMET();
  PlotPhotons::SetupJets();
  PlotPhotons::SetupRecoPhotons();
}

void PlotPhotons::EventLoop()
{
  for (UInt_t entry = 0; entry < fInTree->GetEntries(); entry++)
  {
    fInTree->GetEntry(entry);
    if (entry%fNEvCheck == 0 || entry == 0) std::cout << "Entry " << entry << " out of " << fInTree->GetEntries() << std::endl;

    if (fIsMC)
    {
      PlotPhotons::FillGenInfo();
      PlotPhotons::FillGenParticles();
      PlotPhotons::FillGenJets();
    }
    PlotPhotons::FillObjectCounts();
    PlotPhotons::FillMET();
    PlotPhotons::FillJets();
    PlotPhotons::FillRecoPhotons();
  }
}

void PlotPhotons::FillGenInfo()
{
  fPlots["genpuobs"]->Fill(genpuobs);
  fPlots["genputrue"]->Fill(genputrue);
}

void PlotPhotons::FillGenParticles()
{
  fPlots["nNeutralino"]->Fill(nNeutralino);
  fPlots["nNeutoPhGr"]->Fill(nNeutoPhGr);

  fPlots["genN1mass"]->Fill(genN1mass);
  fPlots["genN1E"]->Fill(genN1E);
  fPlots["genN1pt"]->Fill(genN1pt);
  fPlots["genN1phi"]->Fill(genN1phi);
  fPlots["genN1eta"]->Fill(genN1eta);
  fPlots["genph1E"]->Fill(genph1E);
  fPlots["genph1pt"]->Fill(genph1pt);
  fPlots["genph1phi"]->Fill(genph1phi);
  fPlots["genph1eta"]->Fill(genph1eta);
  fPlots["gengr1E"]->Fill(gengr1E);
  fPlots["gengr1pt"]->Fill(gengr1pt);
  fPlots["gengr1phi"]->Fill(gengr1phi);
  fPlots["gengr1eta"]->Fill(gengr1eta);

  fPlots["genN2mass"]->Fill(genN2mass);
  fPlots["genN2E"]->Fill(genN2E);
  fPlots["genN2pt"]->Fill(genN2pt);
  fPlots["genN2phi"]->Fill(genN2phi);
  fPlots["genN2eta"]->Fill(genN2eta);
  fPlots["genph2E"]->Fill(genph2E);
  fPlots["genph2pt"]->Fill(genph2pt);
  fPlots["genph2phi"]->Fill(genph2phi);
  fPlots["genph2eta"]->Fill(genph2eta);
  fPlots["gengr2E"]->Fill(gengr2E);
  fPlots["gengr2pt"]->Fill(gengr2pt);
  fPlots["gengr2phi"]->Fill(gengr2phi);
  fPlots["gengr2eta"]->Fill(gengr2eta);

  fPlots2D["genN2EvsN1E"]->Fill(genN1E,genN2E);
}

void PlotPhotons::FillGenJets()
{
  fPlots["ngenjets"]->Fill(ngenjets);
  
  for (int igjet = 0; igjet < ngenjets; igjet++)
  {
    fPlots["genjetE"]->Fill((*genjetE)[igjet]);
    fPlots["genjetpt"]->Fill((*genjetpt)[igjet]);
    fPlots["genjetphi"]->Fill((*genjetphi)[igjet]);
    fPlots["genjeteta"]->Fill((*genjeteta)[igjet]);
  }
}

void PlotPhotons::FillObjectCounts()
{
  fPlots["nvtx"]->Fill(nvtx);
 
  int nMatchedJets = 0;
  int nJets = 0;
  for (int ijet = 0; ijet < njets; ijet++)
  {
    if (fApplyJetPtCut && (*jetE)[ijet] < fJetPtCut) continue;
    nJets++;
    if (fIsMC)
    {
      if ( (*jetmatch)[ijet] >= 0 ) nMatchedJets++; 
    }
  }
  fPlots["njets"]->Fill(nJets);  
  if (fIsMC) fPlots["nmatchedjets"]->Fill(nMatchedJets);  

  int nPhotons = 0;
  int nLoosePh = 0;
  int nMediumPh = 0;
  int nTightPh = 0;
  for (int iph = 0; iph < nphotons; iph++)
  { 
    if (fApplyPhPtCut && (*phE)[iph] < fPhPtCut) continue;
    if (fApplyECALAcceptCut && (std::abs((*pheta)[iph]) > 2.5 || (std::abs((*pheta)[iph]) > 1.4442 && std::abs((*pheta)[iph]) < 1.566))) continue;
    nPhotons++;
    if ( (*phVID)[iph] >= 1 ) nLoosePh++;
    if ( (*phVID)[iph] >= 2 ) nMediumPh++;
    if ( (*phVID)[iph] >= 3 ) nTightPh++;
  }
  fPlots["nphotons"]->Fill(nPhotons);  
  fPlots["nlooseph"]->Fill(nLoosePh);
  fPlots["nmediumph"]->Fill(nMediumPh);
  fPlots["ntightph"]->Fill(nTightPh);
}

void PlotPhotons::FillMET()
{
  fPlots["t1pfMETpt"]->Fill(t1pfMETpt);
  fPlots["t1pfMETphi"]->Fill(t1pfMETphi);
  fPlots["t1pfMETsumEt"]->Fill(t1pfMETsumEt);
  fPlots["t1pfMETuncorpt"]->Fill(t1pfMETuncorpt);
  fPlots["t1pfMETuncorphi"]->Fill(t1pfMETuncorphi);
  fPlots["t1pfMETuncorsumEt"]->Fill(t1pfMETuncorsumEt);
  fPlots["t1pfMETcalopt"]->Fill(t1pfMETcalopt);
  fPlots["t1pfMETcalophi"]->Fill(t1pfMETcalophi);
  fPlots["t1pfMETcalosumEt"]->Fill(t1pfMETcalosumEt);
  if (fIsMC)
  {
    fPlots["t1pfMETgenMETpt"]->Fill(t1pfMETgenMETpt);
    fPlots["t1pfMETgenMETphi"]->Fill(t1pfMETgenMETphi);
    fPlots["t1pfMETgenMETsumEt"]->Fill(t1pfMETgenMETsumEt);
  }
}

void PlotPhotons::FillJets()
{
  for (int ijet = 0; ijet < njets; ijet++)
  {
    if (fApplyJetPtCut && (*jetE)[ijet] < fJetPtCut) continue;
    fPlots["jetE"]->Fill((*jetE)[ijet]);
    fPlots["jetpt"]->Fill((*jetpt)[ijet]);
    fPlots["jetphi"]->Fill((*jetphi)[ijet]);
    fPlots["jeteta"]->Fill((*jeteta)[ijet]);
  
    if (fIsMC)
    {
      if ( (*jetmatch)[ijet] >= 0 ) 
      {
	fPlots["jetE_gen"]->Fill((*jetE)[ijet]);
	fPlots["jetpt_gen"]->Fill((*jetpt)[ijet]);
	fPlots["jetphi_gen"]->Fill((*jetphi)[ijet]);
	fPlots["jeteta_gen"]->Fill((*jeteta)[ijet]);
      }
    }
  }
}

void PlotPhotons::FillRecoPhotons()
{
  for (int iph = 0; iph < nphotons; iph++)
  {
    if (fApplyPhPtCut && (*phE)[iph] < fPhPtCut) continue;
    if (fApplyPhVIDCut && (fPhVIDMap[fPhVID] < (*phVID)[iph])) continue;
    if (fApplyECALAcceptCut && (std::abs((*pheta)[iph]) > 2.5 || (std::abs((*pheta)[iph]) > 1.4442 && std::abs((*pheta)[iph]) < 1.566))) continue;

    fPlots["phE"]->Fill((*phE)[iph]);
    fPlots["phpt"]->Fill((*phpt)[iph]);
    fPlots["phphi"]->Fill((*phphi)[iph]);
    fPlots["pheta"]->Fill((*pheta)[iph]);
    fPlots["phscE"]->Fill((*phscE)[iph]);

    if (fIsMC) 
    {
      if ( (*phmatch)[iph] > 0 ) // gen matched photons
      {
	fPlots["phE_gen"]->Fill((*phE)[iph]);
	fPlots["phpt_gen"]->Fill((*phpt)[iph]);
	fPlots["phphi_gen"]->Fill((*phphi)[iph]);
	fPlots["pheta_gen"]->Fill((*pheta)[iph]);
	fPlots["phscE_gen"]->Fill((*phscE)[iph]);
      }
    }
  
    // loop over rechits (+ seed info)
    int nRecHits = 0;
    int nRecHits_gen = 0;
    for (int irh = 0; irh < (*phnrhs)[iph]; irh++)
    {
      if (fApplyrhECut && (*phrhEs)[iph][irh] < frhECut) continue;
      nRecHits++;

      fPlots["phrhEs"]->Fill((*phrhEs)[iph][irh]);
      fPlots["phrhdelRs"]->Fill((*phrhdelRs)[iph][irh]);
      fPlots["phrhtimes"]->Fill((*phrhtimes)[iph][irh]);
      fPlots["phrhOOTs"]->Fill((*phrhOOTs)[iph][irh]);
      if ( (*phseedpos)[iph] == irh ) // seed info
      {
	fPlots["phseedE"]->Fill((*phrhEs)[iph][irh]);
	fPlots["phseeddelR"]->Fill((*phrhdelRs)[iph][irh]);
	fPlots["phseedtime"]->Fill((*phrhtimes)[iph][irh]);
	fPlots["phseedOOT"]->Fill((*phrhOOTs)[iph][irh]);
      }
      
      if (fIsMC)
      {
	if ( (*phmatch)[iph] > 0 ) // gen matched photons
        {
	  nRecHits_gen++;
	  fPlots["phrhEs_gen"]->Fill((*phrhEs)[iph][irh]);
	  fPlots["phrhdelRs_gen"]->Fill((*phrhdelRs)[iph][irh]);
	  fPlots["phrhtimes_gen"]->Fill((*phrhtimes)[iph][irh]);
	  fPlots["phrhOOTs_gen"]->Fill((*phrhOOTs)[iph][irh]);
	  if ( (*phseedpos)[iph] == irh ) // seed info
	  {
	    fPlots["phseedE_gen"]->Fill((*phrhEs)[iph][irh]);
	    fPlots["phseeddelR_gen"]->Fill((*phrhdelRs)[iph][irh]);
	    fPlots["phseedtime_gen"]->Fill((*phrhtimes)[iph][irh]);
	    fPlots["phseedOOT_gen"]->Fill((*phrhOOTs)[iph][irh]);
	  }
	}
      } // end conditional over isMC
    } // end loop over nrechits
    fPlots["phnrhs"]->Fill(nRecHits);
    if (fIsMC) fPlots["phnrhs_gen"]->Fill(nRecHits_gen);
  } // end loop over nphotons
}

void PlotPhotons::SetupGenInfo()
{
  fPlots["genpuobs"] = PlotPhotons::MakeTH1F("genpuobs","Generator N PU Observed",100,0.f,100.f,"nPU (obs)","Events","generator");
  fPlots["genputrue"] = PlotPhotons::MakeTH1F("genputrue","Generator N PU (true)",100,0.f,100.f,"nPU (true)","Events","generator");
}

void PlotPhotons::SetupGenParticles()
{
  fPlots["nNeutralino"] = PlotPhotons::MakeTH1F("nNeutralino","Generator nNeutralino [Max of 2]",3,0.f,3.f,"nNeutralinos","Events","GenParticles");
  fPlots["nNeutoPhGr"]  = PlotPhotons::MakeTH1F("nNeutoPhGr","Generator n(Neutralino -> Photon + Gravitino) [Max of 2]",3,0.f,3.f,"nNeutoPhGr","Events","GenParticles");

  fPlots["genN1mass"] = PlotPhotons::MakeTH1F("genN1mass","Generator Leading Neutralino Mass [GeV]",100,0.f,500.f,"Mass [GeV/c^{2}]","Neutralinos","GenParticles");
  fPlots["genN1E"] = PlotPhotons::MakeTH1F("genN1E","Generator Leading Neutralino E [GeV]",100,0.f,2500.f,"Energy [GeV]","Neutralinos","GenParticles");
  fPlots["genN1pt"] = PlotPhotons::MakeTH1F("genN1pt","Generator Leading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Neutralinos","GenParticles");
  fPlots["genN1phi"] = PlotPhotons::MakeTH1F("genN1phi","Generator Leading Neutralino #phi",100,-3.2,3.2,"#phi","Neutralinos","GenParticles");
  fPlots["genN1eta"] = PlotPhotons::MakeTH1F("genN1eta","Generator Leading Neutralino #eta",100,-6.0,6.0,"#eta","Neutralinos","GenParticles");
  fPlots["genph1E"] = PlotPhotons::MakeTH1F("genph1E","Generator Leading Photon E [GeV]",100,0.f,2500.f,"Energy [GeV]","Photons","GenParticles");
  fPlots["genph1pt"] = PlotPhotons::MakeTH1F("genph1pt","Generator Leading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Photons","GenParticles");
  fPlots["genph1phi"] = PlotPhotons::MakeTH1F("genph1phi","Generator Leading Photon #phi",100,-3.2,3.2,"#phi","Photons","GenParticles");
  fPlots["genph1eta"] = PlotPhotons::MakeTH1F("genph1eta","Generator Leading Photon #eta",100,-6.0,6.0,"#eta","Photons","GenParticles");
  fPlots["gengr1E"] = PlotPhotons::MakeTH1F("gengr1E","Generator Leading Gravitino E [GeV]",100,0.f,2500.f,"Energy [GeV]","Gravitinos","GenParticles");
  fPlots["gengr1pt"] = PlotPhotons::MakeTH1F("gengr1pt","Generator Leading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Gravitinos","GenParticles");
  fPlots["gengr1phi"] = PlotPhotons::MakeTH1F("gengr1phi","Generator Leading Gravitino #phi",100,-3.2,3.2,"#phi","Gravitinos","GenParticles");
  fPlots["gengr1eta"] = PlotPhotons::MakeTH1F("gengr1eta","Generator Leading Gravitino #eta",100,-6.0,6.0,"#eta","Gravitinos","GenParticles");

  fPlots["genN2mass"] = PlotPhotons::MakeTH1F("genN2mass","Generator Subleading Neutralino Mass [GeV]",100,0.f,500.f,"Mass [GeV/c^{2}]","Neutralinos","GenParticles");
  fPlots["genN2E"] = PlotPhotons::MakeTH1F("genN2E","Generator Subleading Neutralino E [GeV]",100,0.f,2500.f,"Energy [GeV]","Neutralinos","GenParticles");
  fPlots["genN2pt"] = PlotPhotons::MakeTH1F("genN2pt","Generator Subleading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Neutralinos","GenParticles");
  fPlots["genN2phi"] = PlotPhotons::MakeTH1F("genN2phi","Generator Subleading Neutralino #phi",100,-3.2,3.2,"#phi","Neutralinos","GenParticles");
  fPlots["genN2eta"] = PlotPhotons::MakeTH1F("genN2eta","Generator Subleading Neutralino #eta",100,-6.0,6.0,"#eta","Neutralinos","GenParticles");
  fPlots["genph2E"] = PlotPhotons::MakeTH1F("genph2E","Generator Subleading Photon E [GeV]",100,0.f,2500.f,"Energy [GeV]","Photons","GenParticles");
  fPlots["genph2pt"] = PlotPhotons::MakeTH1F("genph2pt","Generator Subleading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Photons","GenParticles");
  fPlots["genph2phi"] = PlotPhotons::MakeTH1F("genph2phi","Generator Subleading Photon #phi",100,-3.2,3.2,"#phi","Photons","GenParticles");
  fPlots["genph2eta"] = PlotPhotons::MakeTH1F("genph2eta","Generator Subleading Photon #eta",100,-6.0,6.0,"#eta","Photons","GenParticles");
  fPlots["gengr2E"] = PlotPhotons::MakeTH1F("gengr2E","Generator Subleading Gravitino E [GeV]",100,0.f,2500.f,"Energy [GeV]","Gravitinos","GenParticles");
  fPlots["gengr2pt"] = PlotPhotons::MakeTH1F("gengr2pt","Generator Subleading p_{T} [GeV/c]",100,0.f,2500.f,"p_{T} [GeV/c]","Gravitinos","GenParticles");
  fPlots["gengr2phi"] = PlotPhotons::MakeTH1F("gengr2phi","Generator Subleading Gravitino #phi",100,-3.2,3.2,"#phi","Gravitinos","GenParticles");
  fPlots["gengr2eta"] = PlotPhotons::MakeTH1F("gengr2eta","Generator Subleading Gravitino #eta",100,-6.0,6.0,"#eta","Gravitinos","GenParticles");
  
  // 2D Histogram
  fPlots2D["genN2EvsN1E"] = PlotPhotons::MakeTH2F("genN2EvsN1E","Generator Neutralino2 E vs Neutralino1 E [GeV]",100,0.f,2500.f,"Neutralino1 Energy [GeV]",100,0.f,2500.f,"Neutralino2 Energy [GeV]","GenParticles");
}

void PlotPhotons::SetupGenJets()
{
  fPlots["ngenjets"] = PlotPhotons::MakeTH1F("ngenjets","nGenJets",40,0.f,40.f,"nGenJets","Events","GenJets");
  fPlots["genjetE"] = PlotPhotons::MakeTH1F("genjetE","Generator Jets Energy [GeV]",100,0.f,3000.f,"Energy [GeV]","Generator Jets","GenJets");
  fPlots["genjetpt"] = PlotPhotons::MakeTH1F("genjetpt","Generator Jets p_{T} [GeV/c]",100,0.f,3000.f,"p_{T} [GeV/c]","Generator Jets","GenJets");
  fPlots["genjetphi"] = PlotPhotons::MakeTH1F("genjetphi","Generator Jets #phi",100,-3.2,3.2,"#phi","Generator Jets","GenJets");
  fPlots["genjeteta"] = PlotPhotons::MakeTH1F("genjeteta","Generator Jets #eta",100,-6.0,6.0,"#eta","Generator Jets","GenJets");
}

void PlotPhotons::SetupObjectCounts()
{
  fPlots["nvtx"] = PlotPhotons::MakeTH1F("nvtx","nVertices (reco)",100,0.f,100.f,"nPV","Events","nReco");
  fPlots["njets"] = PlotPhotons::MakeTH1F("njets","nJets (reco)",40,0.f,40.f,"nJets","Events","nReco");
  if (fIsMC) fPlots["nmatchedjets"] = PlotPhotons::MakeTH1F("nmatchedjets","nMatchedJets (reco to gen)",40,0.f,40.f,"nMatchedJets","Events","nReco");
  fPlots["nphotons"] = PlotPhotons::MakeTH1F("nphotons","nPhotons (reco)",20,0.f,20.f,"nPhotons","Events","nReco");
  fPlots["nlooseph"] = PlotPhotons::MakeTH1F("nlooseph","nLoosePhotons",20,0.f,20.f,"nLoosePhotons","Events","nReco");
  fPlots["nmediumph"] = PlotPhotons::MakeTH1F("nmediumph","nMediumPhotons",20,0.f,20.f,"nMediumPhotons","Events","nReco");
  fPlots["ntightph"] = PlotPhotons::MakeTH1F("ntightph","nTightPhotons",20,0.f,20.f,"nTightPhotons","Events","nReco");
}

void PlotPhotons::SetupMET()
{
  fPlots["t1pfMETpt"] = PlotPhotons::MakeTH1F("t1pfMETpt","Type1 PF MET [GeV]",100,0.f,2000.f,"MET [GeV]","Events","MET");
  fPlots["t1pfMETphi"] = PlotPhotons::MakeTH1F("t1pfMETphi","Type1 PF MET #phi",100,-3.2,3.2,"#phi","Events","MET");
  fPlots["t1pfMETsumEt"] = PlotPhotons::MakeTH1F("t1pfMETsumEt","Type1 PF MET #Sigma E_{T} [GeV]",100,0.f,8000.f,"#Sigma E_{T} [GeV]","Events","MET");
  fPlots["t1pfMETuncorpt"] = PlotPhotons::MakeTH1F("t1pfMETuncorpt","Type1 PF MET (Raw) [GeV]",100,0.f,2000.f,"MET [GeV]","Events","MET");
  fPlots["t1pfMETuncorphi"] = PlotPhotons::MakeTH1F("t1pfMETuncorphi","Type1 PF MET (Raw) #phi",100,-3.2,3.2,"#phi","Events","MET");
  fPlots["t1pfMETuncorsumEt"] = PlotPhotons::MakeTH1F("t1pfMETuncorsumEt","Type1 PF MET (Raw) #Sigma E_{T} [GeV]",100,0.f,8000.f,"#Sigma E_{T} [GeV]","Events","MET");
  fPlots["t1pfMETcalopt"] = PlotPhotons::MakeTH1F("t1pfMETcalopt","Type1 PF MET (Calo) [GeV]",100,0.f,8000.f,"MET [GeV]","Events","MET");
  fPlots["t1pfMETcalophi"] = PlotPhotons::MakeTH1F("t1pfMETcalophi","Type1 PF MET (Calo) #phi",100,-3.2,3.2,"#phi","Events","MET");
  fPlots["t1pfMETcalosumEt"] = PlotPhotons::MakeTH1F("t1pfMETcalosumEt","Type1 PF MET (Calo) #Sigma E_{T} [GeV]",100,0.f,8000.f,"#Sigma E_{T} [GeV]","Events","MET");
  if (fIsMC)
  {
    fPlots["t1pfMETgenMETpt"] = PlotPhotons::MakeTH1F("t1pfMETgenMETpt","Type1 PF Gen MET [GeV]",100,0.f,2000.f,"MET [GeV]","Events","MET");
    fPlots["t1pfMETgenMETphi"] = PlotPhotons::MakeTH1F("t1pfMETgenMETphi","Type1 PF Gen MET #phi",100,-3.2,3.2,"#phi","Events","MET");
    fPlots["t1pfMETgenMETsumEt"] = PlotPhotons::MakeTH1F("t1pfMETgenMETsumEt","Type1 PF Gen MET #Sigma E_{T} [GeV]",100,0.f,8000.f,"#Sigma E_{T} [GeV]","Events","MET");
  }
}

void PlotPhotons::SetupJets()
{
  fPlots["jetE"] = PlotPhotons::MakeTH1F("jetE","Jets Energy [GeV] (reco)",100,0.f,3000.f,"Energy [GeV]","Jets","AK4Jets");
  fPlots["jetpt"] = PlotPhotons::MakeTH1F("jetpt","Jets p_{T} [GeV/c] (reco)",100,0.f,3000.f,"p_{T} [GeV/c]","Jets","AK4Jets");
  fPlots["jetphi"] = PlotPhotons::MakeTH1F("jetphi","Jets #phi (reco)",100,-3.2,3.2,"#phi","Jets","AK4Jets");
  fPlots["jeteta"] = PlotPhotons::MakeTH1F("jeteta","Jets #eta (reco)",100,-6.0,6.0,"#eta","Jets","AK4Jets");

  if (fIsMC) 
  {
    fPlots["jetE_gen"] = PlotPhotons::MakeTH1F("jetE_gen","Jets Energy [GeV] (reco: gen matched)",100,0.f,3000.f,"Energy [GeV]","Jets","AK4Jets");
    fPlots["jetpt_gen"] = PlotPhotons::MakeTH1F("jetpt_gen","Jets p_{T} [GeV/c] (reco: gen matched)",100,0.f,3000.f,"p_{T} [GeV/c]","Jets","AK4Jets");
    fPlots["jetphi_gen"] = PlotPhotons::MakeTH1F("jetphi_gen","Jets #phi (reco: gen matched)",100,-3.2,3.2,"#phi","Jets","AK4Jets");
    fPlots["jeteta_gen"] = PlotPhotons::MakeTH1F("jeteta_gen","Jets #eta (reco: gen matched)",100,-6.0,6.0,"#eta","Jets","AK4Jets");
  }
}

void PlotPhotons::SetupRecoPhotons()
{
  // All reco photons
  fPlots["phE"] = PlotPhotons::MakeTH1F("phE","Photons Energy [GeV] (reco)",100,0.f,2500.f,"Energy [GeV]","Photons","RecoPhotons");
  fPlots["phpt"] = PlotPhotons::MakeTH1F("phpt","Photons p_{T} [GeV/c] (reco)",100,0.f,2500.f,"p_{T} [GeV/c]","Photons","RecoPhotons");
  fPlots["phphi"] = PlotPhotons::MakeTH1F("phphi","Photons #phi (reco)",100,-3.2,3.2,"#phi","Photons","RecoPhotons");
  fPlots["pheta"] = PlotPhotons::MakeTH1F("pheta","Photons #eta (reco)",100,-5.0,5.0,"#eta","Photons","RecoPhotons");
  fPlots["phscE"] = PlotPhotons::MakeTH1F("phscE","Photons SuperCluster Energy [GeV] (reco)",100,0.f,2500.f,"Energy [GeV]","Photons","RecoPhotons");
  fPlots["phnrhs"] = PlotPhotons::MakeTH1F("phnrhs","nRecHits from Photons (reco)",100,0.f,100.f,"nRecHits","Photons","RecoPhotons");
  
  // all rec hits + seed info
  fPlots["phrhEs"] = PlotPhotons::MakeTH1F("phrhEs","Photons RecHits Energy [GeV] (reco)",100,0.f,2000.f,"Energy [GeV]","RecHits","RecoPhotons");
  fPlots["phrhdelRs"] = PlotPhotons::MakeTH1F("phrhdelRs","#DeltaR of RecHits to Photon (reco)",100,0.f,1.0f,"Time [ns]","RecHits","RecoPhotons");
  fPlots["phrhtimes"] = PlotPhotons::MakeTH1F("phrhtimes","Photons RecHits Time [ns] (reco)",200,-100.f,100.f,"Time [ns]","RecHits","RecoPhotons");
  fPlots["phrhOOTs"] = PlotPhotons::MakeTH1F("phrhOOTs","Photons RecHits OoT Flag (reco)",2,0.f,2.f,"OoT Flag","RecHits","RecoPhotons");
  fPlots["phseedE"] = PlotPhotons::MakeTH1F("phseedE","Photons Seed RecHit Energy [GeV] (reco)",100,0.f,2000.f,"Energy [GeV]","Seed RecHits","RecoPhotons");
  fPlots["phseeddelR"] = PlotPhotons::MakeTH1F("phseeddelR","#DeltaR of Seed RecHit to Photon (reco)",100,0.f,1.0f,"Time [ns]","Seed RecHits","RecoPhotons");
  fPlots["phseedtime"] = PlotPhotons::MakeTH1F("phseedtime","Photons Seed RecHit Time [ns] (reco)",200,-10.f,10.f,"Time [ns]","Seed RecHits","RecoPhotons");
  fPlots["phseedOOT"] = PlotPhotons::MakeTH1F("phseedOOT","Photons Seed RecHit OoT Flag (reco)",2,0.f,2.f,"OoT Flag","Seed RecHits","RecoPhotons");  

  if (fIsMC)
  {
    // Gen matched reco quantities
    fPlots["phE_gen"] = PlotPhotons::MakeTH1F("phE_gen","Photons Energy [GeV] (reco: gen matched)",100,0.f,2000.f,"Energy [GeV]","Photons","RecoPhotons");
    fPlots["phpt_gen"] = PlotPhotons::MakeTH1F("phpt_gen","Photons p_{T} [GeV/c] (reco: gen matched)",100,0.f,2000.f,"p_{T} [GeV/c]","Photons","RecoPhotons");
    fPlots["phphi_gen"] = PlotPhotons::MakeTH1F("phphi_gen","Photons #phi (reco: gen matched)",100,-3.2,3.2,"#phi","Photons","RecoPhotons");
    fPlots["pheta_gen"] = PlotPhotons::MakeTH1F("pheta_gen","Photons #eta (reco: gen matched)",100,-5.0,5.0,"#eta","Photons","RecoPhotons");
    fPlots["phscE_gen"] = PlotPhotons::MakeTH1F("phscE_gen","Photons SuperCluster Energy [GeV] (reco)",100,0.f,2000.f,"Energy [GeV]","Photons","RecoPhotons");
    fPlots["phnrhs_gen"] = PlotPhotons::MakeTH1F("phnrhs_gen","nRecHits from Photons (reco: gen matched)",100,0.f,100.f,"nRecHits","Photons","RecoPhotons");
    
    // all rec hits + seed info for gen matched photons
    fPlots["phrhEs_gen"] = PlotPhotons::MakeTH1F("phrhEs_gen","Photons RecHits Energy [GeV] (reco: gen matched)",100,0.f,2000.f,"Energy [GeV]","RecHits","RecoPhotons");
    fPlots["phrhdelRs_gen"] = PlotPhotons::MakeTH1F("phrhdelRs_gen","#DeltaR of RecHits to Photon (reco: gen matched)",100,0.f,1.0f,"Time [ns]","RecHits","RecoPhotons");
    fPlots["phrhtimes_gen"] = PlotPhotons::MakeTH1F("phrhtimes_gen","Photons RecHits Time [ns] (reco: gen matched)",200,-100.f,100.f,"Time [ns]","RecHits","RecoPhotons");
    fPlots["phrhOOTs_gen"] = PlotPhotons::MakeTH1F("phrhOOTs_gen","Photons RecHits OoT Flag (reco: gen matched)",2,0.f,2.f,"OoT Flag","RecHits","RecoPhotons");
    fPlots["phseedE_gen"] = PlotPhotons::MakeTH1F("phseedE_gen","Photons Seed RecHit Energy [GeV] (reco: gen matched)",100,0.f,2000.f,"Energy [GeV]","Seed RecHits","RecoPhotons");
    fPlots["phseeddelR_gen"] = PlotPhotons::MakeTH1F("phseeddelR_gen","#DeltaR of Seed RecHit to Photon (reco: gen matched)",100,0.f,1.0f,"Time [ns]","Seed RecHits","RecoPhotons");
    fPlots["phseedtime_gen"] = PlotPhotons::MakeTH1F("phseedtime_gen","Photons Seed RecHit Time [ns] (reco: gen matched)",200,-10.f,10.f,"Time [ns]","Seed RecHits","RecoPhotons");
    fPlots["phseedOOT_gen"] = PlotPhotons::MakeTH1F("phseedOOT_gen","Photons Seed RecHit OoT Flag (reco: gen matched)",2,0.f,2.f,"OoT Flag","Seed RecHits","RecoPhotons");  
  }
}

TH1F * PlotPhotons::MakeTH1F(TString hname, TString htitle, Int_t nbinsx, Float_t xlow, Float_t xhigh, TString xtitle, TString ytitle, TString subdir)
{
  TH1F * hist = new TH1F(hname.Data(),htitle.Data(),nbinsx,xlow,xhigh);
  hist->SetLineColor(kBlack);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();

  fSubDirs[hname] = subdir;

  return hist;
}

TH2F * PlotPhotons::MakeTH2F(TString hname, TString htitle, Int_t nbinsx, Float_t xlow, Float_t xhigh, TString xtitle, Int_t nbinsy, Float_t ylow, Float_t yhigh, TString ytitle, TString subdir)
{
  TH2F * hist = new TH2F(hname.Data(),htitle.Data(),nbinsx,xlow,xhigh,nbinsy,ylow,yhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();

  fSubDirs[hname] = subdir;

  return hist;
}

void PlotPhotons::MakeSubDirs()
{
  for (TStrMapIter mapiter = fSubDirs.begin(); mapiter != fSubDirs.end(); ++mapiter)
  {
    TString subdir = Form("%s/%s",fOutDir.Data(),mapiter->second.Data());

    FileStat_t dummyFileStat; 
    if (gSystem->GetPathInfo(subdir.Data(), dummyFileStat) == 1)
    {
      TString mkDir = Form("mkdir -p %s",subdir.Data());
      gSystem->Exec(mkDir.Data());
      gSystem->Exec(Form("%s/lin",mkDir.Data()));
      gSystem->Exec(Form("%s/log",mkDir.Data()));
    }
  }
}

void PlotPhotons::OutputTH1Fs()
{
  fOutFile->cd();

  for (TH1MapIter mapiter = fPlots.begin(); mapiter != fPlots.end(); ++mapiter) 
  { 
    // save to output file
    mapiter->second->Write(mapiter->second->GetName(),TObject::kWriteDelete); 
    
    // now draw onto canvas to save as png
    TCanvas * canv = new TCanvas("canv","canv");
    canv->cd();
    mapiter->second->Draw("HIST");
    
    // first save as linear, then log
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s/lin/%s.png",fOutDir.Data(),fSubDirs[mapiter->first].Data(),mapiter->first.Data()));
    canv->SaveAs(Form("%s/%s_lin.png",fOutDump.Data(),mapiter->first.Data()));

    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s/log/%s.png",fOutDir.Data(),fSubDirs[mapiter->first].Data(),mapiter->first.Data()));
    canv->SaveAs(Form("%s/%s_log.png",fOutDump.Data(),mapiter->first.Data()));

    delete canv;
    delete mapiter->second;
  }
  fPlots.clear();
}

void PlotPhotons::OutputTH2Fs()
{
  fOutFile->cd();

  for (TH2MapIter mapiter = fPlots2D.begin(); mapiter != fPlots2D.end(); ++mapiter) 
  { 
    // save to output file
    mapiter->second->Write(mapiter->second->GetName(),TObject::kWriteDelete); 
    
    // now draw onto canvas to save as png
    TCanvas * canv = new TCanvas("canv","canv");
    canv->cd();
    mapiter->second->Draw("colz");
    
    // first save as linear, then log
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s/lin/%s.png",fOutDir.Data(),fSubDirs[mapiter->first].Data(),mapiter->first.Data()));
    canv->SaveAs(Form("%s/%s_lin.png",fOutDump.Data(),mapiter->first.Data()));

    delete canv;
    delete mapiter->second;
  }
  fPlots2D.clear();
}

void PlotPhotons::InitTree()
{
  // Set object pointer
  genjetmatch = 0;
  genjetE = 0;
  genjetpt = 0;
  genjetphi = 0;
  genjeteta = 0;
  jetmatch = 0;
  jetE = 0;
  jetpt = 0;
  jetphi = 0;
  jeteta = 0;
  phmatch = 0;
  phVID = 0;
  phE = 0;
  phpt = 0;
  phphi = 0;
  pheta = 0;
  phscX = 0;
  phscY = 0;
  phscZ = 0;
  phscE = 0;
  phnrhs = 0;
  phseedpos = 0;
  phrhXs = 0;
  phrhYs = 0;
  phrhZs = 0;
  phrhEs = 0;
  phrhdelRs = 0;
  phrhtimes = 0;
  phrhIDs = 0;
  phrhOOTs = 0;

  // set branch addresses
  fInTree->SetBranchAddress("event", &event, &b_event);
  fInTree->SetBranchAddress("run", &run, &b_run);
  fInTree->SetBranchAddress("lumi", &lumi, &b_lumi);
  if (fIsMC)
  {
    fInTree->SetBranchAddress("genwgt", &genwgt, &b_genwgt);
    fInTree->SetBranchAddress("genpuobs", &genpuobs, &b_genpuobs);
    fInTree->SetBranchAddress("genputrue", &genputrue, &b_genputrue);
    fInTree->SetBranchAddress("nNeutralino", &nNeutralino, &b_nNeutralino);
    fInTree->SetBranchAddress("nNeutoPhGr", &nNeutoPhGr, &b_nNeutoPhGr);
    fInTree->SetBranchAddress("genN1mass", &genN1mass, &b_genN1mass);
    fInTree->SetBranchAddress("genN1E", &genN1E, &b_genN1E);
    fInTree->SetBranchAddress("genN1pt", &genN1pt, &b_genN1pt);
    fInTree->SetBranchAddress("genN1phi", &genN1phi, &b_genN1phi);
    fInTree->SetBranchAddress("genN1eta", &genN1eta, &b_genN1eta);
    fInTree->SetBranchAddress("genph1E", &genph1E, &b_genph1E);
    fInTree->SetBranchAddress("genph1pt", &genph1pt, &b_genph1pt);
    fInTree->SetBranchAddress("genph1phi", &genph1phi, &b_genph1phi);
    fInTree->SetBranchAddress("genph1eta", &genph1eta, &b_genph1eta);
    fInTree->SetBranchAddress("genph1match", &genph1match, &b_genph1match);
    fInTree->SetBranchAddress("gengr1E", &gengr1E, &b_gengr1E);
    fInTree->SetBranchAddress("gengr1pt", &gengr1pt, &b_gengr1pt);
    fInTree->SetBranchAddress("gengr1phi", &gengr1phi, &b_gengr1phi);
    fInTree->SetBranchAddress("gengr1eta", &gengr1eta, &b_gengr1eta);
    fInTree->SetBranchAddress("genN2mass", &genN2mass, &b_genN2mass);
    fInTree->SetBranchAddress("genN2E", &genN2E, &b_genN2E);
    fInTree->SetBranchAddress("genN2pt", &genN2pt, &b_genN2pt);
    fInTree->SetBranchAddress("genN2phi", &genN2phi, &b_genN2phi);
    fInTree->SetBranchAddress("genN2eta", &genN2eta, &b_genN2eta);
    fInTree->SetBranchAddress("genph2E", &genph2E, &b_genph2E);
    fInTree->SetBranchAddress("genph2pt", &genph2pt, &b_genph2pt);
    fInTree->SetBranchAddress("genph2phi", &genph2phi, &b_genph2phi);
    fInTree->SetBranchAddress("genph2eta", &genph2eta, &b_genph2eta);
    fInTree->SetBranchAddress("genph2match", &genph2match, &b_genph2match);
    fInTree->SetBranchAddress("gengr2E", &gengr2E, &b_gengr2E);
    fInTree->SetBranchAddress("gengr2pt", &gengr2pt, &b_gengr2pt);
    fInTree->SetBranchAddress("gengr2phi", &gengr2phi, &b_gengr2phi);
    fInTree->SetBranchAddress("gengr2eta", &gengr2eta, &b_gengr2eta);
    fInTree->SetBranchAddress("ngenjets", &ngenjets, &b_ngenjets);
    fInTree->SetBranchAddress("genjetmatch", &genjetmatch, &b_genjetmatch);
    fInTree->SetBranchAddress("genjetE", &genjetE, &b_genjetE);
    fInTree->SetBranchAddress("genjetpt", &genjetpt, &b_genjetpt);
    fInTree->SetBranchAddress("genjetphi", &genjetphi, &b_genjetphi);
    fInTree->SetBranchAddress("genjeteta", &genjeteta, &b_genjeteta);
  }
  fInTree->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  fInTree->SetBranchAddress("vtxX", &vtxX, &b_vtxX);
  fInTree->SetBranchAddress("vtxY", &vtxY, &b_vtxY);
  fInTree->SetBranchAddress("vtxZ", &vtxZ, &b_vtxZ);
  fInTree->SetBranchAddress("t1pfMETpt", &t1pfMETpt, &b_t1pfMETpt);
  fInTree->SetBranchAddress("t1pfMETphi", &t1pfMETphi, &b_t1pfMETphi);
  fInTree->SetBranchAddress("t1pfMETsumEt", &t1pfMETsumEt, &b_t1pfMETsumEt);
  fInTree->SetBranchAddress("t1pfMETuncorpt", &t1pfMETuncorpt, &b_t1pfMETuncorpt);
  fInTree->SetBranchAddress("t1pfMETuncorphi", &t1pfMETuncorphi, &b_t1pfMETuncorphi);
  fInTree->SetBranchAddress("t1pfMETuncorsumEt", &t1pfMETuncorsumEt, &b_t1pfMETuncorsumEt);
  fInTree->SetBranchAddress("t1pfMETcalopt", &t1pfMETcalopt, &b_t1pfMETcalopt);
  fInTree->SetBranchAddress("t1pfMETcalophi", &t1pfMETcalophi, &b_t1pfMETcalophi);
  fInTree->SetBranchAddress("t1pfMETcalosumEt", &t1pfMETcalosumEt, &b_t1pfMETcalosumEt);
  if (fIsMC)
  {
    fInTree->SetBranchAddress("t1pfMETgenMETpt", &t1pfMETgenMETpt, &b_t1pfMETgenMETpt);
    fInTree->SetBranchAddress("t1pfMETgenMETphi", &t1pfMETgenMETphi, &b_t1pfMETgenMETphi);
    fInTree->SetBranchAddress("t1pfMETgenMETsumEt", &t1pfMETgenMETsumEt, &b_t1pfMETgenMETsumEt);
  }
  fInTree->SetBranchAddress("njets", &njets, &b_njets);
  if (fIsMC) fInTree->SetBranchAddress("jetmatch", &jetmatch, &b_jetmatch);
  fInTree->SetBranchAddress("jetE", &jetE, &b_jetE);
  fInTree->SetBranchAddress("jetpt", &jetpt, &b_jetpt);
  fInTree->SetBranchAddress("jetphi", &jetphi, &b_jetphi);
  fInTree->SetBranchAddress("jeteta", &jeteta, &b_jeteta);
  fInTree->SetBranchAddress("nphotons", &nphotons, &b_nhotons);
  if (fIsMC) fInTree->SetBranchAddress("phmatch", &phmatch, &b_phmatch);
  fInTree->SetBranchAddress("phVID", &phVID, &b_phVID);
  fInTree->SetBranchAddress("phE", &phE, &b_phE);
  fInTree->SetBranchAddress("phpt", &phpt, &b_phpt);
  fInTree->SetBranchAddress("phphi", &phphi, &b_phphi);
  fInTree->SetBranchAddress("pheta", &pheta, &b_pheta);
  fInTree->SetBranchAddress("phscX", &phscX, &b_phscX);
  fInTree->SetBranchAddress("phscY", &phscY, &b_phscY);
  fInTree->SetBranchAddress("phscZ", &phscZ, &b_phscZ);
  fInTree->SetBranchAddress("phscE", &phscE, &b_phscE);
  fInTree->SetBranchAddress("phnrhs", &phnrhs, &b_phnrhs);
  fInTree->SetBranchAddress("phseedpos", &phseedpos, &b_phseedpos);
  fInTree->SetBranchAddress("phrhXs", &phrhXs, &b_phrhXs);
  fInTree->SetBranchAddress("phrhYs", &phrhYs, &b_phrhYs);
  fInTree->SetBranchAddress("phrhZs", &phrhZs, &b_phrhZs);
  fInTree->SetBranchAddress("phrhEs", &phrhEs, &b_phrhEs);
  fInTree->SetBranchAddress("phrhdelRs", &phrhdelRs, &b_phrhdelRs);
  fInTree->SetBranchAddress("phrhtimes", &phrhtimes, &b_phrhtimes);
  fInTree->SetBranchAddress("phrhIDs", &phrhIDs, &b_phrhIDs);
  fInTree->SetBranchAddress("phrhOOTs", &phrhOOTs, &b_phrhOOTs);
}
