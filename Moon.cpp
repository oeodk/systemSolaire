#include "Moon.h"

Moon::Moon(float dist, float radius, float per_rotation, float per_revolution, const QString& texture_path, Planet* mother_planet, const Material& material)
	: Planet(dist, radius, per_rotation, per_revolution, texture_path, material)
{
	my_planet_ = mother_planet;
}

QVector3D Moon::getPosition()
{
	return Planet::getPosition() + my_planet_->getPosition();
}

