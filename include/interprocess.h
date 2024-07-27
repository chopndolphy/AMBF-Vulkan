#pragma once

#include <boost/interprocess/shared_memory_object.hpp>

boost::interprocess::shared_memory_object sharedMem(boost::interprocess::open_or_create, "shared memory", boost::interprocess::read_write);





