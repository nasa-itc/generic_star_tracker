#include <generic_star_tracker_shmem_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_star_trackerShmemDataProvider,"GENERIC_STAR_TRACKER_SHMEM_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_star_trackerShmemDataProvider::Generic_star_trackerShmemDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("Generic_star_trackerShmemDataProvider::Generic_star_trackerShmemDataProvider:  Constructor executed");
        const std::string shm_name = config.get("simulator.hardware-model.data-provider.shared-memory-name", "Blackboard");
        const size_t shm_size = sizeof(BlackboardData);
        bip::shared_memory_object shm(bip::open_or_create, shm_name.c_str(), bip::read_write);
        shm.truncate(shm_size);
        bip::mapped_region shm_region(shm, bip::read_write);
        _shm_region = std::move(shm_region); // don't le this go out of scope/get destroyed
        _blackboard_data = static_cast<BlackboardData*>(_shm_region.get_address());
        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
        _st = config.get("simulator.hardware-model.data-provider.star_tracker", 0);
    }

    boost::shared_ptr<SimIDataPoint> Generic_star_trackerShmemDataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_star_trackerShmemDataProvider::get_data_point:  Executed");

        boost::shared_ptr<Generic_star_trackerDataPoint> dp;
        {
            dp = boost::shared_ptr<Generic_star_trackerDataPoint>(
                new Generic_star_trackerDataPoint(_sc, _st, _blackboard_data->STValid, _blackboard_data->STqn));
        }
        return dp;
    }
}
