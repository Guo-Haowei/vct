#include "Scene.hpp"

Entity* Scene::RegisterEntity( const char* name, uint32_t flag )
{
    Entity* entity = new Entity( name, flag );
    m_entities.emplace_back( std::shared_ptr<Entity>( entity ) );
    return entity;
}

const std::shared_ptr<Image>& Scene::GetImage( const std::string& key )
{
    auto it = m_images.find( key );
    if ( it == m_images.end() ) {
        return nullptr;
    }

    return it->second;
}