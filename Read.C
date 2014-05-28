/*==============================================
Read.C: a Cint script that experiments
with different TMVA methods to analyze pT data

Written by: Zach
May, 2014
=============================================*/

#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TText.h>

// Histograms of the true values
TH1F * a_hist = new TH1F("True a", "True a;Value of a;Frequency", 100, -150, 150);
TH1F * b_hist = new TH1F("True b", "True b;Value of b;Frequency", 100, -150, 150);
TH1F * z_hist = new TH1F("True z", "True z;Value of z;Frequency", 100, -150, 150);

// Historgrams of the observed values
TH1F * a_obs = new TH1F("Observed a", "Observed a;Value of a;Frequency", 100, -120, 120);
TH1F * b_obs = new TH1F("Observed b", "Observed b;Value of b;Frequency", 100, -120, 120);
TH1F * z_obs = new TH1F("Observed z", "Observed z;Value of z;Frequency", 100, -120, 120);

// Histograms of the z values found through TMVA analyses
TH1F * z_bdt  = new TH1F("BDT z",  "BDT z;Value of z;Frequency",         100, -150, 150);
TH1F * z_smlp = new TH1F("SMLP z", "Shallow MLP z;Value of z;Frequency", 100, -150, 150);
TH1F * z_dmlp = new TH1F("DMLP z", "Deep MLP z;Value of z;Frequency",    100, -150, 150);

// Residuals of observed/calculated values compared to true values 
TH1F * a_residual    = new TH1F("Residual of a",    "Residual a;a_obs - a_true;Frequency",       100, -100, 100);
TH1F * b_residual    = new TH1F("Residual of b",    "Residual b;b_obs - b_true;Frequency",       100, -100, 100);
TH1F * z_residual    = new TH1F("Residual of z",    "Residual z;z_obs - z_true;Frequency",       100, -100, 100);
TH1F * bdt_residual  = new TH1F("Residual of BDT",  "Residual z_bdt;z_bdt - z_true;Frequency",   100, -100, 100);
TH1F * smlp_residual = new TH1F("Residual of SMLP", "Residual z_smlp;z_smlp - z_true;Frequency", 100, -100, 100);
TH1F * dmlp_residual = new TH1F("Residual of DMLP", "Residual z_dmlp;z_dmlp - z_true;Frequency", 100, -100, 100);

// Comparison of TMVA analyses to observed z
TH1F * bdt_comp =  new TH1F("BDT z vs Observed z",  "Residual of z_bdt with respect to z_obs;z_bdt - z_obs;Frequency",   100, -100, 100);
TH1F * smlp_comp = new TH1F("SMLP z vs Observed z", "Residual of z_smlp with respect to z_obs;z_smlp - z_obs;Frequency", 100, -100, 100);
TH1F * dmlp_comp = new TH1F("DMLP z vs Observed z", "Residual of z_dmlp with respect to z_obs;z_dmlp - z_obs;Frequency", 100, -100, 100);

// Correlations
TH2F * corr_true = new TH2F("True a and b Correlation", "True b vs True a;Value of a;Value of b", 100, -150, 150, 100, -150, 150);
TH2F * corr_obs = new TH2F("Observed a and b Correlation", "Observed b vs Observed a;Value of a;Value of b", 100, -150, 150, 100, -150, 150);

// Initialize the TFile to be read
TFile * inFile = new TFile("TMVA_test.root");

