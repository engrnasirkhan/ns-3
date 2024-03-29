/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
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
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef MGT_HEADERS_H
#define MGT_HEADERS_H

#include <stdint.h>

#include "ns3/header.h"
#include "status-code.h"
#include "capability-information.h"
#include "supported-rates.h"
#include "ssid.h"

namespace ns3 {

class MgtAssocRequestHeader : public Header 
{
public:
  MgtAssocRequestHeader ();
  ~MgtAssocRequestHeader ();

  void SetSsid (Ssid ssid);
  void SetSupportedRates (SupportedRates rates);
  void SetListenInterval (uint16_t interval);

  Ssid GetSsid (void) const;
  SupportedRates GetSupportedRates (void) const;
  uint16_t GetListenInterval (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  Ssid m_ssid;
  SupportedRates m_rates;
  CapabilityInformation m_capability;
  uint16_t m_listenInterval;
};

class MgtAssocResponseHeader : public Header {
public:
  MgtAssocResponseHeader ();
  ~MgtAssocResponseHeader ();

  StatusCode GetStatusCode (void);
  SupportedRates GetSupportedRates (void);

  void SetSupportedRates (SupportedRates rates);
  void SetStatusCode (StatusCode code);

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  SupportedRates m_rates;
  CapabilityInformation m_capability;
  StatusCode m_code;
  uint16_t m_aid;
};

class MgtProbeRequestHeader : public Header {
public:
  ~MgtProbeRequestHeader ();

  void SetSsid (Ssid ssid);
  void SetSupportedRates (SupportedRates rates);
  Ssid GetSsid (void) const;
  SupportedRates GetSupportedRates (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
private:

  Ssid m_ssid;
  SupportedRates m_rates;
};

class MgtProbeResponseHeader : public Header {
public:
  MgtProbeResponseHeader ();
  ~MgtProbeResponseHeader ();

  Ssid GetSsid (void) const;
  uint64_t GetBeaconIntervalUs (void) const;
  SupportedRates GetSupportedRates (void) const;

  void SetSsid (Ssid ssid);
  void SetBeaconIntervalUs (uint64_t us);
  void SetSupportedRates (SupportedRates rates);
  uint64_t GetTimestamp();
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint64_t m_timestamp;
  Ssid m_ssid;
  uint64_t m_beaconInterval;
  SupportedRates m_rates;
  CapabilityInformation m_capability;
};

class MgtBeaconHeader : public MgtProbeResponseHeader {};

/**
 * \brief See IEEE 802.11 chapter 7.3.1.11
 *
 * Header format: | category: 1 | action value: 1 |
 */
class WifiActionHeader : public Header
{
public:
  WifiActionHeader ();
  ~WifiActionHeader ();

  /* Compatible with open80211s implementation */
  enum CategoryValue //table 7-24 staring from 4
  {
    MESH_PEERING_MGT = 30,
    MESH_LINK_METRIC = 31,
    MESH_PATH_SELECTION = 32,
    MESH_INTERWORKING = 33,
    MESH_RESOURCE_COORDINATION = 34,
    MESH_PROXY_FORWARDING = 35,
  };
  /* Compatible with open80211s implementation */
  enum PeerLinkMgtActionValue
  {
    PEER_LINK_OPEN = 0,
    PEER_LINK_CONFIRM = 1,
    PEER_LINK_CLOSE = 2,
  };
  enum LinkMetricActionValue
  {
    LINK_METRIC_REQUEST = 0,
    LINK_METRIC_REPORT,
  };
  /* Compatible with open80211s implementation */
  enum PathSelectionActionValue
  {
    PATH_SELECTION = 0,
  };
  enum InterworkActionValue
  {
    PORTAL_ANNOUNCEMENT = 0,
  };
  enum ResourceCoordinationActionValue
  {
    CONGESTION_CONTROL_NOTIFICATION = 0,
    MDA_SETUP_REQUEST,
    MDA_SETUP_REPLY,
    MDAOP_ADVERTISMENT_REQUEST,
    MDAOP_ADVERTISMENTS,
    MDAOP_SET_TEARDOWN,
    BEACON_TIMING_REQUEST,
    BEACON_TIMING_RESPONSE,
    TBTT_ADJUSTMENT_REQUEST,
    MESH_CHANNEL_SWITCH_ANNOUNCEMENT,
  };
  typedef union
  {
    enum PeerLinkMgtActionValue peerLink;
    enum LinkMetricActionValue linkMetrtic;
    enum PathSelectionActionValue pathSelection;
    enum InterworkActionValue interwork;
    enum ResourceCoordinationActionValue resourceCoordination;
  } ActionValue;
  void   SetAction (enum CategoryValue type,ActionValue action);

  CategoryValue GetCategory ();
  ActionValue GetAction ();
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
private:
  uint8_t m_category;
  uint8_t m_actionValue;
};


} // namespace ns3

#endif /* MGT_HEADERS_H */
