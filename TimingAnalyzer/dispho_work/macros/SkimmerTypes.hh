#ifndef _skimmertypes_ 
#define _skimmertypes_ 

#include "TH1F.h"
#include "TBranch.h"

#include <string>
#include <vector>

struct Configuration
{
  // branches
  UInt_t  blindSF;
  Bool_t  applyBlindSF;
  Float_t blindMET;
  Bool_t  applyBlindMET;
  Float_t jetpTmin;
  Int_t   jetIDmin;
  Float_t rhEmin;
  Float_t phpTmin;
  std::string * phIDmin;
  Float_t seedTimemin;
  Bool_t  splitPho;
  Bool_t  onlyGED;
  Bool_t  onlyOOT;
  Bool_t  storeRecHits;
  Bool_t  applyTrigger;
  Float_t minHT;
  Bool_t  applyHT;
  Float_t phgoodpTmin;
  std::string * phgoodIDmin;
  Bool_t  applyPhGood;
  Float_t dRmin;
  Float_t pTres;
  Float_t trackdRmin;
  Float_t trackpTmin;
  std::string * inputPaths;
  std::string * inputFilters;
  Bool_t  isGMSB;
  Bool_t  isHVDS;
  Bool_t  isBkgd;
  Float_t xsec;
  Float_t filterEff;
  Float_t BR;

  // branch names
  std::string s_blindSF = "blindSF";
  std::string s_applyBlindSF = "applyBlindSF";
  std::string s_blindMET = "blindMET";
  std::string s_applyBlindMET = "applyBlindMET";
  std::string s_jetpTmin = "jetpTmin";
  std::string s_jetIDmin = "jetIDmin";
  std::string s_rhEmin = "rhEmin";
  std::string s_phpTmin = "phpTmin";
  std::string s_phIDmin = "phIDmin";
  std::string s_seedTimemin = "seedTimemin";
  std::string s_splitPho = "splitPho";
  std::string s_onlyGED = "onlyGED";
  std::string s_onlyOOT = "onlyOOT";
  std::string s_storeRecHits = "storeRecHits";
  std::string s_applyTrigger = "applyTrigger";
  std::string s_minHT = "minHT";
  std::string s_applyHT = "applyHT";
  std::string s_phgoodpTmin = "phgoodpTmin";
  std::string s_phgoodIDmin = "phgoodIDmin";
  std::string s_applyPhGood = "applyPhGood";
  std::string s_dRmin = "dRmin";
  std::string s_pTres = "pTres";
  std::string s_trackdRmin = "trackdRmin";
  std::string s_trackpTmin = "trackpTmin";
  std::string s_inputPaths = "inputPaths";
  std::string s_inputFilters = "inputFilters";
  // MC types
  std::string s_isGMSB = "isGMSB";
  std::string s_isHVDS = "isHVDS";
  std::string s_isBkgd = "isBkgd";
  std::string s_xsec = "xsec";
  std::string s_filterEff = "filterEff";
  std::string s_BR = "BR";


};

struct Event
{
  // branches
  UInt_t    run;
  UInt_t    lumi;
  ULong64_t event;
  Bool_t    hltSignal;
  Bool_t    hltRefPhoID;
  Bool_t    hltRefDispID;
  Bool_t    hltRefHT;
  Bool_t    hltPho50;
  Int_t     nvtx;
  Float_t   vtxX;
  Float_t   vtxY;
  Float_t   vtxZ;
  Float_t   rho;
  Float_t   t1pfMETpt;
  Float_t   t1pfMETphi;
  Float_t   t1pfMETsumEt;
  Float_t   jetHT;
  Int_t     njets;
  Int_t     nrechits;
  Int_t     nphotons;

  // MC Types
  Float_t   genwgt;
  Int_t     genpuobs;
  Int_t     genputrue;
  // GMSB
  Int_t     nNeutoPhGr;
  // HVDS
  Int_t     nvPions;

