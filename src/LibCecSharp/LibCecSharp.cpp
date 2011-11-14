/*
 * This file is part of the libCEC(R) library.
 *
 * libCEC(R) is Copyright (C) 2011 Pulse-Eight Limited.  All rights reserved.
 * libCEC(R) is an original work, containing original code.
 *
 * libCEC(R) is a trademark of Pulse-Eight Limited.
 *
 * This program is dual-licensed; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * Alternatively, you can license this library under a commercial license,
 * please contact Pulse-Eight Licensing for more information.
 *
 * For more information contact:
 * Pulse-Eight Licensing       <license@pulse-eight.com>
 *     http://www.pulse-eight.com/
 *     http://www.pulse-eight.net/
 */

#include "stdafx.h"
#include <windows.h>
#include <vcclr.h>
#include <msclr/marshal.h>
#include <cec.h>
#using <System.dll>

using namespace System;
using namespace CEC;
using namespace msclr::interop;

public enum class CecDeviceType
{
  Tv              = 0,
  RecordingDevice = 1,
  Reserved        = 2,
  Tuner           = 3,
  PlaybackDevice  = 4,
  AudioSystem     = 5
};

public enum class CecLogLevel
{
  None    = 0,
  Error   = 1,
  Warning = 2,
  Notice  = 4,
  Traffic = 8,
  Debug   = 16,
  All     = 31
};

public enum class CecLogicalAddress
{
  Unknown          = -1, //not a valid logical address
  Tv               = 0,
  RecordingDevice1 = 1,
  RecordingDevice2 = 2,
  Tuner1           = 3,
  PlaybackDevice1  = 4,
  AudioSystem      = 5,
  Tuner2           = 6,
  Tuner3           = 7,
  PlaybackDevice2  = 8,
  RecordingDevice3 = 9,
  Tuner4           = 10,
  PlaybackDevice3  = 11,
  Reserved1        = 12,
  Reserved2        = 13,
  FreeUse          = 14,
  Unregistered     = 15,
  Broadcast        = 15
};

public enum class CecPowerStatus
{
  On                      = 0x00,
  Standby                 = 0x01,
  InTransitionStandbyToOn = 0x02,
  InTransitionOnToStandby = 0x03,
  Unknown                 = 0x99
};

public enum class CecVersion
{
  Unknown = 0x00,
  V1_2    = 0x01,
  V1_2A   = 0x02,
  V1_3    = 0x03,
  V1_3A   = 0x04,
  V1_4    = 0x05
};

public enum class CecDisplayControl
{
  DisplayForDefaultTime = 0x00,
  DisplayUntilCleared   = 0x40,
  ClearPreviousMessage  = 0x80,
  ReservedForFutureUse  = 0xC0
};

public enum class CecMenuState
{
  Activated   = 0,
  Deactivated = 1
};

public enum class CecDeckControlMode
{
  SkipForwardWind   = 1,
  SkipReverseRewind = 2,
  Stop              = 3,
  Eject             = 4
};

public enum class CecDeckInfo
{
  Play               = 0x11,
  Record             = 0x12,
  Reverse            = 0x13,
  Still              = 0x14,
  Slow               = 0x15,
  SlowReverse        = 0x16,
  FastForward        = 0x17,
  FastReverse        = 0x18,
  NoMedia            = 0x19,
  Stop               = 0x1A,
  SkipForwardWind    = 0x1B,
  SkipReverseRewind  = 0x1C,
  IndexSearchForward = 0x1D,
  IndexSearchReverse = 0x1E,
  OtherStatus        = 0x1F
};

public ref class CecAdapter
{
public:
  CecAdapter(String ^ strPath, String ^ strComPort)
  {
    Path = strPath;
    ComPort = strComPort;
  }

  property String ^ Path;
  property String ^ ComPort;
};

public ref class CecDeviceTypeList
{
public:
  CecDeviceTypeList(void)
  {
    Types = gcnew array<CecDeviceType>(5);
    for (unsigned int iPtr = 0; iPtr < 5; iPtr++)
      Types[iPtr] = CecDeviceType::Reserved;
  }

  property array<CecDeviceType> ^ Types;
};

public ref class CecDatapacket
{
public:
  CecDatapacket(void)
  {
    Data = gcnew array<uint8_t>(100);
    Size = 0;
  }

  void PushBack(uint8_t data)
  {
    if (Size < 100)
    {
      Data[Size] = data;
      Size++;
    }
  }

  property array<uint8_t> ^ Data;
  property uint8_t          Size;
};

