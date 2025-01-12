#pragma once
#include <cstddef>
#include <functional>
#if defined linux
#include "../Dependencies/JSON/json.hpp"
#else
#include "json.hpp"
#endif

class Coordinates
{
public:
	struct Hash {
		size_t operator()(const Coordinates& key) const
		{
			return std::hash<int>()(key.m_x) ^ std::hash<int>()(key.m_y);
		}
	};

public:
	Coordinates();
	Coordinates(int x, int y);
	Coordinates(const Coordinates& other);

	Coordinates& operator=(const Coordinates& other) = default; 
	Coordinates(Coordinates&& other) noexcept = default;  
	Coordinates& operator=(Coordinates&& other) noexcept = default; 
	~Coordinates() = default;

	bool operator==(const Coordinates& other) const;
	int GetX() const;
	int GetY() const;
	void SetX(int x);
	void SetY(int y);

private:
	int m_x, m_y;
};


void to_json(nlohmann::json& j, const Coordinates& position);
void from_json(const nlohmann::json& j, Coordinates& position);
