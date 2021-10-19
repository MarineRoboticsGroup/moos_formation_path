/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AvoidAgents.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include <chrono>
#include <math.h>
#include "MBUtils.h"
#include "ACTable.h"
#include "AvoidAgents.h"

using namespace std;
using namespace std::chrono;

//---------------------------------------------------------
// Constructor

AvoidAgents::AvoidAgents()
{
}

//---------------------------------------------------------
// Destructor

AvoidAgents::~AvoidAgents()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AvoidAgents::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if (key == "INTERNAL_POS_REPORT")
    {
      string sval = msg.GetString();

      string name = tokStringParse(sval, "NAME", ',', '=');

      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));

      double time = stod(tokStringParse(sval, "TIME", ',', '='));
      vector<double> vals = {x, y, time};
      all_est_poses[name] = vals;
    }

    else if (key == "INTERNAL_EST_SELF_REPORT")
    {
      string sval = msg.GetString();

      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));

      vector<double> vals = {x, y, time};
      self_pos = vals;
    }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AvoidAgents::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AvoidAgents::Iterate()
{
  AppCastingMOOSApp::Iterate();
  vector<double> forces = {0.0, 0.0};
  vector<double> individual_forces = {};

  if (all_est_poses.size() == num_agents-1)
  {
    for (const auto &pair : all_est_poses)
    {
      string name = pair.first;
      vector<double> vals = pair.second;
      double dx = vals[0] - self_pos[0];
      double dy = vals[1] - self_pos[1];
      double dist = sqrt(dx * dx + dy * dy);
      forces[0] -= dx / pow(dist, 3);
      forces[1] -= dy / pow(dist, 3);

      // Individual forces
      individual_forces.push_back(-dx / pow(dist, 3));
      individual_forces.push_back(-dy / pow(dist, 3));
    }
    forces[0] /= num_agents;
    forces[1] /= num_agents;

    // TODO: Implement normalization if needed

    Notify("AGENT_FORCE", "(" + to_string(forces[0]) + ", " + to_string(forces[1]) + ")");

    // Individual
    string agent_msg = "";
    for (int i = 0; i < individual_forces.size(); i+=2) {
      agent_msg += "(" + to_string(individual_forces[i]) + ", " + to_string(individual_forces[i+1]) + ")";
    }
    Notify("INDIVIDUAL_AGENT_FORCES", agent_msg);
  }
  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AvoidAgents::OnStartUp()
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
    if (param == "name")
    {
      self_name = value;
      handled = true;
    }
    else if (param == "start_pos")
    {
      handled = true;
      //"11,-123" - split & add timestamp
      double comma_index = value.find(',');
      double x = stod(value.substr(0, comma_index));
      double y = stod(value.substr(comma_index + 1, value.size()));

      milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
      string str_ms = to_string(ms.count());
      double usable_time = stod(str_ms);
      usable_time /= 100;

      self_pos = {x, y, usable_time};
    }
    else if (param == "num_agents")
    {
      num_agents = stoi(value);
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

void AvoidAgents::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("INTERNAL_EST_SELF_REPORT", 0);
  Register("INTERNAL_POS_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool AvoidAgents::buildReport()
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
