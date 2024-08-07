
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/unordered_map.hpp>
#include <memory>
#include <functional>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <chrono>
#include <unordered_map>

namespace bip = boost::interprocess;
typedef bip::allocator<char, bip::managed_shared_memory::segment_manager> CharAllocator;
typedef bip::basic_string<char, std::char_traits<char>, CharAllocator> ShmemString;
typedef ShmemString HashKeyType;
struct Transform {
    float array[16] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
};

typedef Transform HashMappedType;
typedef std::pair<const ShmemString, Transform> HashValueType;
typedef bip::allocator<HashValueType, bip::managed_shared_memory::segment_manager> HashMemAllocator;
typedef boost::unordered_map<HashKeyType, HashMappedType, boost::hash<HashKeyType>, std::equal_to<HashKeyType>, HashMemAllocator> HashMap;
class Interprocess {
    public:
        Interprocess(bip::managed_shared_memory& segment, bip::offset_ptr<HashMap>& map)
        : _segment(segment), _map(map) {}
        bip::managed_shared_memory& _segment;
        bip::offset_ptr<HashMap>& _map;
};

int main() {
    boost::interprocess::managed_shared_memory segment(
        boost::interprocess::open_or_create,
        "ambfInterprocess",
        65536
    );
    boost::interprocess::offset_ptr<HashMap> map = segment.find<HashMap>("HashMap").first;
    Interprocess interprocess(segment, map);
    Transform transform;
    ShmemString name("phantom", segment.get_allocator<ShmemString>());
    while (true) {
        auto start = std::chrono::system_clock::now();
        transform = interprocess._map->at(name);
        glm::mat4 trans = glm::make_mat4(transform.array);
        trans = glm::rotate(trans, glm::radians(0.01f), glm::vec3(0.0, 1.0, 0.0));
        memcpy(transform.array, glm::value_ptr(trans), sizeof(trans));
        interprocess._map->at(name) = transform;
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        sleep(0.006 - elapsed.count());
    }
}
