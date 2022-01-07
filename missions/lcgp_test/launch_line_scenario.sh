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
SHORE_LISTEN="9300"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   VNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   VPORT="9000"

#  Define variables for the agents
#-------------------------------------------------------

VNAME1="agent1"
START_POS1="0,0"
TRAJ1="[(0,0),(200,-200)]"

VNAME2="agent2"
START_POS2="5,-5"
TRAJ2="[(5,-5),(205,-205)]"

VNAME3="agent3"
START_POS3="10,-10"
TRAJ3="[(10,-10),(210,-210)]"

VNAME4="agent4"
START_POS4="15,-15"
TRAJ4="[(15,-15),(215,-215)]"

VNAME5="agent5"
START_POS5="20,-20"
TRAJ5="[(20,-20),(220,-220)]"

VNAME6="agent6"
START_POS6="25,-25"
TRAJ6="[(25,-25),(225,-225)]"

#  Start up the agents
#-------------------------------------------------------
nsplug meta_vehicle.moos targ_agent1.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1      START_POS=$START_POS1                 \
   VPORT="9001"       SHARE_LISTEN="9301"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ1

nsplug meta_potential_planner.bhv targ_agent1.bhv -i -f VNAME=$VNAME1  \
    OVNAME=$VNAME1 START_POS=$START_POS1


nsplug meta_vehicle.moos targ_agent2.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME2      START_POS=$START_POS2                 \
   VPORT="9002"       SHARE_LISTEN="9302"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ2

nsplug meta_potential_planner.bhv targ_agent2.bhv -i -f VNAME=$VNAME2  \
    OVNAME=$VNAME2 START_POS=$START_POS2


nsplug meta_vehicle.moos targ_agent3.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME3      START_POS=$START_POS3                 \
   VPORT="9003"       SHARE_LISTEN="9303"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ3

nsplug meta_potential_planner.bhv targ_agent3.bhv -i -f VNAME=$VNAME3  \
    OVNAME=$VNAME3 START_POS=$START_POS3


nsplug meta_vehicle.moos targ_agent4.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME4      START_POS=$START_POS4                 \
   VPORT="9004"       SHARE_LISTEN="9304"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ4

nsplug meta_potential_planner.bhv targ_agent4.bhv -i -f VNAME=$VNAME4  \
    OVNAME=$VNAME4 START_POS=$START_POS4


nsplug meta_vehicle.moos targ_agent5.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME5      START_POS=$START_POS5                 \
   VPORT="9005"       SHARE_LISTEN="9305"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ5

nsplug meta_potential_planner.bhv targ_agent5.bhv -i -f VNAME=$VNAME5  \
    OVNAME=$VNAME5 START_POS=$START_POS5


nsplug meta_vehicle.moos targ_agent6.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME6      START_POS=$START_POS6                 \
   VPORT="9006"       SHARE_LISTEN="9306"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   TRAJ=$TRAJ6

nsplug meta_potential_planner.bhv targ_agent6.bhv -i -f VNAME=$VNAME6  \
    OVNAME=$VNAME6 START_POS=$START_POS6


if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent1.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent2.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent3.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME4 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent4.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME5 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent5.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME6 MOOS Community with WARP:" $TIME_WARP
pAntler targ_agent6.moos >& /dev/null &
sleep 0.25

echo "Launching shoreside MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos
kill -- -$$



