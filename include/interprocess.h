#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <memory>
#include <functional>
#include <utility>

struct IPCTransform {
    float position[3] = {0, 0, 0};
    float rotation[3] = {0, 0, 0};
};

typedef unsigned int KeyType;
typedef IPCTransform MappedType;
typedef std::pair<const unsigned int, IPCTransform> ValueType;
typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;
typedef boost::interprocess::map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> ShmemMap;
class Interprocess {
    public:
        Interprocess();
        void destroy();
        boost::interprocess::managed_shared_memory _segment;
        std::shared_ptr<ShmemAllocator> _alloc;
        boost::interprocess::offset_ptr<ShmemMap> _map;
};