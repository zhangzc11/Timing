#include "PlotRecHits.hh"
#include "TSystem.h"
#include "TCanvas.h"
#include "TObject.h" 

#include <iostream>

PlotRecHits::PlotRecHits(TString filename, TString outdir, Bool_t applyphptcut, Float_t phptcut,
			 Bool_t applyrhecut, Float_t rhEcut, Bool_t applyecalacceptcut) :
  fOutDir(outdir),
  fApplyPhPtCut(applyphptcut), fPhPtCut(phptcut),
  fApplyrhECut(applyrhecut), frhECut(rhEcut),
  fApplyECALAcceptCut(applyecalacceptcut)
{
  // input
  fInFile = TFile::Open(filename.Data());
  fInTree = (TTree*)fInFile->Get("tree/phrhtree");

  // initialize tree
  PlotRecHits::InitTree();

  // in routine initialization
  fNPhCheck = 1000;
  
  // output
  // setup outdir name
  
  if (!fApplyPhPtCut && !fApplyrhECut && ! fApplyECALAcceptCut)
  { 
    fOutDir += "/Inclusive";
  }
  else 
  {
    fOutDir += "/cuts";
    if (fApplyPhPtCut)       fOutDir += Form("_phpt%3.1f" ,fPhPtCut);
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

PlotRecHits::~PlotRecHits()
{
  delete fInTree;
  delete fInFile;

  delete fOutFile;
}

void PlotRecHits::DoPlots()
{
  PlotRecHits::SetupPlots();
  PlotRecHits::PhotonLoop();
  PlotRecHits::MakeSubDirs();
  PlotRecHits::OutputTH1Fs();
  PlotRecHits::OutputTH2Fs();
  PlotRecHits::OutputTotalTH1Fs();
  PlotRecHits::ClearTH1Map();
  PlotRecHits::ClearTH2Map();
}

void PlotRecHits::SetupPlots()
{
  PlotRecHits::SetupRecoPhotons();
}

void PlotRecHits::PhotonLoop()
{
  for (UInt_t entry = 0; entry < fInTree->GetEntries(); entry++)
  {
    fInTree->GetEntry(entry);
    if (entry%fNPhCheck == 0 || entry == 0) std::cout << "Entry " << entry << " out of " << fInTree->GetEntries() << std::endl;

    if (fApplyPhPtCut && phE < fPhPtCut) continue;
    if (fApplyECALAcceptCut && (std::abs(pheta) > 2.5 || (std::abs(pheta) > 1.4442 && std::abs(pheta) < 1.566))) continue;
  
    PlotRecHits::FillRecoPhotons();
  }
}

void PlotRecHits::FillRecoPhotons()
{
  fPlots["phE"]->Fill(phE);
  fPlots["phpt"]->Fill(phpt);
  fPlots["phphi"]->Fill(phphi);
  fPlots["pheta"]->Fill(pheta);

  fPlots["phscE"]->Fill(phscE);
  fPlots["phscphi"]->Fill(phscphi);
  fPlots["phsceta"]->Fill(phsceta);
  
  ///////////////////
  //               //
  // Full Rec Hits //
  //               //
  ///////////////////

  // --> Core Rec Hits <-- //
  int nfRecHits = 0;
  for (int irh = 0; irh < phnfrhs; irh++)
  {
    if (fApplyrhECut && (*phfrhEs)[irh] < frhECut) continue;
    nfRecHits++;

    fPlots["phfrhEs"]->Fill((*phfrhEs)[irh]);
    fPlots["phfrhdelRs"]->Fill((*phfrhdelRs)[irh]);
    fPlots["phfrhtimes"]->Fill((*phfrhtimes)[irh]);
    fPlots["phfrhOOTs"]->Fill((*phfrhOOTs)[irh]);
    if ( phfseedpos == irh ) // seed info
    {
      fPlots["phfseedE"]->Fill((*phfrhEs)[irh]);
      fPlots["phfseeddelR"]->Fill((*phfrhdelRs)[irh]);
      fPlots["phfseedtime"]->Fill((*phfrhtimes)[irh]);
      fPlots["phfseedOOT"]->Fill((*phfrhOOTs)[irh]);
    } // end block over seeds
  } // end loop over "core" rechits
  fPlots["phnfrhs"]->Fill(nfRecHits);

  // --> Add-On Rec Hits <-- //
  int nfRecHits_add = 0;
  for (int irh = phnfrhs; irh < phnfrhs_add; irh++)
  {
    if (fApplyrhECut && (*phfrhEs)[irh] < frhECut) continue;
    nfRecHits_add++;
    
    fPlots["phfrhEs_add"]->Fill((*phfrhEs)[irh]);
    fPlots["phfrhdelRs_add"]->Fill((*phfrhdelRs)[irh]);
    fPlots["phfrhtimes_add"]->Fill((*phfrhtimes)[irh]);
    fPlots["phfrhOOTs_add"]->Fill((*phfrhOOTs)[irh]);
  } // end loop over "core" rechits
  fPlots["phnfrhs_add"]->Fill(nfRecHits_add);

  // total rechits now per photon
  fPlots["phnfrhs_total"]->Fill(nfRecHits+nfRecHits_add);
  
  //////////////////////
  //                  //
  // Reduced Rec Hits //
  //                  //
  //////////////////////

  // --> Core Rec Hits <-- //
  int nrRecHits = 0;
  for (int irh = 0; irh < phnrrhs; irh++)
  {
    if (fApplyrhECut && (*phrrhEs)[irh] < frhECut) continue;
    nrRecHits++;

    fPlots["phrrhEs"]->Fill((*phrrhEs)[irh]);
    fPlots["phrrhdelRs"]->Fill((*phrrhdelRs)[irh]);
    fPlots["phrrhtimes"]->Fill((*phrrhtimes)[irh]);
    fPlots["phrrhOOTs"]->Fill((*phrrhOOTs)[irh]);
    if ( phrseedpos == irh ) // seed info
    {
      fPlots["phrseedE"]->Fill((*phrrhEs)[irh]);
      fPlots["phrseeddelR"]->Fill((*phrrhdelRs)[irh]);
      fPlots["phrseedtime"]->Fill((*phrrhtimes)[irh]);
      fPlots["phrseedOOT"]->Fill((*phrrhOOTs)[irh]);
    } // end block over seeds
  } // end loop over "core" rechits
  fPlots["phnrrhs"]->Fill(nrRecHits);

  // --> Add-On Rec Hits <-- //
  int nrRecHits_add = 0;
  for (int irh = phnrrhs; irh < phnrrhs_add; irh++)
  {
    if (fApplyrhECut && (*phrrhEs)[irh] < frhECut) continue;
    nrRecHits_add++;
    
    fPlots["phrrhEs_add"]->Fill((*phrrhEs)[irh]);
    fPlots["phrrhdelRs_add"]->Fill((*phrrhdelRs)[irh]);
    fPlots["phrrhtimes_add"]->Fill((*phrrhtimes)[irh]);
    fPlots["phrrhOOTs_add"]->Fill((*phrrhOOTs)[irh]);
  } // end loop over "core" rechits
  fPlots["phnrrhs_add"]->Fill(nrRecHits_add);

  // total rechits now per photon
  fPlots["phnrrhs_total"]->Fill(nfRecHits+nrRecHits_add);
}

void PlotRecHits::SetupRecoPhotons()
{
  // All reco photons + associated supercluster
  fPlots["phE"] = PlotRecHits::MakeTH1F("phE","Photons Energy [GeV] (reco)",100,0.f,2000.f,"Energy [GeV]","Photons","Photons/GeneralProps");
  fPlots["phpt"] = PlotRecHits::MakeTH1F("phpt","Photons p_{T} [GeV/c] (reco)",100,0.f,2000.f,"p_{T} [GeV/c]","Photons","Photons/GeneralProps");
  fPlots["phphi"] = PlotRecHits::MakeTH1F("phphi","Photons #phi (reco)",100,-3.2,3.2,"#phi","Photons","Photons/GeneralProps");
  fPlots["pheta"] = PlotRecHits::MakeTH1F("pheta","Photons #eta (reco)",100,-3.0,3.0,"#eta","Photons","Photons/GeneralProps");

  fPlots["phscE"] = PlotRecHits::MakeTH1F("phscE","Photons SuperCluster Energy [GeV] (reco)",100,0.f,2000.f,"Energy [GeV]","Photons","Photons/GeneralProps");
  fPlots["phscphi"] = PlotRecHits::MakeTH1F("phscphi","Photons SuperCluster #phi (reco)",100,-3.2,3.2,"#phi","Photons","Photons/GeneralProps");
  fPlots["phsceta"] = PlotRecHits::MakeTH1F("phsceta","Photons SuperCluster #eta (reco)",100,-3.0,3.0,"#eta","Photons","Photons/GeneralProps");
  
  // full rec hit collections: "core rechits"
  fPlots["phnfrhs"] = PlotRecHits::MakeTH1F("phnfrhs","nfRecHits from Photons (reco)",100,0.f,100.f,"nfRecHits","Photons","RecoPhotons/FullRHs");
  fPlots["phfrhEs"] = PlotRecHits::MakeTH1F("phfrhEs","Photons fRecHits Energy [GeV] (reco)",100,0.f,1000.f,"Energy [GeV]","fRecHits","RecoPhotons/FullRHs");
  fPlots["phfrhdelRs"] = PlotRecHits::MakeTH1F("phfrhdelRs","#DeltaR of fRecHits to Photon (reco)",100,0.f,1.0f,"Time [ns]","fRecHits","RecoPhotons/FullRHs");
  fPlots["phfrhtimes"] = PlotRecHits::MakeTH1F("phfrhtimes","Photons fRecHits Time [ns] (reco)",200,-100.f,100.f,"Time [ns]","fRecHits","RecoPhotons/FullRHs");
  fPlots["phfrhOOTs"] = PlotRecHits::MakeTH1F("phfrhOOTs","Photons fRecHits OoT Flag (reco)",2,0.f,2.f,"OoT Flag","fRecHits","RecoPhotons/FullRHs");
  fPlots["phfseedE"] = PlotRecHits::MakeTH1F("phfseedE","Photons Seed fRecHit Energy [GeV] (reco)",100,0.f,1000.f,"Energy [GeV]","Seed fRecHits","RecoPhotons/FullRHs");
  fPlots["phfseeddelR"] = PlotRecHits::MakeTH1F("phfseeddelR","#DeltaR of Seed fRecHit to Photon (reco)",100,0.f,1.0f,"Time [ns]","Seed fRecHits","RecoPhotons/FullRHs");
  fPlots["phfseedtime"] = PlotRecHits::MakeTH1F("phfseedtime","Photons Seed fRecHit Time [ns] (reco)",200,-100.f,100.f,"Time [ns]","Seed fRecHits","RecoPhotons/FullRHs");
  fPlots["phfseedOOT"] = PlotRecHits::MakeTH1F("phfseedOOT","Photons Seed fRecHit OoT Flag (reco)",2,0.f,2.f,"OoT Flag","Seed fRecHits","RecoPhotons/FullRHs");  

  // full rec hit collections: "add-on recHits"
  fPlots["phnfrhs_add"] = PlotRecHits::MakeTH1F("phnfrhs_add","nfRecHits from Photons (reco) [add-ons]",100,0.f,100.f,"nfRecHits_add","Photons","RecoPhotons/FullRHs");
  fPlots["phfrhEs_add"] = PlotRecHits::MakeTH1F("phfrhEs_add","Photons fRecHits Energy [GeV] (reco) [add-ons]",100,0.f,1000.f,"Energy [GeV]","fRecHits_add","RecoPhotons/FullRHs");
  fPlots["phfrhdelRs_add"] = PlotRecHits::MakeTH1F("phfrhdelRs_add","#DeltaR of fRecHits to Photon (reco) [add-ons]",100,0.f,1.0f,"Time [ns]","fRecHits_add","RecoPhotons/FullRHs");
  fPlots["phfrhtimes_add"] = PlotRecHits::MakeTH1F("phfrhtimes_add","Photons fRecHits Time [ns] (reco) [add-ons]",200,-100.f,100.f,"Time [ns]","fRecHits_add","RecoPhotons/FullRHs");
  fPlots["phfrhOOTs_add"] = PlotRecHits::MakeTH1F("phfrhOOTs_add","Photons fRecHits OoT Flag (reco) [add-ons]",2,0.f,2.f,"OoT Flag","fRecHits_add","RecoPhotons/FullRHs");

  // totals for rull rec hit collections
  fPlots["phnfrhs_total"] = PlotRecHits::MakeTH1F("phnfrhs_total","nfRecHits from Photons (reco) [core+add-ons]",100,0.f,100.f,"nfRecHits_total","Photons","RecoPhotons/FullRHs");

  // reduced rec hit collections: "core rechits"
  fPlots["phnrrhs"] = PlotRecHits::MakeTH1F("phnrrhs","nrRecHits from Photons (reco)",100,0.f,100.f,"nrRecHits","Photons","RecoPhotons/ReducedRHs");
  fPlots["phrrhEs"] = PlotRecHits::MakeTH1F("phrrhEs","Photons rRecHits Energy [GeV] (reco)",100,0.f,1000.f,"Energy [GeV]","rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrrhdelRs"] = PlotRecHits::MakeTH1F("phrrhdelRs","#DeltaR of rRecHits to Photon (reco)",100,0.f,1.0f,"Time [ns]","rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrrhtimes"] = PlotRecHits::MakeTH1F("phrrhtimes","Photons rRecHits Time [ns] (reco)",200,-100.f,100.f,"Time [ns]","rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrrhOOTs"] = PlotRecHits::MakeTH1F("phrrhOOTs","Photons rRecHits OoT Flag (reco)",2,0.f,2.f,"OoT Flag","rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrseedE"] = PlotRecHits::MakeTH1F("phrseedE","Photons Seed rRecHit Energy [GeV] (reco)",100,0.f,1000.f,"Energy [GeV]","Seed rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrseeddelR"] = PlotRecHits::MakeTH1F("phrseeddelR","#DeltaR of Seed rRecHit to Photon (reco)",100,0.f,1.0f,"Time [ns]","Seed rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrseedtime"] = PlotRecHits::MakeTH1F("phrseedtime","Photons Seed rRecHit Time [ns] (reco)",200,-100.f,100.f,"Time [ns]","Seed rRecHits","RecoPhotons/ReducedRHs");
  fPlots["phrseedOOT"] = PlotRecHits::MakeTH1F("phrseedOOT","Photons Seed rRecHit OoT Flag (reco)",2,0.f,2.f,"OoT Flag","Seed rRecHits","RecoPhotons/ReducedRHs");  

  // reduced rec hit collections: "add-on recHits"
  fPlots["phnrrhs_add"] = PlotRecHits::MakeTH1F("phnrrhs_add","nrRecHits from Photons (reco) [add-ons]",100,0.f,100.f,"nrRecHits_add","Photons","RecoPhotons/ReducedRHs");
  fPlots["phrrhEs_add"] = PlotRecHits::MakeTH1F("phrrhEs_add","Photons rRecHits Energy [GeV] (reco) [add-ons]",100,0.f,1000.f,"Energy [GeV]","rRecHits_add","RecoPhotons/ReducedRHs");
  fPlots["phrrhdelRs_add"] = PlotRecHits::MakeTH1F("phrrhdelRs_add","#DeltaR of rRecHits to Photon (reco) [add-ons]",100,0.f,1.0f,"Time [ns]","rRecHits_add","RecoPhotons/ReducedRHs");
  fPlots["phrrhtimes_add"] = PlotRecHits::MakeTH1F("phrrhtimes_add","Photons rRecHits Time [ns] (reco) [add-ons]",200,-100.f,100.f,"Time [ns]","rRecHits_add","RecoPhotons/ReducedRHs");
  fPlots["phrrhOOTs_add"] = PlotRecHits::MakeTH1F("phrrhOOTs_add","Photons rRecHits OoT Flag (reco) [add-ons]",2,0.f,2.f,"OoT Flag","rRecHits_add","RecoPhotons/ReducedRHs");

  // totals for rull rec hit collections
  fPlots["phnrrhs_total"] = PlotRecHits::MakeTH1F("phnrrhs_total","nrRecHits from Photons (reco) [core+add-ons]",100,0.f,100.f,"nrRecHits_total","Photons","RecoPhotons/ReducedRHs");
}

TH1F * PlotRecHits::MakeTH1F(TString hname, TString htitle, Int_t nbinsx, Float_t xlow, Float_t xhigh, TString xtitle, TString ytitle, TString subdir)
{
  TH1F * hist = new TH1F(hname.Data(),htitle.Data(),nbinsx,xlow,xhigh);
  hist->SetLineColor(kBlack);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();

  fSubDirs[hname] = subdir;

  return hist;
}

TH2F * PlotRecHits::MakeTH2F(TString hname, TString htitle, Int_t nbinsx, Float_t xlow, Float_t xhigh, TString xtitle, Int_t nbinsy, Float_t ylow, Float_t yhigh, TString ytitle, TString subdir)
{
  TH2F * hist = new TH2F(hname.Data(),htitle.Data(),nbinsx,xlow,xhigh,nbinsy,ylow,yhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();

  fSubDirs[hname] = subdir;

  return hist;
}

void PlotRecHits::MakeSubDirs()
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

void PlotRecHits::OutputTH1Fs()
{
  fOutFile->cd();

  for (TH1MapIter mapiter = fPlots.begin(); mapiter != fPlots.end(); ++mapiter) 
  { 
    // save relevant plots for making "totals"
    if (mapiter->first.Contains("_add",TString::kExact) &&
	!(mapiter->first.Contains("phnfrhs",TString::kExact) || mapiter->first.Contains("phnrrhs",TString::kExact)))
    {
      TString hname      = mapiter->first;
      TString replacestr = "_add";
      Ssiz_t  length     = replacestr.Length();
      Ssiz_t  hnamepos   = hname.Index(replacestr.Data());
      TString toreplace  = "";
      hname.Replace(hnamepos,length,toreplace);
     
      fTotalNames.push_back(hname);
    }

    // save to output file
    mapiter->second->Write(mapiter->first.Data(),TObject::kWriteDelete); 
    
    // now draw onto canvas to save as png
    TCanvas * canv = new TCanvas("canv","canv");
    canv->cd();
    mapiter->second->Draw("HIST");
    
    // first save as linear, then log
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s/lin/%s.png",fOutDir.Data(),fSubDirs[mapiter->first].Data(),mapiter->first.Data()));
    canv->SaveAs(Form("%s/%s_lin.png",fOutDir.Data(),mapiter->first.Data()));

    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s/log/%s.png",fOutDir.Data(),fSubDirs[mapiter->first].Data(),mapiter->first.Data()));
    canv->SaveAs(Form("%s/%s_log.png",fOutDir.Data(),mapiter->first.Data()));

    delete canv;
  }
}

void PlotRecHits::OutputTotalTH1Fs()
{
  for (auto&& hname : fTotalNames)
  {
    // make a copy of previous plot
    TH1F * htotal = (TH1F*)fPlots[hname]->Clone(Form("%s_total",hname.Data()));
    htotal->SetTitle(Form("%s [totals]",fPlots[hname]->GetTitle()));
    htotal->GetYaxis()->SetTitle(Form("%s [totals]",fPlots[hname]->GetYaxis()->GetTitle()));

    // add on additional rechit info
    htotal->Add(fPlots[Form("%s_add",hname.Data())]);
    
    //now do standard output routines
    // save to output file
    htotal->Write(htotal->GetName(),TObject::kWriteDelete); 
    
    // now draw onto canvas to save as png
    TCanvas * canv = new TCanvas("canv","canv");
    canv->cd();
    htotal->Draw("HIST");
    
    // first save as linear, then log
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s/lin/%s.png",fOutDir.Data(),fSubDirs[hname].Data(),htotal->GetName()));
    canv->SaveAs(Form("%s/%s_lin.png",fOutDir.Data(),htotal->GetName()));

    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s/log/%s.png",fOutDir.Data(),fSubDirs[hname].Data(),htotal->GetName()));
    canv->SaveAs(Form("%s/%s_log.png",fOutDir.Data(),htotal->GetName()));

    delete canv;
    delete htotal;
  }
}

void PlotRecHits::ClearTH1Map()
{
  for (TH1MapIter mapiter = fPlots.begin(); mapiter != fPlots.end(); ++mapiter) 
  { 
    delete mapiter->second;
  }
  fPlots.clear();
}

void PlotRecHits::OutputTH2Fs()
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
    canv->SaveAs(Form("%s/%s_lin.png",fOutDir.Data(),mapiter->first.Data()));

    delete canv;
  }
}

void PlotRecHits::ClearTH2Map()
{
  for (TH2MapIter mapiter = fPlots2D.begin(); mapiter != fPlots2D.end(); ++mapiter) 
  { 
    delete mapiter->second;
  }
  fPlots2D.clear();
}

void PlotRecHits::InitTree()
{
  // need to set vector pointers, otherwise root craps out
  phfrhEs    = 0;
  phfrhphis  = 0;
  phfrhetas  = 0;
  phfrhdelRs = 0;
  phfrhtimes = 0;
  phfrhIDs   = 0;
  phfrhOOTs  = 0;
  phrrhEs    = 0;
  phrrhphis  = 0;
  phrrhetas  = 0;
  phrrhdelRs = 0;
  phrrhtimes = 0;
  phrrhIDs   = 0;
  phrrhOOTs  = 0;

  fInTree->SetBranchAddress("event", &event, &b_event);
  fInTree->SetBranchAddress("run", &run, &b_run);
  fInTree->SetBranchAddress("lumi", &lumi, &b_lumi);
  fInTree->SetBranchAddress("phE", &phE, &b_phE);
  fInTree->SetBranchAddress("phpt", &phpt, &b_phpt);
  fInTree->SetBranchAddress("phphi", &phphi, &b_phphi);
  fInTree->SetBranchAddress("pheta", &pheta, &b_pheta);
  fInTree->SetBranchAddress("phscE", &phscE, &b_phscE);
  fInTree->SetBranchAddress("phscphi", &phscphi, &b_phscphi);
  fInTree->SetBranchAddress("phsceta", &phsceta, &b_phsceta);
  fInTree->SetBranchAddress("phnfrhs", &phnfrhs, &b_phnfrhs);
  fInTree->SetBranchAddress("phnfrhs_add", &phnfrhs_add, &b_phnfrhs_add);
  fInTree->SetBranchAddress("phfrhEs", &phfrhEs, &b_phfrhEs);
  fInTree->SetBranchAddress("phfrhphis", &phfrhphis, &b_phfrhphis);
  fInTree->SetBranchAddress("phfrhetas", &phfrhetas, &b_phfrhetas);
  fInTree->SetBranchAddress("phfrhdelRs", &phfrhdelRs, &b_phfrhdelRs);
  fInTree->SetBranchAddress("phfrhtimes", &phfrhtimes, &b_phfrhtimes);
  fInTree->SetBranchAddress("phfrhIDs", &phfrhIDs, &b_phfrhIDs);
  fInTree->SetBranchAddress("phfrhOOTs", &phfrhOOTs, &b_phfrhOOTs);
  fInTree->SetBranchAddress("phfseedpos", &phfseedpos, &b_phfseedpos);
  fInTree->SetBranchAddress("phnrrhs", &phnrrhs, &b_phnrrhs);
  fInTree->SetBranchAddress("phnrrhs_add", &phnrrhs_add, &b_phnrrhs_add);
  fInTree->SetBranchAddress("phrrhEs", &phrrhEs, &b_phrrhEs);
  fInTree->SetBranchAddress("phrrhphis", &phrrhphis, &b_phrrhphis);
  fInTree->SetBranchAddress("phrrhetas", &phrrhetas, &b_phrrhetas);
  fInTree->SetBranchAddress("phrrhdelRs", &phrrhdelRs, &b_phrrhdelRs);
  fInTree->SetBranchAddress("phrrhtimes", &phrrhtimes, &b_phrrhtimes);
  fInTree->SetBranchAddress("phrrhIDs", &phrrhIDs, &b_phrrhIDs);
  fInTree->SetBranchAddress("phrrhOOTs", &phrrhOOTs, &b_phrrhOOTs);
  fInTree->SetBranchAddress("phrseedpos", &phrseedpos, &b_phrseedpos);
}