// Begin main function
void Read(){
  // Get TestTree from the file
  TTree * testTree  = (TTree*) inFile -> Get("TestTree");
  
  // Initialize values to be read from TTree
  float a_true, b_true, z_true, a_test, b_test, bdt, smlp, dmlp;

  // Specify location of values in TTree
  testTree -> SetBranchAddress("a",           &a_test);
  testTree -> SetBranchAddress("b",           &b_test);
  testTree -> SetBranchAddress("z_true",      &z_true);
  testTree -> SetBranchAddress("a_true",      &a_true);
  testTree -> SetBranchAddress("b_true",      &b_true);
  testTree -> SetBranchAddress("BDT",         &bdt);
  testTree -> SetBranchAddress("Shallow_MLP", &smlp);
  testTree -> SetBranchAddress("Deep_MLP",    &dmlp);
  
  // Fill in histograms
  for (int i = 0; i < testTree -> GetEntries(); i++){
    testTree  -> GetEntry(i);

    a_hist -> Fill(a_true);
    b_hist -> Fill(b_true);
    z_hist -> Fill(z_true);

    a_obs -> Fill(a_test);
    b_obs -> Fill(b_test);
    z_obs -> Fill(a_test + b_test);
    z_bdt -> Fill(bdt);
    z_smlp -> Fill(smlp);
    z_dmlp -> Fill(dmlp);

    a_residual ->    Fill(a_test - a_true);
    b_residual ->    Fill(b_test - b_true);
    z_residual ->    Fill(a_test + b_test - z_true);
    bdt_residual ->  Fill(bdt - z_true);
    smlp_residual -> Fill(smlp - z_true);
    dmlp_residual -> Fill(dmlp - z_true);
  
    bdt_comp ->  Fill(bdt - a_test - b_test);
    smlp_comp -> Fill(smlp - a_test - b_test);
    dmlp_comp -> Fill(dmlp - a_test - b_test);

    corr_true -> Fill(a_true, b_true);
    corr_obs -> Fill(a_test, b_test);
  }

  // Initialize TCanvas for drawing
  TCanvas * canvas = new TCanvas("Canvas");

  a_hist -> Draw();
  canvas -> SaveAs("a_true.jpg");
  b_hist -> Draw();
  canvas -> SaveAs("b_true.jpg");
  z_hist -> Draw();
  canvas -> SaveAs("z_true.jpg");

  a_obs  -> Draw();
  canvas -> SaveAs("a_obs.jpg");
  b_obs  -> Draw();
  canvas -> SaveAs("b_obs.jpg");
  z_obs  -> Draw();
  canvas -> SaveAs("z_obs.jpg");
  z_bdt  -> Draw();
  canvas -> SaveAs("z_bdt.jpg");
  z_smlp -> Draw();
  canvas -> SaveAs("z_smlp.jpg");
  z_dmlp -> Draw();
  canvas -> SaveAs("z_dmlp.jpg");

  a_residual    -> Draw();
  canvas        -> SaveAs("a_res.jpg");
  b_residual    -> Draw();
  canvas        -> SaveAs("b_res.jpg");
  z_residual    -> Draw();
  canvas        -> SaveAs("z_res.jpg");
  bdt_residual  -> Draw();
  canvas        -> SaveAs("bdt_res.jpg");
  smlp_residual -> Draw();
  canvas        -> SaveAs("smlp_res.jpg");
  dmlp_residual -> Draw();
  canvas        -> SaveAs("dmlp_res.jpg");

  bdt_comp -> Draw();
  canvas -> SaveAs("bdt_res_obs.jpg");
  smlp_comp -> Draw();
  canvas -> SaveAs("smlp_res_obs.jpg");
  dmlp_comp -> Draw();
  canvas -> SaveAs("dmlp_res_dmlp.jpg");

  corr_true -> Draw();
  canvas -> SaveAs("corr_true.jpg");
  corr_true   -> Draw("COLZ");
  canvas -> SaveAs("corr_true_color.jpg");
  cout << "True value correlation: " << corr_true -> GetCorrelationFactor() << "\n";

  corr_obs -> Draw();
  canvas -> SaveAs("corr_obs.jpg");
  corr_obs   -> Draw("COLZ");
  canvas -> SaveAs("corr_obs_corr.jpg");
  cout << "Observed value correlation: " << corr_obs -> GetCorrelationFactor() << "\n";

  // CLose TFile
  inFile -> Close();
} // End main function
