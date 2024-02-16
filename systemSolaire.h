#pragma once

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLExtraFunctions>
#include <qopenglfunctions_3_3_compatibility.h>

#include <QTimer>
#include <QKeyEvent>

#include <fstream>
#include <sstream>
#include "Planet.h"
#include "Moon.h"
#include "StarBox.h"

class systemSolaire : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Compatibility
{
    Q_OBJECT

public:
    systemSolaire(QWidget *parent = nullptr);
    ~systemSolaire();
protected :
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    std::string loadShaderSource(const std::string& filename);

    void loadShader(GLuint& program, const std::string& vertex_shader_path, const std::string& frag_shader_path);
    void loadShader(GLuint& program, const std::string& vertex_shader_path, const std::string& frag_shader_path, const std::string& geometry_shader_path);
    void updatePlanets();
    void zoomIn();
    void zoomOut();
    void speedUp();
    void speedDown();
    void updataCamPos();

    void drawPlanets();
    void drawStars();
    void drawOrbits();
    void drawPlanetsShadow();

    // Timer d'animation
    float m_TimeElapsed = 0.0f;
    QTimer m_AnimationTimer;
    float rotation_speed_ = 1.f;

    std::vector<Planet*> planets_;
    StarBox stars_;
    std::array<float, 3> light_color_, light_position_;

    float rotationAngle_;
    QVector3D rotationAxis_;

    QMatrix4x4 view_;
    QVector3D cam_pos_;
    float cam_angle_;
    float cam_distance_ = 120;
    QMatrix4x4 projection_;
    QMatrix4x4 model_;

    GLuint shaderProgram_;

    GLint u_planet_pos_, u_planet_color_, u_planet_radius_, u_planet_rotation_, u_planet_texture_;
    GLint u_model_, u_view_, u_projection_;

    GLint u_material_ambient_, u_material_diffuse_, u_material_specular_, u_material_shininess_;
    GLint u_light_pos_, u_light_color_, u_cam_pos_;

    GLint u_invert_light_;

    const std::array<float, 3 * 4> TRAJECTORY_ = {
       -50, -50, 0,
        50, -50, 0,
        50,  50, 0,
       -50,  50, 0
    };

    const std::array<unsigned int, 6> INDICES_ = { 0,1,2,0,2,3 };
    GLuint trajectory_vao_, trajectory_vbo_, trajectory_ebo_;
    GLuint trajectory_program_;

    GLint u_traj_dist_, u_traj_center_, u_traj_model_, u_traj_view_, u_traj_projection_;
    float dist_[9];                        
    std::array<float,2> center_[9];

    GLuint star_program_;
    GLint u_star_model_, u_star_view_, u_star_projection_;


    const float farPlane = 100;
    const int shadowMapWidth = 1024, shadowMapHeight = 1024;
    unsigned int pointShadowMapFBO_;
    unsigned int depthCubemap_;
    GLint u_shadow_model_;
    GLint u_shadow_pos_, u_shadow_radius_, u_shadow_rotation_;

    GLuint shadow_program_;
};
