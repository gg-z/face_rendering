#ifndef IND_SHADER
#define IND_SHADER


#include <string>

#include <glad/glad.h>


class Shader {
public:
	Shader(std::string vert_path, std::string frag_path);
	Shader(std::string comp_path);
	~Shader();
	GLuint Get() { return shader_; }

private:
	GLuint shader_;
};


#endif // !IND_SHADER

