#include "Interprocess.h"

#include <iostream>
#include <string>
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
        Interprocess(
            boost::interprocess::managed_shared_memory& segment,
            ShmemAllocator& alloc,
            boost::interprocess::offset_ptr<MySHMMap> map
        ) : _segment(segment), _alloc(alloc), _map(map){}

        boost::interprocess::managed_shared_memory& _segment;
        ShmemAllocator& _alloc;
        boost::interprocess::offset_ptr<MySHMMap>& _map;
};

int main() {
    boost::interprocess::managed_shared_memory segment(
        boost::interprocess::open_or_create,
        "ambfInterprocess",
        65536
    );

    ShmemAllocator alloc(segment.get_segment_manager());

    boost::interprocess::offset_ptr<MySHMMap> map = segment.find<MySHMMap>("MySHMMapName").first;

    Interprocess interprocess(segment, alloc, map);
    bool running = true;
    std::string input;
    MySHMMap::iterator iter;
    iter = interprocess._map->begin();
    while (running) {
        std::cout << "Enter stop to end process, otherwise, enter a number to move the scene on the x axis:" << std::endl;
        std::cin >> input;
        if (input == "stop") {
            running = false; 
        } else {
            interprocess._segment.find<MySHMMap>("MySHMMapName").first->find(0)->second = std::stof(input);
        }
    }
    return 0;
}