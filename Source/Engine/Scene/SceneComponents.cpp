#pragma once
#include "SceneComponents.h"

#include "Core/Archive.h"
#include "Core/Check.h"

void HierarchyComponent::Serialize(Archive& archive)
{
    mParent.Serialize(archive);
}

void ObjectComponent::Serialize(Archive& archive)
{
    meshID.Serialize(archive);
}

void AnimationComponent::Serialize(Archive& archive)
{
    unused(archive);
    checkmsg("NOT IMPLMENTED");
}

void ArmatureComponent::Serialize(Archive& archive)
{
    unused(archive);
    checkmsg("NOT IMPLMENTED");
}

void RigidBodyPhysicsComponent::Serialize(Archive& archive)
{
    if (archive.IsWriteMode())
    {
        archive << shape;
        archive << param;
        archive << mass;
    }
    else
    {
        archive >> shape;
        archive >> param;
        archive >> mass;
    }
}
