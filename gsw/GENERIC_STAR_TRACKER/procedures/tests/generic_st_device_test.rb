require 'cosmos'
require 'cosmos/script'
require "generic_st_lib.rb"

##
## This script tests the cFS component device functionality.
## Currently this includes: 
##   Enable / disable, control hardware communications
##   Configuration, reconfigure generic_star_tracker instrument register
##


##
## Enable / disable, control hardware communications
##
GENERIC_STAR_TRACKER_TEST_LOOP_COUNT.times do |n|
    # Get to known state
    safe_generic_star_tracker()

    # Manually command to disable when already disabled
    cmd_cnt = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
    cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DISABLE_CC")
    get_generic_star_tracker_hk()
    check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")

    # Enable
    enable_generic_star_tracker()

    # Confirm device counters increment without errors
    confirm_generic_star_tracker_data_loop()

    # Manually command to enable when already enabled
    cmd_cnt = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
    cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_ENABLE_CC")
    get_generic_star_tracker_hk()
    check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")

    # Reconfirm data remains as expected
    confirm_generic_star_tracker_data_loop()

    # Disable
    disable_generic_star_tracker()
end