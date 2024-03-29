/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 The Boeing Company
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/test.h"
#include "ns3/pcap-file.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/data-rate.h"
#include "ns3/inet-socket-address.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/wifi-net-device.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("PropagationLossModelsTest");

// ===========================================================================
// This is a simple test to validate propagation loss models of ns-3 wifi.
// See the chapter in the ns-3 testing and validation guide for more detail
// ===========================================================================
//
class FriisPropagationLossModelTestCase : public TestCase
{
public:
  FriisPropagationLossModelTestCase ();
  virtual ~FriisPropagationLossModelTestCase ();

private:
  virtual bool DoRun (void);

  typedef struct {
    Vector m_position;
    double m_pt;  // dBm
    double m_pr;  // W
    double m_tolerance;
  } TestVector;

  TestVectors<TestVector> m_testVectors;
};

FriisPropagationLossModelTestCase::FriisPropagationLossModelTestCase ()
  : TestCase ("Check to see that the ns-3 Friis propagation loss model provides correct received power"), m_testVectors ()
{
}

FriisPropagationLossModelTestCase::~FriisPropagationLossModelTestCase ()
{
}

bool
FriisPropagationLossModelTestCase::DoRun (void)
{
  // The ns-3 testing manual gives more background on the values selected
  // for this test.  First, set a few defaults. 

  // wavelength at 2.4 GHz is 0.125m
  Config::SetDefault ("ns3::FriisPropagationLossModel::Lambda", DoubleValue (0.125));
  Config::SetDefault ("ns3::FriisPropagationLossModel::SystemLoss", DoubleValue (1.0));

  // Select a reference transmit power
  // Pt = 10^(17.0206/10)/10^3 = .05035702 W
  double txPowerW = 0.05035702;
  double txPowerdBm = 10 * log10 (txPowerW) + 30;

  //
  // We want to test the propagation loss model calculations at a few chosen 
  // distances and compare the results to those we have manually calculated
  // according to the model documentation.  The model reference specifies, 
  // for instance, that the received power at 100m according to the provided
  // input power will be 4.98265e-10 W.  Since this value specifies the power
  // to 1e-15 significance, we test the ns-3 calculated value for agreement 
  // within 5e-16.
  //
  TestVector testVector;

  testVector.m_position = Vector (100, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 4.98265e-10;
  testVector.m_tolerance = 5e-16;
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (500, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 1.99306e-11;
  testVector.m_tolerance = 5e-17;
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (1000, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 4.98265e-12;
  testVector.m_tolerance = 5e-18;
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (2000, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 1.24566e-12;
  testVector.m_tolerance = 5e-18;
  m_testVectors.Add (testVector);

  // Now, check that the received power values are expected

  Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel> (); 
  a->SetPosition (Vector (0,0,0));
  Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel> (); 

  Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> (); 
  for (uint32_t i = 0; i < m_testVectors.GetN (); ++i)
    {
      testVector = m_testVectors.Get (i);
      b->SetPosition (testVector.m_position);
      double resultdBm = lossModel->CalcRxPower (testVector.m_pt, a, b);
      double resultW =   pow (10.0, resultdBm/10.0)/1000;
      NS_TEST_EXPECT_MSG_EQ_TOL (resultW, testVector.m_pr, testVector.m_tolerance, "Got unexpected rcv power");
    }
	
  return GetErrorStatus ();
}

class LogDistancePropagationLossModelTestCase : public TestCase
{
public:
  LogDistancePropagationLossModelTestCase ();
  virtual ~LogDistancePropagationLossModelTestCase ();

private:
  virtual bool DoRun (void);

  typedef struct {
    Vector m_position;
    double m_pt;  // dBm
    double m_pr;  // W
    double m_tolerance;
  } TestVector;

  TestVectors<TestVector> m_testVectors;
};

LogDistancePropagationLossModelTestCase::LogDistancePropagationLossModelTestCase ()
  : TestCase ("Check to see that the ns-3 Log Distance propagation loss model provides correct received power"), m_testVectors ()
{
}

LogDistancePropagationLossModelTestCase::~LogDistancePropagationLossModelTestCase ()
{
}

bool
LogDistancePropagationLossModelTestCase::DoRun (void)
{
  // reference loss at 2.4 GHz is 40.045997
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.045997));
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::Exponent", DoubleValue (3));

  // Select a reference transmit power
  // Pt = 10^(17.0206/10)/10^3 = .05035702 W
  double txPowerW = 0.05035702;
  double txPowerdBm = 10 * log10 (txPowerW) + 30;

  //
  // We want to test the propagation loss model calculations at a few chosen 
  // distances and compare the results to those we have manually calculated
  // according to the model documentation.  The following "TestVector" objects
  // will drive the test.
  //
  TestVector testVector;

  testVector.m_position = Vector (10, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 4.98265e-9;
  testVector.m_tolerance = 5e-15; 
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (20, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 6.22831e-10;
  testVector.m_tolerance = 5e-16;
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (40, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 7.78539e-11;
  testVector.m_tolerance = 5e-17;
  m_testVectors.Add (testVector);

  testVector.m_position = Vector (80, 0, 0);
  testVector.m_pt = txPowerdBm;
  testVector.m_pr = 9.73173e-12;
  testVector.m_tolerance = 5e-17;
  m_testVectors.Add (testVector);

  Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel> (); 
  a->SetPosition (Vector (0,0,0));
  Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel> (); 

  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> (); 
  for (uint32_t i = 0; i < m_testVectors.GetN (); ++i)
    {
      testVector = m_testVectors.Get (i);
      b->SetPosition (testVector.m_position);
      double resultdBm = lossModel->CalcRxPower (testVector.m_pt, a, b);
      double resultW =   pow (10.0, resultdBm/10.0)/1000;
      NS_TEST_EXPECT_MSG_EQ_TOL (resultW, testVector.m_pr, testVector.m_tolerance, "Got unexpected rcv power");
    }
	
  return GetErrorStatus ();
}

class PropagationLossModelsTestSuite : public TestSuite
{
public:
  PropagationLossModelsTestSuite ();
};

PropagationLossModelsTestSuite::PropagationLossModelsTestSuite ()
  : TestSuite ("propagation-loss-model", UNIT)
{
  AddTestCase (new FriisPropagationLossModelTestCase);
  AddTestCase (new LogDistancePropagationLossModelTestCase);
}

PropagationLossModelsTestSuite WifiPropagationLossModelsTestSuite;
