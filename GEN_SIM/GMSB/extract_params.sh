#!/bin/bash

for lamb in 50 100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500
do
    for ctau in 0.001 0.01 0.1 1 5 10 25 50 100 200 300 500 750 1000 1500 2000 2500 3000
    do
	c_grav=$(grep "c_grav" SLHA/GMSB_Lambda${lamb}TeV_CTau${ctau}cm.slha | cut -d ' ' -f11) # re-interpolate
	width=$(grep "DECAY   1000022" SLHA/GMSB_Lambda${lamb}TeV_CTau${ctau}cm.slha | cut -d ' ' -f6)
	echo ${c_grav} ${lamb} ${width} >> width_cgrav.txt
    done
done
