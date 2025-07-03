// ======================================================================
// \title  Generic_star_tracker.cpp
// \author jstar
// \brief  cpp file for Generic_star_tracker component implementation class
// ======================================================================

#include "st_src/Generic_star_tracker.hpp"
#include "FpConfig.hpp"
#include <Fw/Log/LogString.hpp>

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  Generic_star_tracker ::
    Generic_star_tracker(const char* const compName) :
      Generic_star_trackerComponentBase(compName)
  {
    int32_t status = OS_SUCCESS;

    HkTelemetryPkt.CommandCount = 0;
    HkTelemetryPkt.CommandErrorCount = 0;
    HkTelemetryPkt.DeviceCount = 0;
    HkTelemetryPkt.DeviceErrorCount = 0;
    HkTelemetryPkt.DeviceEnabled = GENERIC_ST_DEVICE_DISABLED;
     /* Open device specific protocols */
    Generic_star_trackerUart.deviceString = GENERIC_STAR_TRACKER_CFG_STRING;
    Generic_star_trackerUart.handle = GENERIC_STAR_TRACKER_CFG_HANDLE;
    Generic_star_trackerUart.isOpen = PORT_CLOSED;
    Generic_star_trackerUart.baud = GENERIC_STAR_TRACKER_CFG_BAUDRATE_HZ;
    status = uart_init_port(&Generic_star_trackerUart);
     if (status == OS_SUCCESS)
    {
        printf("UART device %s configured with baudrate %d \n", Generic_star_trackerUart.deviceString, Generic_star_trackerUart.baud);
    }
    else
    {
        printf("UART device %s failed to initialize! \n", Generic_star_trackerUart.deviceString);
    }

    status = uart_close_port(&Generic_star_trackerUart);

  }

  Generic_star_tracker ::
    ~Generic_star_tracker()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  // void Generic_star_tracker ::
  //   TODO_cmdHandler(
  //       FwOpcodeType opCode,
  //       U32 cmdSeq
  //   )
  // {
  //   // TODO
  //   this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  // }

  // CMD_NOOP
  void Generic_star_tracker :: NOOP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    int32_t status = OS_SUCCESS;
    
    status = GENERIC_STAR_TRACKER_CommandDevice(&Generic_star_trackerUart, GENERIC_STAR_TRACKER_DEVICE_NOOP_CMD, 0);
    if (status == OS_SUCCESS)
    {
      HkTelemetryPkt.CommandCount++;
      Fw::LogStringArg log_msg("Star Tracker NOOP command success\n");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }
    else
    {
      Fw::LogStringArg log_msg("Star Tracker NOOP command failed!\n");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }
    
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_ReportedComponentCount(Generic_star_trackerHK.DeviceCounter);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));
    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // GENERIC_STAR_TRACKER_RequestHK
  void Generic_star_tracker :: REQUEST_HOUSEKEEPING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {

    int32_t status = OS_SUCCESS;

    if(HkTelemetryPkt.DeviceEnabled == GENERIC_ST_DEVICE_ENABLED)
    {  
      HkTelemetryPkt.CommandCount++;
      status = GENERIC_STAR_TRACKER_RequestHK(&Generic_star_trackerUart, &Generic_star_trackerHK);
      if (status == OS_SUCCESS)
      {
        HkTelemetryPkt.DeviceCount++;
        Fw::LogStringArg log_msg("RequestHK command success\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
      else
      {
        HkTelemetryPkt.DeviceErrorCount++;
        Fw::LogStringArg log_msg("RequestHK command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("RequestHK command failed, device disabled!\n");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }

    this->tlmWrite_ReportedComponentCount(Generic_star_trackerHK.DeviceCounter);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));

    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  //GENERIC_STAR_TRACKER_RequestData
  void Generic_star_tracker :: REQUEST_DATA_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    int32_t status = OS_SUCCESS;

    if(HkTelemetryPkt.DeviceEnabled == GENERIC_ST_DEVICE_ENABLED)
    {
      HkTelemetryPkt.CommandCount++;
      status = GENERIC_STAR_TRACKER_RequestData(&Generic_star_trackerUart, &Generic_star_trackerData);
      if(status < 0)
      {
        HkTelemetryPkt.DeviceCount++;
        Fw::LogStringArg log_msg("ST_RequestData: Command Failed");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
      else
      {
        HkTelemetryPkt.DeviceErrorCount++;
        Fw::LogStringArg log_msg("ST_RequestData: Success!");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("Request Data Failed, Device Disabled!");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }
    
    this->tlmWrite_ReportedComponentCount(Generic_star_trackerHK.DeviceCounter);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_Q0_Data(Generic_star_trackerData.Q0);
    this->tlmWrite_Q1_Data(Generic_star_trackerData.Q1);
    this->tlmWrite_Q2_Data(Generic_star_trackerData.Q2);
    this->tlmWrite_Q3_Data(Generic_star_trackerData.Q3);
    this->tlmWrite_IsValid(Generic_star_trackerData.IsValid);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void Generic_star_tracker :: DISABLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    int32_t status = OS_SUCCESS;

    if (HkTelemetryPkt.DeviceEnabled == GENERIC_ST_DEVICE_ENABLED)
    {
        HkTelemetryPkt.CommandCount++;
        status = uart_close_port(&Generic_star_trackerUart);
        if (status == OS_SUCCESS)
        {
          HkTelemetryPkt.DeviceCount++;
          HkTelemetryPkt.DeviceEnabled = GENERIC_ST_DEVICE_DISABLED;
          Fw::LogStringArg log_msg("Successfully Disabled Star Tracker!");
          this->log_ACTIVITY_HI_TELEM(log_msg);
        }
        else
        {
          HkTelemetryPkt.DeviceErrorCount++;
          Fw::LogStringArg log_msg("Disable Failed, UART init fail");
          this->log_ACTIVITY_HI_TELEM(log_msg);
        }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("Disable Failed, Already Disabled");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }

    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void Generic_star_tracker :: ENABLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    int32_t status = OS_SUCCESS;

    if (HkTelemetryPkt.DeviceEnabled == GENERIC_ST_DEVICE_DISABLED)
    {
        HkTelemetryPkt.CommandCount++;

        Generic_star_trackerUart.deviceString  = GENERIC_STAR_TRACKER_CFG_STRING;
        Generic_star_trackerUart.handle        = GENERIC_STAR_TRACKER_CFG_HANDLE;
        Generic_star_trackerUart.isOpen        = PORT_CLOSED;
        Generic_star_trackerUart.baud          = GENERIC_STAR_TRACKER_CFG_BAUDRATE_HZ;
        Generic_star_trackerUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&Generic_star_trackerUart);
        if (status == OS_SUCCESS)
        {
          HkTelemetryPkt.DeviceCount++;
          HkTelemetryPkt.DeviceEnabled = GENERIC_ST_DEVICE_ENABLED;
          Fw::LogStringArg log_msg("Successfully Enabled Star Tracker!");
          this->log_ACTIVITY_HI_TELEM(log_msg);
        }
        else
        {
          HkTelemetryPkt.DeviceErrorCount++;
          Fw::LogStringArg log_msg("Enable Failed, UART init fail");
          this->log_ACTIVITY_HI_TELEM(log_msg);
        }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("Enable Failed, Already Enabled");
      this->log_ACTIVITY_HI_TELEM(log_msg);
    }

    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void Generic_star_tracker :: RESET_COUNTERS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    HkTelemetryPkt.CommandCount = 0;
    HkTelemetryPkt.CommandErrorCount = 0;
    HkTelemetryPkt.DeviceCount = 0;
    HkTelemetryPkt.DeviceErrorCount = 0;

    Fw::LogStringArg log_msg("Reset Counters command successful!");
    this->log_ACTIVITY_HI_TELEM(log_msg);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  inline Generic_star_tracker_ActiveState Generic_star_tracker :: get_active_state(uint8_t DeviceEnabled)
  {
    Generic_star_tracker_ActiveState state;

    if(DeviceEnabled == GENERIC_ST_DEVICE_ENABLED)
    {
      state.e = Generic_star_tracker_ActiveState::ENABLED;
    }
    else
    {
      state.e = Generic_star_tracker_ActiveState::DISABLED;
    }

    return state;
  }

}
