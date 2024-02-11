#include "systemSolaire.h"


const unsigned int WIN_WIDTH_HEIGHT = 790;
const float MAX_DIMENSION = 50.0f;

systemSolaire::systemSolaire(QWidget* parent)
    : QOpenGLWidget(parent), rotationAngle_(0.0f), rotationAxis_(0.0f, 0.0f, 1.0f), cam_angle_(0)
{
    setFixedSize(WIN_WIDTH_HEIGHT, WIN_WIDTH_HEIGHT);

    // Connexion du timer
    connect(&m_AnimationTimer, &QTimer::timeout, [&] {
        m_TimeElapsed += (1.0f / 12.f) * rotation_speed_;
        updatePlanets();
        update();
        });

    m_AnimationTimer.setInterval(10);
    m_AnimationTimer.start();
}



systemSolaire::~systemSolaire()
{
    for (auto& planet : planets_)
    {
        delete planet;
    }
}

void systemSolaire::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    loadShader(shaderProgram_, "vertexshader.glsl", "fragmentshader.glsl");

    loadShader(trajectory_program_, "orbit_vertexshader.glsl", "orbit_fragmentshader.glsl");
    
    loadShader(star_program_, "star_vertexshader.glsl", "star_fragmentshader.glsl");

    glGenVertexArrays(1, &trajectory_vao_);
    glBindVertexArray(trajectory_vao_);

    glGenBuffers(1, &trajectory_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, trajectory_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * TRAJECTORY_.size(), TRAJECTORY_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &trajectory_ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trajectory_ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES_.size() * sizeof(unsigned int), INDICES_.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glBindVertexArray(0);

    cam_pos_ = QVector3D(0.0f,0.f, cam_distance_);
    view_.lookAt(cam_pos_, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));   
    projection_.setToIdentity();
    //projection_.ortho(-MAX_DIMENSION, MAX_DIMENSION, -MAX_DIMENSION * height() / static_cast<float>(width()), MAX_DIMENSION * height() / static_cast<float>(width()), 0.1, 150);

    projection_.perspective(45.0f, width() / float(height()), 0.1f, 300);
   
    model_.rotate(rotationAngle_, rotationAxis_);

    Material mat;
    mat.setAmbient(QVector3D(1.f, 1.f, 1.f));
    mat.setDiffuse(QVector3D(0.8f, 0.8f, 0.8f));
    mat.setSpecular(QVector3D(0.5f, 0.5f, 0.5f));
    mat.shininess = 32;

    planets_.reserve(9);
    planets_.push_back(new Planet(0.0f,  5.0f , 0.0f     , 0.0f     , QVector3D(240, 198, 29) , mat));
    planets_.push_back(new Planet(7.5f,  0.50f, 58.646f  , 87.969f  , QVector3D(200, 248, 242), mat));
    planets_.push_back(new Planet(10.0f, 0.90f, -243.018f, 224.701f , QVector3D(255, 255, 242), mat));
    planets_.push_back(new Planet(13.0f, 0.90f, 0.997f   , 365.256f , QVector3D(11 , 92 , 227), mat));
    planets_.push_back(new Planet(17.5f, 1.50f, 1.025f   , 686.960f , QVector3D(247, 115, 12) , mat));
    planets_.push_back(new Planet(27.0f, 3.00f, 0.413f   , 935.354f , QVector3D(253, 199, 145), mat));
    planets_.push_back(new Planet(35.0f, 2.50f, 0.448f   , 1757.736f, QVector3D(200, 196, 251), mat));
    planets_.push_back(new Planet(40.5f, 1.50f, -0.718f  , 3687.150f, QVector3D(198, 241, 245), mat));
    planets_.push_back(new Planet(45.0f, 1.50f, 0.671f   , 6224.903f, QVector3D(57 , 182, 247), mat));
    planets_.push_back(new Moon(1.5f, 0.3f, 0.671f, 50.f, QVector3D(240, 240, 240), planets_[3], mat));

    planets_[0]->invertLight();

    for (int i = 0; i < 9; i++)
    {
        dist_[i] = planets_[i + 1]->getDist() / 50.f;
        center_[i] = std::array<float, 2>{0, 0 };
    }

    stars_.initStar(120, 100);

    u_planet_pos_ = glGetUniformLocation(shaderProgram_, "offset");
    u_planet_radius_ = glGetUniformLocation(shaderProgram_, "radius");
    u_planet_color_ = glGetUniformLocation(shaderProgram_, "color");
    u_planet_rotation_ = glGetUniformLocation(shaderProgram_, "rotation");

    u_model_ = glGetUniformLocation(shaderProgram_, "model");
    u_view_ = glGetUniformLocation(shaderProgram_, "view");
    u_projection_ = glGetUniformLocation(shaderProgram_, "projection");

    u_material_ambient_ = glGetUniformLocation(shaderProgram_, "material.ambient");
    u_material_diffuse_ = glGetUniformLocation(shaderProgram_, "material.diffuse");
    u_material_specular_ = glGetUniformLocation(shaderProgram_, "material.specular");
    u_material_shininess_ = glGetUniformLocation(shaderProgram_, "material.shininess");
   
    u_light_pos_ = glGetUniformLocation(shaderProgram_, "lightPosition");
    u_light_color_ = glGetUniformLocation(shaderProgram_, "lightColor");
    u_cam_pos_ = glGetUniformLocation(shaderProgram_, "cam_pos");
    
    u_invert_light_ = glGetUniformLocation(shaderProgram_, "invert_light");
    
    u_traj_dist_ = glGetUniformLocation(trajectory_program_, "dist");
    u_traj_center_ = glGetUniformLocation(trajectory_program_, "center");

    u_traj_model_ = glGetUniformLocation(trajectory_program_, "model");
    u_traj_view_ = glGetUniformLocation(trajectory_program_, "view");
    u_traj_projection_ = glGetUniformLocation(trajectory_program_, "projection");
    
    u_star_model_ = glGetUniformLocation(star_program_, "model");
    u_star_view_ = glGetUniformLocation(star_program_, "view");
    u_star_projection_ = glGetUniformLocation(star_program_, "projection");

    light_color_ = { 1.f,1.f,1.f };
    light_position_ = { 0.f, 0.f,0.f };
}

