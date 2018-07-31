#!/bin/bash

## combine info
export combdir="/afs/cern.ch/work/k/kmcdermo/private/dispho/CMSSW_8_1_0/src/HiggsAnalysis/CombinedLimit/working"
export datacard="datacard.txt"

## config info
export inTextExt="txt"

## config dir info
export limitconfigdir="limit_config"

## common output info
export topdir="/afs/cern.ch/user/k/kmcdermo/www"
export disphodir="dispho"

declare -a exts=("eps" "png" "pdf")
export exts

## function to make directory readable
function PrepOutDir ()
{
    fulldir=${1:-"${topdir}/${disphodir}/plots"}
    mkdir -p ${fulldir}

    pushd ${topdir}
    ./makereadable.sh ${fulldir}
    popd
}
export -f PrepOutDir