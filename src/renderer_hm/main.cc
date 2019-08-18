#include <iostream>

#include "face.h"
#include "depvars.h"
#include "includes/ind/window.h"
#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void parseArgs(int argc, char** argv);
void GameLoop(World &world, GLFWWindowDesc &window);
void HandleInput(GLFWWindowDesc *window, World *world);
template<typename T> void Rotate(std::vector<T> *ts);

std::string  kAssetDir;
std::string  kShaderDir;
std::string kObjPath;
std::string kNrmPath;
char* kShaderDir_;

int main(int argc, char** argv){

	parseArgs(argc, argv);

	GLFWWindowDesc window{ kWindowWidth,kWindowHeight,"face" ,true };

	World world;

	LoadRes(&world, kObjPath, kNrmPath);

	HandleInput(&window, &world);

	GameLoop(world, window);

}


void parseArgs(int argc, char** argv) {

	kShaderDir_ = "../src/shaders";
	kObjPath = std::string(argv[1]);
	kNrmPath = std::string(argv[2]);
	if(argc>3)
		kShaderDir_ = argv[3];
}


template<typename T>
void Rotate(std::vector<T> *ts) {
	for (T &t : *ts) {
		if (glm::abs(t.velo) < std::numeric_limits<float>::min())
			continue;

		auto dT = kDeltaTime / 1000.f;
		auto mu = t.velo > 0.f ? t.visco : -t.visco;
		auto dV = -mu * dT;

		// Big dV WILL cause oscillation of small velo.(velo:+-+-+-...)
		if (glm::abs(t.velo) > dV) {
			t.velo += dV;
			auto dA = t.velo * dT;
			auto T = glm::translate(glm::mat4{ 1.f }, -t.box.Center());
			auto Tinv = glm::translate(glm::mat4{ 1.f }, t.box.Center());
			t.Model = glm::rotate(t.Model, dA, glm::vec3{ 0.f,1.f,0.f });
		}
		else { t.velo = 0.f; }
	}
}



void GameLoop(World &world, GLFWWindowDesc &window) {

	glClearColor(0.164f, 0.164f, 0.164f, 1.f);
	glEnable(GL_DEPTH_TEST);
	unsigned frame_cnt = 0;



	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	const char* glsl_version = "#version 450";
	ImGui_ImplGlfw_InitForOpenGL(window.Get(), false);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();


	// render Backmann
	world.renderer->renderBackmann(*world.quad.get());
	// render irridance map 
	// world.renderer->renderLgtIrrid(*world.meshes.get(), *world.quad_irrid.get());
	while (!window.ShouldClose()) {


		//Rotate(world.meshes.get());

#if 0
		auto milli = TimeMeasure([&world]() {world.gloomy->Render(*world.meshes.get()); }, 500);
		printf("render profile %.2f ms per frame\n", milli);
		break;
#endif

		world.renderer->render_hm(*world.meshes.get());

		
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Tweak");
		static int counter = 0;
		ImGui::Text("(%.2fms,%.0ffps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::SliderFloat("m", & world.renderer->m, 0, 1);
		//ImGui::SliderFloat("world_scale", &world.renderer->world_scale, 0, 255);
		//ImGui::SliderInt("ssr flag", &world.renderer->ssr_flag, 0, 1);
		//ImGui::SliderInt("phony flag", &world.renderer->phony_flag, 0, 1);
		//ImGui::SliderFloat("s_alpha", &world.renderer->s_alpha, 0, 1);
		//ImGui::SliderFloat("s_beta", &world.renderer->s_beta, 0, 1);
		ImGui::SliderFloat("light x", &world.lights->at(0).dir.x, -1, 1);
		ImGui::SliderFloat("light y", &world.lights->at(0).dir.y, -1, 1);
		ImGui::SliderFloat("light z", &world.lights->at(0).dir.z, -1, 1);
		//ImGui::SliderFloat("Kd", &world.renderer->Kd, 0, 1);
		ImGui::SliderFloat("albedo", &world.renderer->albedo, 0, 1);
		ImGui::ColorPicker3("lht color", world.renderer->lht_color);
		//ImGui::ColorPicker3("obj color", world.renderer->obj_color);
		//ImGui::ColorPicker3("amb color", world.renderer->amb_color);
		//ImGui::ColorPicker3("spec color", world.renderer->spec_color);


		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window.Get());
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}




void HandleInput(GLFWWindowDesc *window, World *world) {

	window->OnKeyboard([&window, world=world](int key, int action) {

		if (ImGui::GetIO().WantCaptureKeyboard) return;
		switch (key) {
		case GLFW_KEY_K:
			world->meshes->at(0).velo += 1.f;
			break;
		case GLFW_KEY_J:
			world->meshes->at(0).velo -= 1.f;
			break;
		case GLFW_KEY_F:
			if (action == GLFW_PRESS)
				world->view->Zoom(-.5f);
			break;
		case GLFW_KEY_B:
			if (action == GLFW_PRESS)
				world->view->Zoom(.5f);
			break;
		case GLFW_KEY_SPACE:
			world->view->Reset();
		}
	});

	window->OnMouseButton([window=window, world = world](int button, int action, int mods)->void {
		if (ImGui::GetIO().WantCaptureMouse) return;
		if (button == 0) {
			switch (world->view->Track()) {
			case true:
				if (action == GLFW_RELEASE) {
					world->view->UnTrack();
				}
				break;
			case false:
				if (action == GLFW_PRESS) {
					double x, y;
					glfwGetCursorPos(window->Get(), &x, &y);
					world->view->Track(x, y);
				}
				break;
			}
		}
	});

	window->OnCursor([&window, world = world](float x, float y) {
		if (ImGui::GetIO().WantCaptureMouse) return;
		switch (world->view->Track()) {
		case true:
			world->view->Track(x, y);
			break;
		}
	});

	window->OnScroll([&window, world = world](float xoffset, float yoffset) {
		if (ImGui::GetIO().WantCaptureMouse) return;
		world->view->Zoom(yoffset);
	});
}


