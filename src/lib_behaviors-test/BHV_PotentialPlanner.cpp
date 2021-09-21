/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_PotentialPlanner.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_PotentialPlanner.h"
#include "ZAIC_PEAK.h"
#include <math.h>

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_PotentialPlanner::BHV_PotentialPlanner(IvPDomain domain) : IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  // addInfoVars("NAV_X, NAV_Y");

  // All potential forces used
  force_names = {"WAYPOINT_FORCE", "AGENT_FORCE", "OBSTACLE_FORCE", "EOPTIMALITY_FORCE"};
  for (int i = 0; i < force_names.size(); i++)
  {
    addInfoVars(force_names[i], "no_warning");
    forces[force_names[i]] = {0.0, 0.0};
    headings[force_names[1]] = 0.0;
  }
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_PotentialPlanner::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());

  //TODO: Add ability to do relative weighting, including 0.0 weight
  if ((param == "foo") && isNumber(val))
  {
    // Set local member variables here
    return (true);
  }
  else if (param == "bar")
  {
    // return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return (false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_PotentialPlanner::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_PotentialPlanner::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_PotentialPlanner::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_PotentialPlanner::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_PotentialPlanner::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_PotentialPlanner::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_PotentialPlanner::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction *BHV_PotentialPlanner::onRunState()
{
  // Update values
  string val, x_val, y_val;

  for (int i = 0; i < force_names.size(); i++)
  {
    val = getBufferStringVal(force_names[i]);
    if (val.size() > 0)
    {
      x_val = val.substr(1, val.find(","));
      y_val = val.substr(val.find(" "), val.find(")"));
      forces[force_names[i]][0] = stod(x_val);
      forces[force_names[i]][1] = stod(y_val);
      headings[force_names[i]] = atan2(stod(y_val), stod(x_val));
    }
  }

  // Part 1: Build the IvP function
  if (headings["OBSTACLE_FORCE"] > .01) {
    IvPFunction *ipf = 0;
    ipf = buildFunctionWithZAIC();
    if (ipf == 0)
      postWMessage("Problem Creating the IvP Function");

    if (ipf)
      ipf->setPWT(m_priority_wt);

    return (ipf);
  }
  return 0;
}

IvPFunction *BHV_PotentialPlanner::buildFunctionWithZAIC()
{

  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(headings["OBSTACLE_FORCE"]);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);
  crs_zaic.setValueWrap(true);
  if (crs_zaic.stateOK() == false)
  {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return (0);
  }

  IvPFunction *ivp_function = crs_zaic.extractIvPFunction();

  return (ivp_function);
}