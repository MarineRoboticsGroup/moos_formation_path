/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AvoidObstacles.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include <chrono>
#include "MBUtils.h"
#include "ACTable.h"
#include "AvoidObstacles.h"

using namespace std;
using namespace std::chrono;

//---------------------------------------------------------
// Constructor

AvoidObstacles::AvoidObstacles()
{
}

//---------------------------------------------------------
// Destructor

AvoidObstacles::~AvoidObstacles()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AvoidObstacles::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if (key == "GIVEN_OBSTACLE")
    {
      string sval = msg.GetString();
      string name = tokStringParse(sval, "label", ',', '=');

      //Parse pts
      string points = sval.substr(sval.find("{") + 1, sval.find("}") - 5); // Remove curly brackets
      int count = 0;
      size_t index = 0;
      string elt;
      vector<string> separators = {",", ":"};

      vector<string> split_points;
      while ((index = points.find(separators[count % 2])) != string::npos)
      {
        elt = points.substr(0, index);
        split_points.push_back(elt);
        points.erase(0, index + 1);
        count += 1;
      }
      split_points.push_back(points);

      // Convert from string to double
      vector<double> coord;
      vector<vector<double>> vals;
      for (int i = 0; i < split_points.size() - 1; i += 2)
      {
        coord = {stod(split_points[i]), stod(split_points[i + 1])};
        vals.push_back(coord);
      }

      all_obstacles[name] = vals;
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

bool AvoidObstacles::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AvoidObstacles::Iterate()
{
  AppCastingMOOSApp::Iterate();
  vector<double> forces = {0.0, 0.0};
  if (all_obstacles.size() > 0)
  {
    for (const auto &pair : all_obstacles)
    {
      string name = pair.first;
      vector<vector<double>> vals = pair.second;

      // Max / min vals based on GenObstacle format
      double min_x = vals[0][0];
      double min_y = vals[0][1];
      double max_x = vals[2][0];
      double max_y = vals[2][1];

      // Get dist to obstacle
      double dx = max(0.0, max(min_x - self_pos[0], self_pos[0] - max_x));
      if (dx == min_x - self_pos[0])
      {
        dx = -dx;
      }
      double dy = max(0.0, max(min_y - self_pos[1], self_pos[1] - max_y));
      if (dy == min_y - self_pos[1])
      {
        dy = -dy;
      }

      // Scale by distance
      double dist = sqrt(dx * dx + dy * dy);
      if (dist < 0.01)
      {
        dist = .01;
      }

      forces[0] += dx / pow(dist, 3);
      forces[1] += dy / pow(dist, 3);
    }

    forces[0] /= all_obstacles.size();
    forces[1] /= all_obstacles.size();

    // TODO: Implement normalization if needed

    Notify("OBSTACLE_FORCE", "(" + to_string(forces[0]) + ", " + to_string(forces[1]) + ")");
  }

  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AvoidObstacles::OnStartUp()
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
      //"11,-123" - split & add timestamp
      double comma_index = value.find(',');
      double x = stod(value.substr(0, comma_index));
      double y = stod(value.substr(comma_index + 1, value.size()));

      milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
      string str_ms = to_string(ms.count());
      double usable_time = stod(str_ms);
      usable_time /= 100;

      self_pos = {x, y, usable_time};
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

void AvoidObstacles::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("GIVEN_OBSTACLE", 0);
  Register("INTERNAL_EST_SELF_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool AvoidObstacles::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << to_string(all_obstacles.size())
        << "two"
        << "three"
        << "four";
  m_msgs << actab.getFormattedString();

  return (true);
}