public ref class CecCommand
{
public:
  CecCommand(CecLogicalAddress iInitiator, CecLogicalAddress iDestination, bool bAck, bool bEom, int8_t iOpcode, int32_t iTransmitTimeout)
  {
    Initiator       = iInitiator;
    Destination     = iDestination;
    Ack             = bAck;
    Eom             = bEom;
    Opcode          = iOpcode;
    OpcodeSet       = true;
    TransmitTimeout = iTransmitTimeout;
    Parameters      = gcnew CecDatapacket;
    Empty           = false;
  }

  CecCommand(void)
  {
    Initiator       = CecLogicalAddress::Unknown;
    Destination     = CecLogicalAddress::Unknown;
    Ack             = false;
    Eom             = false;
    Opcode          = 0;
    OpcodeSet       = false;
    TransmitTimeout = 0;
    Parameters      = gcnew CecDatapacket;
    Empty           = true;
  }

  void PushBack(uint8_t data)
  {
    if (Initiator == CecLogicalAddress::Unknown && Destination == CecLogicalAddress::Unknown)
    {
      Initiator   = (CecLogicalAddress) (data >> 4);
      Destination = (CecLogicalAddress) (data & 0xF);
    }
    else if (!OpcodeSet)
    {
      OpcodeSet = true;
      Opcode    = data;
    }
    else
    {
      Parameters->PushBack(data);
    }
  }

  property bool               Empty;
  property CecLogicalAddress  Initiator;
  property CecLogicalAddress  Destination;
  property bool               Ack;
  property bool               Eom;
  property int8_t             Opcode;
  property CecDatapacket ^    Parameters;
  property bool               OpcodeSet;
  property int32_t            TransmitTimeout;
};

public ref class CecKeypress
{
public:
  CecKeypress(int iKeycode, unsigned int iDuration)
  {
    Keycode  = iKeycode;
    Duration = iDuration;
    Empty    = false;
  }

  CecKeypress(void)
  {
    Keycode  = 0;
    Duration = 0;
    Empty    = true;
  }

  property bool         Empty;
  property int          Keycode;
  property unsigned int Duration;
};

public ref class CecLogMessage
{
public:
  CecLogMessage(String ^ strMessage, CecLogLevel iLevel, int64_t iTime)
  {
    Message = strMessage;
    Level   = iLevel;
    Time    = iTime;
    Empty   = false;
  }

  CecLogMessage(void)
  {
    Message = "";
    Level   = CecLogLevel::None;
    Time    = 0;
    Empty   = true;
  }

  property bool        Empty;
  property String ^    Message;
  property CecLogLevel Level;
  property int64_t     Time;
};

