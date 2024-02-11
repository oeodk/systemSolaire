#include "Material.h"

void Material::setAmbient(const QVector3D& values)
{
	ambient[0] = values.x();
	ambient[1] = values.y();
	ambient[2] = values.z();
}

void Material::setDiffuse(const QVector3D& values)
{
	diffuse[0] = values.x();
	diffuse[1] = values.y();
	diffuse[2] = values.z();
}

void Material::setSpecular(const QVector3D& values)
{
	specular[0] = values.x();
	specular[1] = values.y();
	specular[2] = values.z();
}