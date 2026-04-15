// set AliRoot: /cvmfs/alice.cern.ch/bin/alienv enter VO_ALICE@AliRoot::v5-10-01-test-hepmc-1_ROOT6-1
// You have to load the class before ... ;-)
// .L DetectorK.cxx++

//void standardPlots() {

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObjArray.h>
#include "DetectorK.h"
#include "AliExternalTrackParam.h"
#endif


void testDetectorALICE3(double phiShift=0.0) {
//void testDetectorALICE3() {
  DetectorK::verboseG = true;
  DetectorK *alice3 = new DetectorK("ALICE3","alice3");

  std::vector<TString> layers = { "ddd0", "ddd1", "ddd2" };
  //std::vector<double> widths = { TMath::Pi()/8, TMath::Pi()/6, TMath::Pi()/4 };

  alice3->SetIntegrationTime(100.e-6); // 100 ns (as in LoI)
  alice3->SetMaxRadiusOfSlowDetectors(0.00001); // no slow detectors


  alice3->SetAvgRapidity(0.5);
  alice3->SetdNdEtaCent(2000);
  //alice3->SetdNdEtaCent(0);
  alice3->SetLhcUPCscale(1.);
  alice3->SetBField(2.); // Tesla
  //alice3->SetParticleMass(0.000511); // electron (default is pion)  
  //alice3->SetParticleMass(1.32); // electron (default is pion)  

  // Layer properties
  Double_t x0IT     = 0.001; // 0.1%
  Double_t x0OT     = 0.010; // 1%
  Double_t resRPhiIT     = 0.00025;  //  2.5 mum
  Double_t resZIT        = 0.00025;  //  2.5 mum
  Double_t resRPhiOT     = 0.0010;   // 10 mum
  Double_t resZOT        = 0.0010;   // 10 mum
  Double_t eff           = 0.98;
  //
  Double_t scaleR = 1.0;
  //
  //
 //  alice3->AddLayer((char*)"ddd01",  0.2 ,  x0IT, resRPhiIT, resZIT,eff);
 // alice3->AddLayer((char*)"ddd02",  0.4 ,  x0IT, resRPhiIT, resZIT,eff);
  alice3->AddLayer((char*)"vertex", 0.0, 0, 0);  // dummy vertex for matrix calculation

  alice3->AddLayer((char*)"bpipe0",  0.48 ,  0.00042);  // 150 mum Be 
  alice3->AddLayer((char*)"ddd0",  0.5 ,  x0IT, resRPhiIT, resZIT,eff);
  alice3->AddLayer((char*)"ddd1",  1.2 ,  x0IT, resRPhiIT, resZIT,eff);
  

  alice3->AddLayer((char*)"ddd2",  2.5 ,  x0IT, resRPhiIT, resZIT,eff);

  alice3->AddLayer((char*)"bpipe1",  5.7,  0.0014);  // 500 mum Be 
  alice3->AddLayer((char*)"ddd3",  7,  x0OT, resRPhiOT, resZOT,eff);

  alice3->AddLayer((char*)"ddd4",  9.,  x0OT, resRPhiOT, resZOT,eff);
  
  alice3->AddLayer((char*)"ddd5",  12. ,  x0OT, resRPhiOT, resZOT,eff);
  
  alice3->AddLayer((char*)"ddd6",  20. ,  x0OT, resRPhiOT, resZOT,eff);
  
  //alice3->AddLayer((char*)"iTOF",  19. ,  3.*x0OT, 0.03, 0.03, 1.0); 
  alice3->AddLayer((char*)"iTOF",  21. ,  3.*x0OT, 0.03, 0.03, 1.0); //NEW 23/03/26 iTOF moved to 21 cm 
  alice3->AddLayer((char*)"ddd7",  30.*scaleR ,  x0OT, resRPhiOT, resZOT,eff);
  alice3->AddLayer((char*)"ddd8",  45.*scaleR ,  x0OT, resRPhiOT, resZOT,eff);
  alice3->AddLayer((char*)"ddd9",  60.*scaleR ,  x0OT, resRPhiOT, resZOT,eff);
  alice3->AddLayer((char*)"ddd10",  80.*scaleR ,  x0OT, resRPhiOT, resZOT,eff); 

  alice3->AddLayer((char*)"oTOF",  92.  , 0.03, 0.15, 0.15,eff); //in the previous work it was commented out 
  
  //alice3->AddLayer((char*)"oTOF",  85.  , 0.03, 0.15, 0.15,eff); // 5 mm / sqrt(12)
  //alice3->AddLayer((char*)"rich",  110. , 0.03, 0.1, 0.1,eff);

  alice3->SetMinRadTrack(20.);
  alice3->SetAtLeastHits(7);
  alice3->SetAtLeastCorr(0);
  alice3->SetAtLeastFake(2);
  alice3->SetAtLeastInnerHits(3);

  // Test VD setup with Felix suggestion (try with cold plate as a separate layer - gives same result)
  //alice3->SetRadiationLength("bpipe0",0.0009); // 150 um AlBe (X0=16.6cm)
  alice3->SetRadiationLength("bpipe0",0.001592); //NEW 23-09-25 150 um AlBe (X0=16.6cm)
  alice3->SetRadiationLength("ddd0",0.00076); // 30 um Si + 50 um glue + carbon foam 0.03%
  alice3->SetRadiationLength("ddd1",0.00096); // 30 um Si + 50 um glue + carbon foam 0.05%
  //alice3->SetRadiationLength("ddd2",0.00167); // 30 um Si + 50 um glue + carbon foam 0.05% + 0.07% Be case 
  alice3->SetRadiationLength("ddd2",0.00096); //NEW 23-09-25 30 um Si + 50 um glue + carbon foam 0.05% + 0.07% Be case 
  //alice3->AddLayer((char*)"coldplate",  2.6,  0.02);  // (1.5 mm Al2O3 2%) 
  alice3->AddLayer((char*)"coldplate",  2.6,  0.0008928);  // NEW fibra carbonio di 0.2mm (23/03/26) using as X0=22.4cm
  alice3->AddLayer((char*)"petal1",  3.7,  0.001592);  // NEW 23-09-25 (1.5 mm Al2O3 2%) 
  alice3->SetRadiationLength("bpipe1",0.0023); // 800 um Be

  alice3->PrintLayout();

  alice3->SolveViaBilloir(0);
  alice3->MakeStandardPlots(0,1,2,kTRUE);


  return;
}




