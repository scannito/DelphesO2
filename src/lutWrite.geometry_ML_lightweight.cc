#include "lutWrite.cc"

void fatInit_geometry_ML_lightweight(float field = 2.0, float rmin = 20.)
{
  // Layer properties
  Double_t x0VD = 0.001;      // 0.1%
  Double_t x0MLMod1 = 0.0045; // 0.45%
  Double_t x0MLMod2 = 0.0025; // 0.25%
  Double_t x0OT = 0.01;       // 1%

  Double_t resRPhiVD = 0.00025; // 2.5 mum
  Double_t resZVD = 0.00025;    // 2.5 mum
  Double_t resRPhiOT = 0.0010;  // 10 mum
  Double_t resZOT = 0.0010;     // 10 mum
  Double_t eff = 0.98;

  fat.AddLayer((char*)"vertex", 0, 0, 0); // dummy vertex for matrix calculation

  fat.AddLayer((char*)"bpipe0", 0.48, 0.00042); // 150 mum Be

  fat.AddLayer((char*)"L00", 0.5, x0VD, resRPhiVD, resZVD, eff);
  fat.AddLayer((char*)"L01", 1.2, x0VD, resRPhiVD, resZVD, eff);
  fat.AddLayer((char*)"L02", 2.5, x0VD, resRPhiVD, resZVD, eff);

  fat.AddLayer((char*)"bpipe1", 3.7, 0.0014); // 500 mum Be

  fat.AddLayer((char*)"L03", 7., x0MLMod1, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L04", 11., x0MLMod1, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L05", 15., x0MLMod1, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"iTOF", 19., 3. * x0OT, 0.03, 0.03, 1.0);

  fat.AddLayer((char*)"L06", 20., x0MLMod1, resRPhiOT, resZOT, eff);

  fat.AddLayer((char*)"L07", 30., x0OT, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L08", 45., x0OT, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L09", 60., x0OT, resRPhiOT, resZOT, eff);
  fat.AddLayer((char*)"L10", 80., x0OT, resRPhiOT, resZOT, eff);

  fat.SetMinRadTrack(20.);

  fat.SetAtLeastHits(7);
  fat.SetAtLeastCorr(0);
  fat.SetAtLeastFake(2);

  fat.SetBField(field);
  fat.SetMinRadTrack(rmin);
}

void lutWrite_geometry_ML_lightweight(const char* filename = "lutCovm.dat", int pdg = 211, float field = 2.0, float rmin = 20.)
{

  // init FAT
  fatInit_geometry_ML_lightweight(field, rmin);
  // write
  lutWrite(filename, pdg, field);
}
