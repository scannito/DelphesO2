#include "lutWrite.cc"

void fatInit_geometry_ML_bent_VD_realistic(float field = 2.0, float rmin = 20.)
{
  // Layer properties
  Double_t x0VD = 0.001;      // 0.1%
  Double_t x0MLMod1 = 0.0045; // 0.45%
  Double_t x0MLMod2 = 0.0025; // 0.25%
  Double_t x0OT = 0.01;       // 1%

  Double_t x0Petal = 0.001592;  // 200 um AlBe (X0=16.6cm)
  Double_t x0VDL0 = 0.00076;    // 30 um Si + 50 um glue + carbon foam 0.03%
  Double_t x0VDL1 = 0.00096;    // 30 um Si + 50 um glue + carbon foam 0.05%
  Double_t x0VDL2 = 0.00096;    // 30 um Si + 50 um glue + carbon foam 0.05%
  Double_t x0Coldplate = 0.02f; // (1.5 mm Al2O3 2%)
  Double_t x0Pipe1 = 0.0023f;   // 800 um Be

  Double_t xrhoVD = 2.3292e-02;           // 100 mum Si
  Double_t xrhoMLMod1 = 4.5 * 2.3292e-02; // 450 mum Si
  Double_t xrhoMLMod2 = 2.5 * 2.3292e-02; // 250 mum Si
  Double_t xrhoOT = 2.3292e-01;           // 1000 mum Si

  Double_t xrho = 0; // for AliPhysics-like DetectorK

  Double_t resRPhiVD = 0.00025; // 2.5 mum
  Double_t resZVD = 0.00025;    // 2.5 mum
  Double_t resRPhiOT = 0.0010;  // 10 mum
  Double_t resZOT = 0.0010;     // 10 mum

  Double_t eff = 0.98;

  fat.AddLayer((char*)"vertex", 0, 0, 0); // dummy vertex for matrix calculation

  fat.AddLayer((char*)"petal0", 0.48, x0Petal, xrho);

  fat.AddLayer((char*)"L00", 0.50, x0VDL0, xrho, resRPhiVD, resZVD, eff);
  fat.AddLayer((char*)"L01", 1.20, x0VDL1, xrho, resRPhiVD, resZVD, eff);
  fat.AddLayer((char*)"L02", 2.50, x0VDL2, xrho, resRPhiVD, resZVD, eff);

  fat.AddLayer((char*)"coldplate", 2.60, x0Coldplate, xrho);

  fat.AddLayer((char*)"petal1", 3.70, x0Petal, xrho);

  fat.AddLayer((char*)"bpipe1", 5.70, x0Pipe1, xrho);

  fat.AddLayer((char*)"L03", 7., x0MLMod2, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L04", 11., x0MLMod2, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L05", 15., x0MLMod1, xrho, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"iTOF", 19., 3. * x0OT, xrho, 0.03, 0.03, 1.0);

  fat.AddLayer((char*)"L06", 20., x0MLMod1, xrho, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"L07", 30., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L08", 45., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L09", 60., x0OT, xrho, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L10", 80., x0OT, xrho, resRPhiOT, resZOT, eff);

  fat.SetMinRadTrack(20.);

  fat.SetAtLeastHits(4);
  fat.SetAtLeastCorr(4);
  fat.SetAtLeastFake(0);

  fat.SetBField(field);
  fat.SetMinRadTrack(rmin);
}

void lutWrite_geometry_ML_bent_VD_realistic(const char* filename = "lutCovm.dat", int pdg = 211, float field = 2.0, float rmin = 20.)
{

  // init FAT
  fatInit_geometry_ML_bent_VD_realistic(field, rmin);
  // write
  lutWrite(filename, pdg, field);
}
