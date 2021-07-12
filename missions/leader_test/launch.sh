#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: Apr 25th 2020
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp] "
	echo "  --just_make, -j                " 
	echo "  --help, -h                     " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
        echo "launch.sh Bad arg:" $ARGI " Exiting with code: 1"
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="leader"         # The first vehicle Community
START_POS1="80,-125"    # Vehicle 1 Behavior configurations

SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_leader.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1      START_POS=$START_POS1                 \
   VPORT="9001"       SHARE_LISTEN="9301"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   SPORT="9000"

nsplug meta_leader.bhv targ_leader.bhv -i -f VNAME=$VNAME1  \
    OVNAME=$VNAME1 START_POS=$START_POS1

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_leader.moos >& /dev/null &
sleep 0.25

echo "Launching shoreside MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos
kill -- -$$



