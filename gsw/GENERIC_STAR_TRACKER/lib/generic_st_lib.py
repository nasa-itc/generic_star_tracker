# Library for GENERIC_STAR_TRACKER Target
import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_STAR_TRACKER/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check, wait_check_packet, wait_check_tolerance
    import time
except ImportError:
    pass

#
# Definitions
#
GENERIC_ST_CMD_SLEEP = 0.25
GENERIC_ST_RESPONSE_TIMEOUT = 5
GENERIC_ST_TEST_LOOP_COUNT = 1
GENERIC_ST_DEVICE_LOOP_COUNT = 5

#
# Functions
#
def get_generic_st_hk():
    cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_REQ_HK")
    wait_check_packet("GENERIC_STAR_TRACKER_DEBUG", "GENERIC_STAR_TRACKER_HK_TLM", 1, GENERIC_ST_RESPONSE_TIMEOUT)
    time.sleep(GENERIC_ST_CMD_SLEEP)

def get_generic_st_data():
    cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_REQ_DATA")
    wait_check_packet("GENERIC_STAR_TRACKER_DEBUG", "GENERIC_STAR_TRACKER_DATA_TLM", 1, GENERIC_ST_RESPONSE_TIMEOUT)
    time.sleep(GENERIC_ST_CMD_SLEEP)

def generic_st_cmd(command_string):
    count = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT") + 1

    if (count == 256):
        count = 0

    cmd(command_string)
    get_generic_st_hk()
    current = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
    if (current != count):
        # Try again
        cmd(command_string)
        get_generic_st_hk()
        current = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
        if (current != count):
            # Third times the charm
            cmd(command_string)
            get_generic_st_hk()
            current = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
            
    check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT >= {count}")

def enable_generic_st():
    # Send command
    generic_st_cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_ENABLE_CC")
    # Confirm
    check("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ENABLED == 'ENABLED'")

def disable_generic_st():
    # Send command
    generic_st_cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DISABLE_CC")
    # Confirm
    check("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ENABLED == 'DISABLED'")

def safe_generic_st():
    get_generic_st_hk()
    state = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ENABLED")
    if (state != "DISABLED"):
        disable_generic_st()

def confirm_generic_st_data():
    dev_cmd_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT")
    
    get_generic_st_data()
    # Note these checks assume default simulator configuration
    diff = 1 
    #Todo check values with lower margin
    #Margin is large, testing that Q values not equal to 0

    #Checking Q0
    st_Q0 = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q0")
    sim42_Q0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0")
    print(f"sim42_q0 is {sim42_Q0}")
    print(f"st_q0 is {st_Q0}")

    wait_check_tolerance("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q0", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0"), diff, 30)

    #Checking Q1
    st_Q1 = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q1")
    sim42_Q1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_1")
    print(f"sim42_q1 is {sim42_Q1}")
    print(f"st_q1 is {st_Q1}")

    wait_check_tolerance("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q1", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_1"), diff, 30)

    #Checking Q2
    st_Q2 = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q2")
    sim42_Q2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_2")
    print(f"sim42_q2 is {sim42_Q2}")
    print(f"st_q2 is {st_Q2}")

    wait_check_tolerance("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q2", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_2"), diff, 30)

    #Checking Q3
    st_Q3 = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q3")
    sim42_Q3 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_3")
    print(f"sim42_q3 is {sim42_Q3}")
    print(f"st_q3 is {st_Q3}")

    wait_check_tolerance("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q3", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_3"), diff, 30)

    get_generic_st_hk()
    check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT >= {dev_cmd_cnt}")
    check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT == {dev_cmd_err_cnt}")

def confirm_generic_st_data_loop():
    for n in range(GENERIC_ST_DEVICE_LOOP_COUNT):
        confirm_generic_st_data()

#
# Simulator Functions
#
def generic_st_prepare_ast():
    # Get to known state
    safe_generic_st()

    # Enable
    enable_generic_st()

    # Confirm data
    confirm_generic_st_data_loop()

def generic_st_sim_enable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_STAR_TRACKER_SIM_ENABLE")

def generic_st_sim_disable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_STAR_TRACKER_SIM_DISABLE")

def generic_st_sim_set_status(status):
    cmd(f"SIM_CMDBUS_BRIDGE GENERIC_STAR_TRACKER_SIM_SET_STATUS with STATUS {status}")