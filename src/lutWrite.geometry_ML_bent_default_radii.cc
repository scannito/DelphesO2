#include "lutWrite.cc"

void fatInit_geometry_ML_baseline_default_radii(float field = 2.0, float rmin = 20.)
{
  // Layer properties
  Double_t x0Pipe0 = 0.001592;      // 150 um AlBe (X0=16.6cm)
  Double_t x0VDL0 = 0.00076;        // 30 um Si + 50 um glue + carbon foam 0.03%
  Double_t x0VDL1 = 0.00096;        // 30 um Si + 50 um glue + carbon foam 0.05%
  Double_t x0VDL2 = 0.00096;        // 30 um Si + 50 um glue + carbon foam 0.05%
  Double_t x0Coldplate = 0.0008928; // NEW 0.2mm carbon fiber (23/03/26) using as X0=22.4cm
  Double_t x0Petal = 0.001592;      // 150 um AlBe (X0=16.6cm)
  Double_t x0Pipe1 = 0.0023;        // 800 um Be

  Double_t x0MLMod1 = 0.0045; // 0.45%
  Double_t x0MLMod2 = 0.0025; // 0.25%
  Double_t x0OT = 0.01;       // 1%

  DDouble_t xrho = 0;

  Double_t resRPhiVD = 0.00025; // 2.5 mum
  Double_t resZVD = 0.00025;    // 2.5 mum
  Double_t resRPhiOT = 0.0010;  // 10 mum
  Double_t resZOT = 0.0010;     // 10 mum

  Double_t eff = 0.98;

  /*Double_t alpha = 7.404 * TMath::DegToRad();  // ~0.1292 rad
  Double_t beta = 21.371 * TMath::DegToRad();  // ~0.3728 rad
  Double_t gamma = 11.134 * TMath::DegToRad(); // ~0.1943 rad

  Int_t nPetals = 3; // 3 petals INCLINED STANDARD*/

  // Geometry layout
  fat.AddLayer((char*)"vertex", 0.0, 0, 0); // dummy vertex for matrix calculation

  fat.AddLayer((char*)"bpipe0", 0.48, x0Pipe0, xrho);

  fat.AddLayer((char*)"ddd0", 0.5, x0VDL0, xrho, resRPhiVD, resZVD, eff);
  // fat.AddPetalGapsToLayer("ddd0", nPetals, 0.240, 0.);
  fat.AddLayer((char*)"ddd1", 1.2, x0VDL1, xrho, resRPhiVD, resZVD, eff);
  // fat.AddPetalGapsToLayer("ddd1", nPetals, 0.1, alpha + beta);
  fat.AddLayer((char*)"ddd2", 2.5, x0VDL2, xrho, resRPhiVD, resZVD, eff);
  // fat.AddPetalGapsToLayer("ddd2", nPetals, 0.048, alpha + beta + gamma);

  fat.AddLayer((char*)"coldplate", 2.6, x0Coldplate, xrho);
  fat.AddLayer((char*)"petal1", 3.7, x0Petal, xrho);

  fat.AddLayer((char*)"bpipe1", 5.7, x0Pipe1, xrho);

  fat.AddLayer((char*)"ddd3", 7, x0MLMod2, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd4", 9., x0MLMod2, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd5", 12., x0MLMod1, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd6", 20., x0MLMod1, xrho, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"iTOF", 21., 3. * x0OT, xrho, 0.03, 0.03, 1.0); // NEW 23/03/26 iTOF moved to 21 cm

  fat.AddLayer((char*)"ddd7", 30., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd8", 45., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd9", 60., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"ddd10", 80., x0OT, xrho, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"oTOF", 92., 3. * x0OT, xrho, 0.15, 0.15, 1.0); // in the previous work it was commented out

  fat.SetAtLeastHits(7);
  fat.SetAtLeastCorr(0);
  fat.SetAtLeastFake(2);
  // fat.SetAtLeastInnerHits(3);

  fat.SetBField(field);
  fat.SetMinRadTrack(rmin);

  return;
}

void lutWrite_geometry_ML_baseline_default_radii(const char* filename = "lutCovm.dat", int pdg = 211, float field = 2.0, float rmin = 20.)
{

  // init FAT
  fatInit_geometry_ML_baseline_default_radii(field, rmin);
  // write
  lutWrite(filename, pdg, field);
}
