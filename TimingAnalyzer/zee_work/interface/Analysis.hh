#ifndef _analysis_
#define _analysis_ 

#include "CommonTypes.hh"
#include "Config.hh"
#include "Common.hh"

#include "TH2F.h"
#include "TF1.h"

#include <array>

struct EcalID
{
  EcalID(){}
  EcalID(Int_t i1, Int_t i2, TString name) : i1_(i1), i2_(i2), name_(name) {}
  Int_t i1_; // iphi (EB) or ix (EE)
  Int_t i2_; // ieta (EB) or iy (EE)
  TString name_;
};
typedef std::unordered_map<Int_t,EcalID> EcalIDMap;

struct IOVPair // Interval of Validty object --> run beginning through run end
{
  IOVPair(Int_t beg, Int_t end) : beg_(beg), end_(end) {}
  Int_t beg_;
  Int_t end_;
};
typedef std::vector<IOVPair> IOVPairVec;

struct ADC2GeVPair // Interval of Validty object --> run beginning through run end
{
  ADC2GeVPair(float EB, float EE) : EB_(EB), EE_(EE) {}
  float EB_;
  float EE_;
};
typedef std::vector<ADC2GeVPair> ADC2GeVPairVec;

typedef std::array<Float_t,3> FltArr3;
typedef std::vector<FltArr3>  FltArr3Vec;

typedef std::unordered_map<Int_t,Float_t> IDNoiseMap;
typedef std::vector<IDNoiseMap>           IDNoiseMapVec;

typedef std::map<TString,TH1F*> TH1Map;
typedef TH1Map::iterator        TH1MapIter;

typedef std::map<TString,TH2F*> TH2Map;
typedef TH2Map::iterator        TH2MapIter;

class Analysis {
public:
  // functions
  Analysis(TString sample, Bool_t isMC);
  ~Analysis();
  void GetDetIDs();
  void GetPedestalNoise();
  void GetADC2GeVConvs();
  void InitTree();
  void EventLoop();
  void SetupStandardPlots();
  void SetupSingleEPlots();
  void SetupEffEPlots();
  void SetupNvtxPlots();
  void SetupEtaPlots();
  void SetupVtxZPlots();
  void SetupRunPlots();
  void SetupTrigEffPlots();
  void FillStandardPlots(const Float_t weight, const Float_t effseedE, const Float_t el1seedE, const Float_t el2seedE, const Float_t timediff, 
			 const Float_t el1time, const Float_t el1seedeta, Bool_t el1eb, Bool_t el1ee, Bool_t el1ep, Bool_t el1em, const FltArr3Vec & el1rhetps,
			 const Float_t el2time, const Float_t el2seedeta, Bool_t el2eb, Bool_t el2ee, Bool_t el2ep, Bool_t el2em, const FltArr3Vec & el2rhetps);
  void FillSingleEPlots(const Float_t weight, const Float_t el1seedE, const Float_t el2seedE, const Float_t timediff, const Float_t el1time, const Float_t el2time, 
			Bool_t el1eb, Bool_t el1ee, Bool_t el2eb, Bool_t el2ee);
  void FillEffEPlots(const Float_t weight, const Float_t effseedE, const Float_t timediff,  
		     Bool_t el1eb, Bool_t el1ee, Bool_t el1ep, Bool_t el1em, Bool_t el2eb, Bool_t el2ee, Bool_t el2ep, Bool_t el2em);
  void FillNvtxPlots(const Float_t weight, const Float_t timediff, const Float_t el1time, const Float_t el2time,
		     Bool_t el1eb, Bool_t el1ee, Bool_t el1ep, Bool_t el1em, Bool_t el2eb, Bool_t el2ee, Bool_t el2ep, Bool_t el2em);
  void FillEtaPlots(const Float_t weight, const Float_t timediff, const Float_t el1time, const Float_t el2time, const Float_t el1seedeta, const Float_t el2seedeta,
		    Bool_t el1eb, Bool_t el1ee, Bool_t el1ep, Bool_t el1em, Bool_t el2eb, Bool_t el2ee, Bool_t el2ep, Bool_t el2em);
  void FillVtxZPlots(const Float_t weight, const Float_t timediff, const Float_t el1time, const Float_t el2time);
  void FillRunPlots(const Float_t weight, const Float_t timediff, Bool_t el1eb, Bool_t el1ee, Bool_t el2eb, Bool_t el2ee);
  void FillTrigEffPlots(const Float_t weight);
  void OutputStandardPlots();
  void OutputSingleEPlots();
  void OutputEffEPlots();
  void OutputNvtxPlots();
  void OutputEtaPlots();
  void OutputVtxZPlots();
  void OutputRunPlots();
  void OutputTrigEffPlots();
  void Make1DTimingPlots(TH2F *& hist2D, const TString subdir2D, const DblVec& bins2D, TString name);
  void Project2Dto1D(TH2F *& hist2d, TString subdir2d, TH1Map & th1map, TStrMap & subdir1dmap, TStrIntMap & th1binmap);
  void ProduceMeanSigma(TH1Map & th1map, TStrIntMap & th1binmap, TString name, TString xtitle, const DblVec vxbins, TString subdir);
  void PrepFit(TF1 *& fit, TH1F *& hist);
  void GetMeanSigma(TF1 *& fit, Float_t & mean, Float_t & emean, Float_t & sigma, Float_t & esigma); 
  void DrawSubComp(TF1 *& fit, TCanvas *& canv, TF1 *& sub1, TF1 *& sub2, TF1 *& sub3);
  TH1F * MakeTH1Plot(TString hname, TString htitle, Int_t nbins, Double_t xlow, Double_t xhigh, TString xtitle, TString ytitle, TStrMap& subdirmap, TString subdir);
  TH2F * MakeTH2Plot(TString hname, TString htitle, const DblVec& vxbins, Int_t nbinsy, Double_t ylow, Double_t yhigh, 
		     TString xtitle, TString ytitle, TStrMap& subdirmap, TString subdir);
  void FillHistFromArr3Vec0(TH1F *& hist, const FltArr3Vec & arr3vec);
  void FillHistFromArr3Vec1(TH1F *& hist, const FltArr3Vec & arr3vec);
  void SaveTH1s(TH1Map & th1map, TStrMap & subdirmap);
  void SaveTH1andFit(TH1F *& hist, TString subdir, TF1 *& fit);
  void SaveTH2s(TH2Map & th2map, TStrMap & subdirmap);
  void DumpTH1Names(TH1Map & th1map, TStrMap & subdirmap);
  void DeleteFit(TF1 *& fit, TF1 *& sub1, TF1 *& sub2, TF1 *& sub3);
  void DeleteTH1s(TH1Map & th1map);
  void DeleteTH2s(TH2Map & th2map);

private:
  // Input
  TFile * fInFile;
  TTree * fInTree;
  TString fSample;
  Bool_t  fIsMC;

