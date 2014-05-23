/*==============================================
testMVA.C: a Cint script that tests the
BDT and MLP packages

Written by: Zach
May, 2014
=============================================*/

#include <iostream>
#include <string>

#include <TRandom.h>
#include <TMVA/Factory.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TCut.h>
#include <TString.h>

// Begin main function
void testMVA(){
  // Number of measurements
  int events = 10000;

  // Initialize random object
  TRandom * random = new TRandom();
  random -> SetSeed(0);

  // Initialize variables for true values and smearing
  float z_true;
  float a_true;
  float b_true;
  float smear_a;
  float smear_b;
  
  // Create measured values
  TNtuple * meas = new TNtuple("ntuple", "Testing", "a:b:z_true:a_true:b_true");
  
  // Fill ntuple with "measured" values and true z
  for (int index = 0; index < events; index++){
    // Somewhat correlated variables
    z_true = random -> Rndm() * 100;
    a_true = random -> Rndm() * 100;
    b_true = z_true - a_true;
    
    // Completely independent variables
    /*a_true = random -> Rndm() * 100 - 50;
    b_true = random -> Rndm() * 100 - 50;
    z_true = a_true + b_true;*/
    
    smear_a = a_true + random -> Gaus(0,10);
    smear_b = b_true + random -> Gaus(0,10);
    
    meas -> Fill(smear_a, smear_b, z_true, a_true, b_true);
  }
  
  // Create TFile to store data and analysis
  TString outfileName = ("TMVA_test.root");
  TFile * out = new TFile(outfileName, "RECREATE");
  
  // Initialize TMVA factory object
  TString option = TString("!V:!Silent:Color:!DrawProgressBar:Transformations=I:AnalysisType=Regression");
  TMVA::Factory * factory = new TMVA::Factory("TMVARegression", out, option);
  
  // Input variables smeared a and b with training toward true z
  factory -> AddVariable("a");
  factory -> AddVariable("b");
  factory -> AddSpectator("a_true");
  factory -> AddSpectator("b_true");
  factory -> AddTarget("z_true");
  factory -> AddRegressionTree(meas);
  
  // Create arbitrary cut
  TCut cut = "";
  
  // Tell the factory to use all remaining events after training for testing
  TString option_train = TString("nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V");
  factory -> PrepareTrainingAndTestTree(cut, option_train);
  
  // BDT analysis
  TString option_BDT = TString("!H:!V:NTrees=5000:MaxDepth=4:nEventsMin=150:nCuts=100:BoostType=Grad:Shrinkage:0.4:NegWeightTreatment=NoNegWeightsInTraining:SeparationType=RegressionVariance:DoBoostMonitor=True:PruneMethod=NoPruning");
   factory -> BookMethod(TMVA::Types::kBDT, "BDT", option_BDT);
  
  // Shallow MLP Analysis
  TString option_SMLP = TString("!H:!V:VarTransform=Norm:NCycles=20000:HiddenLayers=1:NeuronType=tanh:EstimatorType=MSE:TrainingMethod=BFGS:LearningRate=0.0002:DecayRate=0.0001:TestRate=3:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:UseRegulator");
   factory -> BookMethod(TMVA::Types::kMLP, "Shallow_MLP", option_SMLP);
  
  // Deep MLP Analysis
  TString option_DMLP = TString("!H:!V:VarTransform=Norm:NCycles=20000:HiddenLayers=10:NeuronType=tanh:EstimatorType=MSE:TrainingMethod=BFGS:LearningRate=0.0002:DecayRate=0.0001:TestRate=3:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator");
  factory -> BookMethod(TMVA::Types::kMLP, "Deep_MLP", option_DMLP);
  
  // Execute analysis
  std::cout << " ** BEGIN TRAINING ** " << std::endl;
  
  factory -> TrainAllMethodsForRegression();
  
  std::cout << " ** BEGIN TESTING ** " << std::endl;
  
  factory -> TestAllMethods();
  
  std::cout << " ** BEGIN EVALUATION ** " << std::endl;
  
  factory -> EvaluateAllMethods();
  
  std::cout << " ** DONE WITH EVALUATION ** " << std::endl;
  
  //Close TFile
  out -> Close();
  delete factory;

} // End main function
 
