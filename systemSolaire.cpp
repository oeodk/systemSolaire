#include "systemSolaire.h"


const unsigned int WIN_WIDTH_HEIGHT = 790;
const float MAX_DIMENSION = 50.0f;

systemSolaire::systemSolaire(QWidget* parent)
    : QOpenGLWidget(parent), rotationAngle_(0.0f), rotationAxis_(0.0f, 0.0f, 1.0f), cam_angle_(0)
{
    resize(WIN_WIDTH_HEIGHT, WIN_WIDTH_HEIGHT);

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

    loadShader(shadow_program_, "shadow_vertexshader.glsl", "shadow_fragmentshader.glsl", "shadow_geometryshader.glsl");

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
    planets_.push_back(new Planet(0.0f,  5.0f , 5.f      , 0.0f     , "textures/soleil.jpg", mat));
    planets_.push_back(new Planet(7.5f,  0.50f, 58.646f  , 87.969f  , "textures/mercure.jpg", mat));
    planets_.push_back(new Planet(10.0f, 0.90f, -243.018f, 224.701f , "textures/venus.jpg", mat));
    planets_.push_back(new Planet(13.0f, 0.90f, 0.997f   , 365.256f , "textures/terre.jpg", mat));
    planets_.push_back(new Planet(17.5f, 1.50f, 1.025f   , 686.960f , "textures/mars.jpg", mat));
    planets_.push_back(new Planet(27.0f, 3.00f, 0.413f   , 935.354f , "textures/jupiter.jpg", mat));
    planets_.push_back(new Planet(35.0f, 2.50f, 0.448f   , 1757.736f, "textures/saturne.jpg", mat));
    planets_.push_back(new Planet(40.5f, 1.50f, -0.718f  , 3687.150f, "textures/uranus.jpg", mat));
    planets_.push_back(new Planet(45.0f, 1.50f, 0.671f   , 6224.903f, "textures/neptune.jpg", mat));
    planets_.push_back(new Moon(1.5f, 0.3f, 0.671f, 50.f, "textures/lune.jpg", planets_[3], mat));

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
    u_planet_texture_ = glGetUniformLocation(shaderProgram_, "textureSampler");

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

    u_shadow_model_ = glGetUniformLocation(shadow_program_, "model");

    u_shadow_pos_ = glGetUniformLocation(shadow_program_, "offset");
    u_shadow_radius_ = glGetUniformLocation(shadow_program_, "radius");
    u_shadow_rotation_ = glGetUniformLocation(shadow_program_, "rotation");

    light_color_ = { 1.f,1.f,1.f };
    light_position_ = { 0.f, 0.f,0.f };

    
    // Framebuffer for Cubemap Shadow Map
    

    glGenFramebuffers(1, &pointShadowMapFBO_);

    // Texture for Cubemap Shadow Map FBO
    glGenTextures(1, &depthCubemap_);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Matrices needed for the light's perspective on all faces of the cubemap
    QMatrix4x4 shadowProj;
    shadowProj.perspective(90.0f, 1.0f, 0.1f, farPlane);
    QMatrix4x4 shadowTransforms[6];
    QMatrix4x4 tmp_view;
    QVector3D look_dir[] = {
        QVector3D(1.0, 0.0, 0.0),
        QVector3D(-1.0, 0.0, 0.0),
        QVector3D(0.0, 1.0, 0.0),
        QVector3D(0.0, -1.0, 0.0),
        QVector3D(0.0, 0.0, 1.0),
        QVector3D(0.0, 0.0, -1.0)
    };
    QVector3D top_dir[] = {
       QVector3D(0.0, -1.0, 0.0),
       QVector3D(0.0, -1.0, 0.0),
       QVector3D(0.0, 0.0, 1.0),
       QVector3D(0.0, 0.0, -1.0),
       QVector3D(0.0, -1.0, 0.0),
       QVector3D(0.0, -1.0, 0.0)
    };
    QVector3D lightPos(light_position_[0], light_position_[1], light_position_[2]);
    for (int i = 0; i < 6; i++)
    {
        tmp_view.setToIdentity();
        tmp_view.lookAt(lightPos, lightPos + look_dir[i], top_dir[i]);
        shadowTransforms[i] = shadowProj * tmp_view;
}
    // Export all matrices to shader
    glUseProgram(shadow_program_);
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[0]"), 1, GL_FALSE, shadowTransforms[0].data());
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[1]"), 1, GL_FALSE, shadowTransforms[1].data());
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[2]"), 1, GL_FALSE, shadowTransforms[2].data());
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[3]"), 1, GL_FALSE, shadowTransforms[3].data());
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[4]"), 1, GL_FALSE, shadowTransforms[4].data());
    glUniformMatrix4fv(glGetUniformLocation(shadow_program_, "shadowMatrices[5]"), 1, GL_FALSE, shadowTransforms[5].data());
    glUniform3f(glGetUniformLocation(shadow_program_, "lightPos"), lightPos.x(), lightPos.y(), lightPos.z());
    glUniform1f(glGetUniformLocation(shadow_program_, "farPlane"), farPlane);
    glUseProgram(0);

}

