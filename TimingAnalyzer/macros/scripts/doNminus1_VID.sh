#!/bin/sh

for njets in 1 
do
    mkdir -p output/recoskim/njets${njets}
    for jetpt in 30 50
    do
	. /Applications/root_v6.06.08/bin/thisroot.sh
	mkdir -p output/recoskim/novid-sph-smin/njets${njets}/jetpt${jetpt}
	root -l -b -q "effPlot_VID.C("${jetpt},${njets}")"
	. /Applications/root_v5.34.36/bin/thisroot.sh
	for phpt in 30 40 50
	do
	    for novid in full noPhIso noNeuIso noChgIso noSieie noHoE
	    do
		root -l -b -q "runSkim_VID.C("${phpt},\"${novid}\",${jetpt},${njets},1")"
	    done
	done
    done
done

# njets == 0, no loop over jet pt
. /Applications/root_v6.06.08/bin/thisroot.sh
mkdir -p output/recoskim/novid-sph-smin/njets0/jetpt0
root -l -b -q "effPlot_VID.C("0,0")"
. /Applications/root_v5.34.36/bin/thisroot.sh
for phpt in 30 40 50 
do
    for novid in full noPhIso noNeuIso noChgIso noSieie noHoE
    do
	root -l -b -q "runSkim_VID.C("${phpt},\"${novid}\",0,0,1")"
    done
done

. /Applications/root_v6.06.08/bin/thisroot.sh

for njets in 1
do
    for jetpt in 30 50
    do
	root -l -b -q "effStackPlot_VID.C("${jetpt},${njets}")"
    done
done

# njets == 0, no range of pt

root -l -b -q "effStackPlot_VID.C("0,0")"
