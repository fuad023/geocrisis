#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(double n)           : m_x(n),       m_y(n) {}
Vec2::Vec2(double x, double y) : m_x(x),       m_y(y) {}
Vec2::Vec2(const Vec2& rhs)    : m_x(rhs.m_x), m_y(rhs.m_y) {}
 

Vec2 Vec2::operator = (const Vec2& rhs) {
    return Vec2(m_x = rhs.m_x, m_y = rhs.m_y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
    return Vec2(m_x + rhs.m_x, m_y + rhs.m_y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
    return Vec2(m_x - rhs.m_x, m_y - rhs.m_y);
}

Vec2 Vec2::operator * (const Vec2& rhs) const {
    return Vec2(m_x * rhs.m_x, m_y * rhs.m_y);
}
Vec2 Vec2::operator / (const Vec2& rhs) const {
    return Vec2(m_x / rhs.m_x, m_y / rhs.m_y);
}

Vec2 Vec2::operator * (double val) const {
    return Vec2(m_x*val, m_y*val);
}

Vec2 Vec2::operator / (double val) const {
    return Vec2(m_x/val, m_y/val);
}

bool Vec2::operator == (const Vec2& rhs) const {
    return (m_x == rhs.m_x) && (m_y == rhs.m_y);
}

bool Vec2::operator != (const Vec2& rhs) const {
    return !(*this == rhs);
}

void Vec2::operator += (const Vec2& rhs) {
    m_x += rhs.m_x;
    m_y += rhs.m_y;
}
void Vec2::operator -= (const Vec2& rhs) {
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
}

void Vec2::operator *= (const Vec2& rhs) {
    m_x *= rhs.m_x;
    m_y *= rhs.m_y;
}

void Vec2::operator /= (const Vec2& rhs) {
    m_x /= rhs.m_x;
    m_y /= rhs.m_y;
}


void Vec2::operator *= (double val) {
    m_x *= val;
    m_y *= val;
}

void Vec2::operator /= (double val) {
    m_x /= val;
    m_y /= val;
}

void Vec2::print() const {
    std::cout << "x: " << m_x << " y: " << m_y;
}

Vec2& Vec2::add(double val) {
    m_x += val;
    m_y += val;
    return *this;
}

Vec2& Vec2::scale(double val) {
    m_x *= val;
    m_y *= val;
    return *this;
}

double Vec2::dist(const Vec2& rhs) const {
    return sqrt(distSq(rhs));
}

double Vec2::distSq(const Vec2& rhs) const {
    return (m_x - rhs.m_x)*(m_x - rhs.m_x) + (m_y - rhs.m_y)*(m_y - rhs.m_y);
}

double Vec2::length() const {
    return sqrt( m_x*m_x + m_y*m_y );
}

Vec2& Vec2::normalize() {
    double len = Vec2::length();
    m_x /= len;
    m_y /= len;
    return *this;
}

Vec2& Vec2::assign (const Vec2& rhs) {
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    return *this;
}

Vec2& Vec2::abs() {
    if (m_x < 0) m_x *= -1;
    if (m_y < 0) m_y *= -1;
    return *this;
}

Vec2& Vec2::unit(double degree) {
    double radian = degree * (M_PI/180);
    m_x = cos(radian);
    m_y = sin(radian);
    return *this;
}

Vec2& Vec2::unit(Vec2 rhs)
{
    rhs.abs();
    *this /= rhs;
    return *this;
}

Vec2& Vec2::diagonalFix() {
    *this /= sqrt(2);
    return *this;    
}
