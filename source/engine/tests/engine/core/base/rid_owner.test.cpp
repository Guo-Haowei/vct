#include "core/base/rid_owner.h"

#include <latch>

namespace vct {

struct Object {
    Object() {
    }
    Object(int value) : value(value) {
    }
    ~Object() {
    }
    int value;
};

TEST(RIDAllocator, allocate_rid) {

    RIDAllocator<Object> allocator{ 512 };

    std::unordered_map<uint64_t, int> mapping;
    std::vector<RID> rids;

    auto create_objects = [&](int number) {
        for (int i = 0; i < number; ++i) {
            int random_number = rand();
            auto rid = allocator.make_rid(random_number);
            rids.emplace_back(rid);
            mapping[rid.get_id()] = random_number;
        }
    };

    auto remove_objects = [&](float probability) {
        int num_elements_removed = 0;
        rids.erase(std::remove_if(rids.begin(), rids.end(), [&](const RID& rid) {
                       const float f = (float)rand() / (float)RAND_MAX;
                       const bool removed = f < probability;
                       if (removed) {
                           ++num_elements_removed;
                           allocator.free_rid(rid);
                       }
                       return removed;
                   }),
                   rids.end());

        // printf("... %d items removed, rid has size(%llu)\n", num_elements_removed, rids.size());
    };

    create_objects(100);
    remove_objects(0.2f);

    create_objects(100);
    remove_objects(0.5f);

    for (const auto& rid : rids) {
        auto it = mapping.find(rid.get_id());
        ASSERT_TRUE(it != mapping.end());
        auto t = allocator.get_or_null(rid);
        ASSERT_TRUE(t);
        EXPECT_EQ(t->value, it->second);
    }

    for (const auto& rid : rids) {
        allocator.free_rid(rid);
    }
}

TEST(RIDAllocator, concurrency) {
    RIDAllocator<Object, true> allocator{ 128 };
    allocator.set_description("concurrency test");
    const size_t num_workers = 16;
    std::latch all_worker_initialized{ num_workers };
    std::latch start_tasks{ 1 };
    std::latch all_worker_done{ num_workers };

    std::vector<std::thread> threads;
    std::unordered_map<uint64_t, int> mapping;
    std::mutex map_mutex;

    for (size_t i = 0; i < num_workers; ++i) {
        threads.emplace_back([&]() {
            all_worker_initialized.count_down();
            start_tasks.wait();

            // do the alloc free tasks
            int alloc_count1 = rand() % 65 + 64;  // [64, 128]
            int free_count1 = rand() % 65;        // [0, 64]
            int alloc_count2 = 300 - alloc_count1;
            int free_count2 = 100 - free_count1;
            assert(free_count1 <= alloc_count1);

            std::vector<std::tuple<RID, int>> cache;

            auto alloc = [&](int number) {
                for (int j = 0; j < number; ++j) {
                    const int value = rand();
                    RID rid = allocator.make_rid(value);
                    cache.push_back(std::make_tuple(rid, value));
                }
            };
            auto free = [&](int number) {
                for (int j = 0; j < number; ++j) {
                    auto [rid, value] = cache.back();
                    cache.pop_back();
                    allocator.free_rid(rid);
                }
            };

            alloc(alloc_count1);
            free(free_count1);
            alloc(alloc_count2);
            free(free_count2);

            all_worker_done.count_down();
            // collect info
            map_mutex.lock();
            for (auto [rid, value] : cache) {
                mapping[rid.get_id()] = value;
            }
            map_mutex.unlock();
        });
    }

    all_worker_initialized.wait();
    start_tasks.count_down();

    all_worker_done.wait();

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(allocator.get_rid_count(), 200 * static_cast<uint32_t>(num_workers));

    for (auto it : mapping) {
        const uint64_t id = it.first;
        RID rid = RID::from_uint64(id);
        auto t = allocator.get_or_null(rid);
        ASSERT_TRUE(t);
        EXPECT_EQ(t->value, it.second);

        allocator.free_rid(rid);
    }
}

}  // namespace vct
