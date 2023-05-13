#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "glm/glm.hpp"

#include "helper/plane.h"
#include "helper/objmesh.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLuint vaoHandle;
    GLSLProgram prog;

    Plane plane; //plane surface
    std::unique_ptr<ObjMesh> mesh; //staff mesh
    std::unique_ptr<ObjMesh> EnemyMesh; //enemy mesh
    std::unique_ptr<ObjMesh> EnemyStemMesh; //Enemy stem mesh
    std::unique_ptr<ObjMesh> OrbMesh; //orb mesh
    std::unique_ptr<ObjMesh> ParticleMesh; //Particle mesh

    void compile();
    void setMatrices();

public:
    SceneBasic_Uniform();

    void initScene();
    void input(float);
    void update(float);
    void close();
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
