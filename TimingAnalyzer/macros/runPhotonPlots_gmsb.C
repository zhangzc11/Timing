#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"

void setupcpp11() // customize ACLiC's behavior ...
{
  TString o;
  // customize MakeSharedLib
  o = TString(gSystem->GetMakeSharedLib());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeSharedLib(o.Data());
  // customize MakeExe
  o = TString(gSystem->GetMakeExe());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeExe(o.Data());
} 

void runPhotonPlots_gmsb(TString VID1, TString ctau, Bool_t apply, Bool_t isEB, Bool_t isEE, Bool_t phmc) 
{
  setupcpp11(); 

  gROOT->LoadMacro("PlotPhotons.cc+g");

  // config is:
  // filename, isGMSB, isHVDS, isBkg
  // isHLT2, isHLT3
  // applyevcut, outdir, savehists, savesub
  // applyjetptcut, jetptcut, applynjetscut, njetscut
  // applyph1ptcut, ph1ptcut, applyph1vidcut, ph1vid
  // applyphanyptcut, phanyptcut, applyphanyvidcut, phanyvid
  // applyrhecut, rhEcut
  // applyecalacceptcut, applyEBonly, applyEEonly
  // applyphmcmatchingcut, applyexactphmcmatch, applyantiphmcmatch
  
  Float_t ph1pt   = 50.f;
  Float_t phanypt = 10.f;

  TString VIDany = (VID1.Contains("none",TString::kExact)) ? "none" : "loose";

  PlotPhotons photonPlots(Form("input/MC/signal/GMSB/photondump-ctau%s.root",ctau.Data()),
			  true,false,false,
			  false,false,
			  false,Form("output/MC/signal/GMSB/photondump/ctau%s",ctau.Data()),false,false,
			  apply,30.f,apply,3,
			  apply,ph1pt,apply,VID1.Data(),
			  true,phanypt,apply,VIDany.Data(),
			  true,1.f,
			  true,isEB,isEE,
			  phmc,true,false);

  // which plots to do:
  // geninfo, vtxs, met, jets
  // photons, ph1, phdelay
  // trigger, analysis
  photonPlots.DoPlots(false,false,false,false,
 		      true,true,true,
		      false,false);
}
