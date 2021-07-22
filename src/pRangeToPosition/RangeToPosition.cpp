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
  // Ranges
  m_leader_range = 0.0;
  // TODO: Change to list
  m_other_follower_range = 0.0;

  // Used to replicate self-leader angle est
  m_leader_x = 0.0;
  m_leader_y = 0.0;
  self_leader_angle = 0.0;

  // Self pos
  self_updated = false;
  m_self_x = 0.0;
  m_self_y = 0.0;

  prev_self_x = 0.0;
  prev_self_y = 0.0;

  // Est. leader pos
  leader_x_est = 0.0;
  leader_y_est = 0.0;

  prev_leader_x_est = 0.0;
  prev_leader_y_est = 0.0;

  // Time inits
  m_leader_range_time = 0.0;
  m_bearing_time = 0.0;
  m_self_time = 0.0;
  prev_time = 0.0;

  // Report inits
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
        double time = stod(tokStringParse(sval, "time", ',', '='));
        m_leader_range = range;
        prev_time = m_leader_range_time;
        m_leader_range_time = time;
        self_updated = true;
      }
      else
      {
        m_other_follower_range = range;
      }
    }

    // Loc.s for headings
    else if (key == "NODE_REPORT" || key == "TEST_POSTER")
    {
      // Parse msg
      string sval = msg.GetString();

      // Sort by sender
      if (sval.find("leader") != std::string::npos && sval.find("est") == std::string::npos)
      {
        double x = stod(tokStringParse(sval, "X", ',', '='));
        double y = stod(tokStringParse(sval, "Y", ',', '='));
        double time = stod(tokStringParse(sval, "TIME", ',', '='));

        m_leader_x = x;
        m_leader_y = y;
        m_bearing_time = time;
        m_gnd_node_report = sval;
      }
    }

    else if (key == "NODE_REPORT_LOCAL")
    {
      string sval = msg.GetString();
      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));

      prev_self_x = m_self_x;
      prev_self_y = m_self_y;

      m_self_x = x;
      m_self_y = y;

      m_self_time = time;
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
    self_leader_angle = atan2((m_leader_y - m_self_y), (m_leader_x - m_self_x));

    prev_leader_x_est = leader_x_est;
    prev_leader_y_est = leader_y_est;
    leader_x_est = m_self_x + m_leader_range * cos(self_leader_angle);
    leader_y_est = m_self_y + m_leader_range * sin(self_leader_angle);

    // Do nothing if the leader's position hasn't been updated
    if (prev_leader_x_est == leader_x_est && prev_leader_y_est == leader_y_est)
    {
      return (true);
    }
    else if (!self_updated)
    {
      return (true);
    }
    self_updated = false;

    double leader_lat_est = 0.0;
    double leader_lon_est = 0.0;
    m_geodesy.LocalGrid2LatLong(leader_x_est, leader_y_est, leader_lat_est, leader_lon_est);

    // Calc. est. leader vel.
    // double spd = 0.0;
    // double hdg = 0.0;

    // double d_time = m_bearing_time - prev_time;

    // vector<double> prev_self_leader_vec = {prev_leader_x_est - prev_self_x, prev_leader_y_est - prev_self_y};
    // vector<double> self_leader_vec = {leader_x_est - m_self_x, leader_y_est - m_self_y};
    // vector<double> prev_self_self_vec = {m_self_x - prev_self_x, m_self_y - prev_self_y};

    // vector<double> prev_leader_leader_vec = {self_leader_vec[0] + prev_self_self_vec[0] - prev_self_leader_vec[0],
    //                                          self_leader_vec[1] + prev_self_self_vec[1] - prev_self_leader_vec[1]};
    // vector<double> leader_vel = {prev_leader_leader_vec[0] / d_time, prev_leader_leader_vec[1] / d_time};

    // vector<double> leader_vel = {(leader_x_est - prev_leader_x_est) / d_time, (leader_y_est - prev_leader_y_est) / d_time};

    // spd = sqrt(leader_vel[0] * leader_vel[0] + leader_vel[1] * leader_vel[1]);

    // if (isinf(spd))
    // {
    //   spd = 1.0;
    // }
    //Convert to yaw too
    // hdg = atan2(leader_vel[1], leader_vel[0]);
    // hdg = 180*hdg/M_PI;

    // Pub for verification
    // double gnd_spd = stod(tokStringParse(m_gnd_node_report, "SPD", ',', '='));
    // double gnd_hdg = stod(tokStringParse(m_gnd_node_report, "HDG", ',', '='));

    // string f1 = to_string(prev_self_x) + ", " + to_string(prev_self_y);
    // string f2 = to_string(m_self_x) + ", " + to_string(m_self_y);
    // string l1 = to_string(prev_leader_x_est) + ", " + to_string(prev_leader_y_est);
    // string l2 = to_string(leader_x_est) + ", " + to_string(leader_y_est);

    // string f1_l1 = to_string(prev_self_leader_vec[0]) + ", " + to_string(prev_self_leader_vec[1]);
    // string f2_l2 = to_string(self_leader_vec[0]) + ", " + to_string(self_leader_vec[1]);
    // string f1_f2 = to_string(prev_self_self_vec[0]) + ", " + to_string(prev_self_self_vec[1]);
    // string l1_l2 = to_string(prev_leader_leader_vec[0]) + ", " + to_string(prev_leader_leader_vec[1]);

    // string l2_vel = to_string(leader_vel[0]) + ", " + to_string(leader_vel[1]);

    // string big_report = "f1: (" + f1 + "), f2: (" + f2 + "), l1: (" + l1 + "), l2: (" + l2 + "), ";
    // big_report += "f1_l1: (" + f1_l1 + "), f2_l2: (" + f2_l2 + "), ";
    // big_report += "f1_f2: (" + f1_f2 + "), l1_l2: (" + l1_l2 + "), l2_vel: (" + l2_vel + ")";

    // Notify("TEST_POSTER", big_report);

    // string time_report = "Range time: " + to_string(m_leader_range_time) +
    //                      ", Bearing time: " + to_string(m_bearing_time) +
    //                      ", Self time: " + to_string(m_self_time) +
    //                      ", Prev time: " + to_string(prev_time);
    // Notify("TEST_TIMES", time_report);

    // Pub for trail behavior
    // NAME, X, Y, SPD
    est_node_report = "NAME=est_leader,";
    est_node_report += "X=" + to_string(leader_x_est) + ",";
    est_node_report += "Y=" + to_string(leader_y_est) + ",";
    est_node_report += "SPD=" + to_string(leader_speed) + ",";
    // est_node_report += "HDG=" + to_string(hdg) + ",";

    // HDG, DEP
    int start = m_gnd_node_report.find("HDG");
    int end = m_gnd_node_report.find("LAT");
    string unchanged_node_report = m_gnd_node_report.substr(start, end - start);
    est_node_report += unchanged_node_report;

    // LAT, LON
    est_node_report += "LAT=" + to_string(leader_lat_est) + ",";
    est_node_report += "LON=" + to_string(leader_lon_est) + ",";

    // TYPE, MODE, ALLSTOP, INDEX, YAW, TIME, LENGTH
    start = m_gnd_node_report.find("TYPE");
    unchanged_node_report = m_gnd_node_report.substr(start);
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

  // look for latitude, longitude global variables
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if (!ok1 || !ok2)
    reportConfigWarning("Lat or Lon Origin not set in *.moos file.");

  // initialize m_geodesy
  if (!m_geodesy.Initialise(lat_origin, lon_origin))
    reportConfigWarning("Geodesy init failed.");

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
    if (param == "leader_speed") {
      leader_speed = stod(value);
    }
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
