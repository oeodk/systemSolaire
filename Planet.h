#pragma once
#include <vector>
#include <cmath>
#include <QOpenGLExtraFunctions>
#include <QVector3D>
#include <QMatrix4x4>
#include "Material.h"

class Planet : protected QOpenGLExtraFunctions
{
protected :
	void createSphere(float radius, int segments);
	inline static std::vector<float> vertices_;
	inline static std::vector<unsigned int> indices_;

	inline static GLuint vao_, vbo_, ebo_;

	bool invert_light_;
	float dist_;
	float radius_;
	float per_rotation_, per_revoltion_;
	float revolution_angle_;
	float rotation_angle_;
	std::array<float,3> color_;
	Material material_;
public :
	Planet(float dist, float radius, float per_rotation, float per_revolution, const QVector3D& color, const Material& material);
	GLint getVao() { return vao_; }
	GLint getEbo() { return ebo_; }
	unsigned int getIndicesSize() { return indices_.size(); }
	virtual QVector3D getPosition();
	QMatrix4x4 getSelfRotation();
	float getRadius() { return radius_; }
	float* getColor() { return color_.data(); }
	void update(float timeElapsed);
	const Material& getMaterial() { return material_; }
	void invertLight() { invert_light_ = true; }
	bool getInvertLight() { return invert_light_; }
	float getDist() { return dist_; }
};

