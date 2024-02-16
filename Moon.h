#pragma once
#include "Planet.h"
class Moon :
    public Planet
{
public :
    Moon(float dist, float radius, float per_rotation, float per_revolution, const QString& texture_path, Planet * mother_planet, const Material& material);
    QVector3D getPosition();
private :
    Planet* my_planet_;
};

