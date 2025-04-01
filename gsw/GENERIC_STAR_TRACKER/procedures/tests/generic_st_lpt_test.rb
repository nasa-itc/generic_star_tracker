require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"



initial_command_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
initial_device_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")
initial_device_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT")


cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_NOOP_CC")
cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_REQ_HK")

wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT == #{initial_device_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep 3

#enabling Star Tracker
initial_command_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
initial_device_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")

cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_ENABLE_CC")
cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_REQ_HK")

initial_device_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")
initial_device_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT")


wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT == #{initial_device_count}", 30)
wait_check("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep 1

st_Q0 = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q0")
st_Q1 = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q1")
st_Q2 = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q2")
st_Q3 = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q3")

sim42_Q0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0")
sim42_Q1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0")
sim42_Q2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0")
sim42_Q3 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA QN_0")

puts "sim q0 is #{sim42_Q0}"
puts "st q0 is #{st_Q0}"

tempsimq0 = (sim42_Q0).abs()
tempstq0 = (st_Q0).abs()

puts "tempsimq0 is #{tempsimq0}"
puts "tempstq0 is #{tempstq0}"

absQ0 = (tempsimq0 - tempstq0).abs()
diff = 0.025
puts " absq0 is: #{absQ0}"

sleep 1
# wait_check_expression("#{absQ0} >= #{diff}", 15)
wait_check_tolerance("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DATA_TLM STAR_TRACKER_Q0", sim42_Q0, diff, 15)



# truth_42_alpha = -Math.atan2(svb2, svb0)
# truth_42_beta = Math.atan2(svb1, svb0)

# truth_42_alpha_diff = (fss_alpha - truth_42_alpha).abs()
# truth_42_beta_diff = (fss_beta - truth_42_beta).abs()
# diff_margin = 0.025

# wait_check("GENERIC_FSS GENERIC_FSS_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
# wait_check("GENERIC_FSS GENERIC_FSS_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
# if fss_error == 0
#   wait_check_expression("truth_42_alpha_diff <= diff_margin # #{truth_42_alpha_diff} <= #{diff_margin}", 15)

sleep 3
# cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_DISABLE_CC")

# cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_RST_COUNTERS_CC")




# initial_command_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_COUNT")
# initial_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM CMD_ERR_COUNT")
# initial_device_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_COUNT")
# initial_device_error_count = tlm("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_HK_TLM DEVICE_ERR_COUNT")