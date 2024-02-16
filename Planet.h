#pragma once
#include <vector>
#include <cmath>
#include <QOpenGLExtraFunctions>
#include <QVector3D>
//#include <QOpenGLTexture>
#include <QImage>
#include <QMatrix4x4>
#include "Material.h"

class Planet : protected QOpenGLExtraFunctions
{
protected :
	void createSphere(float radius, int segments);
	void loadTexture(const QString& filename);

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
	//QOpenGLTexture* texture_ = nullptr;
	GLuint text_location_;

public :
	Planet(float dist, float radius, float per_rotation, float per_revolution, const QString& texture_path, const Material& material);
	~Planet();
	GLint getVao() { return vao_; }
	GLint getEbo() { return ebo_; }
	unsigned int getIndicesSize() { return indices_.size(); }
	virtual QVector3D getPosition();
	QMatrix4x4 getSelfRotation();
	float getRadius() { return radius_; }
	float* getColor() { return color_.data(); }
	void bindTexture() { glBindTexture(GL_TEXTURE_2D, text_location_); }
	void releaseTexture() { glBindTexture(GL_TEXTURE_2D, 0); }
	GLuint getTextId() { return text_location_; }
	void update(float timeElapsed);
	const Material& getMaterial() { return material_; }
	void invertLight() { invert_light_ = true; }
	bool getInvertLight() { return invert_light_; }
	float getDist() { return dist_; }
};

