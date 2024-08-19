#pragma once

struct Vector2
{
	Vector2(float x, float y)
		: x(x), y(y)
	{

	};

	Vector2()
	{
		x = 0;
		y = 0;
	};

	float x, y;

	static const Vector2 zero;

	Vector2& operator+(const Vector2& other)
	{
		Vector2 result = Vector2(this->x + other.x, this->y + other.y);
		return result;
	}

	Vector2& operator-(const Vector2& other)
	{
		Vector2 result = Vector2(this->x - other.x, this->y - other.y);
		return result;
	}

	Vector2& operator*(const Vector2& other)
	{
		Vector2 result = Vector2(this->x * other.x, this->y * other.y);
		return result;
	}

	Vector2& operator/(const Vector2& other)
	{
		Vector2 result = Vector2(this->x / other.x, this->y / other.y);
		return result;
	}

	Vector2& operator+=(const Vector2& other)
	{
		this->x += other.x;
		this->y += other.y;

		return *this;
	}
};

struct Vector3
{
	Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{

	};

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	float x, y, z;

	static const Vector3 zero;

	Vector3& operator+(const Vector3& other)
	{
		Vector3 result = Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
		return result;
	}

	Vector3& operator-(const Vector3& other)
	{
		Vector3 result = Vector3(this->x - other.x, this->y - other.y, this->z - other.z);
		return result;
	}

	Vector3& operator*(const Vector3& other)
	{
		Vector3 result = Vector3(this->x * other.x, this->y * other.y, this->z * other.z);
		return result;
	}

	Vector3& operator/(const Vector3& other)
	{
		Vector3 result = Vector3(this->x / other.x, this->y / other.y, this->z / other.z);
		return result;
	}

	Vector3& operator+=(const Vector2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
};

struct Vector4
{
	Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{

	};

	Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	};

	float x, y, z, w;

	Vector4& operator+(const Vector4& other)
	{
		Vector4 result = Vector4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
		return result;
	}

	Vector4& operator-(const Vector4& other)
	{
		Vector4 result = Vector4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
		return result;
	}

	Vector4& operator*(const Vector4& other)
	{
		Vector4 result = Vector4(this->x * other.x, this->y * other.y, this->z * other.z, this->w * other.w);
		return result;
	}

	Vector4& operator/(const Vector4& other)
	{
		Vector4 result = Vector4(this->x / other.x, this->y / other.y, this-> z / other.z, this->w + other.w);
		return result;
	}
};