  // Ecal ids, pedestals, and ADC conversion
  EcalIDMap      fEcalIDMap;
  IOVPairVec     fPedNoiseRuns;
  IDNoiseMapVec  fPedNoises;
  IOVPairVec     fADC2GeVRuns;
  ADC2GeVPairVec fADC2GeVs;
  
  // MC weight input
  //  FltVec  fPUweights;
  DblVec  fPUweights;
  Float_t fXsec;
  Float_t fWgtsum;

  // Output
  TString fOutDir;
  TFile*  fOutFile;
  std::ofstream fTH1Dump; 
  
  // Output colors
  Color_t fColor;

  ////////////////////////
  // Standard plot maps //
  ////////////////////////
  TH1Map standardTH1Map; TStrMap standardTH1SubMap;
  TH1Map timingMap;      TStrMap timingSubMap;

  ///////////////////
  // time res maps //
  ///////////////////

  // effective energy plots
  TH2Map effseedE2DMap; TStrMap effseedE2DSubMap; TStrDblVMap effseedEbins;

  // single energy plots
  TH2Map el1seedE2DMap; TStrMap el1seedE2DSubMap; TStrDblVMap el1seedEbins;
  TH2Map el2seedE2DMap; TStrMap el2seedE2DSubMap; TStrDblVMap el2seedEbins;

  // nvtx plots
  TH2Map nvtx2DMap; TStrMap nvtx2DSubMap; DblVec nvtxbins;

  // delta eta plots
  TH2Map deta2DMap; TStrMap deta2DSubMap; DblVec detabins;
  
  // single el eta plots
  TH2Map eleta2DMap; TStrMap eleta2DSubMap; DblVec eletabins;

  // vtxZ plots
  TH2Map vtxZ2DMap; TStrMap vtxZ2DSubMap; DblVec vtxZbins;

  // run numbers (data only)
  TH2Map runs2DMap; TStrMap runs2DSubMap; DblVec dRunNos; 

