#pragma once
#include <mutex>
#include <random>
#include <memory>

class Random
{
public:
	Random(const Random& other) = delete;

	static Random& GetInstance();

	static int Get(int min, int max);

private:
	Random();

	int IGet(int min, int max);
	std::unique_ptr<std::mt19937> m_engine;
};