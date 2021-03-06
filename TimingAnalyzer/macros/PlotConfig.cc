#include "PlotConfig.hh"

namespace Config
{
  UInt_t  NEvCheck = 1000;
  Bool_t  ApplyEvCut = false;

  Bool_t  isHLT2 = false;
  Bool_t  isHLT3 = false;
  Bool_t  isHLT4 = false;
  Bool_t  isuserHLT = false;
  Bool_t  ApplyHLTMatching = false;
  Int_t   filterIdx = -1;

  Bool_t  ApplyJetHtCut = false;
  Float_t JetHtCut = 300.f;
  Bool_t  ApplyMinJetPtCut = false;
  Float_t MinJetPtCut = 15.f;
  Bool_t  ApplyJetPtCut = false;
  Float_t JetPtCut = 35.f;
  Bool_t  ApplyNJetsCut = false;
  Int_t   NJetsCut = 3;

  Bool_t  ApplyPh1PtCut = false;
  Float_t Ph1PtCut = 60.f;
  Bool_t  ApplyPh1VIDCut = false;
  TString Ph1VID = "medium";
  Bool_t  ApplyPh1R9Cut = false;
  Float_t Ph1R9Cut = 0.9;
  Bool_t  ApplyPh1SmajEBMin = false;
  Float_t Ph1SmajEBMin = 0.f;
  Bool_t  ApplyPh1SmajEBMax = false;
  Float_t Ph1SmajEBMax = 1.5;
  Bool_t  ApplyPh1SmajEEMin = false;
  Float_t Ph1SmajEEMin = 0.f;
  Bool_t  ApplyPh1SmajEEMax = false;
  Float_t Ph1SmajEEMax = 1.5;
  Bool_t  ApplyPh1SminEBMin = false;
  Float_t Ph1SminEBMin = 0.0;
  Bool_t  ApplyPh1SminEBMax = false;
  Float_t Ph1SminEBMax = 0.4;
  Bool_t  ApplyPh1SminEEMin = false;
  Float_t Ph1SminEEMin = 0.f;
  Bool_t  ApplyPh1SminEEMax = false;
  Float_t Ph1SminEEMax = 0.4;

  Bool_t  ApplyPhAnyPtCut = true;
  Float_t PhAnyPtCut = 10.f;
  Bool_t  ApplyPhAnyVIDCut = false;
  TString PhAnyVID = "loose";
  Bool_t  ApplyPhAnyR9Cut = false;
  Float_t PhAnyR9Cut = 0.9;
  Bool_t  ApplyPhAnySmajEBMin = false;
  Float_t PhAnySmajEBMin = 0.f;
  Bool_t  ApplyPhAnySmajEBMax = false;
  Float_t PhAnySmajEBMax = 1.5;
  Bool_t  ApplyPhAnySmajEEMin = false;
  Float_t PhAnySmajEEMin = 0.f;
  Bool_t  ApplyPhAnySmajEEMax = false;
  Float_t PhAnySmajEEMax = 1.5;
  Bool_t  ApplyPhAnySminEBMin = false;
  Float_t PhAnySminEBMin = 0.0;
  Bool_t  ApplyPhAnySminEBMax = false;
  Float_t PhAnySminEBMax = 0.4;
  Bool_t  ApplyPhAnySminEEMin = false;
  Float_t PhAnySminEEMin = 0.f;
  Bool_t  ApplyPhAnySminEEMax = false;
  Float_t PhAnySminEEMax = 0.4;
  Bool_t  ApplyrhECut = true;

  Float_t rhECut = 1.f;
  Bool_t  ApplyECALAcceptCut = true;
  Bool_t  ApplyEBOnly = false;
  Bool_t  ApplyEEOnly = false;
  Bool_t  ApplyPhMCMatchingCut = false;
  Bool_t  ApplyExactPhMCMatch = false;
  Bool_t  ApplyAntiPhMCMatch = false;
}
