#pragma once

#include <QVector3D>

struct Material
{
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess;

	void setAmbient(const QVector3D& values);
	void setDiffuse(const QVector3D& values);
	void setSpecular(const QVector3D& values);
};