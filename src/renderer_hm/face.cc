#include "face.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

void LoadRes(World* world, std::string obj_path, std::string nrm_path ) {

	// load scene meshes
	std::vector<std::string> obj_paths{
		obj_path
	};
	std::vector<std::string> txtr_paths{
		"",
	};
	std::vector<std::string> dds_paths{
		"",
	};
	std::vector<std::string> nrml_paths{
		nrm_path
	};
	

	world->meshes = std::make_unique<std::vector<MeshDesc>>(obj_paths.size());

	for (size_t i = 0; i < obj_paths.size(); i++) {
		ReadObjFile(obj_paths[i], &world->meshes->at(i));
		world->meshes->at(i).txtr_path = txtr_paths[i];
		world->meshes->at(i).dds_path = dds_paths[i];
		world->meshes->at(i).nrml_dstrb_path = nrml_paths[i];
		world->meshes->at(i).type = MeshType::Tri; 
	}
	PrepGLData(world->meshes.get());


	// load quad aligned to screen
	world->quad = std::make_unique<QuadDesc>();
	//ReadObjFile(std::string(kAssetDir_) + "/1/agi/quad.obj", &world->quad->mesh);
	world->quad.get()->mesh.txtr_path = "";
	PrepGLDataNTgt(&world->quad.get()->mesh);
	PrepGLQuad(world->quad.get());

	// load quad_irradiance to render irridance texture
	world->quad_irrid = std::make_unique<QuadDesc>();
	//ReadObjFile(std::string(kAssetDir_) + "/1/agi/quad.obj", &world->quad_irrid->mesh);
	world->quad_irrid.get()->mesh.txtr_path = "";
	PrepGLDataNTgt(&world->quad_irrid.get()->mesh);
	PrepGLQuad(world->quad_irrid.get());


	Bounds wbox{};
	for (auto &mesh : *world->meshes.get()) {
		auto box = Bounds{ mesh.ps };
		mesh.box = box;
		wbox.Extend(box);
	}

	world->view = std::make_unique<IndView>(kWindowWidth, kWindowHeight, glm::radians(60.f), wbox);

	world->lights = std::make_unique<std::vector<DirLightDesc>>(1);
	for (size_t i = 0; i < world->lights.get()->size(); i++) {
		world->lights->at(i).dir = glm::vec3(1, -1, 0);
		world->lights->at(i).color = glm::vec3(1.0f);
		world->lights->at(i).shadow_w = kWindowWidth*4;
		world->lights->at(i).shadow_h = kWindowHeight*4;
	}

	// prepare the light's geometry
	PrepGLLights(world->lights.get());
	//ReadObjFile(std::string(kAssetDir_) + "/1/agi/quad.obj", &world->lights.get()->at(0).mesh);
	world->lights.get()->at(0).mesh.type = MeshType::Tri;
	PrepGLDataNTgt(&world->lights.get()->at(0).mesh);


	world->renderer = std::make_unique<Renderer>(world->view.get(), world->lights.get());
	world->fbos = std::make_unique<std::vector<FboDesc>>(12 + 1); // the last is used to compute stretch map
	PrepFboDesces(world->fbos.get());

}



void PrepGLDataNTgt(MeshDesc* mesh) {

	glGenVertexArrays(1, &(mesh->handle));
	glGenBuffers(1, &(mesh->buf_ps));
	glGenBuffers(1, &(mesh->buf_ns));
	glGenBuffers(1, &(mesh->buf_ts));

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ps);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ps.size() * 3, mesh->ps.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ns);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ns.size() * 3, mesh->ns.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ts.size() * 2, mesh->ts.data(), GL_STATIC_DRAW);

	glBindVertexArray(mesh->handle);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ps);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ns);
	glVertexAttribPointer(
		1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ts);
	glVertexAttribPointer(
		2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0); // unbind

						  // load mesh texture
	if (mesh->txtr_path != "") {
		// texture
		glGenTextures(1, &mesh->txtr);
		glBindTexture(GL_TEXTURE_2D, mesh->txtr);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned short *data = stbi_load_16(mesh->txtr_path.c_str(), &width, &height, &nrChannels, 0);
		IndAssert(data != nullptr, "Load Texture Fails", __FILE__, __LINE__);
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
		}
		else if (nrChannels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_SHORT, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}


	// load normal map
	if (mesh->nrml_dstrb_path != "") {

		// texture
		glGenTextures(1, &mesh->nrml_dstrb_txtr);
		glBindTexture(GL_TEXTURE_2D, mesh->nrml_dstrb_txtr);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned char *data = stbi_load(mesh->nrml_dstrb_path.c_str(), &width, &height, &nrChannels, 0);
		IndAssert(data != nullptr, "Load Texture Fails", __FILE__, __LINE__);
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

	}

}








