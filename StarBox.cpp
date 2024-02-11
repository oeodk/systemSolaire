#include "StarBox.h"

const float STAR_SIZE = 0.5;

void StarBox::initStar(float distance, int star_number)
{
	initializeOpenGLFunctions();


	int triangle_stars_num = star_number;
	int square_stars_num = star_number - triangle_stars_num;
	vertices_.reserve(3 * 3 * triangle_stars_num);
	//indices_.reserve(3 * triangle_stars_num);
	const float TRIANGLE_POINT_X[] = { 0,-0.866025,0.866025 };
	const float TRIANGLE_POINT_Y[] = { 1,-0.5,-0.5};
	for (int i = 0; i < triangle_stars_num; i++)
	{
		float center_1 = rand() % int(distance * 2) - distance;
		float center_2 = rand() % int(distance * 2) - distance;
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
			vertices_.push_back(-distance);
		}
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
			vertices_.push_back(distance);
		}
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(-distance);
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
		}
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(distance);
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
		}
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(-distance);
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
		}
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(distance);
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
		}
	}

	/*const float SQUARE_POINT_X[] = { 0,-0.866025,0.866025 };;
	const float SQUARE_POINT_Y[] = { 1,-0.5,-0.5 };
	for (int i = 0; i < square_stars_num; i++)
	{
		float center_1 = rand() % int(distance * 2) - distance;
		float center_2 = rand() % int(distance * 2) - distance;
		for (int j = 0; j < 3; j++)
		{
			vertices_.push_back(center_1 + STAR_SIZE * TRIANGLE_POINT_X[j]);
			vertices_.push_back(center_2 + STAR_SIZE * TRIANGLE_POINT_Y[j]);
			vertices_.push_back(-distance);
		}
	}*/

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
}