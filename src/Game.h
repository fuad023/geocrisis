#pragma once

#include "EntityManager.h"

#include <fstream>
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include <SFML\Window\Joystick.hpp>

struct WindowConfig { unsigned int width, height, frameLimit; bool mode; };
struct FontConfig   { int size, R, G, B; std::string path; };
struct PlayerConfig { double SR, CR, S, OT; int FR, FG, FB, OR, OG, OB, V; };
struct EnemyConfig  { double SR, CR, SMIN, SMAX, OT; int OR, OG, OB, VMIN, VMAX, L, SI; };
struct BulletConfig { double SR, CR, S, OT; int FR, FG, FB, OR, OG, OB, V, L, SI; };

class Game
{
    sf::RenderWindow        m_window;                      // the window we will draw to
    int                     m_Fullscreen         = 8;      // Default screen = 7
    EntityManager           m_entityManager;               // vector of entities to maintain
    sf::Font                m_font;                        // the font we will use to draw
    sf::Text                m_text;                        // the score text to be drawn to the screen
    std::shared_ptr<Entity> m_player;                       

    //WindowConfig            m_windowConfig;
    FontConfig              m_fontConfig;
    PlayerConfig            m_playerConfig;                                              
    EnemyConfig             m_enemyConfig;                               
    BulletConfig            m_bulletConfig;

    sf::Clock               m_deltaClock;                                     
    int                     m_score               = 0;                                     
    int                     m_currentFrame        = 0;                              
    int                     m_lastEnemySpawnTime  = 0;
    int                     m_lastBulletSpawnTime = 0;
    int                     m_spawnIntervalEnemy;              
    int                     m_powerMoveInterval   = 600;                
    int                     m_frameSpecialWeapon  = 0;
    int                     m_spawnIntervalBullet = 1;
    bool                    m_running             = true;                                     
    bool                    m_paused              = false;
    bool                    m_joystick            = false;

    bool                    m_GuiEnemySpawner     = true;
    bool                    m_GuiCollision        = true;
    bool                    m_GuiMovement         = true;
    bool                    m_GuiLifespan         = true;
    bool                    m_GuiRender           = true;

                     

    void init                   (const std::string& config); // initialize the GameState with a config file path
    void initConfig             (const std::string& config); // helper of init()
    void initWindow             ();
    void initText               ();
    void avoidAtPaused          ();
    void sUserInput             ();
    void sJoystick              ();
    void sEnemySpawner          ();
    void sCollision             ();
    bool collisionEntity        ( std::shared_ptr<Entity> head, std::shared_ptr<Entity> tail ); // helper function for Collision System
    void collisionBounceBoarder ( std::shared_ptr<Entity> entity );
    void collisionInputBound    ( std::shared_ptr<Entity> entity );
    void sMovement              ();
    void inputMovement          ( std::shared_ptr<Entity> entity );
    void sLifespan              ();
    void sGUI                   ();
    void RGBtoHSV               (sf::Color& rgb, double& h, double& s, double& v);
    void sRender                ();

    void spawnPlayer            ();
    void initBullet             ( const Vec2& direction );
    void spawnBullet            ( std::shared_ptr<Entity> entity, const Vec2& mousePos );
    void initSpecialWeapon      ();
    void spawnSpecialWeapon     ();
    void spawnEnemy             ();
    void spawnPosVel            ( int& xPos, int& yPos, int& speed, int& vertices, Vec2& velDir, Vec2& velMag );
    void spawnSmallEnemies      ( std::shared_ptr<Entity> entity );
    void spawnVel               ( std::shared_ptr<Entity> entity, Vec2& velDir, Vec2& velMag, double angle );




public:                                                                         

    Game();                                                         
    Game(const std::string& config);                  // constructor, takes in game config
    
    void run();
};