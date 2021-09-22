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
START_POS1="22,-188"
TRAJ1="[(22,-188),(28,-182),(33,-176),(36,-170),(39,-161),(43,-154),(45,-151),(48,-141),(49,-131),(50,-130),(57,-123),(55,-119),(56,-108),(58,-100),(63,-93),(73,-97),(84,-98),(95,-102),(100,-101),(111,-104),(114,-107),(125,-108),(135,-104),(142,-97),(144,-90),(150,-81),(151,-73),(157,-66),(160,-60),(160,-51)]"

VNAME2="agent2"
START_POS2="11,-182"
TRAJ2="[(11,-182),(14,-179),(20,-175),(23,-166),(29,-157),(32,-152),(33,-144),(34,-135),(40,-126),(43,-118),(47,-113),(52,-103),(58,-100),(63,-93),(73,-97),(84,-98),(95,-102),(100,-101),(111,-104),(114,-107),(125,-108),(135,-104),(142,-97),(144,-90),(146,-84),(145,-73),(143,-64),(145,-55),(148,-45)]"

VNAME3="agent3"
START_POS3="17,-182"
TRAJ3="[(17,-182),(20,-175),(23,-166),(29,-157),(32,-152),(33,-144),(34,-135),(40,-126),(43,-118),(47,-113),(52,-103),(58,-100),(63,-93),(73,-97),(84,-98),(95,-102),(100,-101),(111,-104),(114,-107),(125,-108),(135,-104),(142,-97),(144,-90),(150,-81),(151,-73),(157,-66),(156,-57),(155,-52),(154,-45)]"

VNAME4="agent4"
START_POS4="34,-165"
TRAJ4="[(34,-165),(39,-161),(43,-154),(45,-151),(48,-141),(49,-131),(50,-130),(57,-123),(55,-119),(56,-108),(58,-100),(63,-93),(73,-97),(84,-98),(94,-94),(100,-101),(111,-104),(114,-107),(125,-108),(135,-104),(142,-97),(143,-88),(153,-83),(160,-76),(157,-68),(160,-60),(163,-50),(167,-44),(169,-40),(173,-31),(171,-28)]"

VNAME5="agent5"
START_POS5="28,-182"
TRAJ5="[(28,-182),(33,-176),(36,-170),(39,-161),(43,-154),(45,-151),(48,-141),(52,-139),(58,-132),(57,-123),(55,-119),(56,-108),(54,-101),(51,-93),(59,-90),(66,-84),(72,-80),(81,-83),(88,-83),(97,-87),(105,-89),(112,-96),(115,-98),(123,-100),(125,-108),(135,-104),(142,-97),(144,-90),(150,-81),(151,-73),(157,-66),(160,-60),(163,-50),(165,-45)]"

VNAME6="agent6"
START_POS6="11,-165"
TRAJ6="[(11,-165),(13,-162),(16,-153),(18,-147),(22,-142),(26,-137),(29,-128),(30,-117),(31,-115),(38,-108),(42,-100),(44,-98),(51,-93),(59,-90),(66,-84),(74,-88),(77,-91),(84,-98),(95,-102),(100,-101),(111,-104),(114,-107),(125,-108),(135,-104),(142,-97),(144,-90),(146,-84),(145,-73),(143,-64),(141,-57),(142,-52),(138,-42),(142,-33),(148,-28)]"


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