public ref class LibCecSharp
{
public:
   LibCecSharp(String ^ strDeviceName, CecDeviceTypeList ^ deviceTypes)
   {
     marshal_context ^ context = gcnew marshal_context();

     const char* strDeviceNameC = context->marshal_as<const char*>(strDeviceName);

     cec_device_type_list types;
     for (unsigned int iPtr = 0; iPtr < 5; iPtr++)
       types.types[iPtr] = (cec_device_type)deviceTypes->Types[iPtr];
     m_libCec = (ICECAdapter *) CECInit(strDeviceNameC, types);
     delete context;
   }
   
   ~LibCecSharp(void)
   {
     CECDestroy(m_libCec);
     m_libCec = NULL;
   }

protected:
   !LibCecSharp(void)
   {
     CECDestroy(m_libCec);
     m_libCec = NULL;
   }

public:
  array<CecAdapter ^> ^ FindAdapters(String ^ path)
  {
    cec_adapter *devices = new cec_adapter[10];

    marshal_context ^ context = gcnew marshal_context();
    const char* strPathC = path->Length > 0 ? context->marshal_as<const char*>(path) : NULL;

    uint8_t iDevicesFound = m_libCec->FindAdapters(devices, 10, NULL);

    array<CecAdapter ^> ^ adapters = gcnew array<CecAdapter ^>(iDevicesFound);
    for (unsigned int iPtr = 0; iPtr < iDevicesFound; iPtr++)
      adapters[iPtr] = gcnew CecAdapter(gcnew String(devices[iPtr].path), gcnew String(devices[iPtr].comm));

    delete devices;
    delete context;
    return adapters;
  }

  bool Open(String ^ strPort, int iTimeoutMs)
  {
    marshal_context ^ context = gcnew marshal_context();
    const char* strPortC = context->marshal_as<const char*>(strPort);
    bool bReturn = m_libCec->Open(strPortC, iTimeoutMs);
    delete context;
    return bReturn;
  }

  void Close(void)
  {
    m_libCec->Close();
  }

  bool PingAdapter(void)
  {
    return m_libCec->PingAdapter();
  }

  bool StartBootloader(void)
  {
    return m_libCec->StartBootloader();
  }

  int GetMinLibVersion(void)
  {
    return m_libCec->GetMinLibVersion();
  }

  int GetLibVersionMajor(void)
  {
    return m_libCec->GetLibVersionMajor();
  }

  int GetLibVersionMinor(void)
  {
    return m_libCec->GetLibVersionMinor();
  }

  CecLogMessage ^ GetNextLogMessage(void)
  {
    cec_log_message msg;
    if (m_libCec->GetNextLogMessage(&msg))
    {
      return gcnew CecLogMessage(gcnew String(msg.message), (CecLogLevel)msg.level, msg.time);
    }

    return gcnew CecLogMessage();
  }

  CecKeypress ^ GetNextKeypress(void)
  {
    cec_keypress key;
    if (m_libCec->GetNextKeypress(&key))
    {
      return gcnew CecKeypress(key.keycode, key.duration);
    }

    return gcnew CecKeypress();
  }

  CecCommand ^ GetNextCommand(void)
  {
    cec_command command;
    if (m_libCec->GetNextCommand(&command))
    {
      // TODO parameters
      return gcnew CecCommand((CecLogicalAddress)command.initiator, (CecLogicalAddress)command.destination, command.ack == 1 ? true : false, command.eom == 1 ? true : false, command.opcode, command.transmit_timeout);
    }

    return gcnew CecCommand();
  }

  bool Transmit(CecCommand ^ command)
  {
    cec_command ccommand;
    cec_command::Format(ccommand, (cec_logical_address)command->Initiator, (cec_logical_address)command->Destination, (cec_opcode)command->Opcode);
    ccommand.transmit_timeout = command->TransmitTimeout;
    ccommand.eom              = command->Eom;
    ccommand.ack              = command->Ack;
    for (unsigned int iPtr = 0; iPtr < command->Parameters->Size; iPtr++)
      ccommand.parameters.PushBack(command->Parameters->Data[iPtr]);

    return m_libCec->Transmit(ccommand);
  }

  bool SetLogicalAddress(CecLogicalAddress logicalAddress)
  {
    return m_libCec->SetLogicalAddress((cec_logical_address) logicalAddress);
  }

  bool SetPhysicalAddress(int16_t physicalAddress)
  {
    return m_libCec->SetPhysicalAddress(physicalAddress);
  }

  bool PowerOnDevices(CecLogicalAddress logicalAddress)
  {
    return m_libCec->PowerOnDevices((cec_logical_address) logicalAddress);
  }

  bool StandbyDevices(CecLogicalAddress logicalAddress)
  {
    return m_libCec->StandbyDevices((cec_logical_address) logicalAddress);
  }

  bool PollDevice(CecLogicalAddress logicalAddress)
  {
    return m_libCec->PollDevice((cec_logical_address) logicalAddress);
  }

  bool SetActiveSource(CecDeviceType type)
  {
    return m_libCec->SetActiveSource((cec_device_type) type);
  }

  bool SetDeckControlMode(CecDeckControlMode mode, bool sendUpdate)
  {
    return m_libCec->SetDeckControlMode((cec_deck_control_mode) mode, sendUpdate);
  }

  bool SetDeckInfo(CecDeckInfo info, bool sendUpdate)
  {
    return m_libCec->SetDeckInfo((cec_deck_info) info, sendUpdate);
  }

  bool SetInactiveView(void)
  {
    return m_libCec->SetInactiveView();
  }

  bool SetMenuState(CecMenuState state, bool sendUpdate)
  {
    return m_libCec->SetMenuState((cec_menu_state) state, sendUpdate);
  }

  bool SetOSDString(CecLogicalAddress logicalAddress, CecDisplayControl duration, String ^ message)
  {
    marshal_context ^ context = gcnew marshal_context();
    const char* strMessageC = context->marshal_as<const char*>(message);

    bool bReturn = m_libCec->SetOSDString((cec_logical_address) logicalAddress, (cec_display_control) duration, strMessageC);

    delete context;
    return bReturn;
  }

  bool SwitchMonitoring(bool enable)
  {
    return m_libCec->SwitchMonitoring(enable);
  }

  CecVersion GetDeviceCecVersion(CecLogicalAddress logicalAddress)
  {
    return (CecVersion) m_libCec->GetDeviceCecVersion((cec_logical_address) logicalAddress);
  }

  String ^ GetDeviceMenuLanguage(CecLogicalAddress logicalAddress)
  {
    cec_menu_language lang;
    if (m_libCec->GetDeviceMenuLanguage((cec_logical_address) logicalAddress, &lang))
    {
      return gcnew String(lang.language);
    }

    return gcnew String("");
  }

  uint64_t GetDeviceVendorId(CecLogicalAddress logicalAddress)
  {
    return m_libCec->GetDeviceVendorId((cec_logical_address) logicalAddress);
  }

  CecPowerStatus GetDevicePowerStatus(CecLogicalAddress logicalAddress)
  {
    return (CecPowerStatus) m_libCec->GetDevicePowerStatus((cec_logical_address) logicalAddress);
  }

private:
   ICECAdapter *     m_libCec;
};
