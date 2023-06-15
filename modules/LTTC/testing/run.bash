#!/usr/bin/env bash

workDir="_work.d"
if [ ! -d ${workDir} ]; then
    mkdir -p ${workDir}
fi

# echo >&2 "[info] simulate..."
# flsimulate \
#   -c simu.conf \
#   -o ${workDir}/testLTTC-sd.brio \
#   -m ${workDir}/testLTTC-simulate-meta.xml \
#   -N 6
  
echo >&2 "[info] reconstruction..."
flreconstruct \
  -p reco_lttc.conf \
  -i ${workDir}/testLTTC-sd.brio \
  -o ${workDir}/testLTTC-rec-rc.brio > ${workDir}/lttc.log 2>&1

#\
#  > ${workDir}/lttc.log 2>&1

# flvisualize \
#   -i ${workDir}/testLTTC-rec-rc.brio

exit 0
# end
