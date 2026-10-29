import sys
import glob
import time 

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_STAR_TRACKER/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check
    from nos3.generic_st_lib import *
except ImportError:
    pass


def run_generic_st_device_test():
    ##
    ## This script tests the cFS component device functionality.
    ## Currently this includes: 
    ##   Enable / disable, control hardware communications
    ##   Configuration, reconfigure generic_st instrument register
    ##


    ##
    ## Enable / disable, control hardware communications
    ##
    for n in range(GENERIC_ST_TEST_LOOP_COUNT):
        # Get to known state
        safe_generic_st()

        # Manually command to disable when already disabled
        cmd_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
        cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_DISABLE_CC")
        get_generic_st_hk()
        check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

        # Enable
        enable_generic_st()

        time.sleep(1)

        get_generic_st_data()
        get_generic_st_hk()

        # Confirm device counters increment without errors
        confirm_generic_st_data_loop()

        # Manually command to enable when already enabled
        cmd_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
        cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_ENABLE_CC")
        get_generic_st_hk()
        check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

        # Reconfirm data remains as expected
        confirm_generic_st_data_loop()

        time.sleep(1)

        # Disable
        disable_generic_st()
