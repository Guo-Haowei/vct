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
    virtual void clear() = 0;
    virtual void copy(const IComponentManager& other) = 0;
    virtual void merge(IComponentManager& other) = 0;
    virtual void remove(const Entity& entity) = 0;
    virtual bool contains(const Entity& entity) const = 0;
    virtual size_t get_index(const Entity& entity) const = 0;
    virtual size_t get_count() const = 0;
    virtual Entity get_entity(size_t index) const = 0;
};

template<typename T>
class ComponentManager final : public IComponentManager {
public:
    ComponentManager(size_t capacity = 0) { reserve(capacity); }

    void reserve(size_t capacity) {
        if (capacity) {
            m_component_array.reserve(capacity);
            m_entity_array.reserve(capacity);
            m_lookup.reserve(capacity);
        }
    }

    void clear() override {
        m_component_array.clear();
        m_entity_array.clear();
        m_lookup.clear();
    }

    void copy(const ComponentManager<T>& other) {
        clear();
        m_component_array = other.m_component_array;
        m_entity_array = other.m_entity_array;
        m_lookup = other.m_lookup;
    }

    void copy(const IComponentManager& other) override {
        copy((ComponentManager<T>&)other);
    }

    void merge(ComponentManager<T>& other) {
        const size_t reserved = get_count() + other.get_count();
        m_component_array.reserve(reserved);
        m_entity_array.reserve(reserved);
        m_lookup.reserve(reserved);

        for (size_t i = 0; i < other.get_count(); ++i) {
            Entity entity = other.m_entity_array[i];
            DEV_ASSERT(!contains(entity));
            m_entity_array.push_back(entity);
            m_lookup[entity] = m_component_array.size();
            m_component_array.push_back(std::move(other.m_component_array[i]));
        }

        other.clear();
    }

    void merge(IComponentManager& other) override {
        merge((ComponentManager<T>&)other);
    }

    // virtual void serialize(wi::Archive& archive, EntitySerializer& seri) = 0;
    // virtual void Component_Serialize(Entity entity, wi::Archive& archive, EntitySerializer& seri) = 0;
    void remove(const Entity& entity) override {
        vct::unused(entity);
        DEV_ASSERT(0);
    }

    // virtual void Remove_KeepSorted(Entity entity) = 0;
    // virtual void MoveItem(size_t index_from, size_t index_to) = 0;
    bool contains(const Entity& entity) const override {
        if (m_lookup.empty()) {
            return false;
        }
        return m_lookup.find(entity) != m_lookup.end();
    }

    inline T& get_component(size_t idx) {
        DEV_ASSERT(idx < m_component_array.size());
        return m_component_array[idx];
    }

    T* get_component(const Entity& entity) {
        if (!entity.is_valid() || m_lookup.empty()) {
            return nullptr;
        }

        auto it = m_lookup.find(entity);

        if (it == m_lookup.end()) {
            return nullptr;
        }

        return &m_component_array[it->second];
    }

    size_t get_index(const Entity& entity) const override {
        if (m_lookup.empty()) {
            return Entity::INVALID_INDEX;
        }

        const auto it = m_lookup.find(entity);
        if (it == m_lookup.end()) {
            return Entity::INVALID_INDEX;
        }

        return it->second;
    }

    size_t get_count() const override { return m_component_array.size(); }

    Entity get_entity(size_t index) const override {
        DEV_ASSERT(index < m_entity_array.size());
        return m_entity_array[index];
    }

    T& create(const Entity& entity) {
        DEV_ASSERT(entity.is_valid());

        const size_t componentCount = m_component_array.size();
        DEV_ASSERT(m_lookup.find(entity) == m_lookup.end());
        DEV_ASSERT(m_entity_array.size() == componentCount);
        DEV_ASSERT(m_lookup.size() == componentCount);

        m_lookup[entity] = componentCount;
        m_component_array.emplace_back();
        m_entity_array.push_back(entity);
        return m_component_array.back();
    }

    const std::vector<Entity>& GetEntityArray() const { return m_entity_array; }
    std::vector<Entity>& GetEntityArray() { return m_entity_array; }
    const std::vector<T>& get_component_array() const { return m_component_array; }
    std::vector<T>& get_component_array() { return m_component_array; }

    const T& operator[](size_t idx) const { return get_component(idx); }

    T& operator[](size_t idx) { return get_component(idx); }

    void serialize(Archive& archive) {
        size_t count;
        if (archive.is_write_mode()) {
            count = static_cast<uint32_t>(m_component_array.size());
            archive << count;
            for (auto& component : m_component_array) {
                component.serialize(archive);
            }
            for (auto& entity : m_entity_array) {
                entity.serialize(archive);
            }
        } else {
            clear();
            archive >> count;
            m_component_array.resize(count);
            m_entity_array.resize(count);
            for (size_t i = 0; i < count; ++i) {
                m_component_array[i].serialize(archive);
            }
            for (size_t i = 0; i < count; ++i) {
                m_entity_array[i].serialize(archive);
                m_lookup[m_entity_array[i]] = i;
            }
        }
    }

private:
    std::vector<T> m_component_array;
    std::vector<Entity> m_entity_array;
    std::unordered_map<Entity, size_t> m_lookup;
};

class ComponentLibrary {
public:
    struct LibraryEntry {
        std::unique_ptr<IComponentManager> m_manager = nullptr;
        uint64_t m_version = 0;
    };

    template<typename T>
    inline ComponentManager<T>& register_manager(const std::string& name, uint64_t version = 0) {
        DEV_ASSERT(m_entries.find(name) == m_entries.end());
        m_entries[name].m_manager = std::make_unique<ComponentManager<T>>();
        m_entries[name].m_version = version;
        return static_cast<ComponentManager<T>&>(*(m_entries[name].m_manager));
    }

private:
    std::unordered_map<std::string, LibraryEntry> m_entries;

    friend class Scene;
};

}  // namespace vct::ecs
