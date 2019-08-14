#include <vector>
#include <iostream>
#include "depvars.h"
#include "includes/ind/view.h"
#include "includes/ind/utils.h"
#include "includes/ind/loaders.h"
#include "includes/ind/renderer.h"
#include "includes/ind/effects/congo.h"
// single header file
// Size should be 2^k. For the sake of compute shader.
static constexpr unsigned kWindowWidth = 1024;
static constexpr unsigned kWindowHeight = 1024;
static constexpr unsigned kDeltaTime = 20;

struct World {
	std::unique_ptr<std::vector<MeshDesc>> meshes;
	std::unique_ptr<QuadDesc> quad;
	std::unique_ptr<QuadDesc> quad_irrid;
	std::unique_ptr<std::vector<DirLightDesc>> lights;
	std::unique_ptr<std::vector<FboDesc>> fbos;
	std::unique_ptr<IndView> view;
	std::unique_ptr<Renderer> renderer;
};

void PrepGLQuad(QuadDesc * quad);
void PrepGLData(MeshDesc* mesh);
void PrepGLDataNTgt(MeshDesc* mesh);
void PrepGLData(std::vector<MeshDesc>* meshes);
void PrepGLLights(DirLightDesc* light);
void PrepGLLights(std::vector<DirLightDesc>* lights);
void PrepFboDesces(std::vector<FboDesc>* fbodesces);
void PrepFboDesc(FboDesc * fbodesc);
void LoadRes(World* world, std::string obj_path, std::string nrm_path);



