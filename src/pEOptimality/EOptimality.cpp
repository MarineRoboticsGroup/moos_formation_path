/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: EOptimality.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include <chrono>
#include <math.h>
#include "MBUtils.h"
#include "ACTable.h"
#include "EOptimality.h"
// #include <Eigen/Eigenvalues>

using namespace std;
using namespace std::chrono;

//---------------------------------------------------------
// Constructor

EOptimality::EOptimality()
{
  fim = {{0.0}};
}

//---------------------------------------------------------
// Destructor

EOptimality::~EOptimality()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool EOptimality::OnNewMail(MOOSMSG_LIST &NewMail)
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
      all_est_poses[self_name] = vals;
      self_pos = vals;
    }

    else if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return (true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool EOptimality::OnConnectToServer()
{
  registerVariables();
  return (true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool EOptimality::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // First attempt, may be rather slow
  
  // Make FIM
  if (all_est_poses.size() == num_agents)
  {
    fim = build_fischer_matrix();
  }
  // (Get all the eigenpairs)


  // Get the min eigenpair
  // Get the gradient from the eigenpair
  // Normalize if need be
  AppCastingMOOSApp::PostReport();
  return (true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool EOptimality::OnStartUp()
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

void EOptimality::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("INTERNAL_EST_SELF_REPORT", 0);
  Register("INTERNAL_POS_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool EOptimality::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: EOptimality                           " << endl;
  m_msgs << "============================================" << endl;

  for (int i = 0; i < fim.size(); i++)
  {
    for (int j = 0; j < fim.size(); j++)
    {
      m_msgs << "(" + to_string(fim[i][j]) + ")";
    }
    m_msgs << "\n";
  }

  return (true);
}

vector<vector<double>> EOptimality::build_fischer_matrix()
{
  int num_var_agents = num_agents - 3;
  Notify("TEST", "START FIM");
  vector<vector<double>> K(num_var_agents * 2, vector<double>(num_var_agents * 2, 0.0));

  for (int i = 0; i < num_agents; i++)
  {
    string name1 = "agent" + to_string(i + 1);
    vector<double> node_i = all_est_poses[name1];

    Notify("TEST", name1 + ": (" + to_string(node_i[0]) + ", " + to_string(node_i[1]) + ")");
  }

  // Names / accesses
  for (int i = 0; i < num_agents - 1; i++)
  {
    string name1 = "agent" + to_string(i + 1);
    vector<double> node_i = all_est_poses[name1];

    for (int j = i + 1; j < num_agents; j++)
    {
      string name2 = "agent" + to_string(j + 1);
      vector<double> node_j = all_est_poses[name2];

      // The mathy part
      double diff_x = node_i[0] - node_j[0];
      double diff_y = node_i[1] - node_j[1];
      double dist = pow(diff_x * diff_x + diff_y * diff_y, .5);
      // TODO: actually figure out the noise model
      // double denom = std_dev_squared * (dist) * *(2 * alpha);
      double denom = 1.;
      double delX2 = diff_x * diff_x / denom;
      double delY2 = diff_y * diff_y / denom;
      double delXY = diff_x * diff_y / denom;

      // Blocks
      if (i < num_var_agents)
      {
        // Block ii
        K[2 * i][2 * i] += delX2;
        K[2 * i + 1][2 * i + 1] += delY2;
        K[2 * i][2 * i + 1] += delXY;
        K[2 * i + 1][2 * i] += delXY;
      }
      if (j < num_var_agents)
      {
        // Block jj
        K[2 * j][2 * j] += delX2;
        K[2 * j + 1][2 * j + 1] += delY2;
        K[2 * j][2 * j + 1] += delXY;
        K[2 * j + 1][2 * j] += delXY;
      }
      if (i < num_var_agents && j < num_var_agents)
      {
        // Block ij
        K[2 * i][2 * j] = -delX2;
        K[2 * i + 1][2 * j + 1] = -delY2;
        K[2 * i][2 * j + 1] = -delXY;
        K[2 * i + 1][2 * j] = -delXY;

        // Block ji
        K[2 * j][2 * i] = -delX2;
        K[2 * j + 1][2 * i + 1] = -delY2;
        K[2 * j][2 * i + 1] = -delXY;
        K[2 * j + 1][2 * i] = -delXY;
      }
    }
  }

  for (int i = 0; i < fim.size(); i++)
  {
    string k_line = "";
    for (int j = 0; j < fim.size(); j++)
    {
      k_line += "(" + to_string(fim[i][j]) + ")";
    }
    Notify("TEST", k_line);
  }

  return K;
}