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
  // TODO: Change to list
  m_other_follower_range = 0.0;

  m_self_x = 0.0;
  m_self_y = 0.0;
  m_leader_x = 0.0;
  m_leader_y = 0.0;

  self_leader_angle = 0.0;
  prev_leader_range = 0.0;
  prev_self_leader_angle = 0.0;
  prev_self_x = 0.0;
  prev_self_y = 0.0;

  m_time = 0.0;
  prev_time = 0.0;

  m_gnd_node_report = "";
  est_node_report = "";
  est_msg = "";
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
        prev_leader_range = m_leader_range;
        m_leader_range = range;
      }
      else
      {
        m_other_follower_range = range;
      }
    }

    // Loc.s for headings
    else if (key == "NODE_REPORT")
    {
      // Parse msg
      string sval = msg.GetString();

      // Sort by sender
      if (sval.find("leader") != std::string::npos)
      {
        double x = stod(tokStringParse(sval, "X", ',', '='));
        double y = stod(tokStringParse(sval, "Y", ',', '='));
        double time = stod(tokStringParse(sval, "TIME", ',', '='));
        m_leader_x = x;
        m_leader_y = y;
        prev_time = m_time;
        m_time = time;
        m_gnd_node_report = sval;
      }
    }

    else if (key == "NODE_REPORT_LOCAL")
    {
      string sval = msg.GetString();
      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));

      prev_self_x = m_self_x;
      prev_self_y = m_self_y;

      m_self_x = x;
      m_self_y = y;
    }

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
  if (m_gnd_node_report.length() > 0 && m_leader_range > 0.0)
  {
    // Calc. est. leader pos.
    prev_self_leader_angle = self_leader_angle;
    self_leader_angle = atan2((m_leader_y - m_self_y), (m_leader_x - m_self_x));

    double leader_x_est = m_self_x + m_leader_range * cos(self_leader_angle);
    double leader_y_est = m_self_y + m_leader_range * sin(self_leader_angle);

    // Calc. est. leader vel.
    double spd = 0.0;
    double hdg = 0.0;
    if (prev_self_x == 0)
    {
      double d_time = m_time - prev_time;

      vector<double> prev_self_leader_vec = {prev_leader_range * cos(prev_self_leader_angle), prev_leader_range * sin(prev_self_leader_angle)};
      vector<double> self_leader_vec = {m_leader_range * cos(self_leader_angle), m_leader_range * sin(self_leader_angle)};
      vector<double> prev_self_self_vec = {m_self_x - prev_self_x, m_self_y - prev_self_y};

      vector<double> prev_leader_leader_vec = {self_leader_vec[0] + prev_self_self_vec[0] - prev_self_leader_vec[0],
                                               self_leader_vec[1] + prev_self_self_vec[1] - prev_self_leader_vec[1]};

      vector<double> leader_vel = {prev_leader_leader_vec[0]/d_time, prev_leader_leader_vec[1]/d_time};
      spd = sqrt(leader_vel[0]*leader_vel[0] + leader_vel[1]*leader_vel[1]);
      hdg = atan2(leader_vel[1], leader_vel[0]);
    }

    // Pub for verification
    est_msg = "x=" + to_string(leader_x_est) + ", y=" + to_string(leader_y_est);
    Notify("EST_POSITION", est_msg);

    // Pub for trail behavior
    est_node_report = "NAME=est_leader,";
    est_node_report += "X=" + to_string(leader_x_est) + ",";
    est_node_report += "Y=" + to_string(leader_y_est) + ",";
    est_node_report += "SPD=" + to_string(spd) + ",";
    // est_node_report += "HDG=" + to_string(hdg) + ",";

    int start = m_gnd_node_report.find("HDG");
    string unchanged_node_report = m_gnd_node_report.substr(start);
    est_node_report += unchanged_node_report;

    Notify("NODE_REPORT", est_node_report);
  }
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
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool RangeToPosition::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: RangeToPosition                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable tab_1(2);
  tab_1 << "Current estimate | leader range";
  tab_1.addHeaderLines();
  tab_1 << est_msg
        << to_string(m_leader_range);
  m_msgs << tab_1.getFormattedString() << endl;

  ACTable tab_2(2);
  tab_2 << "Self x | self y";
  tab_2.addHeaderLines();
  tab_2 << m_self_x << m_self_y;
  m_msgs << tab_2.getFormattedString() << endl;

  ACTable tab_3(2);
  tab_3 << "Leader x | leader y";
  tab_3.addHeaderLines();
  tab_3 << m_leader_x << m_leader_y;
  m_msgs << tab_3.getFormattedString() << endl;

  return (true);
}