void systemSolaire::resizeGL(int w, int h)
{
    glViewport(0, 0, w * 1.25, h * 1.25);
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

    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO_);

    glClear(GL_DEPTH_BUFFER_BIT);
    drawPlanetsShadow();

    drawStars();
    drawPlanets();
    drawOrbits();

    auto err = glGetError();
    if (err != GL_NONE)
    {
        qDebug() << err;
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

void systemSolaire::loadShader(GLuint& program, const std::string& vertex_shader_path, const std::string& frag_shader_path, const std::string& geometry_shader_path)
{
    GLint success, vertexShader, fragmentShader, geometryShader;
    char infoLog[512];
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

    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    const std::string geometryShaderSource = loadShaderSource(geometry_shader_path);
    const char* gs = geometryShaderSource.c_str();
    glShaderSource(geometryShader, 1, &gs, nullptr);
    glCompileShader(geometryShader);
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
        qDebug() << "geo Shader Compilation Failed:" << infoLog;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glAttachShader(program, geometryShader);
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

void systemSolaire::drawPlanets()
{
    glUseProgram(shaderProgram_);
    for (const auto& planet : planets_)
    {
        glActiveTexture(GL_TEXTURE1);

        planet->bindTexture();
        glBindVertexArray(planet->getVao());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet->getEbo());
        glUniformMatrix4fv(u_model_, 1, GL_FALSE, model_.data());
        glUniformMatrix4fv(u_view_, 1, GL_FALSE, view_.data());
        glUniformMatrix4fv(u_projection_, 1, GL_FALSE, projection_.data());

        QVector3D pos = planet->getPosition();
        glUniform3f(u_planet_pos_, pos.x(), pos.y(), pos.z());
        glUniform1f(u_planet_radius_, planet->getRadius());
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

        glUniform1i(u_planet_texture_,1);


        glDrawElements(GL_TRIANGLES, planet->getIndicesSize(), GL_UNSIGNED_INT, 0);
        planet->releaseTexture();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
}

void systemSolaire::drawPlanetsShadow()
{

    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUseProgram(shadow_program_);
    for (int i = 1; i < planets_.size(); i++)
    {
        glBindVertexArray(planets_[i]->getVao());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planets_[i]->getEbo());
        glUniformMatrix4fv(u_shadow_model_, 1, GL_FALSE, model_.data());

        QVector3D pos = planets_[i]->getPosition();
        glUniform3f(u_shadow_pos_, pos.x(), pos.y(), pos.z());
        glUniform1f(u_shadow_radius_, planets_[i]->getRadius());
        glUniformMatrix4fv(u_shadow_rotation_, 1, GL_FALSE, planets_[i]->getSelfRotation().data());
      
        glDrawElements(GL_TRIANGLES, planets_[i]->getIndicesSize(), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Switch back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // Switch back to the default viewport
    glViewport(0, 0, width()*1.25, height()*1.25);
    

    // Bind the custom framebuffer
    // Specify the color of the background
    glClearColor(0.f,0.f,0.f, 1.0f);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing since it's disabled when drawing the framebuffer rectangle
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_);        
    glUniform1i(glGetUniformLocation(shaderProgram_, "shadowCubeMap"), 0);
    glUseProgram(0);
}

void systemSolaire::drawStars()
{
    glUseProgram(star_program_);
    glBindVertexArray(stars_.getVao());

    glUniformMatrix4fv(u_star_model_, 1, GL_FALSE, model_.data());
    glUniformMatrix4fv(u_star_view_, 1, GL_FALSE, view_.data());
    glUniformMatrix4fv(u_star_projection_, 1, GL_FALSE, projection_.data());

    glDrawArrays(GL_TRIANGLES, 0, stars_.getVerticesSize());

    glBindVertexArray(0);
}

void systemSolaire::drawOrbits()
{
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
}
