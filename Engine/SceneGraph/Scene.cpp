#include "Scene.hpp"

Entity* Scene::RegisterEntity( const char* name, uint32_t flag )
{
    Entity* entity = new Entity( name, flag );
    m_entities.emplace_back( std::shared_ptr<Entity>( entity ) );
    return entity;
}