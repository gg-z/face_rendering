#include "./window.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./utils.h"






GLFWWindowDesc::GLFWWindowDesc(unsigned width, unsigned height, const char * title, bool debug)
	:width_{ width }, height_{ height } {
	IND_ASSERT(glfwInit() == GLFW_TRUE, "Window init failed.");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 32);
	if (debug)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	wptr_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!wptr_) {
		glfwTerminate();
		IND_LOG("Create window failed(nullptr).");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(wptr_);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// fps=60/Interval
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(wptr_, this);

	glfwSetKeyCallback(wptr_, [](GLFWwindow *wptr, int key, int, int action, int) {
		auto wd = static_cast<GLFWWindowDesc*>(glfwGetWindowUserPointer(wptr));
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			wd->should_close_ = true;
			glfwSetWindowShouldClose(wd->wptr_, GLFW_TRUE);
		}
	});

	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugCallback, nullptr);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API,
			GL_DEBUG_TYPE_ERROR,
			GL_DEBUG_SEVERITY_MEDIUM,
			0, nullptr, GL_TRUE);
	}
}

GLFWWindowDesc::~GLFWWindowDesc() {
	glfwDestroyWindow(wptr_);
	glfwTerminate();
}

void GLFWWindowDesc::OnKeyboard(std::function<void(int, int)> handler) {
	keyboard_handler_ = handler;
	glfwSetKeyCallback(wptr_, [](GLFWwindow *wptr, int key, int, int action, int) {
		auto wd = static_cast<GLFWWindowDesc*>(glfwGetWindowUserPointer(wptr));
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			wd->should_close_ = true;
			glfwSetWindowShouldClose(wd->wptr_, GLFW_TRUE);
		}
		wd->keyboard_handler_(key, action);
	});
}

void GLFWWindowDesc::OnCursor(std::function<void(float, float)> handler) {
	cursor_handler_ = handler;
	glfwSetCursorPosCallback(wptr_, [](GLFWwindow* wptr, double xpos, double ypos) {
		auto wd = static_cast<GLFWWindowDesc*>(glfwGetWindowUserPointer(wptr));
		wd->cursor_handler_(xpos, ypos);
	});
}

void GLFWWindowDesc::OnMouseButton(std::function<void(int, int, int)> handler) {
	mouse_button_handler_ = handler;
	glfwSetMouseButtonCallback(wptr_, [](GLFWwindow *wptr, int button, int action, int mods) {
		auto wd = static_cast<GLFWWindowDesc*>(glfwGetWindowUserPointer(wptr));
		wd->mouse_button_handler_(button, action, mods);
	});
}

void GLFWWindowDesc::OnScroll(std::function<void(float, float)> handler) {
	scroll_handler_ = handler;
	glfwSetScrollCallback(wptr_, [](GLFWwindow *wptr, double xoffset, double yoffset) {
		auto wd = static_cast<GLFWWindowDesc*>(glfwGetWindowUserPointer(wptr));
		wd->scroll_handler_(xoffset,yoffset);
	});
}

void GLFWWindowDesc::DebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar * message,
	const void * userParam
) {
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:
		std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}