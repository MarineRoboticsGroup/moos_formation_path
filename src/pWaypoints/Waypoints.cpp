/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Waypoints.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Waypoints.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Waypoints::Waypoints()
{
  nav_x = 0.0;
  nav_y = 0.0;
  waypoint_x = 0.0;
  waypoint_y = 0.0;
}

//---------------------------------------------------------
// Destructor

Waypoints::~Waypoints()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Waypoints::OnNewMail(MOOSMSG_LIST &NewMail)
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

    // if (key == "NAV_X")
    // {
    //   nav_x = msg.GetDouble();
    // }
    // else if (key == "NAV_Y")
    // {
    //   nav_y = msg.GetDouble();
    // }
    if (key == "INTERNAL_EST_SELF_REPORT")
    {
      string sval = msg.GetString();

      nav_x = stod(tokStringParse(sval, "X", ',', '='));
      nav_x = stod(tokStringParse(sval, "Y", ',', '='));
      // double time = stod(tokStringParse(sval, "TIME", ',', '='));
    }
    else if (key == "CURRENT_WAYPOINT")
    {
      string val = msg.GetString();
      waypoint_x = stod(val.substr(1, val.find(",")));
      waypoint_y = stod(val.substr(val.find(" "), val.find(")")));
    }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Waypoints::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Waypoints::Iterate()
{
  AppCastingMOOSApp::Iterate();
  //Distance between current pos and waypoint
  // Notify("TEST", "All values: (" + to_string(waypoint_x) + ", " + to_string(nav_x) + ", " + to_string(waypoint_y) + ", " + to_string(nav_y) + ")");
  // Notify("TEST", "Differences: (" + to_string(waypoint_x - nav_x) + ", " + to_string(waypoint_y - nav_y) + ")");
  Notify("WAYPOINT_FORCE", "(" + to_string(waypoint_x - nav_x) + ", " + to_string(waypoint_y - nav_y) + ")");
  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Waypoints::OnStartUp()
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

    bool handled = false;
    if (param == "foo")
    {
      handled = true;
    }
    else if (param == "bar")
    {
      handled = true;
    }

    if (!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Waypoints::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("NAV_X", 0);
  // Register("NAV_Y", 0);
  Register("INTERNAL_EST_SELF_REPORT", 0);
  Register("CURRENT_WAYPOINT", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Waypoints::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one"
        << "two"
        << "three"
        << "four";
  m_msgs << actab.getFormattedString();

  return (true);
}
