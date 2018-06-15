#!/bin/bash

cutflowconfig=${1:-"cut_config/cuts_v2/one_at_a_time/control_qcd.txt"}
pdname=${2:-"SinglePhoton"}
outfiletext=${3:-"skim"}
doskim=${4:-1}
sampleconfig=${5:-""}

## produce slimmed skim
root -l -b -q runFastSkimmer.C\(\"${cutflowconfig}\",\"${pdname}\",\"${outfiletext}\",${doskim},\"${sampleconfig}\"\)
