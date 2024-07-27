#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <memory>
#include <functional>
#include <utility>

typedef int KeyType;
typedef float MappedType;
typedef std::pair<const int, float> ValueType;
typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;
typedef boost::interprocess::map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MySHMMap;
class Interprocess {
    public:
        Interprocess();
        void destroy();
        boost::interprocess::managed_shared_memory _segment;
        std::shared_ptr<ShmemAllocator> _alloc;
        boost::interprocess::offset_ptr<MySHMMap> _map;
};