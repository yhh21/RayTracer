#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace texture
{


template <typename T>
class Texture
{
public:
    virtual ~Texture()
    {}

    virtual T Evaluate(const interaction::SurfaceInteraction &si) const = 0;
};


}
}