  //////////////////////
  // trigger eff maps //
  //////////////////////
  TH1Map  trTH1Map; TStrMap trTH1SubMap;
  TH1F * n_hltdoubleel_el1pt; TH1F * d_hltdoubleel_el1pt;
  TH1F * n_hltdoubleel_el2pt; TH1F * d_hltdoubleel_el2pt;

public:
  // Declaration of leaf types
  UInt_t    run;
  UInt_t    lumi;
  ULong64_t event;
  Bool_t    hltdoubleel23_12;
  Bool_t    hltdoubleel33_33;
  Bool_t    hltdoubleel37_27;
  Int_t     nvtx;
  Float_t   vtxX;
  Float_t   vtxY;
  Float_t   vtxZ;
  Int_t     el1pid;
  Float_t   el1E;
  Float_t   el1p;
  Float_t   el1pt;
  Float_t   el1eta;
  Float_t   el1phi;
  Float_t   el2E;
  Float_t   el2p;
  Int_t     el2pid;
  Float_t   el2pt;
  Float_t   el2eta;
  Float_t   el2phi;
  Float_t   el1scX;
  Float_t   el1scY;
  Float_t   el1scZ;
  Float_t   el1scE;
  Float_t   el2scX;
  Float_t   el2scY;
  Float_t   el2scZ;
  Float_t   el2scE;
  Int_t     el1nrh;
  Int_t     el1seedpos;
  Int_t     el2nrh;
  Int_t     el2seedpos;
  std::vector<float> * el1rhXs;
  std::vector<float> * el1rhYs;
  std::vector<float> * el1rhZs;
  std::vector<float> * el1rhEs;
  std::vector<float> * el1rhtimes;
  std::vector<float> * el2rhXs;
  std::vector<float> * el2rhYs;
  std::vector<float> * el2rhZs;
  std::vector<float> * el2rhEs;
  std::vector<float> * el2rhtimes;
  std::vector<int>   * el1rhids;
  std::vector<bool>  * el1rhOOTs;
  std::vector<bool>  * el1rhgain1s;
  std::vector<bool>  * el1rhgain6s;
  std::vector<int>   * el2rhids;
  std::vector<bool>  * el2rhOOTs;
  std::vector<bool>  * el2rhgain1s;
  std::vector<bool>  * el2rhgain6s;
  Float_t   zE;
  Float_t   zp;
  Float_t   zpt;
  Float_t   zeta;
  Float_t   zphi;
  Float_t   zmass;

  // MC
  Float_t   puobs;
  Float_t   putrue;
  Float_t   wgt;
  Int_t     genzpid;
  Float_t   genzE;
  Float_t   genzp;
  Float_t   genzpt;
  Float_t   genzeta;
  Float_t   genzphi;
  Float_t   genzmass;
  Int_t     genel1pid;
  Float_t   genel1E;
  Float_t   genel1p;
  Float_t   genel1pt;
  Float_t   genel1eta;
  Float_t   genel1phi;
  Int_t     genel2pid;
  Float_t   genel2E;
  Float_t   genel2p;
  Float_t   genel2pt;
  Float_t   genel2eta;
  Float_t   genel2phi;

  // List of branches
  TBranch * b_run;
  TBranch * b_lumi;
  TBranch * b_event;
  TBranch * b_hltdoubleel23_12;
  TBranch * b_hltdoubleel33_33;
  TBranch * b_hltdoubleel37_27;
  TBranch * b_nvtx;
  TBranch * b_vtxX;
  TBranch * b_vtxY;
  TBranch * b_vtxZ;
  TBranch * b_el1pid;
  TBranch * b_el1E;
  TBranch * b_el1p;
  TBranch * b_el1pt;
  TBranch * b_el1eta;
  TBranch * b_el1phi;
  TBranch * b_el2E;
  TBranch * b_el2p;
  TBranch * b_el2pid;
  TBranch * b_el2pt;
  TBranch * b_el2eta;
  TBranch * b_el2phi;
  TBranch * b_el1scX;
  TBranch * b_el1scY;
  TBranch * b_el1scZ;
  TBranch * b_el1scE;
  TBranch * b_el2scX;
  TBranch * b_el2scY;
  TBranch * b_el2scZ;
  TBranch * b_el2scE;
  TBranch * b_el1nrh;
  TBranch * b_el1seedpos;
  TBranch * b_el2nrh;
  TBranch * b_el2seedpos;
  TBranch * b_el1rhXs;
  TBranch * b_el1rhYs;
  TBranch * b_el1rhZs;
  TBranch * b_el1rhEs;
  TBranch * b_el1rhtimes;
  TBranch * b_el2rhXs;
  TBranch * b_el2rhYs;
  TBranch * b_el2rhZs;
  TBranch * b_el2rhEs;
  TBranch * b_el2rhtimes;
  TBranch * b_el1rhids;
  TBranch * b_el1rhOOTs;
  TBranch * b_el1rhgain1s;
  TBranch * b_el1rhgain6s;
  TBranch * b_el2rhids;
  TBranch * b_el2rhOOTs;
  TBranch * b_el2rhgain1s;
  TBranch * b_el2rhgain6s;
  TBranch * b_zE;
  TBranch * b_zp;
  TBranch * b_zpt;
  TBranch * b_zeta;
  TBranch * b_zphi;
  TBranch * b_zmass;

  // MC
  TBranch * b_puobs;
  TBranch * b_putrue;
  TBranch * b_wgt;
  TBranch * b_genzpid;
  TBranch * b_genzE;
  TBranch * b_genzp;
  TBranch * b_genzpt;
  TBranch * b_genzeta;
  TBranch * b_genzphi;
  TBranch * b_genzmass;
  TBranch * b_genel1pid;
  TBranch * b_genel1E;
  TBranch * b_genel1p;
  TBranch * b_genel1pt;
  TBranch * b_genel1eta;
  TBranch * b_genel1phi;
  TBranch * b_genel2pid;
  TBranch * b_genel2E;
  TBranch * b_genel2p;
  TBranch * b_genel2pt;
  TBranch * b_genel2eta;
  TBranch * b_genel2phi;
};

#endif
