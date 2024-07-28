#include <iostream>
#include <string>
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
        Interprocess(
            boost::interprocess::managed_shared_memory& segment,
            ShmemAllocator& alloc,
            boost::interprocess::offset_ptr<ShmemMap>& map
        ) : _segment(segment), _alloc(alloc), _map(map){}

        boost::interprocess::managed_shared_memory& _segment;
        ShmemAllocator& _alloc;
        boost::interprocess::offset_ptr<ShmemMap>& _map;
};

int main() {
    boost::interprocess::managed_shared_memory segment(
        boost::interprocess::open_or_create,
        "ambfInterprocess",
        65536
    );
    ShmemAllocator alloc(segment.get_segment_manager());
    boost::interprocess::offset_ptr<ShmemMap> map = segment.find<ShmemMap>("IPCTransformMap").first;
    Interprocess interprocess(segment, alloc, map);

    IPCTransform ipct{};
    while (true) {
        std::cout << "Enter 3 pos and 3 rot floats:" << std::endl;
        std::cin >> ipct.position[0] >> ipct.position[1] >> ipct.position[2]
                 >> ipct.rotation[0] >> ipct.rotation[1] >> ipct.rotation[2];
            interprocess._map->find(0)->second = ipct;
    }
}
