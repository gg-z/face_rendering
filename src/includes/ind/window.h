#ifndef IND_WINDOW
#define IND_WINDOW


#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class GLFWWindowDesc {
public:
	GLFWWindowDesc(
		unsigned width,
		unsigned height,
		const char* title,
		bool debug = false
	);
	~GLFWWindowDesc();

	void OnKeyboard(std::function<void(int, int)> handler);
	void OnCursor(std::function<void(float, float)> handler);
	void OnMouseButton(std::function<void(int, int, int)> handler);
	void OnScroll(std::function<void(float, float)> handler);
	bool ShouldClose() const {
		return (1 == glfwWindowShouldClose(wptr_)) || should_close_;
	}
	GLFWwindow *Get() { return wptr_; }
	unsigned Width() { return width_; }
	unsigned Height() { return height_; }

	static void DebugCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam);

	GLFWwindow *wptr_ = nullptr;
	bool should_close_ = false;
	std::function<void(int, int)> keyboard_handler_;
	std::function<void(float, float)> cursor_handler_;
	std::function<void(int, int, int)> mouse_button_handler_;
	std::function<void(float, float)> scroll_handler_;
	unsigned width_, height_;
};


#endif // !IND_WINDOW
