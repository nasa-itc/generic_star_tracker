#ifndef NOS3_GENERIC_STAR_TRACKER_SHMEM_DATAPROVIDER_HPP
#define NOS3_GENERIC_STAR_TRACKER_SHMEM_DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <generic_star_tracker_data_point.hpp>
#include <sim_data_42socket_provider.hpp>
#include <blackboard_data.hpp>

namespace Nos3
{
    namespace bip = boost::interprocess;

    class Generic_star_trackerShmemDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_star_trackerShmemDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_star_trackerShmemDataProvider(void) {};
        Generic_star_trackerShmemDataProvider& operator=(const Generic_star_trackerShmemDataProvider&) {return *this;};

        int16_t _sc;  /* Which spacecraft number to parse out of 42 data */
        int16_t _st;  /* Which star tracker number to parse out of 42 data */

        bip::mapped_region _shm_region;
        BlackboardData*    _blackboard_data;
    };
}

#endif