  // branch names
  std::string s_run = "run";
  std::string s_lumi = "lumi";
  std::string s_event = "event";
  std::string s_hltSignal = "hltSignal";
  std::string s_hltRefPhoID = "hltRefPhoID";
  std::string s_hltRefDispID = "hltRefDispID";
  std::string s_hltRefHT = "hltRefHT";
  std::string s_hltPho50 = "hltPho50";
  std::string s_nvtx = "nvtx";
  std::string s_vtxX = "vtxX";
  std::string s_vtxY = "vtxY";
  std::string s_vtxZ = "vtxZ";
  std::string s_rho = "rho";
  std::string s_t1pfMETpt = "t1pfMETpt";
  std::string s_t1pfMETphi = "t1pfMETphi";
  std::string s_t1pfMETsumEt = "t1pfMETsumEt";
  std::string s_jetHT = "jetHT";
  std::string s_njets = "njets";
  std::string s_nrechits = "nrechits";
  std::string s_nphotons = "nphotons";

  // MC types
  std::string s_genwgt = "genwgt";
  std::string s_genpuobs = "genpuobs";
  std::string s_genputrue = "genputrue";
  // GMSB
  std::string s_nNeutoPhGr = "nNeutoPhGr";
  // HVDS
  std::string s_nvPions = "nvPions";
};

struct Jet
{
  Float_t E;
  Float_t pt;
  Float_t eta;
  Float_t phi;
  
  std::string s_E = "jetE";
  std::string s_pt = "jetpt";
  std::string s_eta = "jeteta";
  std::string s_phi = "jetphi";
};
typedef std::vector<Jet> JetVec;

struct RecHits
{
  std::vector<Float_t> * eta;
  std::vector<Float_t> * phi;
  std::vector<Float_t> * E;
  std::vector<Float_t> * time;
  std::vector<Int_t>   * OOT;
  std::vector<UInt_t>  * ID;

  std::string s_eta = "rheta";
  std::string s_phi = "rhphi";
  std::string s_E = "rhE";
  std::string s_time = "rhtime";
  std::string s_OOT = "rhOOT";
  std::string s_ID = "rhID";
};

struct Pho
{
  Float_t E;
  Float_t pt;
  Float_t eta;
  Float_t phi;
  Float_t scE;
  Float_t sceta;
  Float_t scphi;
  Float_t HoE;
  Float_t r9;
  Float_t ChgHadIso;
  Float_t NeuHadIso;
  Float_t PhoIso;
  Float_t EcalPFClIso;
  Float_t HcalPFClIso;
  Float_t TrkIso;
  Float_t sieie;
  Float_t sipip;
  Float_t sieip;
  Float_t smaj;
  Float_t smin;
  Float_t alpha;
  Int_t   seed;
  std::vector<Int_t> * recHits;
  Bool_t  isOOT;
  Bool_t  isEB;
  Bool_t  isHLT;
  Bool_t  isTrk;
  Int_t   ID;
  // !storeRecHits
  Float_t seedtime;
  Float_t seedE;
  UInt_t  seedID;
  // MC types
  Bool_t  isGen;
  Int_t   isSignal;

  std::string s_E = "phoE";
  std::string s_pt = "phopt";
  std::string s_eta = "phoeta";
  std::string s_phi = "phophi";
  std::string s_scE = "phoscE";
  std::string s_sceta = "phosceta";
  std::string s_scphi = "phoscphi";
  std::string s_HoE = "phoHoE";
  std::string s_r9 = "phor9";
  std::string s_ChgHadIso = "phoChgHadIso";
  std::string s_NeuHadIso = "phoNeuHadIso";
  std::string s_PhoIso = "phoPhoIs";
  std::string s_EcalPFClIso = "phoEcalPFClIso";
  std::string s_HcalPFClIso = "phoHcalPFClIso";
  std::string s_TrkIso = "phoTrkIso";
  std::string s_sieie = "phosieie";
  std::string s_sipip = "phosipip";
  std::string s_sieip = "phosieip";
  std::string s_smaj = "phosmaj";
  std::string s_smin = "phosmin";
  std::string s_alpha = "phoalpha";
  std::string s_seed = "phoseed";
  std::string s_recHits = "phorecHits";
  std::string s_isOOT = "phoisOOT";
  std::string s_isEB = "phoisEB";
  std::string s_isHLT = "phoisHLT";
  std::string s_isTrk = "phoisTrk";
  std::string s_ID = "phoID";
  // !storeRecHits
  std::string s_seedtime = "phoseedtime";
  std::string s_seedE = "phoseedE";
  std::string s_seedID = "phoseedID";
  // MC Types
  std::string s_isGen = "phoisGen";
  std::string s_isSignal = "phoisSignal";
};
typedef std::vector<Pho> PhoVec;

