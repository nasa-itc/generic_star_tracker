import sys
import glob
import time

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_STAR_TRACKER/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check, wait_check
    from nos3.generic_st_lib import *
except ImportError:
    pass


def run_generic_st_ast_test():
    ##
    ## This script tests the cFS component in an automated scenario.
    ## Currently this includes: 
    ##   Hardware failure
    ##   Hardware status reporting fault
    ##

    ##
    ## Hardware failure
    ##
    for n in range(GENERIC_ST_TEST_LOOP_COUNT):
        # Prepare
        generic_st_prepare_ast()

        # Disable sim and confirm device error counts increase
        dev_cmd_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")
        dev_cmd_err_cnt = tlm("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT")
        
        generic_st_sim_disable()
    
        time.sleep(1)
        
        cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_REQ_DATA")
        
        cmd("GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_REQ_HK")

        wait_check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT >= {dev_cmd_cnt}", 5)
        wait_check(f"GENERIC_STAR_TRACKER_DEBUG GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT >= {dev_cmd_err_cnt}", 5)

        # Enable sim and confirm return to nominal operation
        generic_st_sim_enable()
        
        time.sleep(1)
        
        confirm_generic_st_data_loop()
        