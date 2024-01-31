#pragma once
#include "SceneComponents.h"

#include "Archive.h"

void HierarchyComponent::Serialize(Archive& archive) { mParent.Serialize(archive); }

void ObjectComponent::Serialize(Archive& archive) { meshID.Serialize(archive); }

void AnimationComponent::Serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void ArmatureComponent::Serialize(Archive& archive) {
    unused(archive);
    CRASH_NOW_MSG("NOT IMPLMENTED");
}

void RigidBodyPhysicsComponent::Serialize(Archive& archive) {
    if (archive.IsWriteMode()) {
        archive << shape;
        archive << param;
        archive << mass;
    } else {
        archive >> shape;
        archive >> param;
        archive >> mass;
    }
}
