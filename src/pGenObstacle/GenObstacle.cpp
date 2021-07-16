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
  m_new_obstacle = {};
  env = 0;
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

    if (key == "NEW_OBSTACLE")
    {
      string sval = msg.GetString();
      m_new_obstacle.push_back(sval);
    }
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

  while (!m_new_obstacle.empty())
  {
    // Get relevant values from message
    string current_obstacle = m_new_obstacle.back();
    m_new_obstacle.pop_back();

    string name = tokStringParse(current_obstacle, "name", ',', '=');
    double min_x = stod(tokStringParse(current_obstacle, "min_x", ',', '='));
    double max_x = stod(tokStringParse(current_obstacle, "max_x", ',', '='));
    double min_y = stod(tokStringParse(current_obstacle, "min_y", ',', '='));
    double max_y = stod(tokStringParse(current_obstacle, "max_y", ',', '='));
    int steps = stoi(tokStringParse(current_obstacle, "steps", ',', '='));

    // Visualize the obstacle
    double step_x = (max_x - min_x) / (steps - 1);
    double step_y = (max_y - min_y) / (steps - 1);

    for (int i = 0; i < steps; i++)
    {
      vector<vector<double>> msgs = {{min_x, min_y + step_y * i},
                                     {max_x, min_y + step_y * i},
                                     {min_x + step_x * i, min_y},
                                     {min_x + step_x * i, max_y}};

      for (int msg_idx = 0; msg_idx < msgs.size(); msg_idx++)
      {
        string viz_msg = "x=" + to_string(msgs[msg_idx][0]) + ",y=" + to_string(msgs[msg_idx][1]);
        viz_msg += ",active=true,label=" + name + to_string(+msgs.size() * i + msg_idx) + ",msg=" + name;
        viz_msg += ",type=obstacle,label_color=invisible,vertex_color=yellow,vertex_size=2";

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

    if (param == "new_obstacle")
    {
      m_new_obstacle.push_back(value);
    }
    else if (param == "env")
    {
      env = stoi(value);
      setEnv();
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
// Procedure: setEnv()
//            sets obstacles
void GenObstacle::setEnv()
{
  // Valid env #s
  vector<int> valid_env_ids = {1, 2, 3};

  // boundaries
  int int_min_x = 0;
  int int_max_x = 200;
  int int_min_y = -200;
  int int_max_y = 0;

  // Convert boundaries to usable vals
  string inside_min_x = to_string(int_min_x);
  string inside_max_x = to_string(int_max_x);
  string inside_min_y = to_string(int_min_y);
  string inside_max_y = to_string(int_max_y);

  string outside_min_x = to_string(int_min_x - 1);
  string outside_max_x = to_string(int_max_x + 1);
  string outside_min_y = to_string(int_min_y - 1);
  string outside_max_y = to_string(int_max_y + 1);

  // Boundaries
  if (count(valid_env_ids.begin(), valid_env_ids.end(), env))
  {
    m_new_obstacle.push_back("name=l_bound,min_x=" + outside_min_x + ",max_x=" + inside_min_x + ",min_y=" + inside_min_y + ",max_y=" + inside_max_y + ",steps=50");
    m_new_obstacle.push_back("name=r_bound,min_x=" + inside_max_x + ",max_x=" + outside_max_x + ",min_y=" + inside_min_y + ",max_y=" + inside_max_y + ",steps=50");
    m_new_obstacle.push_back("name=t_bound,min_x=" + inside_min_x + ",max_x=" + inside_max_x + ",min_y=" + outside_min_y + ",max_y=" + inside_min_y + ",steps=50");
    m_new_obstacle.push_back("name=b_bound,min_x=" + inside_min_x + ",max_x=" + inside_max_x + ",min_y=" + inside_min_y + ",max_y=" + outside_max_y + ",steps=50");
  }

  switch (env)
  {
    case 1: // Plain env
    {
      break;
    }
    case 2: // Curve Maze
    {
      // Wall 1
      string case_2_wall_1_min_x = to_string(int_min_x + 2.0 / 3.0 * (int_max_x - int_min_x) - .5);
      string case_2_wall_1_max_x = to_string(int_min_x + 2.0 / 3.0 * (int_max_x - int_min_x) + .5);
      string case_2_wall_1_min_y = to_string((int_min_y + int_max_y) / 2.0);
      string case_2_wall_1_max_y = to_string(int_max_y);

      m_new_obstacle.push_back("name=case_2_wall_1,min_x=" + case_2_wall_1_min_x + ",max_x=" + case_2_wall_1_max_x + ",min_y=" + case_2_wall_1_min_y + ",max_y=" + case_2_wall_1_max_y + ",steps=50");

      // Wall 2
      string case_2_wall_2_min_x = to_string(int_min_x + 1.0 / 3.0 * (int_max_x - int_min_x) - .5);
      string case_2_wall_2_max_x = to_string(int_min_x + 1.0 / 3.0 * (int_max_x - int_min_x) + .5);
      string case_2_wall_2_min_y = to_string(int_min_y);
      string case_2_wall_2_max_y = to_string((int_min_y + int_max_y) / 2.0);

      m_new_obstacle.push_back("name=case_2_wall_2,min_x=" + case_2_wall_2_min_x + ",max_x=" + case_2_wall_2_max_x + ",min_y=" + case_2_wall_2_min_y + ",max_y=" + case_2_wall_2_max_y + ",steps=50");

      break;
    }
    case 3: // Split Maze
    {
      // Wall 1
      string case_3_wall_1_min_x = to_string(int_min_x + 1.0 / 3.0 * (int_max_x - int_min_x));
      string case_3_wall_1_max_x = to_string(int_max_x);
      string case_3_wall_1_min_y = to_string((int_min_y + int_max_y) / 2.0 - .5);
      string case_3_wall_1_max_y = to_string((int_min_y + int_max_y) / 2.0 + .5);

      m_new_obstacle.push_back("name=case_3_wall_1,min_x=" + case_3_wall_1_min_x + ",max_x=" + case_3_wall_1_max_x + ",min_y=" + case_3_wall_1_min_y + ",max_y=" + case_3_wall_1_max_y + ",steps=50");

      break;
    }
  }
}

//---------------------------------------------------------
// Procedure: registerVariables

void GenObstacle::registerVariables()
{
  Register("NEW_OBSTACLE", 0);
  AppCastingMOOSApp::RegisterVariables();
}

//------------------------------------------------------------
// Procedure: buildReport()

bool GenObstacle::buildReport()
{
  //TODO: Pretty prints once it's working
  // m_msgs << "============================================" << endl;
  // m_msgs << "File:                                       " << endl;
  // m_msgs << "============================================" << endl;

  // ACTable actab(4);
  // actab << "TL | BL | TR | BR";
  // actab.addHeaderLines();
  // actab << "(" + to_string(min_x) + ", " + to_string(min_y) + ")"
  //       << "(" + to_string(min_x) + ", " + to_string(max_y) + ")"
  //       << "(" + to_string(max_x) + ", " + to_string(min_y) + ")"
  //       << "(" + to_string(max_x) + ", " + to_string(max_y) + ")";
  // m_msgs << actab.getFormattedString();

  return (true);
}
