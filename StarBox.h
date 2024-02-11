#pragma once
#define _USE_MATH_DEFINES
#include <QOpenGLExtraFunctions>
#include <vector>
#include <cmath>

class StarBox : protected QOpenGLExtraFunctions
{
public :
	void initStar(float distance, int star_number);
	int getVao() { return vao_; }
	int getVerticesSize() { return vertices_.size(); }
private :
	std::vector<float> vertices_;
	std::vector<float> indices_;

	GLuint vao_, vbo_;
};

