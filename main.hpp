#ifndef FLUIDPHYSICS_MAIN_HPP
#define FLUIDPHYSICS_MAIN_HPP

#endif //FLUIDPHYSICS_MAIN_HPP


#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>


struct Vector2 {
    int x;
    int y;

    Vector2(int i, int j) : x(i), y(j) {}

    Vector2() : x(0.0), y(0.0) {}

    // Addition
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Subtraction
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Multiplication (scalar * vector)
    Vector2 operator*(int scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // Division (vector / scalar)
    Vector2 operator/(int scalar) const {
        if (scalar != 0.0) {
            return Vector2(x / scalar, y / scalar);
        } else {
            // Handle division by zero; you may choose to return a default value or throw an exception.
            // Here, we return a vector with x and y set to zero.
            return Vector2(0.0, 0.0);
        }
    }

    // In-place addition
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // In-place subtraction
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    int magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalize() const {
        int mag = magnitude();
        if (mag != 0.0) {
            return Vector2(x / mag, y / mag);
        } else {
            // Handle division by zero; you may choose to return a default value or throw an exception.
            // Here, we return a vector with x and y set to zero.
            return Vector2(0.0, 0.0);
        }
    }

    // Calculate the dot product between two vectors
    int dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

};


struct Double2 {
    double x;
    double y;

    Double2(double i, double j) : x(i), y(j) {}

    Double2() : x(0.0), y(0.0) {}

    // Addition
    Double2 operator+(const Double2& other) const {
        return Double2(x + other.x, y + other.y);
    }

    // Subtraction
    Double2 operator-(const Double2& other) const {
        return Double2(x - other.x, y - other.y);
    }

    // Multiplication (scalar * vector)
    Double2 operator*(double scalar) const {
        return Double2(x * scalar, y * scalar);
    }

    // Division (vector / scalar)
    Double2 operator/(double scalar) const {
        if (scalar != 0.0) {
            return Double2(x / scalar, y / scalar);
        } else {
            // Handle division by zero; you may choose to return a default value or throw an exception.
            // Here, we return a vector with x and y set to zero.
            return Double2(0.0, 0.0);
        }
    }

    // In-place addition
    Double2& operator+=(const Double2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // In-place subtraction
    Double2& operator-=(const Double2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    double magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Double2 normalize() const {
        double mag = magnitude();
        if (mag != 0.0) {
            return Double2(x / mag, y / mag);
        } else {
            // Handle division by zero; you may choose to return a default value or throw an exception.
            // Here, we return a vector with x and y set to zero.
            return Double2(0.0, 0.0);
        }
    }

    // Calculate the dot product between two vectors
    double dot(const Double2& other) const {
        return x * other.x + y * other.y;
    }

};



bool initializeSDL();
void closeSDL();
void drawCircle(int32_t centreX, int32_t centreY, int32_t radius);
void updateFluid();
double distanceBetweenPoints(Double2 circle1, Double2 circle2);
int updateFluidSection(void * data);
void handleCollisions();