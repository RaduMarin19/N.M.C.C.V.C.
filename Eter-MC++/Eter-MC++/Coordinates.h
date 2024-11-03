#pragma once
#include <stddef.h>

class Coordinates
{
public:
	Coordinates();
	Coordinates(short x, short y);
	Coordinates(const Coordinates& other);
	size_t operator() (const Coordinates& key) const;
	bool operator== (const Coordinates& other) const;
	short GetX() const;
	short GetY() const;
	void SetX(short x);
	void SetY(short y);

private:
	short m_x, m_y;
};

