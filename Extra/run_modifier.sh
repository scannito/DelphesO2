#!/bin/bash

# Check that exactly 4 arguments were passed
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <geometry_type> <radii_type> <pdg_in_out> <pdg_petals_ideal>"
    echo "Geometry options: baseline | lightweight | bent"
    echo "Radii options:    default | new"
    echo "Example:          $0 lightweight default 211 321"
    exit 1
fi

GEO_TYPE=$1
RADII_TYPE=$2
PDG_MAIN=$3
PDG_OTHER=$4

# Function to map PDG code to the particle name used in files
get_part_name() {
    local pdg=$1
    if [ "$pdg" -eq 11 ]; then
        echo "el"
    elif [ "$pdg" -eq 13 ]; then
        echo "mu"
    elif [ "$pdg" -eq 211 ]; then
        echo "pi"
    elif [ "$pdg" -eq 321 ]; then
        echo "ka"
    elif [ "$pdg" -eq 2212 ]; then
        echo "pr"
    else
        echo "undef"
    fi
}

# Get string representations for both PDGs
PART_MAIN=$(get_part_name "$PDG_MAIN")
PART_OTHER=$(get_part_name "$PDG_OTHER")

if [ "$PART_MAIN" == "undef" ] || [ "$PART_OTHER" == "undef" ]; then
    echo "Warning: One of the PDGs ($PDG_MAIN or $PDG_OTHER) is not recognized in the standard mapping."
fi

# Build the file paths using the appropriate particle names
INPUT_LUT="../LUTs/lutCovm.${PART_MAIN}.20kG.rmin20.0.geometry_ML_${GEO_TYPE}_${RADII_TYPE}_radii.dat"
OUTPUT_LUT="../FinalLUTs/lutCovm_${PART_MAIN}_ML_${GEO_TYPE}_${RADII_TYPE}_radii.dat"

PETALS_LUT="../OtherLUTs/lutCovm_${PART_OTHER}_3Petals_Inclined_min2IRIShits_7hits.dat"
IDEAL_LUT="../OtherLUTs/lutCovm_${PART_OTHER}_ideal_min2IRIShits_7hits.dat"

echo "================================================="
echo "Running for:"
echo " Geometry:      $GEO_TYPE"
echo " Radii:         $RADII_TYPE"
echo " Input/Output:  $PART_MAIN (PDG $PDG_MAIN)"
echo " Petals/Ideal:  $PART_OTHER (PDG $PDG_OTHER)"
echo "================================================="

# Launch ROOT in batch mode (-b) to execute commands in sequence and then quit (-q)

# Launch ROOT using -e flags to ensure commands are explicitly executed
root -l -b -q \
    -e 'gSystem->Load("libO2PhysicsALICE3Core");' \
    -e '.L improved_lut_modifier.cpp+' \
    -e "modifyLUTWithLUTRatio(\"$INPUT_LUT\", \"$PETALS_LUT\", \"$IDEAL_LUT\", \"$OUTPUT_LUT\", $PDG_MAIN, $PDG_OTHER);"

echo "Done!"