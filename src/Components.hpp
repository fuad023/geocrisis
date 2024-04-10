#pragma once

#include "Vec2.h"
#include "include\SFML_GRAPHICS.h"

class CTransform
{
public:
    Vec2   pos     = { 0.0, 0.0 };
    int    speed   = 0;
    Vec2   velMag  = { 0.0, 0.0 };
    Vec2   velAxis = { 0.0, 0.0 }; // (+) x-axis = (+) y-axis = 1 // (-) x-axis = (-) y-axis = -1
    Vec2   accel   = { 0.0, 0.0 }; // in future :( pls add the acceleration to velocity 
    Vec2   scale;
    double angle   = 0.0;

    CTransform() {}
    CTransform(const Vec2& position, const Vec2& velMagnitude, const Vec2& velDirection, double angle)
    : pos(position), velMag(velMagnitude), velAxis(velDirection), angle(angle) {}
};

class CShape
{
public:
    sf::CircleShape circle;
    sf::Color FILL, OUTLINE;
    
    CShape() {}
    CShape(double radius, std::size_t points, const sf::Color& fill, const sf::Color& outline, double thickness)
        : circle(radius, points), FILL(fill), OUTLINE(outline)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }

    void setAlphaFill(const int alpha) { FILL.a = alpha; }
    void setAlphaOutline(const int alpha) { OUTLINE.a = alpha; }
    void setAlpha(const int alpha) { FILL.a = alpha; OUTLINE.a = alpha; }
};

class CCollision
{
public:
    double r = 0.0;
    CCollision(double radius) : r(radius) {}
};

class CInput
{
public:

    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool jMove  = false;
    Vec2 mFactor = {0.0, 0.0};

    bool shootK = false;
    bool shootJ = false;
    Vec2 sFactor = {0.0, 0.0};

    bool specialWeapon = false;

    CInput() {}
};

class CScore
{
public:
    int score = 0;
    CScore(int s) : score(s) {}
};

class CLifeSpan
{
public:
    int remaining  = 0; // amount of lifespan remaining on the entity
    int total      = 0; // the total initial amount of lifespan
    CLifeSpan(int total) : remaining(total), total(total) {}
};
