#include "SeparableBSSRDF.h"
#include "BSDF.h"
#include "SeparableBSSRDFAdapter.h"
#include "../primitive/Primitive.h"
#include "../scene/Scene.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace bxdf
{


core::color::Spectrum SeparableBSSRDF::Sample_S(const core::scene::Scene &scene, Float u1,
    const common::math::Vec2f &u2, common::tool::MemoryArena &arena,
    core::interaction::SurfaceInteraction *si, Float *pdf) const
{
    //ProfilePhase pp(Prof::BSSRDFSampling);
    core::color::Spectrum Sp = Sample_Sp(scene, u1, u2, arena, si, pdf);
    if (!Sp.IsBlack())
    {
        // Initialize material model at sampled surface interaction
        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
        si->bsdf->Add(ARENA_ALLOC(arena, SeparableBSSRDFAdapter)(this));
        si->wo = common::math::Vec3f(si->shading.n);
    }
    return Sp;
}

core::color::Spectrum SeparableBSSRDF::Sample_Sp(const core::scene::Scene &scene, Float u1,
    const common::math::Vec2f &u2, common::tool::MemoryArena &arena,
    core::interaction::SurfaceInteraction *pi, Float *pdf) const
{
    //ProfilePhase pp(Prof::BSSRDFEvaluation);
    // Choose projection axis for BSSRDF sampling
    common::math::Vec3f vx, vy, vz;
    if (u1 < FLOAT_INV_2)
    {
        vx = ss;
        vy = ts;
        vz = common::math::Vec3f(ns);
        u1 *= FLOAT_2;
    }
    else if (u1 < static_cast<Float>(0.75F))
    {
        // Prepare for sampling rays with respect to _ss_
        vx = ts;
        vy = common::math::Vec3f(ns);
        vz = ss;
        u1 = (u1 - FLOAT_INV_2) * static_cast<Float>(4.0F);
    }
    else
    {
        // Prepare for sampling rays with respect to _ts_
        vx = common::math::Vec3f(ns);
        vy = ss;
        vz = ts;
        u1 = (u1 - static_cast<Float>(0.75F)) * static_cast<Float>(4.0F);
    }

    // Choose spectral channel for BSSRDF sampling
    int ch = common::math::Clamp(static_cast<int>(u1 * core::color::Spectrum::SAMPLE_NUMBER)
        , 0, core::color::Spectrum::SAMPLE_NUMBER - 1);
    u1 = u1 * core::color::Spectrum::SAMPLE_NUMBER - ch;

    // Sample BSSRDF profile in polar coordinates
    Float r = Sample_Sr(ch, u2[0]);
    if (r < FLOAT_0)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    Float phi = FLOAT_2 * common::math::PI * u2[1];

    // Compute BSSRDF profile bounds and intersection height
    Float rMax = Sample_Sr(ch, static_cast<Float>(0.999F));
    if (r >= rMax)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    Float l = FLOAT_2 * std::sqrt(rMax * rMax - r * r);

    // Compute BSSRDF sampling ray segment
    core::interaction::Interaction base;
    base.p = po.p + r * (vx * std::cos(phi) + vy * std::sin(phi)) - l * vz * FLOAT_INV_2;
    base.time = po.time;
    common::math::Vec3f pTarget = base.p + l * vz;

    // Intersect BSSRDF sampling ray against the scene geometry

    // Declare _IntersectionChain_ and linked list
    struct IntersectionChain
    {
        core::interaction::SurfaceInteraction si;
        IntersectionChain *next = nullptr;
    };
    IntersectionChain *chain = ARENA_ALLOC(arena, IntersectionChain)();

    // Accumulate chain of intersections along ray
    IntersectionChain *ptr = chain;
    int nFound = 0;
    while (true)
    {
        common::math::Rayf r = base.SpawnRayTo(pTarget);
        if (common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0) == r.dir || !scene.Intersect(r, &ptr->si))
        {
            break;
        }

        base = ptr->si;
        // Append admissible intersection to _IntersectionChain_
        if (ptr->si.primitive->GetMaterial() == this->material)
        {
            IntersectionChain *next = ARENA_ALLOC(arena, IntersectionChain)();
            ptr->next = next;
            ptr = next;
            nFound++;
        }
    }

    // Randomly choose one of several intersections during BSSRDF sampling
    if (0 == nFound)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    int selected = common::math::Clamp(static_cast<int>(u1 * nFound), 0, nFound - 1);
    while (selected-- > 0)
    {
        chain = chain->next;
    }
    *pi = chain->si;

    // Compute sample PDF and return the spatial BSSRDF term $\Sp$
    *pdf = this->Pdf_Sp(*pi) / nFound;
    return this->Sp(*pi);
}

Float SeparableBSSRDF::Pdf_Sp(const core::interaction::SurfaceInteraction &pi) const
{
    // Express $\pti-\pto$ and $\bold{n}_i$ with respect to local coordinates at
    // $\pto$
    common::math::Vec3f d = po.p - pi.p;
    common::math::Vec3f dLocal(Dot(ss, d), Dot(ts, d), Dot(ns, d));
    common::math::Vec3f nLocal(Dot(ss, pi.n), Dot(ts, pi.n), Dot(ns, pi.n));

    // Compute BSSRDF profile radius under projection along each axis
    Float rProj[3] = {std::sqrt(dLocal.y * dLocal.y + dLocal.z * dLocal.z),
        std::sqrt(dLocal.z * dLocal.z + dLocal.x * dLocal.x),
        std::sqrt(dLocal.x * dLocal.x + dLocal.y * dLocal.y)};

    // Return combined probability from all BSSRDF sampling strategies
    Float pdf = FLOAT_0, axisProb[3] = {static_cast<Float>(0.25F), static_cast<Float>(0.25F), FLOAT_INV_2};
    Float chProb = FLOAT_1 / core::color::Spectrum::SAMPLE_NUMBER;
    for (int axis = 0; axis < 3; ++axis)
    {
        for (int ch = 0; ch < core::color::Spectrum::SAMPLE_NUMBER; ++ch)
        {
            pdf += Pdf_Sr(ch, rProj[axis]) * std::abs(nLocal[axis]) * chProb *
                axisProb[axis];
        }
    }
    return pdf;
}


}
}