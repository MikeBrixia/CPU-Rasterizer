
#pragma once

#include <iostream>
#include <stdexcept>
#include <__msvc_ostream.hpp>

template<typename T, int rows, int cols>
class Matrix
{
    
protected:
    T* data;

public:
    Matrix();
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    virtual ~Matrix();
    
    T& operator()(int row, int col) { return data[row * cols + col]; }
    const T& operator()(int row, int col) const { return data[row * cols + col]; }
    Matrix& operator=(const Matrix& other);
    
    Matrix operator*(const Matrix& other);
    Matrix operator*(const T& scalar);
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator-() const;
    Matrix operator/(const T& scalar) const;
    
    static constexpr int get_rows() { return rows; }
    static constexpr int get_cols() { return cols; }
    const T* get_data() const { return data; }
};

template <typename T, int rows, int cols>
::Matrix<T, rows, cols>::Matrix()
{
    data = new T[rows * cols];
    for (int i = 0; i < rows * cols; i++)
    {
        data[i] = 0;
    }
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols>::Matrix(const Matrix& other)
{
    if (this != &other)
    {
        this->data = new T[rows * cols];
        if (other.data != nullptr)
        {
            // Copy vector data into this matrix.
            for (int i = 0; i < rows * cols; i++)
            {
                this->data[i] = other.data[i];
            }  
        }
    }
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols>::Matrix(Matrix&& other) noexcept
{
    this->data = other.data;
    other.data = nullptr;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols>::~Matrix()
{
    if (data == nullptr)
        return;
    delete[] data;
}

template<typename T, int rows, int cols, int other_rows, int other_cols>
Matrix<T, rows, other_cols> operator*(const Matrix<T, rows, cols>& lhs,
    const Matrix<T, other_rows, other_cols>& rhs)
{
    Matrix<T, rows, other_cols> result;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < other_cols; j++)
        {
            for (int k = 0; k < cols; k++)
            {
                result(i, j) += lhs.get_data()[i * cols + k] * rhs.get_data()[k * other_cols + j];
            }
        }
    }
    
    return result;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols>& Matrix<T, rows, cols>::operator=(const Matrix& other)
{
    if (this != &other)
    {
        for (int i = 0; i < rows * cols; i++)
        {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator*(const Matrix& other)
{
    Matrix result;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < cols; k++)
            {
                result(i, j) += data[i * cols + k] * other.data[k * cols + j];
            }
        }   
    }
    return result;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator*(const T& scalar)
{
    Matrix result = *this;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) *= scalar;
        }   
    }
    return result;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator+(const Matrix& other) const
{
    Matrix result = *this;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) += other(i, j);
        }   
    }
    return result; 
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator-(const Matrix& other) const
{
    Matrix result = *this;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) -= other(i, j);
        }   
    }
    return result;
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator-() const
{
    Matrix result = *this;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) = -result(i, j);
        }   
    }
    return result;   
}

template <typename T, int rows, int cols>
Matrix<T, rows, cols> Matrix<T, rows, cols>::operator/(const T& scalar) const
{
    Matrix result = *this;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result(i, j) /= scalar;
        }   
    }
    return result;  
}

typedef Matrix<float, 4, 4> Matrix4f;
