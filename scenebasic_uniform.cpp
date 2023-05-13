#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <sstream>
#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"


#include "glm/glm.hpp"



#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
using glm::vec3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100)
{
	mesh = ObjMesh::load("media/Staff.obj",true);
	OrbMesh = ObjMesh::load("media/Orb.obj", true);
	EnemyMesh = ObjMesh::load("media/Enemy.obj", true);
	EnemyStemMesh = ObjMesh::load("media/EnemyStem.obj", true);
	ParticleMesh = ObjMesh::load("media/particle.obj", true);
}

float angle = 90.0f;
float angle2 = 0.0f;

float enemyx = 2.0f;
float enemyy = 0.0f;

int inputcooldown = 20;
int shootcooldown = 20;

float staffx = -0.56f;
float staffy = 0.0f;

float projectilex = -0.4f;
float projectiley = -10.0f;

float projectile2x = -0.4f;
float projectile2y = -10.0f;

bool firstprojectile = true;

bool gameover = false;
float t = 0.0f;

std::vector <vec3> particlepositions;
std::vector <vec3> particlespeeds;
std::vector <int> particlelifetimes;

void SceneBasic_Uniform::initScene()
{
	compile();
	glEnable(GL_DEPTH_TEST);

	view = glm::lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);
	float x, z;
	for (int i = 0; i < 3; i++)
	{
		std::stringstream name;
		name << "lights[" << i << "].Position";
		x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
		prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
	}

	//Difuse
	prog.setUniform("lights[0].Ld", vec3(0.0f, 0.0f, 1.1f));
	prog.setUniform("lights[1].Ld", vec3(0.0f, 1.1f, 0.0f));
	prog.setUniform("lights[2].Ld", vec3(1.1f, 0.0f, 0.0f));

	//Specular
	prog.setUniform("lights[0].Ls", vec3(0.0f, 0.0f, 0.8f));
	prog.setUniform("lights[1].Ls", vec3(0.0f, 0.8f, 0.0f));
	prog.setUniform("lights[2].Ls", vec3(0.8f, 0.0f, 0.0f));

	//Ambient
	prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.8f));
	prog.setUniform("lights[1].La", vec3(0.0f, 0.8f, 0.0f));
	prog.setUniform("lights[2].La", vec3(0.8f, 0.0f, 0.0f));


	prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Shininess", 100.0f);
	
}

