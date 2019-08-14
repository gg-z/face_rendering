#ifndef IND_UTILS
#define IND_UTILS


#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>


#define IND_ASSERT(success, message) IndAssert(success,message,__FILE__,__LINE__)
#define IND_LOG(message) IndLog(message,__FILE__,__LINE__)

void IndAssert(bool success, std::string message, std::string file, int line);
void IndLog(std::string message, std::string file, int line);
void printDevILErrors();


//Returns millisecs.
template<typename TFN>
float TimeMeasure(TFN fn, int num_iters) {
	auto op = std::chrono::system_clock::now();
	for (int i = 0; i < 100; ++i)
		fn();
	auto ed = std::chrono::system_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(ed - op);
	return diff.count() / 1000.f / num_iters;
}

// Sync time with constant delta-time step.
// With each step, work-load can be attached.
class Catcher {
public:
	Catcher(unsigned milli);

	template<typename THandler>
	void Sync(THandler fn) {
		while (std::chrono::system_clock::now() > next_update_time_) {
			fn();
			next_update_time_ += update_interval_;
		}
	}

	void SyncNoOp() {
		next_update_time_ = std::chrono::system_clock::now();
	}

private:
	std::chrono::time_point<std::chrono::system_clock> next_update_time_;
	std::chrono::milliseconds update_interval_;
};

glm::vec3 HexToRGB(unsigned hex);
glm::vec4 HexToRGBA(unsigned hex);

class SimpleRandom {
public:
	SimpleRandom(float low = .0f, float high = 1.f);

	float rand();

private:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> distr;
};

//
// Uniform helper functions.
//
inline void AssignUniform(GLuint handle, const glm::vec2 &value) {
	glUniform2f(handle, value.x, value.y);
}

inline void AssignUniform(GLuint handle, const glm::vec3 &value) {
	glUniform3f(handle, value.x, value.y, value.z);
}

inline void AssignUniform(GLuint handle, const glm::vec4 &value) {
	glUniform4f(handle, value.x, value.y, value.z, value.w);
}

inline void AssignUniform(GLuint handle, const glm::mat4 &value) {
	glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(value));
}

inline void AssignUniform(GLuint handle, const GLuint value) {
	glUniform1ui(handle, value);
}

inline void AssignUniform(GLuint handle, const GLint value) {
	glUniform1i(handle, value);
}

inline void AssignUniform(GLuint handle, const GLfloat value) {
	glUniform1f(handle, value);
}

inline void AssignUniform(GLuint handle, const GLfloat* value) {
	glUniform4fv(handle, 2 ,value);
}

template<typename T>
void AssignUniform(GLuint shader, const char *name, const T &value) {
	auto handle = glGetUniformLocation(shader, name);
	AssignUniform(handle, value);
}


#endif // !IND_UTILS
