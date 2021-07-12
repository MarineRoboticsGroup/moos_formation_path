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

VNAME2="follower_1"         # The second vehicle Community
START_POS2="50,-50"   # Vehicle 2 Behavior configurations
ANGLE2="200"          # Follower 2's angle relative to leader
RADIUS2="35"            #Follower 2's distance relative to leader

VNAME3="follower_2"       # The third vehicle Community
START_POS3="110,-50"   # Vehicle 3 Behavior configurations
ANGLE3="160"          # Follower 3's angle relative to leader
RADIUS3="35"            #Follower 3's distance relative to leader

SHORE_LISTEN="9300"

nsplug meta_vehicle.moos targ_leader.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME1      START_POS=$START_POS1                 \
   VPORT="9001"       SHARE_LISTEN="9301"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN

nsplug meta_vehicle.moos targ_follower_1.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME2      START_POS=$START_POS2                 \
   VPORT="9002"       SHARE_LISTEN="9302"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   KNOWS_CONTACTS=1

nsplug meta_vehicle.moos targ_follower_2.moos -f WARP=$TIME_WARP \
   VNAME=$VNAME3      START_POS=$START_POS3                 \
   VPORT="9003"       SHARE_LISTEN="9303"                   \
   VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN            \
   KNOWS_CONTACTS=1

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                  \
   SPORT="9000"

nsplug meta_leader.bhv targ_leader.bhv -i -f VNAME=$VNAME1  \
    OVNAME=$VNAME1 START_POS=$START_POS1

nsplug meta_follower.bhv targ_follower_1.bhv -i -f VNAME=$VNAME2  \
    OVNAME=$VNAME2 START_POS=$START_POS2                        \
    ANGLE=$ANGLE2        RADIUS=$RADIUS2

nsplug meta_follower.bhv targ_follower_2.bhv -i -f VNAME=$VNAME3  \
    OVNAME=$VNAME3 START_POS=$START_POS3                    \
    ANGLE=$ANGLE3        RADIUS=$RADIUS3

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
echo "Launching $VNAME1 MOOS Community with WARP:" $TIME_WARP
pAntler targ_leader.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME2 MOOS Community with WARP:" $TIME_WARP
pAntler targ_follower_1.moos >& /dev/null &
sleep 0.25

echo "Launching $VNAME3 MOOS Community with WARP:" $TIME_WARP
pAntler targ_follower_2.moos >& /dev/null &
sleep 0.25

echo "Launching shoreside MOOS Community with WARP:"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done "

uMAC targ_shoreside.moos
kill -- -$$



