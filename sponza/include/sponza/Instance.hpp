#pragma once

#include "sponza_fwd.hpp"

namespace sponza {

class Instance
{
public:
    Instance(InstanceId i);

    InstanceId getId() const;

    bool isStatic() const;

    MeshId getMeshId() const;

    MaterialId getMaterialId() const;

    Matrix4x3 getTransformationMatrix() const;

    void setStatic(bool b);
    void setMeshId(MeshId id);
    void setMaterialId(MaterialId id);
    void setTransformationMatrix(Matrix4x3 m);

private:
    InstanceId id{ 0 };
    MeshId mesh_id{ 0 };
    MaterialId material_id{ 0 };
    Matrix4x3 xform;
    bool is_static{ false };

};

} // end namespace sponza
