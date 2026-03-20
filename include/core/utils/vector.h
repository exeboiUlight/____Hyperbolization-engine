#pragma once

#include <cmath>
#include <iostream>

namespace Engine {
    
    struct Vector2;
    struct Vector3;
    struct Vector4;
    
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float v) : x(v), y(v) {}
        Vector2(float x, float y) : x(x), y(y) {}
        
        Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
        Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
        Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }
        
        Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
        Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
        
        bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Vector2& other) const { return !(*this == other); }
        
        float Dot(const Vector2& other) const { return x * other.x + y * other.y; }
        float Length() const { return std::sqrt(x * x + y * y); }
        float LengthSquared() const { return x * x + y * y; }
        
        Vector2 Normalized() const { float len = Length(); return len > 0 ? *this / len : Vector2(0); }
        void Normalize() { float len = Length(); if (len > 0) *this /= len; }
        
        float& operator[](int index) { return (&x)[index]; }
        const float& operator[](int index) const { return (&x)[index]; }
        
        float* Data() { return &x; }
        const float* Data() const { return &x; }
        
        static Vector2 Zero() { return Vector2(0, 0); }
        static Vector2 One() { return Vector2(1, 1); }
        static Vector2 UnitX() { return Vector2(1, 0); }
        static Vector2 UnitY() { return Vector2(0, 1); }
    };
    
    inline Vector2 operator*(float scalar, const Vector2& vec) {
        return vec * scalar;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
        os << "Vector2(" << vec.x << ", " << vec.y << ")";
        return os;
    }
    
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float v) : x(v), y(v), z(v) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        Vector3(const Vector2& xy, float z) : x(xy.x), y(xy.y), z(z) {}
        
        Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
        Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
        Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
        Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }
        
        Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
        
        bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
        bool operator!=(const Vector3& other) const { return !(*this == other); }
        
        float Dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
        Vector3 Cross(const Vector3& other) const { 
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }
        float Length() const { return std::sqrt(x * x + y * y + z * z); }
        float LengthSquared() const { return x * x + y * y + z * z; }
        
        Vector3 Normalized() const { float len = Length(); return len > 0 ? *this / len : Vector3(0); }
        void Normalize() { float len = Length(); if (len > 0) *this /= len; }
        
        float& operator[](int index) { return (&x)[index]; }
        const float& operator[](int index) const { return (&x)[index]; }
        
        float* Data() { return &x; }
        const float* Data() const { return &x; }
        
        Vector2 xy() const { return Vector2(x, y); }
        Vector2 xz() const { return Vector2(x, z); }
        Vector2 yz() const { return Vector2(y, z); }
        
        static Vector3 Zero() { return Vector3(0, 0, 0); }
        static Vector3 One() { return Vector3(1, 1, 1); }
        static Vector3 UnitX() { return Vector3(1, 0, 0); }
        static Vector3 UnitY() { return Vector3(0, 1, 0); }
        static Vector3 UnitZ() { return Vector3(0, 0, 1); }
    };
    
    inline Vector3 operator*(float scalar, const Vector3& vec) {
        return vec * scalar;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        os << "Vector3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
    
    struct Vector4 {
        float x, y, z, w;
        
        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(float v) : x(v), y(v), z(v), w(v) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector2& xy, float z, float w) : x(xy.x), y(xy.y), z(z), w(w) {}
        Vector4(const Vector3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        
        Vector4 operator+(const Vector4& other) const { 
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); 
        }
        
        Vector4 operator-(const Vector4& other) const { 
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); 
        }
        
        Vector4 operator*(float scalar) const { 
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); 
        }
        
        Vector4 operator/(float scalar) const { 
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); 
        }
        
        Vector4& operator+=(const Vector4& other) { 
            x += other.x; y += other.y; z += other.z; w += other.w; 
            return *this; 
        }
        
        Vector4& operator-=(const Vector4& other) { 
            x -= other.x; y -= other.y; z -= other.z; w -= other.w; 
            return *this; 
        }
        
        Vector4& operator*=(float scalar) { 
            x *= scalar; y *= scalar; z *= scalar; w *= scalar; 
            return *this; 
        }
        
        Vector4& operator/=(float scalar) { 
            x /= scalar; y /= scalar; z /= scalar; w /= scalar; 
            return *this; 
        }
        
        bool operator==(const Vector4& other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }
        
        bool operator!=(const Vector4& other) const {
            return !(*this == other);
        }
        
        float Dot(const Vector4& other) const { 
            return x * other.x + y * other.y + z * other.z + w * other.w; 
        }
        
        float Length() const { 
            return std::sqrt(x * x + y * y + z * z + w * w); 
        }
        
        float LengthSquared() const { 
            return x * x + y * y + z * z + w * w; 
        }
        
        Vector4 Normalized() const { 
            float len = Length(); 
            return len > 0 ? *this / len : Vector4(0); 
        }
        
        void Normalize() { 
            float len = Length(); 
            if (len > 0) *this /= len; 
        }
        
        Vector2 xy() const { return Vector2(x, y); }
        Vector2 zw() const { return Vector2(z, w); }
        Vector3 xyz() const { return Vector3(x, y, z); }
        
        float& operator[](int index) { 
            return (&x)[index]; 
        }
        
        const float& operator[](int index) const { 
            return (&x)[index]; 
        }
        
        float* Data() { return &x; }
        const float* Data() const { return &x; }
        
        static Vector4 Zero() { return Vector4(0, 0, 0, 0); }
        static Vector4 One() { return Vector4(1, 1, 1, 1); }
        static Vector4 UnitX() { return Vector4(1, 0, 0, 0); }
        static Vector4 UnitY() { return Vector4(0, 1, 0, 0); }
        static Vector4 UnitZ() { return Vector4(0, 0, 1, 0); }
        static Vector4 UnitW() { return Vector4(0, 0, 0, 1); }
    };
    
    inline Vector4 operator*(float scalar, const Vector4& vec) {
        return vec * scalar;
    }
    
    inline std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
        os << "Vector4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }
    
}