void SceneBasic_Uniform::input(float inputnumber)
{
	switch ((int)inputnumber)
	{
	case 0:
		//angle += 5;


		break;
	case 1:
		if (shootcooldown == 0) {
			if (firstprojectile) {
				shootcooldown = 32;
				projectilex = staffx;
				projectilex += 0.1f;
				projectiley = staffy;
			}
			else {
				shootcooldown = 32;
				projectile2x = staffx;
				projectile2x += 0.1f;
				projectile2y = staffy;
			}
			firstprojectile = !firstprojectile;
		}
		//angle -= 5;
		break;
	case 2:
		//angle2 += 5;
		if (inputcooldown == 0 && staffy < 0.2f) {
			inputcooldown = 20;
			staffy += 0.2f;
		}
		break;
	default:
		if (inputcooldown == 0 && staffy > -0.2f) {
			inputcooldown = 20;
			staffy -= 0.2f;
		}
		//angle2 -= 5;
		break;
	}
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float inputnumber)//, float u
{
	angle += 5;
	if (angle > 360) {
		angle -= 360;
	}
	if (angle < 0) {
		angle += 360;
	}
	if (angle2 > 360) {
		angle2 -= 360;
	}
	if (angle2 < 0) {
		angle2 += 360;
	}
	if (enemyx > staffx) {
		enemyx -= 0.01f;
		if (enemyx - projectilex < 0.1f && enemyy == projectiley) {
			enemyx = 2.0f;
			enemyy = staffy;
		}
		if (enemyx - projectile2x < 0.1f && enemyy == projectile2y) {
			enemyx = 2.0f;
			enemyy = staffy;
		}
	}
	else {
		gameover = true;
	}
	particlepositions.push_back(vec3(staffx, staffy + 0.08f, 0.0f));

	particlespeeds.push_back(vec3(float((rand() % 9) - 4) / 400.0f, float((rand() % 9) - 4) / 400.0f, 0.0f));

	particlelifetimes.push_back(10);

	if (projectilex < 2.0f) {
		projectilex += 0.02f;
		if (enemyx - projectilex < 0.1f && enemyy == projectiley) {
			enemyx = 2.0f;
			enemyy = staffy;
		}

		particlepositions.push_back(vec3(projectilex, projectiley + 0.08f, 0.0f));

		particlespeeds.push_back(vec3(float((rand() % 9) - 4) / 400.0f, float((rand() % 9) - 4) / 400.0f, 0.0f));

		particlelifetimes.push_back(10);
	}
	else {
		projectiley = -10.1f;
	}
	if (projectile2x < 2.0f) {
		projectile2x += 0.02f;
		if (enemyx - projectile2x < 0.1f && enemyy == projectile2y) {
			enemyx = 2.0f;
			enemyy = staffy;
		}

		particlepositions.push_back(vec3(projectile2x, projectile2y + 0.08f, 0.0f));
		
		particlespeeds.push_back(vec3(float((rand() % 9) - 4) / 400.0f, float((rand() % 9) - 4) / 400.0f, 0.0f));

		particlelifetimes.push_back(10);
	}
	else {
		projectile2y = -10.1f;
	}
	if (inputcooldown > 0) {
		inputcooldown--;
	}
	if (shootcooldown > 0) {
		shootcooldown--;
	}
}
void SceneBasic_Uniform::render()
{
	t += 0.1f;
	glClearColor(0.2, 0.3, 0.4, 1);
	prog.setUniform("Material.Time", t);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_CLAMP);

	prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Shininess", 180.0f);
	prog.setUniform("Material.Colour", vec3(0.0f, 0.0f, 0.0f));
	model = mat4(1.0f);// Staff
	prog.setUniform("Material.animated", false);
	model = glm::translate(model, vec3(staffx, staffy, -0.4f));
	model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(angle2), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(0.1f));
	setMatrices();
	mesh->render();
	
	prog.setUniform("Material.Colour", vec3(0.6f, 0.0f, 0.0f));
	model = mat4(1.0f); // Enemy
	prog.setUniform("Material.animated", true);
	model = glm::translate(model, vec3(enemyx, enemyy + 0.08f, -0.4f));
	model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(angle2), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(0.1f));
	setMatrices();
	EnemyMesh->render();

	prog.setUniform("Material.Colour", vec3(0.1f, 0.3f, 0.0f));
	model = mat4(1.0f); // Enemy Stem
	prog.setUniform("Material.animated", true);
	model = glm::translate(model, vec3(enemyx, enemyy + 0.14f, -0.4f));
	model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(angle2), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(0.08f));
	setMatrices();
	EnemyStemMesh->render();

	prog.setUniform("Material.Colour", vec3(0.0f, 0.3f, 0.0f));
	model = mat4(1.0f); // Projectile
	prog.setUniform("Material.animated", false);
	model = glm::translate(model, vec3(projectilex, projectiley + 0.08f, -0.4f));
	model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(angle2 + 90.0f), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(0.1f));
	setMatrices();
	OrbMesh->render();

	model = mat4(1.0f); // Projectile2
	model = glm::translate(model, vec3(projectile2x, projectile2y + 0.08f, -0.4f));
	model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(angle2 + 90.0f), vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, vec3(0.1f));
	setMatrices();
	OrbMesh->render();
	
	prog.setUniform("Material.Colour", vec3(0.1f, 0.3f, 0.0f));
	for (int counter = 0; counter < particlepositions.size(); counter += 1) {
		vec3 position = particlepositions.at(counter);
		model = mat4(1.0f); // Projectile2
		model = glm::translate(model, vec3(position.x, position.y, -0.4f));
		model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angle2 + 90.0f), vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, vec3(0.02f));
		setMatrices();
		ParticleMesh->render();
		particlepositions.at(counter) += particlespeeds.at(counter);
	}
	prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Ka", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Shininess", 180.0f);
	prog.setUniform("Material.Colour", vec3(0.0f, 0.0f, 0.0f));
	for (int counter = particlepositions.size()-1; counter >= 0; counter -= 1) {
		if (particlelifetimes.at(counter) <= 0) {
			particlelifetimes.erase(std::next(particlelifetimes.begin(), counter), std::next(particlelifetimes.begin(), counter + (int)1));
			particlepositions.erase(std::next(particlepositions.begin(), counter), std::next(particlepositions.begin(), counter + (int)1));
			particlespeeds.erase(std::next(particlespeeds.begin(), counter), std::next(particlespeeds.begin(), counter + (int)1));
		}
		else {
			particlelifetimes.at(counter) -= 1;
		}
	}

	prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
	prog.setUniform("Material.Shininess", 180.0f);
	prog.setUniform("Material.Colour", vec3(0.0f, 0.3f, 0.0f));

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, -0.6f, 0.0f));

	setMatrices();
	plane.render();


}
void SceneBasic_Uniform::setMatrices() {
	mat4 mv = projection * model;
	prog.setUniform("“ModelViewMatrix”","mv");

	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(90.0f), (float)w / h, 0.4f, 500.0f);
}
void SceneBasic_Uniform::close()
{
	particlepositions.clear();
	particlelifetimes.clear();
	particlespeeds.clear();
}
