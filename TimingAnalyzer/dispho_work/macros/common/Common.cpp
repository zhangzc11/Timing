#include "Common.hh"
#include "TLatex.h"
#include "TColor.h"

#include <iostream>

void CheckValidFile(const TFile * file, const TString & fname)
{
  if (file == (TFile*) NULL) // check if valid file
  {
    std::cerr << "Input file is bad pointer: " << fname.Data()
	      << " ...exiting..." << std::endl;
    exit(1);
  }
  else 
  {
    std::cout << "Successfully opened file: " << fname.Data() << std::endl;
  }
}

void CheckValidTree(const TTree * tree, const TString & tname, const TString & fname)
{
  if (tree == (TTree*) NULL) // check if valid plot
  {
    std::cerr << "Input TTree is bad pointer: " << tname.Data() << " in input file: " << fname.Data()
	      << " ...exiting..." << std::endl;
    exit(1);
  }
  else 
  {
    std::cout << "Successfully opened tree: " << tname.Data() << " in input file: " << fname.Data() << std::endl;
  }
}

void CheckValidTH1F(const TH1F * plot, const TString & pname, const TString & fname)
{
  if (plot == (TH1F*) NULL) // check if valid plot
  {  std::cerr << "Input TH1F is bad pointer: " << pname.Data() << " in input file: " << fname.Data() 
	      << " ...exiting..." << std::endl;
    exit(1);
  }
}

void CMSLumi(TCanvas * canv, const Int_t iPosX) 
{
  const TString  cmsText     = "CMS";
  const Double_t cmsTextFont = 61;  // default is helvetic-bold
  
  // extraText is either "Simulation" or "Preliminary"
  const Bool_t   writeExtraText  = !(Config::extraText.EqualTo("",TString::kExact));
  const Double_t extraTextFont   = 52;  // default is helvetica-italics

  const TString lumiText = Form("%5.2f fb^{-1} (13 TeV)", Config::lumi); // must change this spec once we are in fb range!
  
  // text sizes and text offsets with respect to the top frame
  // in unit of the top margin size
  const Double_t lumiTextSize     = 0.6;
  const Double_t lumiTextOffset   = 0.2;
  const Double_t cmsTextSize      = 0.75;
  const Double_t cmsTextOffset    = 0.1;  // only used in outOfFrame version

  const Double_t relPosX    = 0.045;
  const Double_t relPosY    = 0.035;
  const Double_t relExtraDY = 1.2;
 
  // ratio of "CMS" and extra text size
  const Double_t extraOverCmsTextSize  = 0.76;
 
  const Bool_t outOfFrame = (iPosX/10 == 0);

  Int_t alignY_=3;
  Int_t alignX_=2;
  if (iPosX    == 0) {alignY_ = 1;}
  if (iPosX/10 <= 1) {alignX_ = 1;}
  if (iPosX/10 == 2) {alignX_ = 2;}
  if (iPosX/10 == 3) {alignX_ = 3;}
  const Int_t align_ = 10*alignX_ + alignY_;

  const Double_t H = canv->GetWh();
  const Double_t W = canv->GetWw();
  const Double_t l = canv->GetLeftMargin();
  const Double_t t = canv->GetTopMargin();
  const Double_t r = canv->GetRightMargin();
  const Double_t b = canv->GetBottomMargin();
  const Double_t e = 0.025;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  const Double_t extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

  if (outOfFrame) 
  {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); 
    latex.SetTextSize(cmsTextSize*t);    
    latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
  }
  
  Double_t posX_ = 0;
  if (iPosX%10 <= 1)  
  {
    posX_ =  l + relPosX*(1-l-r);
  }
  else if (iPosX%10 == 2) 
  {
    posX_ =  l + 0.5*(1-l-r);
  }
  else if (iPosX%10 == 3) 
  {
    posX_ =  1-r - relPosX*(1-l-r);
  }

  Double_t posY_ = 1-t - relPosY*(1-t-b);

  if (!outOfFrame) 
  {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextSize(cmsTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, cmsText);
    
    if (writeExtraText) 
    {
      latex.SetTextFont(extraTextFont);
      latex.SetTextAlign(align_);
      latex.SetTextSize(extraTextSize*t);
      latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, Config::extraText);
    }
  }
  
  else if (outOfFrame && writeExtraText)
  {
    if (iPosX == 0) 
    {
      posX_ = l +  relPosX*(1-l-r)+0.05;
      posY_ = 1-t+lumiTextOffset*t;
    }
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, Config::extraText);
  }
}

void SetTDRStyle(TStyle * tdrStyle)
{  
  // For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(700); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //Position on screen
  tdrStyle->SetCanvasDefY(0);

  // For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

  // For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);

  // For the histo:
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  tdrStyle->SetEndErrorSize(2);
  tdrStyle->SetErrorX(0.5);
  tdrStyle->SetMarkerStyle(20);
  tdrStyle->SetMarkerSize(0.6);

  //For the fit/function:
  tdrStyle->SetOptFit(0);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);

  //For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);

  // For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  
  // Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.14);
  tdrStyle->SetPadLeftMargin(0.18);
  tdrStyle->SetPadRightMargin(0.15); 

  // For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);

  // For the axis titles:
  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(Config::TitleSize, "XYZ");
  tdrStyle->SetTitleXOffset(Config::TitleXOffset);
  tdrStyle->SetTitleYOffset(Config::TitleYOffset);

  // For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(Config::LabelOffset, "XYZ");
  tdrStyle->SetLabelSize(Config::LabelSize, "XYZ");

  // For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(Config::TickLength, "XYZ");
  tdrStyle->SetNdivisions(Config::Ndivisions, "X");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

  // Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

  // Postscript options:
  // tdrStyle->SetPaperSize(15.,15.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);
  
  // for a nice color palette
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  tdrStyle->SetNumberContours(NCont);

  tdrStyle->cd();
}
