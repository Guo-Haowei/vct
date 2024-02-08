#pragma once
#include "core/io/archive.h"
#include "entity.h"

namespace vct {
class Scene;
}

namespace vct::ecs {

class IComponentManager {
    IComponentManager(const IComponentManager&) = delete;
    IComponentManager& operator=(const IComponentManager&) = delete;

public:
    IComponentManager() = default;
    virtual ~IComponentManager() = default;
    virtual void Clear() = 0;
    virtual void Copy(const IComponentManager& other) = 0;
    virtual void Merge(IComponentManager& other) = 0;
    virtual void Remove(const Entity& entity) = 0;
    virtual bool contains(const Entity& entity) const = 0;
    virtual size_t get_index(const Entity& entity) const = 0;
    virtual size_t get_count() const = 0;
    virtual Entity get_entity(size_t index) const = 0;
};

template<typename T>
class ComponentManager final : public IComponentManager {
public:
    ComponentManager(size_t capacity = 0) { Reserve(capacity); }

    void Reserve(size_t capacity) {
        if (capacity) {
            mComponentArray.reserve(capacity);
            mEntityArray.reserve(capacity);
            mLookup.reserve(capacity);
        }
    }

    virtual void Clear() override {
        mComponentArray.clear();
        mEntityArray.clear();
        mLookup.clear();
    }

    void Copy(const ComponentManager<T>& other) {
        Clear();
        mComponentArray = other.mComponentArray;
        mEntityArray = other.mEntityArray;
        mLookup = other.mLookup;
    }

    void Copy(const IComponentManager& other) override { Copy((ComponentManager<T>&)other); }

    void Merge(ComponentManager<T>& other) {
        const size_t reserved = get_count() + other.get_count();
        mComponentArray.reserve(reserved);
        mEntityArray.reserve(reserved);
        mLookup.reserve(reserved);

        for (size_t i = 0; i < other.get_count(); ++i) {
            Entity entity = other.mEntityArray[i];
            DEV_ASSERT(!contains(entity));
            mEntityArray.push_back(entity);
            mLookup[entity] = mComponentArray.size();
            mComponentArray.push_back(std::move(other.mComponentArray[i]));
        }

        other.Clear();
    }

    virtual void Merge(IComponentManager& other) override { Merge((ComponentManager<T>&)other); }

    // virtual void serialize(wi::Archive& archive, EntitySerializer& seri) = 0;
    // virtual void Component_Serialize(Entity entity, wi::Archive& archive, EntitySerializer& seri) = 0;
    virtual void Remove(const Entity& entity) override {
        vct::unused(entity);
        DEV_ASSERT(0);
    }

    // virtual void Remove_KeepSorted(Entity entity) = 0;
    // virtual void MoveItem(size_t index_from, size_t index_to) = 0;
    virtual bool contains(const Entity& entity) const override {
        if (mLookup.empty()) {
            return false;
        }
        return mLookup.find(entity) != mLookup.end();
    }

    inline T& get_component(size_t idx) {
        DEV_ASSERT(idx < mComponentArray.size());
        return mComponentArray[idx];
    }

    T* get_component(const Entity& entity) {
        if (!entity.IsValid() || mLookup.empty()) {
            return nullptr;
        }

        auto it = mLookup.find(entity);

        if (it == mLookup.end()) {
            return nullptr;
        }

        return &mComponentArray[it->second];
    }

    virtual size_t get_index(const Entity& entity) const override {
        if (mLookup.empty()) {
            return Entity::INVALID_INDEX;
        }

        const auto it = mLookup.find(entity);
        if (it == mLookup.end()) {
            return Entity::INVALID_INDEX;
        }

        return it->second;
    }

    inline virtual size_t get_count() const override { return mComponentArray.size(); }

    inline virtual Entity get_entity(size_t index) const override {
        DEV_ASSERT(index < mEntityArray.size());
        return mEntityArray[index];
    }

    T& create(const Entity& entity) {
        DEV_ASSERT(entity.IsValid());

        const size_t componentCount = mComponentArray.size();
        DEV_ASSERT(mLookup.find(entity) == mLookup.end());
        DEV_ASSERT(mEntityArray.size() == componentCount);
        DEV_ASSERT(mLookup.size() == componentCount);

        mLookup[entity] = componentCount;
        mComponentArray.emplace_back();
        mEntityArray.push_back(entity);
        return mComponentArray.back();
    }

    const std::vector<Entity>& GetEntityArray() const { return mEntityArray; }
    std::vector<Entity>& GetEntityArray() { return mEntityArray; }
    const std::vector<T>& get_component_array() const { return mComponentArray; }
    std::vector<T>& get_component_array() { return mComponentArray; }

    const T& operator[](size_t idx) const { return get_component(idx); }

    T& operator[](size_t idx) { return get_component(idx); }

    void serialize(Archive& archive) {
        size_t count;
        if (archive.is_write_mode()) {
            count = static_cast<uint32_t>(mComponentArray.size());
            archive << count;
            for (auto& component : mComponentArray) {
                component.serialize(archive);
            }
            for (auto& entity : mEntityArray) {
                entity.serialize(archive);
            }
        } else {
            Clear();
            archive >> count;
            mComponentArray.resize(count);
            mEntityArray.resize(count);
            for (size_t i = 0; i < count; ++i) {
                mComponentArray[i].serialize(archive);
            }
            for (size_t i = 0; i < count; ++i) {
                mEntityArray[i].serialize(archive);
                mLookup[mEntityArray[i]] = i;
            }
        }
    }

private:
    std::vector<T> mComponentArray;
    std::vector<Entity> mEntityArray;
    std::unordered_map<Entity, size_t> mLookup;
};

class ComponentLibrary {
public:
    struct LibraryEntry {
        std::unique_ptr<IComponentManager> mManager = nullptr;
        uint64_t mVersion = 0;
    };

    template<typename T>
    inline ComponentManager<T>& Register(const std::string& name, uint64_t version = 0) {
        DEV_ASSERT(mEntries.find(name) == mEntries.end());
        mEntries[name].mManager = std::make_unique<ComponentManager<T>>();
        mEntries[name].mVersion = version;
        return static_cast<ComponentManager<T>&>(*(mEntries[name].mManager));
    }

private:
    std::unordered_map<std::string, LibraryEntry> mEntries;

    friend class Scene;
};

}  // namespace vct::ecs
