require 'cosmos'
require 'cosmos/script'
require 'mission_lib.rb'

class ST_LPT < Cosmos::Test
  def setup
      enable_TO_and_verify()
  end

  def test_lpt
    start("tests/generic_st_lpt_test.rb")
  end

  def teardown
      cmd("CFS_RADIO TO_PAUSE_OUTPUT")
  end
end

class ST_CPT < Cosmos::Test
  def setup
      
  end

  def test_cpt
    start("tests/generic_st_cpt_test.rb")
  end

  def teardown
  end
end

class Generic_st_Test < Cosmos::TestSuite
  def initialize
      super()
      add_test('ST_CPT')
      add_test('ST_LPT')
  end

  def setup
  end
  
  def teardown
  end
end