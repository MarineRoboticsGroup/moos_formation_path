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
#include <Eigen/Eigenvalues>

using namespace std;
using namespace std::chrono;
using namespace Eigen;

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

  if (all_est_poses.size() == num_agents)
  {
    // Make FIM
    fim = buildFischerMatrix();

    // Get the min eigenpair
    vector<vector<double>> min_pair = getMinFIMEigenpair();
    double lambda = min_pair[0][0];
    vector<double> v = min_pair[1];

    string line = "";
    for (int i = 0; i < v.size(); i++)
    {
      line += to_string(v[i]) + ", ";
    }

    // Get the gradient from the eigenpair
    vector<double> grad = getGradientOfFIMEigenpair(lambda, v);

    int self_id = stoi(self_name.substr(5));
    // Notify("TEST", to_string(self_id) + self_name);
    if (self_id <= num_agents - 3)
    {
      Notify("EOPTIMALITY_FORCE", "(" + to_string(grad[2*self_id]) + ", " + to_string(grad[2*self_id + 1]) + ")");
    }
  }

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

//#############################################################################
//    Everything after this point is a translation of math_utils.py           #
//    in the MarineRoboticsGroup/lcgp repo                                    #
//                                                                            #
//#############################################################################

//---------------------------------------------------------
// Procedure: buildFischerMatrix()
//            updates fischer information matrix based on agent positions

vector<vector<double>> EOptimality::buildFischerMatrix()
{
  int num_var_agents = num_agents - 3;
  vector<vector<double>> K(num_var_agents * 2, vector<double>(num_var_agents * 2, 0.0));

  for (int i = 0; i < num_agents; i++)
  {
    string name1 = "agent" + to_string(i + 1);
    vector<double> node_i = all_est_poses[name1];
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

      // TODO: rename new_i to i and new_j to j
      int new_i = i - 3;
      int new_j = j - 3;

      // Blocks
      if (0 <= new_i)
      {
        // Block ii
        K[2 * new_i][2 * new_i] += delX2;
        K[2 * new_i + 1][2 * new_i + 1] += delY2;
        K[2 * new_i][2 * new_i + 1] += delXY;
        K[2 * new_i + 1][2 * new_i] += delXY;
      }
      if (0 <= new_j)
      {
        // Block jj
        K[2 * new_j][2 * new_j] += delX2;
        K[2 * new_j + 1][2 * new_j + 1] += delY2;
        K[2 * new_j][2 * new_j + 1] += delXY;
        K[2 * new_j + 1][2 * new_j] += delXY;
      }
      if (0 <= new_i && 0 <= new_j)
      {
        // Block ij
        K[2 * new_i][2 * new_j] = -delX2;
        K[2 * new_i + 1][2 * new_j + 1] = -delY2;
        K[2 * new_i][2 * new_j + 1] = -delXY;
        K[2 * new_i + 1][2 * new_j] = -delXY;

        // Block ji
        K[2 * new_j][2 * new_i] = -delX2;
        K[2 * new_j + 1][2 * new_i + 1] = -delY2;
        K[2 * new_j][2 * new_i + 1] = -delXY;
        K[2 * new_j + 1][2 * new_i] = -delXY;
      }
    }
  }
  return K;
}

//---------------------------------------------------------
// Procedure: getMinFIMEigenpair()
//            returns minimum eigenvalue and
//            corresponding vector from the FIM

vector<vector<double>> EOptimality::getMinFIMEigenpair()
{
  // Build matrix using fim
  int side_length = fim.size();
  MatrixXf m(side_length, side_length);
  for (int i = 0; i < side_length; i++)
  {
    for (int j = 0; j < side_length; j++)
    {
      m(i, j) = fim[i][j];
    }
  }

  // Solver matrix & get results
  SelfAdjointEigenSolver<MatrixXf> es;
  es.compute(m);
  string line = "";
  for (int i = 0; i < es.eigenvalues().size(); i++)
  {
    line += to_string(es.eigenvalues()[i]) + ", ";
  }

  double lambda = es.eigenvalues()[0];
  VectorXd eigen_v = es.eigenvectors().col(0).cast<double>();

  // Format result and return
  vector<double> v;
  for (int i = 0; i < eigen_v.size(); i++)
  {
    v.push_back(double(eigen_v[i]));
  }
  vector<vector<double>> output{
      {lambda},
      v};
  return output;
}

