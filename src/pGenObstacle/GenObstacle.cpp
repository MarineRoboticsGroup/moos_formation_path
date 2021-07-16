/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: GenObstacle.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenObstacle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenObstacle::GenObstacle()
{
  steps = 2; // Default to just the corners
  all_posted = false;
}

//---------------------------------------------------------
// Destructor

GenObstacle::~GenObstacle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenObstacle::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if (key == "FOO")
      cout << "great!";

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenObstacle::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenObstacle::Iterate()
{
  AppCastingMOOSApp::Iterate();
  if (!all_posted) {
  double step_x = (max_x - min_x) / (steps - 1);
  double step_y = (max_y - min_y) / (steps - 1);

  // Visualize the obstacle
  for (int i = 0; i < steps; i++)
  {
    vector<vector<double>> msgs = {{min_x, min_y + step_y * i}, {max_x, min_y + step_y * i}, {min_x + step_x * i, min_y}, {min_x + step_x * i, max_y}};
    for (int msg_idx = 0; msg_idx < msgs.size(); msg_idx++)
    {
      string viz_msg = "x=" + to_string(msgs[msg_idx][0]) + ",y=" + to_string(msgs[msg_idx][1]) + ",active=true,label=" + to_string(msgs.size() * i + msg_idx) + ",msg=" + name + ",type=obstacle,label_color=invisible,vertex_color=yellow,vertex_size=2";
      Notify("VIEW_POINT", viz_msg);
    }
  }

  // Sense the obstacle
  string sense_msg = "pts={";
  sense_msg += to_string(int(min_x)) + "," + to_string(int(min_y)) + ":";
  sense_msg += to_string(int(min_x)) + "," + to_string(int(max_y)) + ":";
  sense_msg += to_string(int(max_x)) + "," + to_string(int(max_y)) + ":";
  sense_msg += to_string(int(max_x)) + "," + to_string(int(min_y)) + "}";
  sense_msg += ",label=" + name + ",duration=3600"; //hour duration - shouldn't have to exceed that.  hopefully.
  Notify("GIVEN_OBSTACLE", sense_msg);
  }
  all_posted = true;

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenObstacle::OnStartUp()
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
      name = value;
    }
    else if (param == "min_x")
    {
      min_x = stod(value);
    }
    else if (param == "max_x")
    {
      max_x = stod(value);
    }
    else if (param == "min_y")
    {
      min_y = stod(value);
    }
    else if (param == "max_y")
    {
      max_y = stod(value);
    }
    else if (param == "steps")
    {
      steps = stoi(value);
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

void GenObstacle::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
}

//------------------------------------------------------------
// Procedure: buildReport()

bool GenObstacle::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "UL | LL | UR | LR";
  actab.addHeaderLines();
  actab << "(" + to_string(min_x) + ", " + to_string(min_y) + ")"
        << "(" + to_string(min_x) + ", " + to_string(max_y) + ")"
        << "(" + to_string(max_x) + ", " + to_string(min_y) + ")"
        << "(" + to_string(max_x) + ", " + to_string(max_y) + ")";
  m_msgs << actab.getFormattedString();

  return (true);
}
