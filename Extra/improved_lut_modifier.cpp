// Versione migliorata con debug e test con valori dummy
// Carica con: .L improved_lut_modifier.cpp++

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TString.h"

// O2 includes
#include "ALICE3/Core/DelphesO2TrackSmearer.h"

// Funzione migliorata per ispezionare una LUT con debug avanzato
void inspectLUTDetailed(const char* lutFile, int pdg = 211) {
    std::cout << "=== ISPEZIONE DETTAGLIATA LUT ===" << std::endl;
    std::cout << "File: " << lutFile << std::endl;
    std::cout << "PDG: " << pdg << std::endl;
    
    try {
        o2::delphes::DelphesO2TrackSmearer smearer;
        smearer.loadTable(pdg, lutFile);
        
        auto lutHeader = smearer.getLUTHeader(pdg);
        if (!lutHeader) {
            std::cout << "Errore: impossibile caricare l'header!" << std::endl;
            return;
        }
        
        std::cout << "\nHeader caricato con successo!" << std::endl;
        std::cout << "Mass: " << lutHeader->mass << std::endl;
        std::cout << "Field: " << lutHeader->field << std::endl;
        std::cout << "Nch bins: " << lutHeader->nchmap.nbins 
                 << " range [" << lutHeader->nchmap.min << ", " << lutHeader->nchmap.max << "] log=" << lutHeader->nchmap.log << std::endl;
        std::cout << "Radius bins: " << lutHeader->radmap.nbins 
                 << " range [" << lutHeader->radmap.min << ", " << lutHeader->radmap.max << "] log=" << lutHeader->radmap.log << std::endl;
        std::cout << "Eta bins: " << lutHeader->etamap.nbins 
                 << " range [" << lutHeader->etamap.min << ", " << lutHeader->etamap.max << "] log=" << lutHeader->etamap.log << std::endl;
        std::cout << "Pt bins: " << lutHeader->ptmap.nbins 
                 << " range [" << lutHeader->ptmap.min << ", " << lutHeader->ptmap.max << "] log=" << lutHeader->ptmap.log << std::endl;
        
        // Calcola alcuni valori di test per capire il binning
        std::cout << "\n=== VALORI DI BINNING ===" << std::endl;
        
        // Test dei primi valori di pt
        std::cout << "\nPrimi valori di pT:" << std::endl;
        for (int i = 0; i < std::min(5, lutHeader->ptmap.nbins); ++i) {
            float pt = lutHeader->ptmap.eval(i);
            std::cout << "Bin " << i << ": pt = " << pt << " GeV/c" << std::endl;
        }
        
        // Test dei primi valori di eta  
        std::cout << "\nPrimi valori di eta:" << std::endl;
        for (int i = 0; i < std::min(5, lutHeader->etamap.nbins); ++i) {
            float eta = lutHeader->etamap.eval(i);
            std::cout << "Bin " << i << ": eta = " << eta << std::endl;
        }
        
        // Test dei valori di nch
        std::cout << "\nPrimi valori di nch:" << std::endl;
        for (int i = 0; i < std::min(5, lutHeader->nchmap.nbins); ++i) {
            float nch = lutHeader->nchmap.eval(i);
            std::cout << "Bin " << i << ": nch = " << nch << std::endl;
        }
        
        // Test con diversi parametri per trovare entries valide
        std::cout << "\n=== RICERCA ENTRIES VALIDE ===" << std::endl;
        int validEntries = 0;
        int totalTested = 0;
        
        // Prova con il centro delle distribuzioni
        float nch_center = lutHeader->nchmap.eval(lutHeader->nchmap.nbins / 2);
        float radius_center = lutHeader->radmap.eval(0); // Solo 1 bin
        
        // Prova con eta = 0 (centro del barrel)
        float eta_center = 0.0;
        
        // Testa diversi valori di pt
        for (int ipt = 0; ipt < std::min(200, lutHeader->ptmap.nbins); ipt += 10) {
            float pt = lutHeader->ptmap.eval(ipt);
            
            float dummyEff;
            auto lutEntry = smearer.getLUTEntry(pdg, nch_center, radius_center, eta_center, pt, dummyEff);
            totalTested++;
            
            if (lutEntry && lutEntry->valid) {
                validEntries++;
                if (validEntries <= 20) { // Stampa solo le prime 5 entries valide
                    float ptRes = sqrt(lutEntry->covm[14]) * pt;
                    std::cout << "VALIDA - pt=" << pt << " GeV/c, eff=" << lutEntry->eff 
                             << ", pt_res=" << ptRes << " GeV/c (rel: " << ptRes/pt*100 << "%) - (nch=" << nch_center 
                             << ", radius=" << radius_center << ", eta=" << eta_center << ")" << std::endl;
                }
            } else {
                if (totalTested <= 20) { // Stampa dettagli per i primi tentativi
                    std::cout << "NON VALIDA - pt=" << pt << " GeV/c (nch=" << nch_center 
                             << ", radius=" << radius_center << ", eta=" << eta_center << ")" << std::endl;
                }
            }
        }
        
        std::cout << "\nRisultato ricerca: " << validEntries << "/" << totalTested << " entries valide trovate" << std::endl;
        
        // Se non troviamo entries valide, proviamo con parametri diversi
        if (validEntries == 0) {
            std::cout << "\n=== RICERCA ALTERNATIVA ===" << std::endl;
            // Prova con il primo bin di tutto
            float nch_first = lutHeader->nchmap.eval(0);
            float eta_first = lutHeader->etamap.eval(lutHeader->etamap.nbins / 2); // Eta centrale
            float pt_first = lutHeader->ptmap.eval(lutHeader->ptmap.nbins / 2); // Pt centrale
            
            float dummyEff;
            auto lutEntry = smearer.getLUTEntry(pdg, nch_first, radius_center, eta_first, pt_first, dummyEff);
            
            if (lutEntry && lutEntry->valid) {
                std::cout << "TROVATA con parametri alternativi!" << std::endl;
                std::cout << "nch=" << nch_first << ", eta=" << eta_first << ", pt=" << pt_first << std::endl;
                std::cout << "eff=" << lutEntry->eff << ", pt_res=" << sqrt(lutEntry->covm[14]) * pt_first << std::endl;
            } else {
                std::cout << "Nessuna entry valida trovata nemmeno con parametri alternativi" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
    }
}

// Funzione per creare LUT modificate con valori dummy
bool createTestLUT(const char* inputLUT, const char* outputLUT, int pdg = 211) {
    std::cout << "=== CREAZIONE LUT DI TEST ===" << std::endl;
    std::cout << "Input: " << inputLUT << std::endl;
    std::cout << "Output: " << outputLUT << std::endl;
    
    try {
        // Carica le LUT esistenti
        o2::delphes::DelphesO2TrackSmearer smearer;
        smearer.loadTable(pdg, inputLUT);
        
        auto lutHeader = smearer.getLUTHeader(pdg);
        if (!lutHeader) {
            std::cout << "Errore nel caricare l'header!" << std::endl;
            return false;
        }
        
        // Apri il file di output
        std::ofstream outFile(outputLUT, std::ios::binary);
        if (!outFile.is_open()) {
            std::cout << "Errore nell'aprire il file di output!" << std::endl;
            return false;
        }
        
        // Copia l'header
        outFile.write(reinterpret_cast<const char*>(lutHeader), sizeof(*lutHeader));
        
        // Calcola il numero totale di entries
        int totalEntries = lutHeader->nchmap.nbins * lutHeader->radmap.nbins * 
                          lutHeader->etamap.nbins * lutHeader->ptmap.nbins;
        
        std::cout << "Processando " << totalEntries << " entries..." << std::endl;
        
        int processedEntries = 0;
        int modifiedEntries = 0;
        
        // Definisci funzioni dummy per efficienza e risoluzione
        auto dummyEfficiency = [](float pt) -> float {
            // Efficienza che cresce con pt e si stabilizza
            if (pt < 0.1) return 0.5;
            if (pt > 10.0) return 0.95;
            return 0.5 + 0.45 * (1.0 - exp(-pt/2.0));
        };
        
        auto dummyResolution = [](float pt) -> float {
            // Risoluzione relativa che migliora con pt (in percentuale)
            if (pt < 0.1) return 10.0; // 10%
            if (pt > 10.0) return 1.0;  // 1%
            return 1.0 + 9.0 * exp(-pt/2.0); // Da 10% a 1%
        };
        
        // Loop su tutte le dimensioni
        for (int inch = 0; inch < lutHeader->nchmap.nbins; ++inch) {
            float nch = lutHeader->nchmap.eval(inch);
            for (int irad = 0; irad < lutHeader->radmap.nbins; ++irad) {
                float radius = lutHeader->radmap.eval(irad);
                for (int ieta = 0; ieta < lutHeader->etamap.nbins; ++ieta) {
                    float eta = lutHeader->etamap.eval(ieta);
                    for (int ipt = 0; ipt < lutHeader->ptmap.nbins; ++ipt) {
                        float pt = lutHeader->ptmap.eval(ipt);
                        
                        float dummyEff;
                        auto lutEntry = smearer.getLUTEntry(pdg, nch, radius, eta, pt, dummyEff);
                        
                        if (lutEntry && lutEntry->valid) {
                            // Applica modifiche dummy
                            lutEntry->eff = dummyEfficiency(pt);
                            
                            // Risoluzione relativa in percentuale convertita in elemento matrice
                            float relRes = dummyResolution(pt) / 100.0; // Converti da % a frazione
                            lutEntry->covm[14] = (relRes / pt) * (relRes / pt);
                            
                            modifiedEntries++;
                        }
                        
                        // Scrivi l'entry
                        if (lutEntry) {
                            outFile.write(reinterpret_cast<const char*>(lutEntry), sizeof(*lutEntry));
                        }
                        
                        processedEntries++;
                        if (processedEntries % 50000 == 0) {
                            std::cout << "Processate " << processedEntries << "/" << totalEntries 
                                     << " (" << (processedEntries*100/totalEntries) << "%)" << std::endl;
                        }
                    }
                }
            }
        }
        
        outFile.close();
        
        std::cout << "Completato!" << std::endl;
        std::cout << "Entries processate: " << processedEntries << std::endl;
        std::cout << "Entries modificate: " << modifiedEntries << std::endl;
        std::cout << "File salvato: " << outputLUT << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
        return false;
    }
}

// Funzione per confrontare due LUT
void compareLUTs(const char* lut1, const char* lut2, int pdg = 211) {
    std::cout << "=== CONFRONTO LUT ===" << std::endl;
    std::cout << "LUT 1: " << lut1 << std::endl;
    std::cout << "LUT 2: " << lut2 << std::endl;
    
    try {
        o2::delphes::DelphesO2TrackSmearer smearer1, smearer2;
        smearer1.loadTable(pdg, lut1);
        smearer2.loadTable(pdg, lut2);
        
        auto header = smearer1.getLUTHeader(pdg);
        if (!header) {
            std::cout << "Errore nel caricare header!" << std::endl;
            return;
        }
        
        std::cout << "\nConfrontando entries per diversi valori di pt:" << std::endl;
        
        float nch = header->nchmap.eval(header->nchmap.nbins / 2);
        float radius = header->radmap.eval(0);
        float eta = 0.0;
        
        for (int i = 0; i < std::min(100, header->ptmap.nbins); i += header->ptmap.nbins / 10) {
            float pt = header->ptmap.eval(i);
            
            float eff1, eff2;
            auto entry1 = smearer1.getLUTEntry(pdg, nch, radius, eta, pt, eff1);
            auto entry2 = smearer2.getLUTEntry(pdg, nch, radius, eta, pt, eff2);
            
            if (entry1 && entry1->valid && entry2 && entry2->valid) {
                float res1 = sqrt(entry1->covm[14]) * pt * 100.0; // in %
                float res2 = sqrt(entry2->covm[14]) * pt * 100.0; // in %
                
                std::cout << "pt=" << pt << " GeV/c:" << std::endl;
                std::cout << "  Eff: " << entry1->eff << " -> " << entry2->eff;
                if (entry1->eff != entry2->eff) std::cout << " (CAMBIATA)";
                std::cout << std::endl;
                std::cout << "  Res: " << res1 << "% -> " << res2 << "%";
                if (abs(res1 - res2) > 0.001) std::cout << " (CAMBIATA)";
                std::cout << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
    }
}

// Funzione di test completo
void runCompleteTest(const char* inputLUT = "lutCovm.pi.20kG.rmin20.geometry_v2.dat") {
    std::cout << "=== TEST COMPLETO MODIFICATORE LUT ===" << std::endl;
    
    const char* outputLUT = "lutCovm_modified_test.dat";
    
    std::cout << "\nPasso 1: Ispezione LUT originale" << std::endl;
    inspectLUTDetailed(inputLUT, 211);
    
    std::cout << "\nPasso 2: Creazione LUT modificata con valori dummy" << std::endl;
    bool success = createTestLUT(inputLUT, outputLUT, 211);
    
    if (success) {
        std::cout << "\nPasso 3: Ispezione LUT modificata" << std::endl;
        inspectLUTDetailed(outputLUT, 211);
        
        std::cout << "\nPasso 4: Confronto tra LUT originale e modificata" << std::endl;
        compareLUTs(inputLUT, outputLUT, 211);
    } else {
        std::cout << "\nErrore nella creazione della LUT modificata!" << std::endl;
    }
}

// Struttura per organizzare i tuoi file e graph
struct GraphConfig {
    const char* filename;
    const char* graphname;
    const char* description;
};

// Funzione per modificare LUT con i tuoi 4 TGraph con riscalamento relativo
bool modifyLUTWithYourGraphs(const char* inputLUT, const char* outputLUT, int pdg = 211,
                              float nch_ref = 2000, float eta_ref = 0.5, float radius_ref = 0., bool debug = false, float pt_threshold = 0.0) {
    
    std::cout << "=== MODIFICA LUT CON I TUOI TGRAPH (RISCALAMENTO RELATIVO) ===" << std::endl;
    std::cout << "Input LUT: " << inputLUT << std::endl;
    std::cout << "Output LUT: " << outputLUT << std::endl;
    std::cout << "Valori di riferimento per i TGraph:" << std::endl;
    std::cout << "  Nch_ref: " << (nch_ref < 0 ? "AUTO" : std::to_string(nch_ref)) << std::endl;
    std::cout << "  Eta_ref: " << eta_ref << std::endl;
    std::cout << "  Radius_ref: " << (radius_ref < 0 ? "AUTO" : std::to_string(radius_ref)) << std::endl;
    std::cout << "  pT_threshold of TGraph (original LUT rescaling below): " << pt_threshold << " GeV/c" << std::endl;

    // *** MODIFICA QUESTI NOMI FILE SE NECESSARIO ***
    GraphConfig configs[4] = {
        {"4Petals_radial_3IRIShits/avgGraph_eff_500_pi_4petals_radial_min3IRIShits_7hits.root", "grEff1_avg", "Efficienza (%)"},
        {"4Petals_radial_3IRIShits/avgGraph_momRes_500_pi_4petals_radial_min3IRIShits_7hits.root", "grMomRes1_avg", "grMomRes1_avg pT relativa (%)"},
        {"4Petals_radial_3IRIShits/avgGraph_pointResXY_500_pi_4petals_radial_min3IRIShits_7hits.root", "grPointResXY_avg", "Risoluzione rPhi (μm)"},
        {"4Petals_radial_3IRIShits/avgGraph_pointResZ_500_pi_4petals_radial_min3IRIShits_7hits.root", "grPointResZ_avg", "Risoluzione z (μm)"}
    };
    
    try {
        // Carica i 4 TGraph
        TGraph* effGraph = nullptr;
        TGraph* momResGraph = nullptr;
        TGraph* rphiResGraph = nullptr;
        TGraph* zResGraph = nullptr;
        
        TFile* files[4] = {nullptr, nullptr, nullptr, nullptr};
        
        // Carica efficienza
        files[0] = TFile::Open(configs[0].filename);
        if (files[0] && !files[0]->IsZombie()) {
            effGraph = dynamic_cast<TGraph*>(files[0]->Get(configs[0].graphname));
            if (effGraph) {
                std::cout << "✓ " << configs[0].description << " caricato: " 
                         << effGraph->GetN() << " punti" << std::endl;
            } else {
                std::cout << "✗ " << configs[0].description << " non trovato" << std::endl;
            }
        } else {
            std::cout << "✗ Errore nell'aprire " << configs[0].filename << std::endl;
        }

        // Carica risoluzione momento
        files[1] = TFile::Open(configs[1].filename);
        if (files[1] && !files[1]->IsZombie()) {
            momResGraph = dynamic_cast<TGraph*>(files[1]->Get(configs[1].graphname));
            if (momResGraph) {
                std::cout << "✓ " << configs[1].description << " caricato: " 
                         << momResGraph->GetN() << " punti" << std::endl;
            } else {
                std::cout << "✗ " << configs[1].description << " non trovato" << std::endl;
            }
        } else {
            std::cout << "✗ Errore nell'aprire " << configs[1].filename << std::endl;
        }
        
        // Carica risoluzione rPhi
        files[2] = TFile::Open(configs[2].filename);
        if (files[2] && !files[2]->IsZombie()) {
            rphiResGraph = dynamic_cast<TGraph*>(files[2]->Get(configs[2].graphname));
            if (rphiResGraph) {
                std::cout << "✓ " << configs[2].description << " caricato: " 
                         << rphiResGraph->GetN() << " punti" << std::endl;
            } else {
                std::cout << "✗ " << configs[2].description << " non trovato" << std::endl;
            }
        } else {
            std::cout << "✗ Errore nell'aprire " << configs[2].filename << std::endl;
        }
        
        // Carica risoluzione z
        files[3] = TFile::Open(configs[3].filename);
        if (files[3] && !files[3]->IsZombie()) {
            zResGraph = dynamic_cast<TGraph*>(files[3]->Get(configs[3].graphname));
            if (zResGraph) {
                std::cout << "✓ " << configs[3].description << " caricato: " 
                         << zResGraph->GetN() << " punti" << std::endl;
            } else {
                std::cout << "✗ " << configs[3].description << " non trovato" << std::endl;
            }
        } else {
            std::cout << "✗ Errore nell'aprire " << configs[3].filename << std::endl;
        }

        // Verifica che almeno un grafico sia stato caricato
        if (!effGraph && !momResGraph && !rphiResGraph && !zResGraph) {
            std::cout << "Errore: nessun grafico caricato con successo!" << std::endl;
            for (int i = 0; i < 4; ++i) {
                if (files[i]) files[i]->Close();
            }
            return false;
        }
        
        // Stampa i range dei grafici
        auto printGraphRange = [](TGraph* graph, const char* name) {
            if (graph && graph->GetN() > 0) {
                double xmin, xmax, ymin, ymax;
                graph->ComputeRange(xmin, ymin, xmax, ymax);
                std::cout << "  " << name << " range pt: [" << xmin << ", " << xmax << "] GeV/c" << std::endl;
            }
        };
        
        std::cout << "\nRange dei grafici:" << std::endl;
        printGraphRange(effGraph, "Efficienza");
        printGraphRange(momResGraph, "Risoluzione pT");
        printGraphRange(rphiResGraph, "Risoluzione rPhi");
        printGraphRange(zResGraph, "Risoluzione z");
        
        // Carica le LUT esistenti
        o2::delphes::DelphesO2TrackSmearer smearer;
        smearer.loadTable(pdg, inputLUT);
        
        auto lutHeader = smearer.getLUTHeader(pdg);
        if (!lutHeader) {
            std::cout << "Errore nel caricare l'header delle LUT!" << std::endl;
            for (int i = 0; i < 4; ++i) {
                if (files[i]) files[i]->Close();
            }
            return false;
        }
        
        TH2F *histo_modifiedEff = new TH2F("histo_modifiedEff","Cases (for pT trend) with eff modified",lutHeader->etamap.nbins,0,lutHeader->etamap.nbins-1,lutHeader->nchmap.nbins,0,lutHeader->nchmap.nbins-1);
        TH2F *histo_badEff = new TH2F("histo_badEff","Cases (for pT trend) with eff modified but outside [0,1]",lutHeader->etamap.nbins,0,lutHeader->etamap.nbins-1,lutHeader->nchmap.nbins,0,lutHeader->nchmap.nbins-1);
        TH2F *histo_badEffFraction = new TH2F("histo_badEffFraction","Fraction of cases (for pT trend) with eff outside [0,1]",lutHeader->etamap.nbins,0,lutHeader->etamap.nbins-1,lutHeader->nchmap.nbins,0,lutHeader->nchmap.nbins-1);
        histo_modifiedEff->SetDirectory(0);  // importante!
        histo_badEff->SetDirectory(0);  // importante!
        histo_badEffFraction->SetDirectory(0);  // importante!

        // Determina valori di riferimento automaticamente se non specificati
        if (nch_ref < 0) {
            nch_ref = lutHeader->nchmap.eval(lutHeader->nchmap.nbins / 2);
            std::cout << "Nch_ref automatico: " << nch_ref << std::endl;
        }
        if (radius_ref < 0) {
            radius_ref = lutHeader->radmap.eval(0); // Di solito c'è solo 1 bin per radius
            std::cout << "Radius_ref automatico: " << radius_ref << std::endl;
        }
        
        std::cout << "\nValori di riferimento finali:" << std::endl;
        std::cout << "  Nch_ref: " << nch_ref << std::endl;
        std::cout << "  Eta_ref: " << eta_ref << std::endl;
        std::cout << "  Radius_ref: " << radius_ref << std::endl;
        
        // *** FIX: PRE-CALCOLA I VALORI DI RIFERIMENTO UNA SOLA VOLTA ***
        std::cout << "\nPre-calcolando valori di riferimento per evitare inconsistenze..." << std::endl;
        std::map<float, float> effRefMap;
        std::map<float, float> momResRefMap;
        std::map<float, float> rphiResRefMap;
        std::map<float, float> zResRefMap;

        for (int ipt = 0; ipt < lutHeader->ptmap.nbins; ++ipt) {
            float pt = lutHeader->ptmap.eval(ipt);
            float dummyEff;
            auto refEntry = smearer.getLUTEntry(pdg, nch_ref, radius_ref, eta_ref, pt, dummyEff);
            
            if (refEntry && refEntry->valid) {
                effRefMap[pt] = refEntry->eff;
                momResRefMap[pt] = sqrt(refEntry->covm[14]) * pt * 100.0;
                rphiResRefMap[pt] = sqrt(refEntry->covm[0]) * 1e4;
                zResRefMap[pt] = sqrt(refEntry->covm[1]) * 1e4;
                
                if (ipt < 5) { // Debug primi valori
                    std::cout << "Riferimento pT=" << pt << ": eff=" << effRefMap[pt] 
                             << ", momRes=" << momResRefMap[pt] << "%" << std::endl;
                }
            } else {
                effRefMap[pt] = -1;
                momResRefMap[pt] = -1;
                rphiResRefMap[pt] = -1;
                zResRefMap[pt] = -1;
            }
        }
        std::cout << "Pre-calcolo completato. I valori di riferimento dovrebbero essere ~0.99, non ~0.73!" << std::endl;

        // Funzione helper che usa le mappe pre-calcolate
        auto getRefValue = [&](float pt, const char* type) -> float {
            if (strcmp(type, "efficiency") == 0) {
                return effRefMap.count(pt) ? effRefMap[pt] : -1;
            } else if (strcmp(type, "momres") == 0) {
                return momResRefMap.count(pt) ? momResRefMap[pt] : -1;
            } else if (strcmp(type, "rphires") == 0) {
                return rphiResRefMap.count(pt) ? rphiResRefMap[pt] : -1;
            } else if (strcmp(type, "zres") == 0) {
                return zResRefMap.count(pt) ? zResRefMap[pt] : -1;
            }
            return -1;
        };

        // Apri il file di output
        std::ofstream outFile(outputLUT, std::ios::binary);
        if (!outFile.is_open()) {
            std::cout << "Errore nell'aprire il file di output!" << std::endl;
            for (int i = 0; i < 4; ++i) {
                if (files[i]) files[i]->Close();
            }
            return false;
        }
        
        // Copia l'header
        outFile.write(reinterpret_cast<const char*>(lutHeader), sizeof(*lutHeader));
        
        // Calcola il numero totale di entries
        int totalEntries = lutHeader->nchmap.nbins * lutHeader->radmap.nbins * 
                          lutHeader->etamap.nbins * lutHeader->ptmap.nbins;
        
        std::cout << "\nProcessando " << totalEntries << " entries..." << std::endl;
        
        int processedEntries = 0;
        int modifiedEntries = 0;
        int effModified = 0;
        int momResModified = 0;
        int rphiResModified = 0;
        int zResModified = 0;
        
        // Funzioni per valutare i TGraph
        // Funzione per efficienza (permette valore 0)
        auto safeEvaluateEfficiency = [](TGraph* graph, float pt) -> float {
            if (!graph) return -1;
            
            double value = graph->Eval(pt);
            
            if (std::isnan(value) || std::isinf(value)) {
                return -1;
            }
            
            // Per l'efficienza, 0 è un valore valido (nessuna efficienza)
            // Controllo solo che non sia negativo
            if (value < 0) {
                return -1;
            }
            
            return value;
        };

        // Funzione per risoluzioni (richiede valore > 0)  
        auto safeEvaluateResolution = [](TGraph* graph, float pt) -> float {
            if (!graph) return -1;
            
            double value = graph->Eval(pt);
            
            if (std::isnan(value) || std::isinf(value)) {
                return -1;
            }
            
            // Per le risoluzioni, il valore deve essere > 0
            if (value <= 0) {
                return -1;
            }
            
            return value;
        };
                        
        // Loop su tutte le dimensioni
        for (int inch = 0; inch < lutHeader->nchmap.nbins; ++inch) {
            float nch = lutHeader->nchmap.eval(inch);
            for (int irad = 0; irad < lutHeader->radmap.nbins; ++irad) {
                float radius = lutHeader->radmap.eval(irad);
                for (int ieta = 0; ieta < lutHeader->etamap.nbins; ++ieta) {
                    float eta = lutHeader->etamap.eval(ieta);
                    int counterEntriesMod = 0;
                    int counterEntriesOutOfBounds = 0;
   		            
                    for (int ipt = 0; ipt < lutHeader->ptmap.nbins; ++ipt) {
                        float pt = lutHeader->ptmap.eval(ipt);
                        
                        float dummyEff;
                        auto lutEntry = smearer.getLUTEntry(pdg, nch, radius, eta, pt, dummyEff);
                        
                        if (lutEntry && lutEntry->valid) {
                            bool entryModified = false;

                            // Modifica efficienza con riscalamento relativo e gestione threshold
                            if (effGraph) {
                                if (pt >= pt_threshold) {
                                    // Comportamento normale per pT >= threshold
                                    float newEffPercent = safeEvaluateEfficiency(effGraph, pt);
                                    if (newEffPercent >= 0 && newEffPercent <= 100.0) {
                                        // Valore di riferimento dalla LUT originale
                                        float effRef = getRefValue(pt, "efficiency");
                                        float effCurrent = lutEntry->eff;
                                        if (debug) if (nch > 1900 && eta > 0.4 && eta < 1) printf("Valore LUT per pt, eta, mch = %.3f, %.3f, %.3f: originale = %.4f, al riferimento di nch e eta = %.4f\n",pt,eta,nch,effCurrent,effRef);
                                        if (effRef > 0) {
                                            // Riscalamento relativo
                                            float scaleFactor = effCurrent / effRef;
                                            lutEntry->eff = (newEffPercent / 100.0) * scaleFactor;
                                            if (debug) if (nch > 1900 && eta > 0.4 && eta < 1) printf("Modificando efficienza da %.4f a %.4f (riferimento LUT originale da %.4f a %.4f) - pT, eta, nch = %.3f, %.3f, %.3f\n",newEffPercent/100.,newEffPercent/100. * scaleFactor,effRef,effCurrent,pt,eta,nch);                                    
                                            
                                            // Assicurati che rimanga nel range [0,1]
                                            if (lutEntry->eff > 1.0) { 
                                                printf("Attenzione! Rescaling efficienza per valori > 1 (%.3f)! Sostituendo con 1...\n",lutEntry->eff);
                                                lutEntry->eff = 1.0; 
                                                counterEntriesOutOfBounds++;
                                                if (debug) if (nch > 1900 && eta > 0.4 && eta < 1) getchar();
                                            }
                                            if (lutEntry->eff < 0.0) {
                                                printf("Attenzione! Rescaling efficienza per valori > 1 (%.3f)! Sostituendo con 1...\n",lutEntry->eff);
                                                lutEntry->eff = 0.0;
                                                counterEntriesOutOfBounds++;
                                            }
                                            entryModified = true;
                                            effModified++;
                                            counterEntriesMod++;
                                        } else {
                                            // Se non c'è valore di riferimento, usa direttamente il valore dal TGraph
                                            lutEntry->eff = newEffPercent / 100.0;
                                            entryModified = true;
                                            effModified++;
                                            counterEntriesMod++;
                                        }
                                    }
                                } else {
                                    // Comportamento speciale per pT < threshold: usa valore threshold come riferimento
                                    float effAtThreshold = safeEvaluateEfficiency(effGraph, pt_threshold);
                                    if (effAtThreshold >= 0 && effAtThreshold <= 100.0) {
                                        // Ottieni il valore della LUT originale al threshold per calcolare il rapporto
                                        float lutEffAtThreshold = getRefValue(pt_threshold, "efficiency");
                                        
                                        if (lutEffAtThreshold > 0) {
                                            // Applica la formula: nuovo_valore = valore_originale * (TGraph_threshold / LUT_originale_threshold)
                                            float scaleFactor = (effAtThreshold / 100.0) / lutEffAtThreshold;
                                            lutEntry->eff *= scaleFactor;
                                            
                                            // Assicurati che rimanga nel range [0,1]
                                            if (lutEntry->eff > 1.0) { 
                                                printf("Attenzione! Rescaling efficienza per valori > 1 (%.3f)! Sostituendo con 1...\n",lutEntry->eff);
                                                lutEntry->eff = 1.0; 
                                                counterEntriesOutOfBounds++;
                                                if (debug) if (nch > 200 && eta > -2 && eta < 2) getchar();
                                            }
                                            if (lutEntry->eff < 0.0) {
                                                printf("Attenzione! Rescaling efficienza per valori > 1 (%.3f)! Sostituendo con 1...\n",lutEntry->eff);
                                                lutEntry->eff = 0.0;
                                                counterEntriesOutOfBounds++;
                                            }
                                            
                                            entryModified = true;
                                            effModified++;
                                            counterEntriesMod++;
                                        }
                                    }
                                }
                            }

                            // Modifica risoluzione momento con riscalamento relativo
                            if (momResGraph) {
                                float newMomResPercent = safeEvaluateResolution(momResGraph, pt);
                                if (newMomResPercent > 0) {
                                    // Valore di riferimento dalla LUT originale
                                    float momResRef = getRefValue(pt, "momres");
                                    float momResCurrent = sqrt(lutEntry->covm[14]) * pt * 100.0; // Risoluzione attuale in %
                                    
                                    if (momResRef > 0) {
                                        // Riscalamento relativo
                                        float scaleFactor = momResCurrent / momResRef;
                                        float finalMomRes = newMomResPercent * scaleFactor;
                                        
                                        // Converti in elemento di matrice
                                        float relResFraction = finalMomRes / 100.0;
                                        lutEntry->covm[14] = (relResFraction / pt) * (relResFraction / pt);
                                        
                                        entryModified = true;
                                        momResModified++;
                                    } else {
                                        // Se non c'è valore di riferimento, usa direttamente il valore dal TGraph
                                        float relResFraction = newMomResPercent / 100.0;
                                        lutEntry->covm[14] = (relResFraction / pt) * (relResFraction / pt);
                                        entryModified = true;
                                        momResModified++;
                                    }
                                }
                            }
                            
                            // Modifica risoluzione rPhi (covm[0]) con riscalamento relativo
                            if (rphiResGraph) {
                                float newRPhiResMicron = safeEvaluateResolution(rphiResGraph, pt);
                                if (newRPhiResMicron > 0) {
                                    // Valore di riferimento dalla LUT originale
                                    float rphiResRef = getRefValue(pt, "rphires");
                                    float rphiResCurrent = sqrt(lutEntry->covm[0]) * 1e4; // Risoluzione attuale in μm
                                    
                                    if (rphiResRef > 0) {
                                        // Riscalamento relativo
                                        float scaleFactor = rphiResCurrent / rphiResRef;
                                        float finalRPhiRes = newRPhiResMicron * scaleFactor;
                                        
                                        // Converti in elemento di matrice
                                        float rphiResCm = finalRPhiRes * 1e-4; // μm → cm
                                        lutEntry->covm[0] = rphiResCm * rphiResCm;
                                        
                                        entryModified = true;
                                        rphiResModified++;
                                    } else {
                                        // Se non c'è valore di riferimento, usa direttamente il valore dal TGraph
                                        float rphiResCm = newRPhiResMicron * 1e-4; // μm → cm
                                        lutEntry->covm[0] = rphiResCm * rphiResCm;
                                        entryModified = true;
                                        rphiResModified++;
                                    }
                                }
                            }
                            
                            // Modifica risoluzione z (covm[1]) con riscalamento relativo
                            if (zResGraph) {
                                float newZResMicron = safeEvaluateResolution(zResGraph, pt);
                                if (newZResMicron > 0) {
                                    // Valore di riferimento dalla LUT originale
                                    float zResRef = getRefValue(pt, "zres");
                                    float zResCurrent = sqrt(lutEntry->covm[1]) * 1e4; // Risoluzione attuale in μm
                                    
                                    if (zResRef > 0) {
                                        // Riscalamento relativo
                                        float scaleFactor = zResCurrent / zResRef;
                                        float finalZRes = newZResMicron * scaleFactor;
                                        
                                        // Converti in elemento di matrice
                                        float zResCm = finalZRes * 1e-4; // μm → cm
                                        lutEntry->covm[1] = zResCm * zResCm;
                                        
                                        entryModified = true;
                                        zResModified++;
                                    } else {
                                        // Se non c'è valore di riferimento, usa direttamente il valore dal TGraph
                                        float zResCm = newZResMicron * 1e-4; // μm → cm
                                        lutEntry->covm[1] = zResCm * zResCm; 
                                        entryModified = true;
                                        zResModified++;
                                    }
                                }
                            }
                            
                            if (entryModified) modifiedEntries++;
                        }
                        
                        // Scrivi l'entry
                        if (lutEntry) {
                            outFile.write(reinterpret_cast<const char*>(lutEntry), sizeof(*lutEntry));
                        }
                        
                        processedEntries++;
                        if (processedEntries % 50000 == 0) {
                            std::cout << "Processate " << processedEntries << "/" << totalEntries 
                                     << " (" << (processedEntries*100/totalEntries) << "%)" << std::endl;
                        }
                    }
                    // Dopo il loop sul pT, riempi l'istogramma con la frazione di casi con efficienza fuori range [0,1]
                    double badEffFraction = (counterEntriesMod > 0) ? (double)counterEntriesOutOfBounds/(double)counterEntriesMod : 0;
                    histo_modifiedEff->SetBinContent(ieta+1,inch+1,counterEntriesMod);
                    histo_badEff->SetBinContent(ieta+1,inch+1,counterEntriesOutOfBounds);
                    histo_badEffFraction->SetBinContent(ieta+1,inch+1,badEffFraction);  
                }
            }
        }
        
        outFile.close();
        
        // Chiudi i file
        for (int i = 0; i < 4; ++i) {
            if (files[i]) files[i]->Close();
        }

        TFile *file_histos = new TFile(Form("Histos_for_%s.root",outputLUT),"recreate");
        file_histos->cd(); 
        histo_modifiedEff->Write();
        histo_badEff->Write();
        histo_badEffFraction->Write();
        file_histos->Close();
        delete file_histos;

        std::cout << "\n=== RISULTATI FINALI ===" << std::endl;
        std::cout << "Entries processate: " << processedEntries << std::endl;
        std::cout << "Entries totali modificate: " << modifiedEntries << std::endl;
        std::cout << "Efficienze modificate: " << effModified << std::endl;
        std::cout << "Risoluzioni pT modificate: " << momResModified << std::endl;
        std::cout << "Risoluzioni rPhi modificate: " << rphiResModified << std::endl;
        std::cout << "Risoluzioni z modificate: " << zResModified << std::endl;
        std::cout << "File salvato: " << outputLUT << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
        return false;
    }
}

// Funzione per modificare LUT usando il rapporto tra due LUT di riferimento
bool modifyLUTWithLUTRatio(const char* inputLUT, const char* modLUT, const char* defaultLUT, 
                           const char* outputLUT, int input_pdg = 321, int ref_pdg = 211) {
    
    std::cout << "=== MODIFICA LUT CON RAPPORTO TRA DUE LUT DI RIFERIMENTO ===" << std::endl;
    std::cout << "Input LUT (da modificare): " << inputLUT << std::endl;
    std::cout << "Mod LUT (numeratore): " << modLUT << std::endl;
    std::cout << "Default LUT (denominatore): " << defaultLUT << std::endl;
    std::cout << "Output LUT: " << outputLUT << std::endl;
    std::cout << "PDG input: " << input_pdg << std::endl;
    std::cout << "PDG riferimento: " << ref_pdg << std::endl;
    
    try {
        // Carica le tre LUT
        o2::delphes::DelphesO2TrackSmearer inputSmearer, modSmearer, defaultSmearer;
        
        std::cout << "\nCaricamento LUT..." << std::endl;
        
        // Carica input LUT
        inputSmearer.loadTable(input_pdg, inputLUT);
        auto inputHeader = inputSmearer.getLUTHeader(input_pdg);
        if (!inputHeader) {
            std::cout << "Errore nel caricare input LUT!" << std::endl;
            return false;
        }
        std::cout << "✓ Input LUT caricata" << std::endl;
        
        // Carica mod LUT 
        modSmearer.loadTable(ref_pdg, modLUT);
        auto modHeader = modSmearer.getLUTHeader(ref_pdg);
        if (!modHeader) {
            std::cout << "Errore nel caricare mod LUT!" << std::endl;
            return false;
        }
        std::cout << "✓ Mod LUT caricata" << std::endl;
        
        // Carica default LUT
        defaultSmearer.loadTable(ref_pdg, defaultLUT);
        auto defaultHeader = defaultSmearer.getLUTHeader(ref_pdg);
        if (!defaultHeader) {
            std::cout << "Errore nel caricare default LUT!" << std::endl;
            return false;
        }
        std::cout << "✓ Default LUT caricata" << std::endl;
        
        // Verifica compatibilità delle dimensioni (usa input come riferimento)
        std::cout << "\nVerifica compatibilità dimensioni:" << std::endl;
        std::cout << "Input LUT - Nch:" << inputHeader->nchmap.nbins << ", Radius:" << inputHeader->radmap.nbins 
                 << ", Eta:" << inputHeader->etamap.nbins << ", Pt:" << inputHeader->ptmap.nbins << std::endl;
        std::cout << "Mod LUT - Nch:" << modHeader->nchmap.nbins << ", Radius:" << modHeader->radmap.nbins 
                 << ", Eta:" << modHeader->etamap.nbins << ", Pt:" << modHeader->ptmap.nbins << std::endl;
        std::cout << "Default LUT - Nch:" << defaultHeader->nchmap.nbins << ", Radius:" << defaultHeader->radmap.nbins 
                 << ", Eta:" << defaultHeader->etamap.nbins << ", Pt:" << defaultHeader->ptmap.nbins << std::endl;
        
        // Apri il file di output
        std::ofstream outFile(outputLUT, std::ios::binary);
        if (!outFile.is_open()) {
            std::cout << "Errore nell'aprire il file di output!" << std::endl;
            return false;
        }
        
        // Copia l'header della input LUT
        outFile.write(reinterpret_cast<const char*>(inputHeader), sizeof(*inputHeader));
        
        // Calcola il numero totale di entries
        int totalEntries = inputHeader->nchmap.nbins * inputHeader->radmap.nbins * 
                          inputHeader->etamap.nbins * inputHeader->ptmap.nbins;
        
        std::cout << "\nProcessando " << totalEntries << " entries..." << std::endl;
        
        int processedEntries = 0;
        int modifiedEntries = 0;
        int effModified = 0;
        int momResModified = 0;
        int rphiResModified = 0;
        int zResModified = 0;
        int skipCount = 0;
        
        // Funzione helper per calcolare il rapporto con controlli di sicurezza
        auto calculateRatio = [](float numerator, float denominator, const char* quantity) -> float {
            if (denominator <= 0) {
                return -1; // Segnala errore
            }
            if (numerator < 0) {
                return -1; // Segnala errore
            }
            return numerator / denominator;
        };
        
        // Loop su tutte le dimensioni della input LUT
        for (int inch = 0; inch < inputHeader->nchmap.nbins; ++inch) {
            float nch = inputHeader->nchmap.eval(inch);
            for (int irad = 0; irad < inputHeader->radmap.nbins; ++irad) {
                float radius = inputHeader->radmap.eval(irad);
                for (int ieta = 0; ieta < inputHeader->etamap.nbins; ++ieta) {
                    float eta = inputHeader->etamap.eval(ieta);
                    for (int ipt = 0; ipt < inputHeader->ptmap.nbins; ++ipt) {
                        float pt = inputHeader->ptmap.eval(ipt);
                        
                        // Ottieni le entries dalle tre LUT
                        float inputEff, modEff, defaultEff;
                        auto inputEntry = inputSmearer.getLUTEntry(input_pdg, nch, radius, eta, pt, inputEff);
                        auto modEntry = modSmearer.getLUTEntry(ref_pdg, nch, radius, eta, pt, modEff);
                        auto defaultEntry = defaultSmearer.getLUTEntry(ref_pdg, nch, radius, eta, pt, defaultEff);
                        
                        // Processa solo se tutte le entries sono valide
                        if (inputEntry && inputEntry->valid && 
                            modEntry && modEntry->valid && 
                            defaultEntry && defaultEntry->valid) {
                            
                            bool entryModified = false;
                            
                            // Modifica efficienza
                            float effRatio = calculateRatio(modEntry->eff, defaultEntry->eff, "efficienza");
                            if (effRatio > 0) {
                                float newEff = inputEntry->eff * effRatio;
                                // Assicurati che rimanga nel range [0,1]
                                if (newEff > 1.0) newEff = 1.0;
                                if (newEff < 0.0) newEff = 0.0;
                                inputEntry->eff = newEff;
                                entryModified = true;
                                effModified++;
                            }
                            
                            // Modifica risoluzione momento (covm[14])
                            if (inputEntry->covm[14] > 0 && modEntry->covm[14] > 0 && defaultEntry->covm[14] > 0) {
                                float momResRatio = calculateRatio(modEntry->covm[14], defaultEntry->covm[14], "risoluzione momento");
                                if (momResRatio > 0) {
                                    inputEntry->covm[14] *= momResRatio;
                                    entryModified = true;
                                    momResModified++;
                                }
                            }
                            
                            // Modifica risoluzione rPhi (covm[0])
                            if (inputEntry->covm[0] > 0 && modEntry->covm[0] > 0 && defaultEntry->covm[0] > 0) {
                                float rphiResRatio = calculateRatio(modEntry->covm[0], defaultEntry->covm[0], "risoluzione rPhi");
                                if (rphiResRatio > 0) {
                                    inputEntry->covm[0] *= rphiResRatio;
                                    entryModified = true;
                                    rphiResModified++;
                                }
                            }
                            
                            // Modifica risoluzione z (covm[1])
                            if (inputEntry->covm[1] > 0 && modEntry->covm[1] > 0 && defaultEntry->covm[1] > 0) {
                                float zResRatio = calculateRatio(modEntry->covm[1], defaultEntry->covm[1], "risoluzione z");
                                if (zResRatio > 0) {
                                    inputEntry->covm[1] *= zResRatio;
                                    entryModified = true;
                                    zResModified++;
                                }
                            }
                            
                            if (entryModified) modifiedEntries++;
                        } else {
                            // Almeno una entry non è valida
                            skipCount++;
                        }
                        
                        // Scrivi l'entry (modificata o originale)
                        if (inputEntry) {
                            outFile.write(reinterpret_cast<const char*>(inputEntry), sizeof(*inputEntry));
                        }
                        
                        processedEntries++;
                        if (processedEntries % 50000 == 0) {
                            std::cout << "Processate " << processedEntries << "/" << totalEntries 
                                     << " (" << (processedEntries*100/totalEntries) << "%)" << std::endl;
                        }
                    }
                }
            }
        }
        
        outFile.close();
        
        std::cout << "\n=== RISULTATI FINALI ===" << std::endl;
        std::cout << "Entries processate: " << processedEntries << std::endl;
        std::cout << "Entries totali modificate: " << modifiedEntries << std::endl;
        std::cout << "Entries saltate (non valide): " << skipCount << std::endl;
        std::cout << "Efficienze modificate: " << effModified << std::endl;
        std::cout << "Risoluzioni pT modificate: " << momResModified << std::endl;
        std::cout << "Risoluzioni rPhi modificate: " << rphiResModified << std::endl;
        std::cout << "Risoluzioni z modificate: " << zResModified << std::endl;
        std::cout << "File salvato: " << outputLUT << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Errore: " << e.what() << std::endl;
        return false;
    }
}

// Funzione helper per cambiare facilmente i nomi dei file
bool modifyLUTWithCustomGraphs(const char* inputLUT, const char* outputLUT, int pdg,
                              const char* effFile, const char* effGraph,
                              const char* momResFile, const char* momResGraph, 
                              const char* rphiResFile, const char* rphiResGraph,
                              const char* zResFile, const char* zResGraph,
                              float nch_ref = 1, float eta_ref = 0., float radius_ref = 0.) {
    
    std::cout << "=== MODIFICA LUT CON TGRAPH PERSONALIZZATI ===" << std::endl;
    std::cout << "Input LUT: " << inputLUT << std::endl;
    std::cout << "Output LUT: " << outputLUT << std::endl;
    
    // Versione flessibile dove puoi specificare tutti i nomi
    // Implementazione simile ma con parametri personalizzabili
    
    std::cout << "Files da caricare:" << std::endl;
    std::cout << "  Efficienza: " << effFile << " → " << effGraph << std::endl;
    std::cout << "  Risoluzione pT: " << momResFile << " → " << momResGraph << std::endl;
    std::cout << "  Risoluzione rPhi: " << rphiResFile << " → " << rphiResGraph << std::endl;
    std::cout << "  Risoluzione z: " << zResFile << " → " << zResGraph << std::endl;
    
    // [Implementazione completa simile alla funzione sopra ma con nomi personalizzabili]
    // Per ora ritorno true, implemento completamente se necessario
    
    std::cout << "Usa modifyLUTWithYourGraphs() per ora, o dimmi se vuoi che implementi questa versione completa" << std::endl;
    return false;
}