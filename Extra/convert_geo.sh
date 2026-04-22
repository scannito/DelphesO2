#!/bin/bash

# Check that exactly 1 argument is passed
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <configuration_name>"
    echo "Example: $0 ML_baseline_default_radii"
    exit 1
fi

CONFIG="$1"

# =========================================================
# HARDCODED PATHS - Modify these if your folder structure is different
# =========================================================
# I assume the input file is in the src directory and follows this naming convention:
INPUT_FILE="../src/lutWrite.geometry_${CONFIG}.cc"

# Output will be generated in the current folder
OUTPUT_FILE="../IniFiles/a3geo_${CONFIG}.ini"
# =========================================================

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Input file not found at:"
    echo "$INPUT_FILE"
    exit 1
fi

echo "================================================="
echo " Processing configuration: $CONFIG"
echo " Input:  $INPUT_FILE"
echo " Output: $OUTPUT_FILE"
echo "================================================="

# 1. Dynamically find the name of your function (e.g., fatInit_geometry_...)
FUNC_NAME=$(grep -m 1 -oE 'void[ \t]+[a-zA-Z0-9_]+' "$INPUT_FILE" | awk '{print $2}')

if [ -z "$FUNC_NAME" ]; then
    echo "Error: Could not find function declaration 'void function_name' in the input file."
    exit 1
fi

# 2. Create the first part of the macro
cat << EOF > tmp_runner.C
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "TMath.h"

// Mock class to intercept AddLayer calls and write to a text file
class TextGeometryExporter {
private:
    std::ofstream out;
public:
    TextGeometryExporter(const std::string& filename) {
        out.open(filename);
        out << "# Example of the ALICE3 geometry file in TEnv format\n";
        out << "# Example of layers configuration for FastTracker\n";
        out << "# Each layer is defined by a set of parameters\n";
        out << "# Format:\n";
        out << "# <layer_name>.<parameter>: <value>\n";
        out << "# Example:\n";
        out << "# layer1.r: 0.5 # radius in cm\n";
        out << "# layer1.z: 250 # half-length in cm\n";
        out << "# layer1.x0: 0.001 # radiation length\n";
        out << "# layer1.xrho: 0 # density in g/cm^2\n";
        out << "# layer1.resRPhi: 0.0001 # resolution in R-Phi in cm\n";
        out << "# layer1.resZ: 0.0001 # resolution in Z in cm\n";
        out << "# layer1.eff: 1.0 # efficiency (0 to 1)\n";
        out << "# layer1.type: 1 #  type of layer (0: passive, 1: active)\n";
        out << "# layer1.deadPhiRegions: /path/to/dead_regions.root # optional dead regions file\n\n";
    }

    ~TextGeometryExporter() {
        out << "# Lookup tables\n";
        // Here we inject the CONFIG variable dynamically from bash!
        out << "global.lutEl: ccdb:/Users/s/scannito/LUTs_${CONFIG}/el\n";
        out << "global.lutMu: ccdb:/Users/s/scannito/LUTs_${CONFIG}/mu\n";
        out << "global.lutPi: ccdb:/Users/s/scannito/LUTs_${CONFIG}/pi\n";
        out << "global.lutKa: ccdb:/Users/s/scannito/LUTs_${CONFIG}/ka\n";
        out << "global.lutPr: ccdb:/Users/s/scannito/LUTs_${CONFIG}/pr\n\n";
        out << "# in kGauss\n";
        out << "global.magneticfield: 20\n";
        out.close();
    }

    void AddLayer(const char* name, double r, double x0, double xrho, 
                  double resRPhi = 0.0, double resZ = 0.0, double eff = 0.0) {
        std::string sname(name);
        if (sname == "vertex") return; 

        // Renaming logic: dddX -> B0X
        if (sname.length() >= 4 && sname.substr(0, 3) == "ddd") {
            int num = std::stoi(sname.substr(3)); 
            char newName[10];
            snprintf(newName, sizeof(newName), "B%02d", num); 
            sname = newName;
        }

        // Active (1) if efficiency > 0, otherwise passive (0)
        int type = (eff > 0.0) ? 1 : 0;

        // Force iTOF and oTOF to be passive layers (type 0)
        if (sname == "iTOF" || sname == "oTOF") {
            type = 0;
        }
        
        out << sname << ".r: " << r << "\n";
        out << sname << ".z: 250\n";
        out << sname << ".x0: " << x0 << "\n";
        out << sname << ".xrho: " << xrho << "\n";
        out << sname << ".resRPhi: " << resRPhi << "\n";
        out << sname << ".resZ: " << resZ << "\n";
        out << sname << ".eff: " << eff << "\n";
        out << sname << ".type: " << type << "\n\n";
    }

    // Dummy methods to ignore unwanted configuration calls
    void AddPetalGapsToLayer(const char*, int, double, double) {}
    void SetAtLeastHits(int) {}
    void SetAtLeastCorr(int) {}
    void SetAtLeastFake(int) {}
    void SetAvgRapidity(double) {}
    void SetdNdEtaCent(int) {}
    void SetBField(double) {}
    void SetMinRadTrack(double) {}
};

// Global object used by the included geometry function
TextGeometryExporter fat("$OUTPUT_FILE");

// Dummy global function to ignore lutWrite calls
template <typename... Args>
void lutWrite(Args... args) {}

// ================= ORIGINAL GEOMETRY CODE START =================
EOF

# 3. Append your code physically into the same file (stripping includes)
grep -v "^[[:space:]]*#include" "$INPUT_FILE" >> tmp_runner.C

# 4. Append the ROOT entry point
cat << EOF >> tmp_runner.C

// ================= ROOT ENTRY POINT =================
void tmp_runner() {
    $FUNC_NAME();
}
EOF

# Compile and execute
root -l -b -q tmp_runner.C+

# Cleanup
rm tmp_runner.C tmp_runner_C* 2>/dev/null
echo "Conversion finished successfully!"