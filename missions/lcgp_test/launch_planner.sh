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
# 6 agents on rect
# VNAME1="agent1"
# START_POS1="17,-182"
# TRAJ1="[(17,-182),(33,-176),(37,-169),(51,-165),(61,-151),(72,-138),(86,-122),(90,-115),(102,-106),(109,-93),(116,-87),(125,-70),(141,-57),(154,-40),(154,-40),(154,-40)]"

# VNAME2="agent2"
# START_POS2="22,-188"
# TRAJ2="[(22,-188),(35,-183),(34,-165),(47,-150),(58,-132),(62,-125),(79,-109),(95,-102),(109,-93),(116,-87),(125,-70),(143,-64),(160,-45),(160,-45),(160,-45),(160,-45)]"

# VNAME3="agent3"
# START_POS3="11,-182"
# TRAJ3="[(11,-182),(17,-182),(33,-176),(37,-169),(51,-165),(61,-151),(72,-138),(86,-122),(90,-115),(102,-106),(109,-93),(116,-87),(125,-70),(141,-57),(148,-40),(148,-40)]"

# VNAME4="agent4"
# START_POS4="34,-165"
# TRAJ4="[(34,-165),(44,-156),(61,-151),(72,-138),(86,-122),(90,-115),(102,-106),(123,-100),(144,-90),(146,-84),(143,-64),(160,-45),(164,-33),(171,-22),(171,-22),(171,-22)]"

# VNAME5="agent5"
# START_POS5="28,-182"
# TRAJ5="[(28,-182),(37,-169),(51,-165),(61,-151),(72,-138),(86,-122),(90,-115),(102,-106),(123,-100),(144,-90),(146,-84),(157,-66),(162,-59),(165,-40),(165,-40),(165,-40)]"

# VNAME6="agent6"
# START_POS6="11,-165"
# TRAJ6="[(11,-165),(16,-153),(40,-143),(58,-132),(69,-112),(81,-96),(88,-83),(104,-80),(113,-61),(124,-44),(131,-31),(148,-22),(148,-22),(148,-22),(148,-22),(148,-22)]"

VNAME1="agent1"
START_POS1="17,-182"
TRAJ1="[(17,-182),(14,-179),(11,-165),(16,-153),(26,-137),(41,-124),(46,-104),(53,-86),(76,-76),(88,-83),(109,-93),(123,-100),(132,-110),(137,-103),(146,-84),(143,-64),(154,-45),(154,-45),(154,-45),(154,-45)]"

VNAME2="agent2"
START_POS2="22,-188"
TRAJ2="[(22,-188),(33,-176),(37,-169),(47,-150),(44,-130),(46,-104),(53,-86),(76,-76),(88,-83),(109,-93),(123,-100),(132,-110),(137,-103),(146,-84),(157,-66),(160,-51),(160,-51),(160,-51),(160,-51),(160,-51)]"

VNAME3="agent3"
START_POS3="11,-182"
TRAJ3="[(11,-182),(11,-165),(16,-153),(19,-140),(30,-117),(32,-97),(51,-93),(60,-79),(83,-70),(104,-80),(116,-87),(113,-113),(134,-116),(155,-118),(151,-98),(146,-84),(143,-64),(148,-45),(148,-45),(148,-45)]"

VNAME4="agent4"
START_POS4="34,-165"
TRAJ4="[(34,-165),(47,-150),(44,-130),(55,-119),(51,-93),(53,-86),(76,-76),(88,-83),(109,-93),(123,-100),(132,-110),(137,-103),(146,-84),(143,-64),(154,-45),(164,-33),(171,-28),(171,-28),(171,-28),(171,-28)]"

VNAME5="agent5"
START_POS5="28,-182"
TRAJ5="[(28,-182),(37,-169),(47,-150),(44,-130),(46,-104),(51,-93),(60,-79),(83,-70),(104,-80),(116,-87),(113,-113),(134,-116),(155,-118),(162,-105),(160,-85),(157,-66),(162,-59),(165,-45),(165,-45),(165,-45)]"

VNAME6="agent6"
START_POS6="11,-165"
TRAJ6="[(11,-165),(16,-153),(19,-140),(30,-117),(32,-97),(39,-91),(48,-78),(67,-73),(71,-60),(92,-63),(104,-80),(116,-87),(123,-100),(132,-110),(144,-90),(139,-77),(141,-57),(145,-38),(148,-28),(148,-28)]"


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



