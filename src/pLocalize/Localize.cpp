/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Localize.cpp                             */
/************************************************************/

#include <iterator>
#include <math.h>
#include <chrono>
#include "MBUtils.h"
#include "ACTable.h"
#include "Localize.h"

using namespace std;
using namespace std::chrono;

//---------------------------------------------------------
// Constructor

Localize::Localize()
{
}

//---------------------------------------------------------
// Destructor

Localize::~Localize()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Localize::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for (p = NewMail.begin(); p != NewMail.end(); p++)
  {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();

    if (key == "INTERNAL_RANGE_REPORT")
    {
      string sval = msg.GetString();
      string name1 = tokStringParse(sval, "NAME1", ',', '=');
      string name2 = tokStringParse(sval, "NAME2", ',', '=');
      double range = stod(tokStringParse(sval, "RANGE", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));
      vector<double> vals = {range, time};

      if (name1 == self_name)
      {
        neighbor_ranges[name2] = vals;
      }
      else if (name2 == self_name)
      {
        neighbor_ranges[name1] = vals;
      }
    }

    else if (key == "INTERNAL_POS_REPORT")
    {
      string sval = msg.GetString();

      string name = tokStringParse(sval, "NAME", ',', '=');

      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));

      double time = stod(tokStringParse(sval, "TIME", ',', '='));
      vector<double> vals = {x, y, time};
      all_est_poses[name] = vals;
    }

    else if (key == "INTERNAL_GND_SELF_REPORT")
    {
      string sval = msg.GetString();

      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));

      vector<double> vals = {x, y, time};
      self_gnd_pos = vals;
    }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Localize::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Localize::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Zelazo update rule
  // c is last agent
  // compare anchor name to self_name for running extra steps

  // Only update non-anchors, euqation adapted from Zelazo 2015
  if (self_id <= num_agents - 3 && neighbor_ranges.size() == num_agents - 1 && all_est_poses.size() == num_agents - 1)
  {
    vector<double> delta_pos = {0.0, 0.0};
    for (const auto &pair : all_est_poses)
    {
      string other_agent_name = pair.first;
      vector<double> other_agent_pos = pair.second;

      double delta_x = other_agent_pos[0] - self_est_pos[0];
      double delta_y = other_agent_pos[1] - self_est_pos[1];

      double est_dist = sqrt(delta_x * delta_x + delta_y * delta_y);
      double range = neighbor_ranges[other_agent_name][0];

      delta_pos[0] += (est_dist - range) * delta_x / est_dist;
      delta_pos[1] += (est_dist - range) * delta_y / est_dist;
    }
    self_est_pos[0] += delta_pos[0];
    self_est_pos[1] += delta_pos[1];
  }
  // Use accurate pos for anchors
  else if (self_id > num_agents - 3 && self_gnd_pos.size() == 3)
  {
    self_est_pos = self_gnd_pos;
  }

  string self_est_pos_report = "NAME=" + self_name +
                               ",X=" + to_string(self_est_pos[0]) +
                               ",Y=" + to_string(self_est_pos[1]) +
                               ",TIME=" + to_string(self_est_pos[2]);
  Notify("INTERNAL_EST_SELF_REPORT", self_est_pos_report);

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Localize::OnStartUp()
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

    if (param == "name")
    {
      self_name = value;
      // Isolate digits
      int start_index = 0;
      for (; start_index < self_name.length(); start_index++)
      {
        if (isdigit(self_name[start_index]))
          break;
      }
      string name_id = self_name.substr(start_index, self_name.length() - start_index);
      self_id = stoi(name_id);
    }
    else if (param == "start_pos")
    {
      //"11,-123" - split & add timestamp
      double comma_index = value.find(',');
      double x = stod(value.substr(0, comma_index));
      double y = stod(value.substr(comma_index + 1, value.size()));

      milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
      string str_ms = to_string(ms.count());
      double usable_time = stod(str_ms);
      usable_time /= 100;

      self_est_pos = {x, y, usable_time};
    }
    else if (param == "num_agents")
    {
      num_agents = stoi(value);
    }
    else
      reportUnhandledConfigWarning(orig);
  }
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Localize::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("INTERNAL_RANGE_REPORT", 0);
  Register("INTERNAL_POS_REPORT", 0);
  Register("INTERNAL_GND_SELF_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Localize::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:   Localize                            " << endl;
  m_msgs << "============================================" << endl;
  ACTable actab(4);
  actab << "Ranges |ests | x | y";
  actab.addHeaderLines();
  actab << to_string(neighbor_ranges.size())
        << to_string(all_est_poses.size())
        << to_string(self_est_pos[0])
        << to_string(self_est_pos[1]);
  m_msgs << actab.getFormattedString();

  return (true);
}