void systemSolaire::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void systemSolaire::updatePlanets()
{
    for (auto& planet : planets_)
    {
        planet->update(m_TimeElapsed);
    }
    QVector3D moon_center = planets_[3]->getPosition() / 50.f;
    center_[8] = std::array<float, 2>{moon_center.x(), moon_center.y()};
}

void systemSolaire::zoomIn()
{
    cam_distance_ -= 30;
    if (cam_distance_ < 30)
    {
        cam_distance_ = 30;
    }
    cam_pos_ = QVector3D(0.0f, 0.f, cam_distance_);
    view_.setToIdentity();
    view_.lookAt(cam_pos_, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
}

void systemSolaire::zoomOut()
{
    cam_distance_ += 30;
    if (cam_distance_ > 120)
    {
        cam_distance_ = 120;
    }
    cam_pos_ = QVector3D(0.0f, 0.f, cam_distance_);
    view_.setToIdentity();
    view_.lookAt(cam_pos_, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
}

void systemSolaire::speedUp()
{
    rotation_speed_ += 3.f;
}

void systemSolaire::speedDown()
{
    rotation_speed_ -= 3;
    if (rotation_speed_ < 1)
    {
        rotation_speed_ = 1;
    }
}

void systemSolaire::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(star_program_);
    glBindVertexArray(stars_.getVao());

    glUniformMatrix4fv(u_star_model_, 1, GL_FALSE, model_.data());
    glUniformMatrix4fv(u_star_view_, 1, GL_FALSE, view_.data());
    glUniformMatrix4fv(u_star_projection_, 1, GL_FALSE, projection_.data());

    glDrawArrays(GL_TRIANGLES, 0, stars_.getVerticesSize());

    glBindVertexArray(0);
    glUseProgram(shaderProgram_);
    for(const auto& planet : planets_)
    {
        glBindVertexArray(planet->getVao());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet->getEbo());

        glUniformMatrix4fv(u_model_, 1, GL_FALSE, model_.data());
        glUniformMatrix4fv(u_view_, 1, GL_FALSE, view_.data());
        glUniformMatrix4fv(u_projection_, 1, GL_FALSE, projection_.data());

        QVector3D pos = planet->getPosition();
        glUniform3f(u_planet_pos_, pos.x(), pos.y(), pos.z());
        glUniform1f(u_planet_radius_,planet->getRadius());
        glUniformMatrix4fv(u_planet_rotation_, 1, GL_FALSE, planet->getSelfRotation().data());

        glUniform3fv(u_planet_color_, 1, planet->getColor());

        glUniform3fv(u_material_ambient_, 1, planet->getMaterial().ambient);
        glUniform3fv(u_material_diffuse_, 1, planet->getMaterial().diffuse);
        glUniform3fv(u_material_specular_, 1, planet->getMaterial().specular);
        glUniform1f(u_material_shininess_, planet->getMaterial().shininess);

        glUniform3f(u_cam_pos_, cam_pos_.x(), cam_pos_.y(), cam_pos_.z());

        glUniform3fv(u_light_pos_, 1, light_position_.data());
        glUniform3fv(u_light_color_, 1, light_color_.data());

        glUniform1i(u_invert_light_, planet->getInvertLight());

        glDrawElements(GL_TRIANGLES, planet->getIndicesSize(), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glUseProgram(trajectory_program_);
    glBindVertexArray(trajectory_vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trajectory_ebo_);

    glUniformMatrix4fv(u_traj_model_, 1, GL_FALSE, model_.data());
    glUniformMatrix4fv(u_traj_view_, 1, GL_FALSE, view_.data());
    glUniformMatrix4fv(u_traj_projection_, 1, GL_FALSE, projection_.data());

    glUniform1fv(u_traj_dist_, 9, dist_);
    glUniform2fv(u_traj_center_, 9, center_->data());

    glDrawElements(GL_TRIANGLES, INDICES_.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    glUseProgram(0);
}

void systemSolaire::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // Activation/Arret de l'animation
    case Qt::Key_Enter:
        if (m_AnimationTimer.isActive())
        {
            m_AnimationTimer.stop();
        }
        else
        {
            m_AnimationTimer.start();
        }
        break;
    case Qt::Key_Up:
        cam_angle_ += 1;
        updataCamPos();
        break;
    case Qt::Key_Down:
        cam_angle_ -= 1;
        updataCamPos();
        break;
    case Qt::Key_0:
        cam_angle_ = 0;
        updataCamPos();
        break;
    case Qt::Key_Right:
        zoomIn();
        break;
    case Qt::Key_Left:
        zoomOut();
        break;
    case Qt::Key_Plus:
        speedUp();
        break;
    case Qt::Key_Minus:
        speedDown();
        break;
    // Cas par defaut
    default:
        // Ignorer l'evenement
        event->ignore();
        return;
    
    }
    // Acceptation de l'evenement et mise a jour de la scene
    event->accept();
    update();
}

std::string systemSolaire::loadShaderSource(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        qDebug() << "Could not open the file: " + filename + '\n';
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void systemSolaire::loadShader(GLuint& program, const std::string& vertex_shader_path, const std::string& frag_shader_path)
{
    GLint success, vertexShader, fragmentShader;
    char* infoLog;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const std::string vertexShaderSource = loadShaderSource(vertex_shader_path);
    const char* vs = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vs, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        qDebug() << "Vertex Shader Compilation Failed:" << infoLog;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const std::string fragmentShaderSource = loadShaderSource(frag_shader_path);
    const char* fs = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fs, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        qDebug() << "frag Shader Compilation Failed:" << infoLog;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        qDebug() << "Shader Program Linking Failed:" << infoLog;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(0);
}

void systemSolaire::updataCamPos()
{
    //cam_pos_ = QVector3D(0.0f, CAM_DISTANCE_ * std::sin(cam_angle_), CAM_DISTANCE_ * std::cos(cam_angle_));
    //view_.lookAt(cam_pos_, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    model_.setToIdentity();
    model_.rotate(cam_angle_, 1.f, 0.f, 0.f);
}