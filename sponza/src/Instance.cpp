#include <sponza/sponza.hpp>

using namespace sponza;

Instance::Instance(InstanceId i) : id(i)
{
}

InstanceId Instance::getId() const
{
    return id;
}

bool Instance::isStatic() const
{
    return is_static;
}

void Instance::setStatic(bool b)
{
    is_static = b;
}

MeshId Instance::getMeshId() const
{
    return mesh_id;
}

void Instance::setMeshId(MeshId id)
{
    mesh_id = id;
}

MaterialId Instance::getMaterialId() const
{
    return material_id;
}

void Instance::setMaterialId(MaterialId id)
{
    material_id = id;
}

Matrix4x3 Instance::getTransformationMatrix() const
{
    return xform;
}

void Instance::setTransformationMatrix(Matrix4x3 m)
{
    xform = m;
}
