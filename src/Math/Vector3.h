#pragma once
#include <cmath>

#include "Matrix.h"

template<typename T>
class Vector3 : public Matrix<T, 3, 1>
{
    
public:
    Vector3() = default;
    Vector3(T x, T y, T z);
    Vector3(const Matrix<T, 3, 1>& other) : Matrix<T, 3, 1>(other)
    {
        this->data[0] = other.data[0];
        this->data[1] = other.data[1];
        this->data[2] = other.data[2];  
    }

    float operator*(const Vector3& other) const { return dot(other); }
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(const T& scalar) const;
    Vector3 operator/(const T& scalar) const;
    Vector3 operator-() const;
    
    T x() const { return this->data[0]; }
    T y() const { return this->data[1]; }
    T z() const { return this->data[2]; }

    float length() const;
    float dot(const Vector3& other) const;
    Vector3 normalized() const;
    Vector3 cross(const Vector3& other) const;
};

template <typename T>
Vector3<T>::Vector3(T x, T y, T z)
{
    this->data[0] = x;
    this->data[1] = y;
    this->data[2] = z;   
}

template <typename T>
Vector3<T> Vector3<T>::operator+(const Vector3& other) const
{
    return Vector3(x() + other.x(), y() + other.y(), z() + other.z());   
}

template <typename T>
Vector3<T> Vector3<T>::operator-(const Vector3& other) const
{
    return Vector3(x() - other.x(), y() - other.y(), z() - other.z());  
}

template <typename T>
Vector3<T> Vector3<T>::operator*(const T& scalar) const
{
    return Vector3(x() * scalar, y() * scalar, z() * scalar); 
}

template <typename T>
Vector3<T> Vector3<T>::operator/(const T& scalar) const
{
    return Vector3(x() / scalar, y() / scalar, z() / scalar);
}

template <typename T>
Vector3<T> Vector3<T>::operator-() const
{
    return Vector3(-x(), -y(), -z());
}

template <typename T>
float Vector3<T>::length() const
{
    float _x = x();
    float _y = y();
    float _z = z();
    return std::sqrt(_x * _x + _y * _y + _z * _z);
}

template <typename T>
float Vector3<T>::dot(const Vector3& other) const
{
    return x() * other.x() + y() * other.y() + z() * other.z();
}

template <typename T>
Vector3<T> Vector3<T>::normalized() const
{
    float len = length();

    // In the case of length 0, return the null-vector.
    if (len == 0.f)
        return Vector3(0, 0, 0);
    
    return *this / len;
}

template <typename T>
Vector3<T> Vector3<T>::cross(const Vector3& other) const
{
    Vector3 a = *this;
    Vector3 b = other;
    
    float x = a.y() * b.z() - a.z() * b.y();
    float y = a.z() * b.x() - a.x() * b.z();
    float z = a.x() * b.y() - a.y() * b.x();
    Vector3 result(x, y, z);
    
    return result;
}

typedef Vector3<float> Vector3f;
