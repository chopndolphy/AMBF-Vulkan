#include "interprocess.h"

Interprocess::Interprocess()
{
    boost::interprocess::shared_memory_object::remove("ambfInterprocess");

    _segment = boost::interprocess::managed_shared_memory(
        boost::interprocess::create_only,
        "ambfInterprocess",
        65536
    );

    _alloc = std::make_shared<ShmemAllocator>(_segment.get_segment_manager());

    _map = _segment.construct<MySHMMap>("MySHMMapName")(std::less<int>(), *_alloc);

    _map->insert(std::pair<const int, float>(0, (float)0));
}

void Interprocess::destroy()
{
    boost::interprocess::shared_memory_object::remove("ambfInterprocess");
}
