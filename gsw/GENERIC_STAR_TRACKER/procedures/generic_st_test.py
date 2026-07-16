import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_STAR_TRACKER/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.script.suite import Suite, Group

try:
    from nos3.generic_st_lib import *
    from nos3.generic_st_app_test import run_generic_st_app_test
    from nos3.generic_st_device_test import run_generic_st_device_test
    from nos3.generic_st_ast_test import run_generic_st_ast_test
except ImportError:
    pass

class GENERIC_ST_Functional_Test(Group):
    def setup(self):
        safe_generic_st()

    def script_application(self):
        run_generic_st_app_test()

    def script_device(self):
        run_generic_st_device_test()

    def teardown(self):
        safe_generic_st()

class GENERIC_ST_Automated_Scenario_Test(Group):
    def setup(self):
        safe_generic_st()

    def script_ast(self):
        run_generic_st_ast_test()

    def teardown(self):
        safe_generic_st()

class Generic_st_Test(Suite):
    def __init__(self):
        super().__init__()
        self.add_group(GENERIC_ST_Functional_Test)
        self.add_group(GENERIC_ST_Automated_Scenario_Test)

    def setup(self):
        safe_generic_st()
  
    def teardown(self):
        safe_generic_st()