/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: CommsMgr.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "CommsMgr.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommsMgr::CommsMgr()
{
  max_bits_per_min = 1000;
  num_agents = 0;
  is_anchor = false;

  self_name = "";
  self_pos = {0.0, 0.0, 0.0};
}

//---------------------------------------------------------
// Destructor

CommsMgr::~CommsMgr()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommsMgr::OnNewMail(MOOSMSG_LIST &NewMail)
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
    // Ranges
    if (key == "CRS_RANGE_REPORT")
    {
      string sval = msg.GetString();
      string name = tokStringParse(sval, "target", ',', '=');
      double range = stod(tokStringParse(sval, "range", ',', '='));
      double time = stod(tokStringParse(sval, "time", ',', '='));

      vector<double> vals = {range, time};
      self_to_agent_range[name] = vals;
    }

    else if (key == "ALL_RANGES")
    {
      string sval = msg.GetString();
      string name1 = tokStringParse(sval, "NAME1", ',', '=');
      string name2 = tokStringParse(sval, "NAME2", ',', '=');
      double range = stod(tokStringParse(sval, "RANGE", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));

      vector<string> names = {name1, name2};
      vector<double> vals = {range, time};
      agent_to_agent_range[names] = vals;
    }

    // Anchor positions
    else if (key == "ALL_ANCHORS")
    {
      // Parse msg
      string sval = msg.GetString();
      string name = tokStringParse(sval, "NAME", ',', '=');
      if (name != self_name)
      {
        double x = stod(tokStringParse(sval, "X", ',', '='));
        double y = stod(tokStringParse(sval, "Y", ',', '='));
        double time = stod(tokStringParse(sval, "TIME", ',', '='));

        vector<double> vals = {x, y, time};
        anchor_to_pos[name] = vals;
      }
    }

    else if (key == "NODE_REPORT_LOCAL")
    {
      string sval = msg.GetString();
      double x = stod(tokStringParse(sval, "X", ',', '='));
      double y = stod(tokStringParse(sval, "Y", ',', '='));
      double time = stod(tokStringParse(sval, "TIME", ',', '='));

      vector<double> vals = {x, y, time};
      self_pos = vals;
      if (is_anchor)
      {
        anchor_to_pos[self_name] = vals;
      }
    }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommsMgr::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommsMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();
  //Internal Comms
  // Pass on measurement for each range connection
  for (const auto &pair : agent_to_agent_range)
  {
    vector<string> ordered_names = pair.first;
    vector<double> vals = pair.second;
    string agent_range_report = "NAME1=" + ordered_names[0] +
                                ",NAME2=" + ordered_names[1] +
                                ",RANGE=" + to_string(vals[0]) +
                                ",TIME=" + to_string(vals[1]);
    Notify("INTERNAL_RANGE_REPORT", agent_range_report);
  }

  // Pass on anchor position
  // Anchor positions
  for (const auto &pair : anchor_to_pos)
  {
    string name = pair.first;
    vector<double> vals = pair.second;
    string anchor_pos_report = "NAME=" + name +
                               ",X=" + to_string(vals[0]) +
                               ",Y=" + to_string(vals[1]) +
                               ",TIME=" + to_string(vals[2]);
    Notify("INTERNAL_POS_REPORT", anchor_pos_report);
  }

  // Pass on self position
  if (self_pos[2] != 0)
  {
    // Self pos
    string self_pos_report = "NAME=" + self_name +
                             ",X=" + to_string(self_pos[0]) +
                             ",Y=" + to_string(self_pos[1]) +
                             ",TIME=" + to_string(self_pos[2]);
    Notify("INTERNAL_POS_REPORT", self_pos_report);
  }

  // External Comms
  for (const auto &pair : self_to_agent_range)
  {
    string name = pair.first;
    vector<double> vals = pair.second;
    vector<string> ordered_names = {"", ""};
    if (name < self_name)
    {
      ordered_names = {name, self_name};
    }
    else
    {
      ordered_names = {self_name, name};
    }
    // Pass on to all other agents
    string agent_range_report = "NAME1=" + ordered_names[0] +
                                ",NAME2=" + ordered_names[1] +
                                ",RANGE=" + to_string(vals[0]) +
                                ",TIME=" + to_string(vals[1]);
    Notify("AGENT_RANGE_REPORT", agent_range_report);
  }

  // Anchor specific
  if (is_anchor)
  {
    string anchor_msg = "NAME=" + self_name +
                        ",X=" + to_string(self_pos[0]) +
                        ",Y=" + to_string(self_pos[1]) +
                        ",TIME=" + to_string(self_pos[2]);
    Notify("ANCHOR_REPORT", anchor_msg);
  }
  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommsMgr::OnStartUp()
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
    else if (param == "num_agents")
    {
      num_agents = stoi(value);
      handled = true;
    }
    else if (param == "max_rate")
    {
      max_bits_per_min = stoi(value);
      handled = true;
    }

    // Set is_anchor when enough info is known
    if (num_agents != 0 && self_name.length() > 0)
    {
      // Isolate digits
      int start_index = 0;
      for (; start_index < self_name.length(); start_index++)
      {
        if (isdigit(self_name[start_index]))
          break;
      }
      string name_id = self_name.substr(start_index, self_name.length() - start_index);

      // Determine if in range of anchors
      int id = stoi(name_id);
      is_anchor = id > num_agents - 3;
    }

    if (!handled)
      reportUnhandledConfigWarning(orig);
  }
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void CommsMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("CRS_RANGE_REPORT", 0);
  Register("ALL_RANGES", 0);
  Register("ALL_ANCHORS", 0);
  Register("NODE_REPORT_LOCAL", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool CommsMgr::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: CommsMgr                              " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Self-Agent | Agent-Agent | Anchor | Self";
  actab.addHeaderLines();
  actab << to_string(self_to_agent_range.size())
        << to_string(agent_to_agent_range.size())
        << to_string(anchor_to_pos.size())
        << to_string(self_pos[0]) + ", " + to_string(self_pos[1]) + ", " + to_string(self_pos[2]);
  m_msgs << actab.getFormattedString();

  return (true);
}
