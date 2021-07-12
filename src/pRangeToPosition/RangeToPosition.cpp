/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: RangeToPosition.cpp                             */
/************************************************************/

#include <iterator>
#include <math.h>
#include "MBUtils.h"
#include "ACTable.h"
#include "RangeToPosition.h"


using namespace std;

//---------------------------------------------------------
// Constructor

RangeToPosition::RangeToPosition()
{
  m_leader_range = 0.0;
  m_other_follower_range = 0.0;
  m_self_heading = 0.0;

  //temp hardcoding
  m_leader_heading = 0.0;

  est_msg = "init";
}

//---------------------------------------------------------
// Destructor

RangeToPosition::~RangeToPosition()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RangeToPosition::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for (p = NewMail.begin(); p != NewMail.end(); p++)
  {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();

    // Ranges
    if (key == "CRS_RANGE_REPORT")
    {
      // Parse msg
      string sval = msg.GetString();
      double range = stod(tokStringParse(sval, "range", ',', '='));

      // Sort by sender
      if (sval.find("leader") != std::string::npos)
      {
        m_leader_range = range;
      }
      else
      {
        m_other_follower_range = range;
      }
    }

    // Headings
    else if (key == "NAV_HEADING")
    {
      m_self_heading = msg.GetDouble();
    }

    // else if (key == "HEADING_REPORT")
    // {
    //   // Parse msg
    //   string sval = msg.GetString();
    //   double heading = 0.0;

    //   // Sort by sender
    //   if (sval.find("leader") != std::string::npos)
    //   {
    //     m_leader_heading = heading;
    //   }
    //   else
    //   {
    //     m_other_follower_heading = heading;
    //   }
    // }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RangeToPosition::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool RangeToPosition::Iterate()
{
  AppCastingMOOSApp::Iterate();
  //TODO: doublecheck this math
  double angle = m_self_heading - m_leader_heading;
  double x_est = m_leader_range * cos(angle);
  double y_est = m_leader_range * sin(angle);
  est_msg = "x=" + to_string(x_est) + ", y=" + to_string(y_est);
  Notify("EST_POSITION", est_msg);
  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool RangeToPosition::OnStartUp()
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

    if (!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void RangeToPosition::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("CRS_RANGE_REPORT", 0);
  Register("NAV_HEADING", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool RangeToPosition::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: RangeToPosition                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(2);
  actab << "Current estimate | leader range";
  actab.addHeaderLines();
  actab << est_msg
        << to_string(m_leader_range);
  m_msgs << actab.getFormattedString();

  return (true);
}
