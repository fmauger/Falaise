#!/bin/bash
#
# Author: F. Mauger
# Date:   2020-07-08
#
# Test of the DBD gA feature
# ==========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``simu.conf`` : main configuration file for flsimulate
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-feature-dbd_gA] [--visu]
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-feature-dbd_gA-validation"
with_visu=0
work_dir=""
cfg_dir=""

function parse_cl_opts()
{
    while [ -n "$1" ]; do
	local opt="$1"
	if [ "${opt}" == "--work-dir" ]; then
	    shift 1
	    work_dir="$1"
	elif [ "${opt}" == "--cfg-dir" ]; then
	    shift 1
	    cfg_dir="$1"
	elif [ "${opt}" == "--visu" ]; then
	    with_visu=1
	else
	    echo >&2 "[error] Invalid command line option '${opt}'! Abort!"
	    return 1
	fi
	shift 1
    done
    return 0
}

parse_cl_opts $@
if [ $? -ne 0 ]; then
    exit 1
fi

if [ -z "${work_dir}" ]; then
    work_dir="_work.d"
fi
if [ -z "${cfg_dir}" ]; then
    cfg_dir="."
fi

#########################################
export FLWORKDIR="${work_dir}/${label}"

echo >&2 "[info] cfg_dir   = '${cfg_dir}'"
echo >&2 "[info] work_dir  = '${work_dir}'"
echo >&2 "[info] FLWORKDIR = '${FLWORKDIR}'"

function my_exit()
{
    local error_code="$1"
    shift 1
    local error_msg="$@"
    if [ -n "${error_msg}" ]; then
	echo >&2 "[error] $@"
    fi
    if [ -d ${FLWORKDIR} ]; then
	echo >&2 "[info] Removing work dir..."
	rm -fr ${FLWORKDIR}
    fi
    exit ${error_code}
}

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

if [ ! -d ${FLWORKDIR} ]; then
    mkdir -p ${FLWORKDIR}
fi

step1=1
step2=1
step3=1

dbdmodes="g0 g2 g22 g4"
dbdmodes="g0" # g2 g22 g4"
# Example:
# $ flsimulate-configure --no-gui -t "urn:snemo:demonstrator:simulation:2.2" -s "vertexes:generator=source_pads_bulk" -s "primary_events:generator=Se82.2nubb_gA_g0" -s "simulation:output_profile=none" -o "feature-dbd_gA_g0-validation.vprofile"
# $ flsimulate -c simu.conf -o feature-dbd_gA_g0-validation.xml > flsim-dbd_gA_g0.log 2>&1

###############################################################################
for dbdmode in ${dbdmodes}; do
    echo >&2 "[info] Running flsimulate-configure  Se82 2nubb dbd_gA_${dbdmode} shot from the source bulk..."
    # continue
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "vertexes:generator=source_pads_bulk" \
			 -s "primary_events:generator=Se82.2nubb_gA_${dbdmode}" \
			 -s "simulation:output_profile=none" \
			 -o "${FLWORKDIR}/feature-dbd_gA_${dbdmode}-validation.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-dbd_gA_${dbdmode}-validation.vprofile' :"
    cat ${FLWORKDIR}/feature-dbd_gA_${dbdmode}-validation.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_${dbdmode}.conf -o ${FLWORKDIR}/feature-dbd_gA_${dbdmode}-validation.xml > ${FLWORKDIR}/flsim${dbdmode}.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    # Extract the energy deposited in the calo hits:
    grep --after-context=42 "<first>calo</first>" ${FLWORKDIR}/feature-dbd_gA_${dbdmode}-validation.xml | \
	tr -s "[[:space:]]" | grep -B 1 particle_name  | grep energy_deposit | sed -e 's@^\t@@g' -e "s@<energy_deposit>@@g" -e "s@</energy_deposit>@@g" > ${FLWORKDIR}/calo_de_${dbdmode}.data
    gsl-histogram 0.0 4.0 100 < ${FLWORKDIR}/calo_de_${dbdmode}.data > ${FLWORKDIR}/calo_de_${dbdmode}.his
    cat > ${FLWORKDIR}/calo_de_${dbdmode}.gp <<EOF
set grid
set xlabel "E_{calo} (MeV)"
set ylabel "Count/40 keV"
set title "DBD gA-${dbdmode} - Energy deposit in calo blocks"
plot [0:][0:] '${FLWORKDIR}/calo_de_${dbdmode}.his' using 1:3 title "" with histeps
set terminal push
set terminal pdfcairo
set output "${FLWORKDIR}/fig_calo_de_${dbdmode}.pdf"
replot
set output
set terminal pop
EOF
    gnuplot ${FLWORKDIR}/calo_de_${dbdmode}.gp
done

###############################################################################
tree ${FLWORKDIR}/
if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/feature-dbd_gA_g0-validation.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

my_exit 0

# end
