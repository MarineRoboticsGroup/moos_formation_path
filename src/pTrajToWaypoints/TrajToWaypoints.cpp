/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: TrajToWaypoints.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "TrajToWaypoints.h"
#include "XYSegList.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TrajToWaypoints::TrajToWaypoints()
{
  waypoints = {};
  visit_radius = 1.0;

  dist_to_waypoint = 0.0;
  current_index = 0;
  update_pt = "";

  synced_waypoint = 0;
  go_to_next = false;

  no_posts = true;
}

//---------------------------------------------------------
// Destructor

TrajToWaypoints::~TrajToWaypoints()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TrajToWaypoints::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for (p = NewMail.begin(); p != NewMail.end(); p++)
  {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    if (key == "NAV_X")
    {
      nav_x = msg.GetDouble();
    }
    else if (key == "NAV_Y")
    {
      nav_y = msg.GetDouble();
    }
    else if (key == "INTERNAL_WAYPOINT_COUNTER")
    {
      synced_waypoint = stoi(msg.GetString());
    }
    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TrajToWaypoints::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TrajToWaypoints::Iterate()
{
  AppCastingMOOSApp::Iterate();
  XYSegList my_seglist;

  // First waypoint
  if (no_posts)
  {
    my_seglist.add_vertex(waypoints[current_index][0], waypoints[current_index][1]);
    update_pt = "points = " + my_seglist.get_spec();
    Notify("TRAJ_WAYPOINTS", update_pt);
    Notify("CURRENT_WAYPOINT", "(" + to_string(waypoints[current_index][0]) + ", " + to_string(waypoints[current_index][0]) + ")");
    Notify("CONST_SPD_UPDATES", "speed = 1.0");

    no_posts = false;

    // Visualize endgoal
    vector<double> goal_coords = waypoints.back();
    Notify("VIEW_MARKER", "type=circle,x=" + to_string(goal_coords[0]) + ",y=" + to_string(goal_coords[1]) + ",label=goal,color=green,width=" + to_string(visit_radius));
  }
  else
  {
    dist_to_waypoint = pow(pow(nav_x - waypoints[current_index][0], 2) + pow(nav_y - waypoints[current_index][1], 2), .5);

    // Check if at waypoint
    if (dist_to_waypoint < visit_radius || go_to_next)
    {
      go_to_next = true;

      Notify("TEST", to_string(synced_waypoint) + ", " + to_string(current_index));
      // Wait for everyone
      if (synced_waypoint != current_index) {
        Notify("CONST_SPD_UPDATES", "speed = 0.0");
        Notify("INTERNAL_WAYPOINT", to_string(current_index));
      }
      // Go to next waypoint
      else if (current_index < waypoints.size() - 1)
      {
        current_index++;
        my_seglist.add_vertex(waypoints[current_index][0], waypoints[current_index][1]);
        update_pt = "points = " + my_seglist.get_spec();
        Notify("TRAJ_WAYPOINTS", update_pt);
        Notify("CONST_SPD_UPDATES", "speed = 1.0");
        Notify("CURRENT_WAYPOINT", "(" + to_string(waypoints[current_index][0]) + ", " + to_string(waypoints[current_index][1]) + ")");
        go_to_next = false;
      }
      // At goal
      else
      {
        Notify("TRAJ_WAYPOINTS", "endflag = TRAJ = false");
        Notify("TRAJ_WAYPOINTS", "endflag = STATION_KEEP = true");
      }
    }
  }

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TrajToWaypoints::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++)
  {
    string orig = *p;
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    if (param == "traj")
    {
      string traj = value;
      vector<char> extra_chars = {'(', ')', '[', ']', ' '};
      for (int i = 0; i < extra_chars.size(); i++)
      {
        traj.erase(remove(traj.begin(), traj.end(), extra_chars[i]), traj.end());
      }

      vector<string> str_waypoints = parseString(traj, ',');
      double traj_x = 0.0;
      double traj_y = 0.0;
      for (int i = 0; i < str_waypoints.size(); i += 2)
      {
        traj_x = stod(str_waypoints[i]);
        traj_y = stod(str_waypoints[i + 1]);
        vector<double> coords = {traj_x, traj_y};
        waypoints.push_back(coords);
      }
    }
    else if (param == "visit_radius")
    {
      visit_radius = stod(value);
    }
    else
    {
      reportUnhandledConfigWarning(orig);
    }
  }
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void TrajToWaypoints::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("INTERNAL_WAYPOINT_COUNTER", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool TrajToWaypoints::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Index | Dist | Msg | four";
  actab.addHeaderLines();
  actab << to_string(current_index)
        << to_string(dist_to_waypoint)
        << update_pt
        << "four";
  m_msgs << actab.getFormattedString();

  return (true);
}
