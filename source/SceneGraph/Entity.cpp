#include "Entity.hpp"

void Entity::GetCalculatedTransform( mat4& out ) const
{
    out = m_trans;
    for ( Entity* cursor = m_pParent; cursor; cursor = cursor->m_pParent ) {
        out = cursor->m_trans * out;
    }
}

void Entity::AddChild( Entity* child )
{
    m_children.push_back( child );
    child->m_pParent = this;
}