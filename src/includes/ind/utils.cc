#include "utils.h"

#include <iostream>
#include <chrono>
#include <string>
#include <random>
#include <math.h>

#include <glm/glm.hpp>


void IndAssert(bool success, std::string message, std::string file, int line) {
	if (success) return;
	std::cout << "\nERROR (" << message << ")\n";
	std::cout << "~In file (" << file << ") " << ", line (" << line << ")\n";
	std::exit(1);
}

void IndLog(std::string message, std::string file, int line) {
	std::cout << "\nLOG (" << message << ")\n";
	std::cout << "~In file (" << file << ") " << ", line (" << line << ")\n";
	return;
}


// Catcher
Catcher::Catcher(unsigned milli) {
	update_interval_ = std::chrono::milliseconds(milli);
	next_update_time_ = std::chrono::system_clock::now();
}

SimpleRandom::SimpleRandom(float low, float high) {
	gen = std::mt19937(rd());
	distr = std::uniform_real_distribution<float>(low, high);
}

float SimpleRandom::rand() {
	return distr(gen);
}

glm::vec3 HexToRGB(unsigned hex) {
	constexpr float _255inv = 1.f / 255.f;
	auto r = ((hex & 0xff0000) >> 16) * _255inv;
	auto g = ((hex & 0x00ff00) >> 8)* _255inv;
	auto b = (hex & 0x0000ff)* _255inv;
	return { r,g,b };
}

glm::vec4 HexToRGBA(unsigned hex) {
	constexpr float _255inv = 1.f / 255.f;
	auto r = ((hex & 0xff000000) >> 24) * _255inv;
	auto g = ((hex & 0x00ff0000) >> 16)* _255inv;
	auto b = ((hex & 0x0000ff00) >> 8)* _255inv;
	auto a = (hex & 0x000000ff) * _255inv;
	return { r,g,b,a };
}


int ceilPow2(int x) {

	if (x < 0) return 0;
	int order = 1;
	int x_ = x;
	while (int(x_ / 2) > 0) {
		x_ = x_ / 2;
		order = order + 1;
	}

	return pow(2, order);
}