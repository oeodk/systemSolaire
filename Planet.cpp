#include "Planet.h"
#define _USE_MATH_DEFINES
#include <iostream>

Planet::Planet(float dist, float radius, float per_rotation, float per_revolution, const QString& texture_path, const Material& material)
    : dist_(dist), radius_(radius), per_rotation_(per_rotation), per_revoltion_(per_revolution), invert_light_(false)
{
    initializeOpenGLFunctions();
    revolution_angle_ = 0;
    rotation_angle_ = 0;
    material_ = material;

    loadTexture(texture_path);

    color_[0] = 1.f;
    color_[1] = 1.f;
    color_[2] = 1.f;
    if (vertices_.size() == 0)
    {
        createSphere(1, 20);
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
}

Planet::~Planet()
{
    glDeleteTextures(1, &text_location_);
}


void Planet::update(float timeElapsed)
{
    if(per_revoltion_ != 0)
    {
        revolution_angle_ = timeElapsed / per_revoltion_ * (2 * M_PI);
    }
    if (per_rotation_ != 0)
    {
        rotation_angle_ = timeElapsed / per_rotation_ * (2 * M_PI);
    }
}

QVector3D Planet::getPosition()
{
    return QVector3D(dist_ * std::cos(revolution_angle_), dist_ * std::sin(revolution_angle_), 0);
}

QMatrix4x4 Planet::getSelfRotation()
{
    QMatrix4x4 self_rotation;
    self_rotation.rotate(rotation_angle_, 0, 0, 1);
    return self_rotation;
}

void Planet::loadTexture(const QString& filename) 
{
    QImage image;
    if (!image.load(":/systemSolaire/" + filename))
    {
        // Gestion de l'erreur si le chargement de l'image a échoué
        qDebug() << "Texture " + filename + " could not load";
        return;
    }

    // Conversion de l'image en format compatible OpenGL
    image = image.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &text_location_);
    glBindTexture(GL_TEXTURE_2D, text_location_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);


    glBindTexture(GL_TEXTURE_2D, 0);
}

void Planet::createSphere(float radius, int segments)
{
    const float pi = 3.14159265358979323846;
    const float pi2 = 2.0 * pi;

    for (int lat = 0; lat <= segments; ++lat) {
        float theta = lat * pi / segments;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int lon = 0; lon <= segments; ++lon) {
            float phi = lon * pi2 / segments;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Coordonnées sphériques
            float x;
            float y;
            float z;
            if(false)//lon < segments / 8)
            {
                x = radius * sinTheta * cosPhi*2;
                y = radius * sinTheta * sinPhi*2;
                z = radius * cosTheta*2;
            }
            else
            {
                x = radius * sinTheta * cosPhi;
                y = radius * sinTheta * sinPhi;
                z = radius * cosTheta;
            }


            vertices_.push_back(x);
            vertices_.push_back(y);
            vertices_.push_back(z);

            vertices_.push_back(lon / float(segments));
            vertices_.push_back(lat / float(segments));
        }
    }

    // Génération des indices
    for (int lat = 0; lat < segments; ++lat) {
        for (int lon = 0; lon < segments; ++lon) {
            int current = lat * (segments + 1) + lon;
            int next = current + segments + 1;

            indices_.push_back(current);
            indices_.push_back(next);
            indices_.push_back(current + 1);
                   
            indices_.push_back(next);
            indices_.push_back(next + 1);
            indices_.push_back(current + 1);
        }
    }
}