#include "Coordinates.h"
#include <functional>

Coordinates::Coordinates() : m_x{ 0 }, m_y{ 0 } {}

Coordinates::Coordinates(int x, int y) : m_x{ x }, m_y{ y } {}

Coordinates::Coordinates(const Coordinates& other) : m_x{ other.m_x }, m_y{ other.m_y } {}

bool Coordinates::operator==(const Coordinates& other) const
{
	return (m_x == other.m_x && m_y == other.m_y);
}

int Coordinates::GetX() const
{
	return m_x;
}

int Coordinates::GetY() const
{
	return m_y;
}

void Coordinates::SetX(int x)
{
	m_x = x;
}

void Coordinates::SetY(int y)
{
	m_y = y;
}

void to_json(nlohmann::json& j, const Coordinates& position)
{
	j["x"] = position.GetX();
	j["y"] = position.GetY();
}

void from_json(const nlohmann::json& j, Coordinates& position)
{
	position.SetX(j.at("x"));
	position.SetY(j.at("y"));
}