void testDetectorCurr() {
  DetectorK its("ALICE","ITS");
  its.MakeAliceCurrent(0,0);
  its.SetAtLeastCorr(4);
  its.SetAtLeastFake(1);
  its.PrintLayout();
  its.SolveViaBilloir(0);
 
  its.MakeStandardPlots(0,2,1,kTRUE);
  //  return;
  its.AddTPC(0.1,0.1);
  its.SolveViaBilloir(0);
 
  its.MakeStandardPlots(1,1,1,kTRUE);
  
}

void particleDependendResolution() { 
// particle dependency on resolution

  // .L DetectorK.cxx++

  DetectorK its("ALICE","ITS");

  its.MakeAliceCurrent(); 
  its.PrintLayout();
  its.PlotLayout();

  its.SolveViaBilloir(0);
 
  its.SetRadius("bpipe",2.1);
  its.AddLayer("spd0",2.2,0.001,0.0012,0.0012);

  TCanvas *c1 = new TCanvas("c1","c1");
 
  c1->Divide(2,1);
  c1->cd(1); gPad->SetGridx();   gPad->SetGridy(); 
  gPad->SetLogx(); //gPad->SetLogy();
  c1->cd(2); gPad->SetGridx();   gPad->SetGridy(); 
  gPad->SetLogx(); //gPad->SetLogy();

  
  // compare to telescope equation ?
  //  c1->cd(1); its.GetGraphPointingResolutionTeleEqu(0,1)->Draw("AC");
  //  c1->cd(2); its.GetGraphPointingResolutionTeleEqu(1,1)->Draw("AC");

  its.SetParticleMass(0.140); // pion  
  its.SolveViaBilloir(0,0);
  c1->cd(1); its.GetGraphPointingResolution(0,1)->Draw("AC");
  c1->cd(2); its.GetGraphPointingResolution(1,1)->Draw("AC");
 
  its.SetParticleMass(0.498); // kaon  
  its.SolveViaBilloir(0,0);
  c1->cd(1); its.GetGraphPointingResolution(0,2)->Draw("C");
  c1->cd(2); its.GetGraphPointingResolution(1,2)->Draw("C");

  its.SetParticleMass(0.00051); // electron  
  its.SolveViaBilloir(0,0);
  c1->cd(1); its.GetGraphPointingResolution(0,3)->Draw("C");
  c1->cd(2); its.GetGraphPointingResolution(1,3)->Draw("C");

  its.SetParticleMass(0.938); // proton  
  its.SolveViaBilloir(0,0);
  c1->cd(1); its.GetGraphPointingResolution(0,4)->Draw("C");
  c1->cd(2); its.GetGraphPointingResolution(1,4)->Draw("C");



}

