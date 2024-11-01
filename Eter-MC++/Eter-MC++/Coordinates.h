#pragma once
class Coordinates
{
public:
	Coordinates();
	Coordinates(int x, int y);
	Coordinates(const Coordinates& other);
	size_t operator() (const Coordinates& key) const;
	bool operator== (const Coordinates& other) const;
	int GetX() const;
	int GetY() const;
	void SetX(int x);
	void SetY(int y);

private:
	int m_x, m_y;
};

