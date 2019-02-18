#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace material
{


enum class TransportMode
{
    Radiance,
    Importance
};


class Material
{
public:

    virtual ~Material()
    {}

    virtual void ComputeScatteringFunctions(interaction::SurfaceInteraction *si,
        common::tool::MemoryArena &arena,
        TransportMode mode,
        bool allowMultipleLobes) const = 0;

    static void Bump(const std::shared_ptr<texture::Texture<Float>> &d,
        interaction::SurfaceInteraction *si);
};

}
}