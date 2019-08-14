#include "./shader.h"

#include <fstream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./utils.h"


void ReadFile(std::string path, std::string *content) {
	std::ifstream fin(path);
	IND_ASSERT(!fin.fail(), "(WARNING) File path: " + path + " does not exist.\n");

	fin.ignore(std::numeric_limits<std::streamsize>::max());
	size_t size = fin.gcount();
	fin.clear();
	//IND_LOG("Shader path: " + path + ", size: " + std::to_string(size));

	fin.seekg(0, std::ios_base::beg);
	auto source = std::unique_ptr<char>(new char[size]);
	fin.read(source.get(), size);
	content->swap(std::string(source.get(), size));
}

GLuint CreateShader(GLenum type, std::string code) {
	GLuint shader_handle = glCreateShader(type);
	auto code_ = code.c_str();
	glShaderSource(shader_handle, 1, &code_, nullptr);
	glCompileShader(shader_handle);

	int success = 0, log_len = 0;
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

	if (!success) {
		std::string shader_type_name{ "Unknown shader" };
		if (type == GL_VERTEX_SHADER)
			shader_type_name = "Vertex shader";
		if (type == GL_FRAGMENT_SHADER)
			shader_type_name = "Fragment shader";
		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len <= 0)
			IND_LOG(shader_type_name + ": no log found");
		else {
			auto log = std::unique_ptr<char>(new char[log_len]);
			glGetShaderInfoLog(shader_handle, log_len, &log_len, log.get());
			IND_LOG(shader_type_name + " compile log : " + log.get());
		}
		IND_ASSERT(false, shader_type_name + " compile error");
	}
	return shader_handle;
}

void CheckShader(GLuint shader) {
	int success = 0, log_len = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len <= 0)
			IND_LOG("No log found");
		else {
			auto log = std::unique_ptr<char>(new char[log_len]);
			glGetProgramInfoLog(shader, log_len, nullptr, log.get());
			IND_LOG(std::string("Program compile log :") + log.get());
		}
		IND_ASSERT(false, "Program compile error");
	}
}

Shader::Shader(std::string vert_path, std::string frag_path) {
	std::string vert_shader, frag_shader;
	ReadFile(vert_path, &vert_shader);
	ReadFile(frag_path, &frag_shader);

	GLuint vertobj = CreateShader(GL_VERTEX_SHADER, vert_shader);
	GLuint fragobj = CreateShader(GL_FRAGMENT_SHADER, frag_shader);

	shader_ = glCreateProgram();
	glAttachShader(shader_, vertobj);
	glAttachShader(shader_, fragobj);
	glLinkProgram(shader_);

	CheckShader(shader_);

	glDeleteShader(vertobj);
	glDeleteShader(fragobj);
}

Shader::Shader(std::string comp_path) {
	std::string comp_shader;
	ReadFile(comp_path, &comp_shader);
	GLuint compobj = CreateShader(GL_COMPUTE_SHADER, comp_shader);

	shader_ = glCreateProgram();
	glAttachShader(shader_, compobj);
	glLinkProgram(shader_);

	CheckShader(shader_);

	glDeleteShader(compobj);
}

Shader::~Shader() {
	glDeleteProgram(shader_);
}