void PrepGLData(MeshDesc* mesh) {

	glGenVertexArrays(1, &(mesh->handle));
	glGenBuffers(1, &(mesh->buf_ps));
	glGenBuffers(1, &(mesh->buf_ns));
	glGenBuffers(1, &(mesh->buf_ts));
	glGenBuffers(1, &(mesh->buf_tgts));

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ps);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ps.size()*3, mesh->ps.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ns);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ns.size() * 3, mesh->ns.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_tgts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->tgts.size() * 3, mesh->tgts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->ts.size()*2, mesh->ts.data(), GL_STATIC_DRAW);

	glBindVertexArray(mesh->handle);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ps);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ns);
	glVertexAttribPointer(
		1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_ts);
	glVertexAttribPointer(
		2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buf_tgts);
	glVertexAttribPointer(
		3,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(3);




	glBindVertexArray(0); // unbind

	// load mesh texture
	if (mesh->txtr_path != "") {
		// texture
		glGenTextures(1, &mesh->txtr);
		glBindTexture(GL_TEXTURE_2D, mesh->txtr);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned short *data = stbi_load_16(mesh->txtr_path.c_str(), &width, &height, &nrChannels, 0);
		IndAssert(data != nullptr, "Load Texture Fails", __FILE__, __LINE__);
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
		}
		else if (nrChannels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_SHORT, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}


	// load normal map
	if (mesh->nrml_dstrb_path != "") {

		// texture
		glGenTextures(1, &mesh->nrml_dstrb_txtr);
		glBindTexture(GL_TEXTURE_2D, mesh->nrml_dstrb_txtr);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned char *data = stbi_load(mesh->nrml_dstrb_path.c_str(), &width, &height, &nrChannels, 0);
		IndAssert(data != nullptr, "Load Texture Fails", __FILE__, __LINE__);
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

	}

}


void PrepGLData(std::vector<MeshDesc>* meshes) {

	for (auto &mesh : *meshes) {
		PrepGLData(&mesh);
	}
}


void PrepGLLights(DirLightDesc* light) {

	glGenFramebuffers(1, &light->fbo);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, light->fbo);

	// create depth texture
	glGenTextures(1, &light->txture);
	glBindTexture(GL_TEXTURE_2D, light->txture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, light->shadow_w, light->shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->txture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}



void PrepGLLights(std::vector<DirLightDesc>* lights) {

	for (auto &light : *lights) {
		PrepGLLights(&light);
	}
}


void PrepGLQuad(QuadDesc * quad) {

	quad->render_w = kWindowWidth*4;
	quad->render_h = kWindowHeight*4;

	glGenFramebuffers(1, &quad->fbo);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, quad->fbo);

	// create depth texture
	glGenTextures(1, &quad->txture);
	glBindTexture(GL_TEXTURE_2D, quad->txture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, quad->render_w, quad->render_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quad->txture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	quad->mvp = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))
		*glm::mat4(glm::ortho(-1, +1, -1, 1, 1, 2));
}


void PrepFboDesces(std::vector<FboDesc>* fbodesces) {

	for (auto &fbodesc : *fbodesces) {
		PrepFboDesc(&fbodesc);
	}
}


void PrepFboDesc(FboDesc * fbodesc) {

	fbodesc->render_w = kWindowWidth*4;
	fbodesc->render_h = kWindowHeight*4;

	glGenFramebuffers(1, &fbodesc->fbo);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbodesc->fbo);


	 // create depth texture
	glGenTextures(1, &fbodesc->txture);
	glBindTexture(GL_TEXTURE_2D, fbodesc->txture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbodesc->render_w, fbodesc->render_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbodesc->txture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}