// Procedure: getGradientOfEigenpair()
//            returns the gradient of the eigenvalue corresponding to the eigvec and FIM

vector<double> EOptimality::getGradientOfFIMEigenpair(double lambda, vector<double> v_vec)
{
  int side_length = fim.size();
  VectorXd grad(side_length);

  MatrixXd A(side_length, side_length);
  VectorXd v(v_vec.size());

  for (int i = 0; i < v_vec.size(); i++)
  {
    v(i) = v_vec[i];
  }

  for (int agent_num = 0; agent_num < num_agents; agent_num++)
  {

    vector<vector<double>> A_vec = getPartialDerivOfMatrix(agent_num);
    for (int i = 0; i < side_length; i++)
    {
      for (int j = 0; j < side_length; j++)
      {
        A(i, j) = A_vec[i][j];
      }
    }

    grad(agent_num) = v.transpose() * A * v;
  }
  double norm = grad.norm();

  string line = "";
  vector<double> out(side_length);
  for (int i = 0; i < grad.size(); i++)
  {
    line += to_string(grad(i) / norm) + ", ";
    out[i] = grad(i) / norm;
  }

  return out;
}

vector<vector<double>> EOptimality::getPartialDerivOfMatrix(int index)
{
  int side_length = fim.size();
  int i = (int)index / 2;
  vector<vector<double>> a(side_length, vector<double>(side_length, 0.));

  // Get the pos for the agent w/ ID i+1
  string name_i = "agent" + to_string(i + 1);
  vector<double> node_i = all_est_poses[name_i];

  for (int j = 0; j < num_agents - 3; j++)
  {
    if (i == j)
    {
      continue;
    }
    // Get the pos for the agent w/ ID j+1
    string name_j = "agent" + to_string(j + 1);
    vector<double> node_j = all_est_poses[name_j];

    vector<vector<double>> dFIMii_di(2, vector<double>(2, 0.));
    vector<vector<double>> dFIMjj_di(2, vector<double>(2, 0.));
    vector<vector<double>> dFIMij_di(2, vector<double>(2, 0.));
    vector<vector<double>> dFIMji_di(2, vector<double>(2, 0.));

    if (index % 2 == 0) // x
    {
      dFIMii_di[0] = {2 * (node_i[0] - node_j[0]), node_i[1] - node_j[1]};
      dFIMii_di[1] = {node_i[1] - node_j[1], 0.};

      dFIMij_di[0] = {2 * (node_j[0] - node_i[0]), node_j[1] - node_i[1]};
      dFIMij_di[1] = {node_j[1] - node_i[1], 0.};

      dFIMjj_di = dFIMii_di;
      dFIMji_di = dFIMij_di;
    }
    else // y
    {
      dFIMii_di[0] = {0., node_i[0] - node_j[0]};
      dFIMii_di[1] = {node_i[0] - node_j[0], 2 * (node_i[1] - node_j[1])};

      dFIMij_di[0] = {0., node_j[0] - node_i[0]};
      dFIMij_di[1] = {node_j[0] - node_i[0], 2 * (node_j[1] - node_i[1])};

      dFIMjj_di = dFIMii_di;
      dFIMji_di = dFIMij_di;
    }

    for (int place = 0; place < 4; place++)
    {
      int delta_x = (int)place % 2;
      int delta_y = (int)place / 2;

      //  FIMii
      a[2 * i + delta_x][2 * i + delta_y] += dFIMii_di[delta_x][delta_y];
      //  FIMjj
      a[2 * j + delta_x][2 * j + delta_y] += dFIMjj_di[delta_x][delta_y];
      //  FIMij
      a[2 * i + delta_x][2 * j + delta_y] += dFIMij_di[delta_x][delta_y];
      //  FIMji
      a[2 * j + delta_x][2 * i + delta_y] += dFIMji_di[delta_x][delta_y];
    }
  }

  return a;
}