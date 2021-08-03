/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Localize.cpp                             */
/************************************************************/

#include <iterator>
#include <math.h>
#include "MBUtils.h"
#include "ACTable.h"
#include "Localize.h"

using namespace std;

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

    if (key != "APPCAST_REQ") // handled by AppCastingMOOSApp
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
  //Fill in
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

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
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

void Localize::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Localize::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:   Localize                            " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return (true);
}