struct GMSB
{
  Float_t genNmass;
  Float_t genNE;
  Float_t genNpt;
  Float_t genNphi;
  Float_t genNeta;
  Float_t genNprodvx;
  Float_t genNprodvy;
  Float_t genNprodvz;
  Float_t genNdecayvx;
  Float_t genNdecayvy;
  Float_t genNdecayvz;
  Float_t genphE;
  Float_t genphpt;
  Float_t genphphi;
  Float_t genpheta;
  Int_t   genphmatch;
  Float_t gengrmass;
  Float_t gengrE;
  Float_t gengrpt;
  Float_t gengrphi;
  Float_t gengreta;

  std::string s_genNmass = "genNmass";
  std::string s_genNE = "genNE";
  std::string s_genNpt = "genNpt";
  std::string s_genNphi = "genNphi";
  std::string s_genNeta = "genNeta";
  std::string s_genNprodvx = "genNprodvx";
  std::string s_genNprodvy = "genNprodvy";
  std::string s_genNprodvz = "genNprodvz";
  std::string s_genNdecayvx = "genNedcayvx";
  std::string s_genNdecayvy = "genNdecayvy";
  std::string s_genNdecayvz = "genNdecayvz";
  std::string s_genphE = "genphE";
  std::string s_genphpt = "genphpt";
  std::string s_genphphi = "genphphi";
  std::string s_genpheta = "genpheta";
  std::string s_genphmatch = "genphmatch";
  std::string s_gengrmass = "gengrmatch";
  std::string s_gengrE = "gengrE";
  std::string s_gengrpt = "gengrpt";
  std::string s_gengrphi = "gengrphi";
  std::string s_gengreta = "gengreta";
};
typedef std::vector<GMSB> GMSBVec;

struct HVDS
{
  Float_t genvPionmass;
  Float_t genvPionE;
  Float_t genvPionpt;
  Float_t genvPionphi;
  Float_t genvPioneta;
  Float_t genvPionprodvx;
  Float_t genvPionprodvy;
  Float_t genvPionprodvz;
  Float_t genvPiondecayvx;
  Float_t genvPiondecayvy;
  Float_t genvPiondecayvz;
  Float_t genHVph0E;
  Float_t genHVph0pt;
  Float_t genHVph0phi;
  Float_t genHVph0eta;
  Int_t   genHVph0match;
  Float_t genHVph1E;
  Float_t genHVph1pt;
  Float_t genHVph1phi;
  Float_t genHVph1eta;
  Int_t   genHVph1match;

  std::string s_genvPionmass = "genvPionmass";
  std::string s_genvPionE = "genvPionE";
  std::string s_genvPionpt = "genvPionpt";
  std::string s_genvPionphi = "genvPionphi";
  std::string s_genvPioneta = "genvPioneta";
  std::string s_genvPionprodvx = "genvPionprodvx";
  std::string s_genvPionprodvy = "genvPionprodvy";
  std::string s_genvPionprodvz = "genvPionprodvz";
  std::string s_genvPiondecayvx = "genvPiondecayvx";
  std::string s_genvPiondecayvy = "genvPiondecayvy";
  std::string s_genvPiondecayvz = "genvPiondecayvz";
  std::string s_genHVph0E = "genHVph0E";
  std::string s_genHVph0pt = "genHVph0pt";
  std::string s_genHVph0phi = "genHVph0phi";
  std::string s_genHVph0eta = "genHVph0eta";
  std::string s_genHVph0match = "genHVph0match";
  std::string s_genHVph1E = "genHVph1E";
  std::string s_genHVph1pt = "genHVph1pt";
  std::string s_genHVph1phi = "genHVph1phi";
  std::string s_genHVph1eta = "genHVph1eta";
  std::string s_genHVph1match = "genHVph1match";
};
typedef std::vector<HVDS> HVDSVec;

#endif