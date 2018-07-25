#!/bin/bash

## source first
source scripts/common_variables.sh

## config
infilename=${1:-"${skimdir}/jetphi_0_control_gjets_DoubleEG.root"}
rescaleconfig=${2:-"${rescaleconfigdir}/qcdMC_to_gjetsMC.${inTextExt}"}
plotconfig=${3:-"${plotconfigdir}/jetphi_0.${inTextExt}"}
miscconfig=${4:-"${miscconfigdir}/misc.${inTextExt}"}
outfiletext=${5:-"rescaled_jetphi_0"}
dir=${6:-"plots/ntuples_v4/test"}

## first make plot
root -l -b -q runRescalePlotter.C\(\"${infilename}\",\"${rescaleconfig}\",\"${plotconfig}\",\"${miscconfig}\",\"${outfiletext}\"\)

## make out dirs
fulldir=${topdir}/${disphodir}/${dir}
PrepOutDir ${fulldir}

## copy everything
for canvscale in "${canvscales[@]}"
do
    for ext in "${exts[@]}"
    do
	cp ${outfiletext}_${canvscale}.${ext} ${fulldir}
    done
done
cp ${outfiletext}.root ${outfiletext}_integrals.${outTextExt} ${fulldir}

## Final message
echo "Finished RescalePlotting for:" ${infilename}
