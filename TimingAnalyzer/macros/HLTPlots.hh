#ifndef _hltplots_
#define _hltplots_

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "TEfficiency.h"
#include "TString.h"

#include <vector>

struct JetInfo
{
  Float_t pfjetHT = 0.f;
  Int_t nJets = 0;
};

class HLTPlots 
{
public :
  HLTPlots(const TString infile, const TString outdir, const Bool_t isoph, const Bool_t isidL, const Bool_t iser, 
	   const Bool_t applyht, const Float_t htcut, const Bool_t applyphdenom, const Bool_t applyphpt);
  ~HLTPlots();
  
  void InitTree();
  void DoPlots();
  void HT(const Int_t, JetInfo &);
  void OutputEfficiency(TEfficiency *&, const TString);
  
private :
  TFile * fInFile;
  TTree * fInTree;

  TString fOutDir;
  TFile * fOutFile;
  
  const Bool_t fIsER;
  const Bool_t fIsoPh;
  const Bool_t fIsIdL;
  const Bool_t fApplyHT;
  const Float_t fHTCut;
  const Bool_t fApplyPhDenom;
  const Bool_t fApplyPhPt;

  // Declaration of leaf types
  ULong64_t       event;
  UInt_t          run;
  UInt_t          lumi;

  Int_t           njets;
  Int_t           nphotons;
  std::vector<Float_t> * jetE;
  std::vector<Float_t> * jetpt;
  std::vector<Float_t> * jetphi;
  std::vector<Float_t> * jeteta;
  std::vector<Bool_t>  * jetidL;
  std::vector<Bool_t>  * triggerBits;
  std::vector<Float_t> * phE;
  std::vector<Float_t> * phpt;
  std::vector<Float_t> * phphi;
  std::vector<Float_t> * pheta;
  std::vector<Float_t> * phscE;
  std::vector<Float_t> * phsceta;
  std::vector<Float_t> * phscphi;
  std::vector<Float_t> * phHoE;
  std::vector<Float_t> * phr9;
  std::vector<Bool_t>  * phEleVeto;
  std::vector<Bool_t>  * phPixSeed;
  std::vector<Float_t> * phPFClEcalIso;
  std::vector<Float_t> * phPFClHcalIso;
  std::vector<Float_t> * phHollowTkIso;
  std::vector<Float_t> * phsieie;
  std::vector<Float_t> * phsipip;
  std::vector<Float_t> * phsieip;
  std::vector<Float_t> * phsmaj;
  std::vector<Float_t> * phsmin;
  std::vector<Float_t> * phalpha;
  std::vector<Int_t>   * phnrh;
  std::vector<Float_t> * phseedeta;
  std::vector<Float_t> * phseedphi;
  std::vector<Float_t> * phseedE;
  std::vector<Float_t> * phseedtime;
  std::vector<Int_t>   * phseedID;
  std::vector<Int_t>   * phseedOOT;
  std::vector<std::vector<Int_t> > * phIsHLTMatched;

  // List of branches
  TBranch * b_event;
  TBranch * b_run;
  TBranch * b_lumi;
  TBranch * b_triggerBits;
  TBranch * b_njets;
  TBranch * b_jetE;
  TBranch * b_jetpt;
  TBranch * b_jetphi;
  TBranch * b_jeteta;
  TBranch * b_jetidL;
  TBranch * b_nphotons;
  TBranch * b_phE;
  TBranch * b_phpt;
  TBranch * b_phphi;
  TBranch * b_pheta;
  TBranch * b_phscE;
  TBranch * b_phsceta;
  TBranch * b_phscphi;
  TBranch * b_phHoE;
  TBranch * b_phr9;
  TBranch * b_phEleVeto;
  TBranch * b_phPixSeed;
  TBranch * b_phPFClEcalIso;
  TBranch * b_phPFClHcalIso;
  TBranch * b_phHollowTkIso;
  TBranch * b_phsieie;
  TBranch * b_phsipip;
  TBranch * b_phsieip;
  TBranch * b_phsmaj;
  TBranch * b_phsmin;
  TBranch * b_phalpha;
  TBranch * b_phIsHLTMatched;
  TBranch * b_phnrh;
  TBranch * b_phseedeta;
  TBranch * b_phseedphi;
  TBranch * b_phseedE;
  TBranch * b_phseedtime;
  TBranch * b_phseedID;
  TBranch * b_phseedOOT;
};

#endif