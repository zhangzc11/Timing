#!/bin/bash

## input
label=${1}
bin=${2}

## global vars
base="2017/MC/gjets_HT"
tune="TuneCUETP8M1_13TeV-madgraphMLM-pythia8"
eosbase="/store/user/${USER}"
rootbase="root://eoscms"

## filenames
infiles="dispho_*.root"
outfile="tree.root"

## directories needed
indir="${eosbase}/GJets_HT-${bin}_${tune}/${label}_GJets_HT-${bin}_${tune}"
tmpdir="/tmp/${USER}/${base}"
outdir="${eosbase}/nTuples/${base}/${bin}"

## make tmp dir
mkdir -p ${tmpdir}/${bin}/

## Copy to tmp from EOS
timestamp=$(eos ls ${indir})
xrdcp -r ${rootbase}/${indir}/${timestamp}/0000/ ${tmpdir}/${bin}/

## Hadd on tmp 
hadd -O -k ${tmpdir}/${bin}/${outfile} ${tmpdir}/${bin}/${infiles}
rm -rf ${tmpdir}/${bin}/${infiles}
rm -rf ${tmpdir}/${bin}/failed/

## Copy back to EOS
eos mkdir -p ${outdir}
xrdcp -r ${tmpdir}/${bin}/${outfile} ${rootbase}/${outdir}/
rm -rf ${tmpdir}/${bin}/${outfile}
