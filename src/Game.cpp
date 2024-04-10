#include "Game.h"
#include <random>

Game::Game() {};
Game::Game(const std::string&  config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    initConfig(path);                    // set up Config to structure
    initWindow();                        // set up default window parameters
    initText();                          // set up font and text

    ImGui::SFML::Init(m_window);                
    ImGui::GetStyle().ScaleAllSizes(1.0f);      

    // set up initial values
    spawnPlayer();
    m_spawnIntervalEnemy = m_enemyConfig.SI;                             
    m_spawnIntervalBullet = m_bulletConfig.SI;
}
void Game::initConfig(const std::string& path)
{
    std::ifstream fin(path);
    std::string tag;
    while (fin && fin >> tag) 
    {
        if (tag == "Window")
        {
            fin >> m_windowConfig.width
                >> m_windowConfig.height
                >> m_windowConfig.frameLimit
                >> m_windowConfig.mode;
        }

        if (tag == "Font")
        {
            fin >> m_fontConfig.path
                >> m_fontConfig.size
                >> m_fontConfig.R
                >> m_fontConfig.G
                >> m_fontConfig.B;
        }

        if (tag == "Player")
        {
            fin >> m_playerConfig.SR    // Shape Radius         double    
                >> m_playerConfig.CR    // Collision Radius     double    
                >> m_playerConfig.S     // Speed                double 
                >> m_playerConfig.FR    // Fill Red             int    
                >> m_playerConfig.FG    // Fill Green           int    
                >> m_playerConfig.FB    // Fill Blue            int    
                >> m_playerConfig.OR    // Outline Red          int    
                >> m_playerConfig.OG    // Outline Green        int    
                >> m_playerConfig.OB    // Outline Blue         int    
                >> m_playerConfig.OT    // Outline Thickness    double    
                >> m_playerConfig.V;    // Shape Vertices       int    
        }

        if (tag == "Enemy")
        {
            fin >> m_enemyConfig.SR     // Shape Radius         double    
                >> m_enemyConfig.CR     // Collision Radius     double    
                >> m_enemyConfig.SMIN   // Min Speed            double
                >> m_enemyConfig.SMAX   // Max Speed            double
                >> m_enemyConfig.OR     // Outline Red          int
                >> m_enemyConfig.OG     // Outline Green        int
                >> m_enemyConfig.OB     // Outline Blue         int
                >> m_enemyConfig.OT     // Outline Thickness    double
                >> m_enemyConfig.VMIN   // Min Vertices         int
                >> m_enemyConfig.VMAX   // Max Vertices         int
                >> m_enemyConfig.L      // Small Lifespan       int
                >> m_enemyConfig.SI;    // Spawn Interval       int             
        } 

        if (tag == "Bullet")
        {
            fin >> m_bulletConfig.SR    // Shape Radius         double
                >> m_bulletConfig.CR    // Collision Radius     double
                >> m_bulletConfig.S     // Speed                double
                >> m_bulletConfig.FR    // Fill Red             int
                >> m_bulletConfig.FG    // Fill Green           int 
                >> m_bulletConfig.FB    // Fill Blue            int    
                >> m_bulletConfig.OR    // Outline Red          int    
                >> m_bulletConfig.OG    // Outline Green        int    
                >> m_bulletConfig.OB    // Outline Blue         int    
                >> m_bulletConfig.OT    // Outline Thickness    double    
                >> m_bulletConfig.V     // Shape Vertices       int
                >> m_bulletConfig.L     // Lifespan             int
                >> m_bulletConfig.SI;   // Spawn Interval       int   
        }

        if (tag == "Joystick")
        {
            fin >> m_joystick;          // Connected            bool            
        }
    }  
}
void Game::initWindow()
{
    if (!m_windowConfig.mode) { m_Fullscreen = 7; }
    m_window.create( sf::VideoMode( m_windowConfig.width, m_windowConfig.height ), "GeoCrisis", m_Fullscreen );
    m_window.setFramerateLimit(m_windowConfig.frameLimit);
}
void Game::initText()
{
    if ( !m_font.loadFromFile(m_fontConfig.path) )
    {
        std::cerr << "Could not load font!\n";
        exit(-1);
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(m_fontConfig.size);
    m_text.setFillColor( sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B) );
}


void Game::run()
{
    while (m_running)
    {
        m_entityManager.update(); //update the entity manager
        ImGui::SFML::Update( m_window, m_deltaClock.restart() ); // required update call to ImGui
        
        if (m_joystick)                      sJoystick();
                                             sUserInput();
        if ( !m_paused )                     avoidAtPaused();
        if (m_player->cInput->specialWeapon) spawnSpecialWeapon();
                                             sGUI();
        if (m_GuiRender)                     sRender();

        // increment the current frame
        if ( !m_paused ) { m_currentFrame++; } 
    }
}
void Game::avoidAtPaused()
{
    if (m_player->cInput->shootK) initBullet( Vec2(sf::Mouse::getPosition(m_window).x, sf::Mouse::getPosition(m_window).y) );
    if (m_player->cInput->shootJ) initBullet( m_player->cInput->sFactor );
    if (m_GuiEnemySpawner)        sEnemySpawner(); 
    if (m_GuiCollision)           sCollision();
    if (m_GuiMovement)            sMovement();
    if (m_GuiLifespan)            sLifespan();
                                  initSpecialWeapon();
}





void Game::spawnPlayer()
{
    auto entity        = m_entityManager.addEntity("player");
    entity->cTransform = std::make_shared<CTransform> ( Vec2(m_windowConfig.width/2, m_windowConfig.height/2), Vec2(m_playerConfig.S), Vec2(0.0), 0.0 );
    entity->cShape     = std::make_shared<CShape> ( m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT );
    entity->cCollision = std::make_shared<CCollision> ( m_playerConfig.CR );  
    entity->cInput     = std::make_shared<CInput> ();
    entity->cScore     = std::make_shared<CScore> (0);

    m_player = entity;
}

void Game::initBullet(const Vec2& direction)
{
    if (m_currentFrame - m_lastBulletSpawnTime >= m_spawnIntervalBullet)
    {
        spawnBullet(m_player, direction);
    }
}
void Game::spawnBullet(std::shared_ptr<Entity> player, const Vec2& target)
{
    // velocity direction and magnitude calculation
    Vec2 velMag, velDir = target - player->cTransform->pos;
    velDir.normalize();

    velMag.assign(velDir).scale(m_bulletConfig.S).abs();
    velDir.unit(velDir);

    auto e = m_entityManager.addEntity("bullet");
    e->cTransform = std::make_shared<CTransform> (m_player->cTransform->pos, velMag, velDir, 0.0);
    e->cShape     = std::make_shared<CShape>     (m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    e->cCollision = std::make_shared<CCollision> (m_bulletConfig.CR); 
    e->cLifeSpan  = std::make_shared<CLifeSpan>  (m_bulletConfig.L);
    
    m_lastBulletSpawnTime = m_currentFrame;  
} 

void Game::initSpecialWeapon()
{
    if (m_frameSpecialWeapon < m_powerMoveInterval)
    {
        ++m_frameSpecialWeapon;
    }
    if (m_frameSpecialWeapon >= m_powerMoveInterval  && m_player->cInput->specialWeapon)
    {
        auto& vector = m_entityManager.getEntities("enemy");
        int halfSize = vector.size() / 2;
        m_frameSpecialWeapon = halfSize*8;
        m_paused = true;
    }
}
void Game::spawnSpecialWeapon()
{
    if (m_paused)
    {
        if (m_frameSpecialWeapon%8 == 0)
        {
            for (auto e:m_entityManager.getEntities("enemy"))
            {
                e->destroy(); 
                spawnSmallEnemies(e);
                break;
            }
        }

        for (auto e : m_entityManager.getEntities("enemySmall")) {
            e->cTransform->pos += e->cTransform->velMag * e->cTransform->velAxis;                
        }
        sLifespan();

        if (m_frameSpecialWeapon%6 == 3)
            for (auto& e : m_entityManager.getEntities())
            {
                e->cTransform->pos.m_x += 10;
            }
        if (m_frameSpecialWeapon%6 == 0)
            for (auto& e : m_entityManager.getEntities())
            {
                e->cTransform->pos.m_x -= 10;
            }
        
        if (!m_frameSpecialWeapon--) 
        {
            m_paused = false;
            m_frameSpecialWeapon = 0;
            m_player->cInput->specialWeapon = false;
        }
    }
    else m_player->cInput->specialWeapon = false;
}

void Game::sEnemySpawner()
{
    if ( m_currentFrame - m_lastEnemySpawnTime >= m_spawnIntervalEnemy) {
        spawnEnemy();
    }

}
void Game::spawnPosVel(int& xPos, int& yPos, int& speed, int& vertices, Vec2& velDir, Vec2& velMag)
{
    srand(time(0));
    std::random_device r;
    std::mt19937 generator(r());
    int yMin = - (2*m_enemyConfig.CR); int yMax =   (2*m_enemyConfig.CR) + m_windowConfig.height;
    int xMin = - (2*m_enemyConfig.CR); int xMax =   (2*m_enemyConfig.CR) + m_windowConfig.width;
            
    int rd = rand() % 2;
    if (rd)
    {
        std::uniform_int_distribution<int> ydiff(yMin, yMax);
        yPos = ydiff(generator);
        
        do {                                                         // xPos out position
            std::uniform_int_distribution<int> xdiff(xMin, xMax);
            xPos = xdiff(generator);
        } while ( (-m_enemyConfig.CR) <= xPos && xPos <= (m_enemyConfig.CR + m_windowConfig.width) );
    }

    else
    {
        std::uniform_int_distribution<int> xdiff(xMin, xMax);
        xPos = xdiff(generator);

        do {                                                         // yPos out position
            std::uniform_int_distribution<int> ydiff(yMin, yMax);
            yPos = ydiff(generator);
        } while ( (-m_enemyConfig.CR) <= yPos && yPos <= (m_enemyConfig.CR + m_windowConfig.height) );
    }


/*
    if (rd)
    {
        int yMin = - (2*m_enemyConfig.CR);
        int yMax =   (2*m_enemyConfig.CR) + m_windowConfig.height;
        int yDiff = yMax - yMin + 1;
        yPos = ( rand() % yDiff ) + yMin;
        
        do {                                                         // xPos out
            int xMin = - (2*m_enemyConfig.CR);
            int xMax =   (2*m_enemyConfig.CR) + m_windowConfig.width;
            int xDiff = xMax - xMin + 1;
            xPos = ( rand() % xDiff ) + xMin;
        } while ( (-m_enemyConfig.CR) <= xPos && xPos <= (m_enemyConfig.CR + m_windowConfig.width) );
    }

    else
    {
        int xMin = - (2*m_enemyConfig.CR);
        int xMax =   (2*m_enemyConfig.CR) + m_windowConfig.width;
        int xDiff = xMax - xMin + 1;
        xPos = ( rand() % xDiff ) + xMin;

        do {                                                         // yPos out
            int yMin = - (2*m_enemyConfig.CR);
            int yMax =   (2*m_enemyConfig.CR) + m_windowConfig.height;
            int yDiff = yMax - yMin + 1;
            yPos = ( rand() % yDiff ) + yMin;
        } while ( (-m_enemyConfig.CR) <= yPos && yPos <= (m_enemyConfig.CR + m_windowConfig.height) );
    }
*/

    //int diffVer = m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1;      
    //vertices = ( rand() % diffVer ) + m_enemyConfig.VMIN;        // entity vertices
    std::uniform_int_distribution<int> diffVer(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    vertices = diffVer(generator);

    int diffSpeed = m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1;
    speed = ( rand() % diffSpeed ) + m_enemyConfig.SMIN;         // entity speed

    //int degree = rand() % 360;
    std::uniform_int_distribution<int> dig(1, 360);
    int degree = dig(generator);
    velDir = velMag.unit(-degree);
    velMag.scale(speed).abs();                                   // velocity magnitude
    velDir.unit(velDir);                                         // velocity direction

}
void Game::spawnEnemy()
{
    int xPos, yPos, vertices, speed;
    Vec2 velDir, velMag;
    spawnPosVel(xPos, yPos, speed, vertices, velDir, velMag); 

    auto e = m_entityManager.addEntity("enemy");
    e->cTransform = std::make_shared<CTransform> ( Vec2(xPos, yPos), velMag, velDir, 0.0 ); e->cTransform->speed = speed;
    if (vertices == 3) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color(244, 100, 119), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    if (vertices == 4) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color(156, 237,  64), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    if (vertices == 5) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color(  9,  84, 124), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    if (vertices == 6) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color(201, 205, 219), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    if (vertices == 7) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color(177,  73, 229), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    if (vertices == 8) e->cShape = std::make_shared<CShape> ( m_enemyConfig.SR, vertices, sf::Color( 51,  23, 117), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT );
    e->cCollision = std::make_shared<CCollision> (m_enemyConfig.CR);
    e->cScore = std::make_shared<CScore> (100*vertices);
    
    m_lastEnemySpawnTime = m_currentFrame;
}
void Game::spawnVel(std::shared_ptr<Entity> entity, Vec2& velDir, Vec2& velMag, double degree)
{
    int speed = entity->cTransform->speed;      // entity speed
    
    velDir = velMag.unit(-degree);
    velMag = velDir;
    velMag.scale(speed).abs();                  // velocity magnitude
    velDir.unit(velDir);                        // velocity direction

}
void Game::spawnSmallEnemies(std::shared_ptr<Entity> eBig)
{
    double angle = 360;
    int vertices = eBig->cShape->circle.getPointCount();
    int count = 0;
    double diff = (1.0/vertices) * angle;
    
    while (count < vertices)
    {        
        Vec2 velDir, velMag;
        spawnVel(eBig, velDir, velMag, angle);
            
        auto eSmall = m_entityManager.addEntity("enemySmall");
        eSmall->cTransform = std::make_shared<CTransform> ( eBig->cTransform->pos, velMag, velDir, 0.0 );
        eSmall->cShape     = std::make_shared<CShape>     ( eBig->cShape->circle.getRadius()/2, vertices, eBig->cShape->FILL, eBig->cShape->OUTLINE, eBig->cShape->circle.getOutlineThickness() );
        eSmall->cCollision = std::make_shared<CCollision> ( eBig->cCollision->r/2 );
        eSmall->cScore     = std::make_shared<CScore>     ( eBig->cScore->score*2 );
        eSmall->cLifeSpan  = std::make_shared<CLifeSpan>  ( m_enemyConfig.L );
        angle -= diff;
        ++count;
    }
}

void Game::sCollision()
{
    for ( auto b : m_entityManager.getEntities("bullet") ) { // dave did this call by value, not reference
        for ( auto e : m_entityManager.getEntities("enemy") ) {
            if ( b->isActive() && e->isActive() && collisionEntity(b, e) ) { spawnSmallEnemies(e); b->destroy(); e->destroy(); m_player->cScore->score += e->cScore->score; }
        }
        for ( auto e : m_entityManager.getEntities("enemySmall") ) {
            if ( b->isActive() && e->isActive() && collisionEntity(b, e) ) { b->destroy(); e->destroy(); m_player->cScore->score += e->cScore->score; }
        }
    }
    for ( auto e : m_entityManager.getEntities("enemy") ) {
            if ( m_player->isActive() && e->isActive() && collisionEntity(m_player, e) ) 
            { 
                m_score += m_player->cScore->score; spawnSmallEnemies(e); m_player->destroy(); e->destroy(); spawnPlayer(); 
            }
    }

    // enemy m_window border bound
    for (auto& e : m_entityManager.getEntities("enemy") ) {
        collisionBounceBoarder(e);
    }

    // m_player m_window border bound
    if ( m_player->cInput ) {
        collisionInputBound(m_player);
    }
}
bool Game::collisionEntity (std::shared_ptr<Entity> head, std::shared_ptr<Entity> tail)
{
    Vec2 headPos(head->cTransform->pos.m_x, head->cTransform->pos.m_y);
    Vec2 tailPos(tail->cTransform->pos.m_x, tail->cTransform->pos.m_y);
    double distSq = headPos.distSq(tailPos);
    double distRadiusSq = (head->cCollision->r + tail->cCollision->r) * (head->cCollision->r + tail->cCollision->r);
    return ( (distRadiusSq >= distSq) ?  true : false ); // (r1 + r2)^2  >= (x1 - x2)^2 + (y1 - y2)^2
}
void Game::collisionBounceBoarder( std::shared_ptr<Entity> e )
{    
    if ( e->cTransform->pos.m_x + e->cCollision->r >= m_windowConfig.width )            // Right side
    {                                                                                   
        if (e->cTransform->velAxis.m_x > 0) { e->cTransform->velAxis.m_x *= -1; }       
    }                                                                                                        
    if ( e->cTransform->pos.m_x - e->cCollision->r <= 0 )                               // Left side
    {                                                                                                                    
        if (e->cTransform->velAxis.m_x < 0) { e->cTransform->velAxis.m_x *= -1; }       
    }                                                                                                                     
    if ( e->cTransform->pos.m_y + e->cCollision->r >= m_windowConfig.height )           // Down side
    {                                                                                                                 
        if (e->cTransform->velAxis.m_y > 0) { e->cTransform->velAxis.m_y *= -1; }           
    }                                                                                                                   
        if ( e->cTransform->pos.m_y - e->cCollision->r <= 0 )                           // Up side
    {                                                                                                                    
        if (e->cTransform->velAxis.m_y < 0) { e->cTransform->velAxis.m_y *= -1; }
    }
}
void Game::collisionInputBound( std::shared_ptr<Entity> entity )
{
    if ( entity->cTransform->pos.m_x + entity->cCollision->r >= m_windowConfig.width )   // Right side
    {                                                                                                                   
        entity->cInput->right = false;
    }                                                                                                        
    if ( entity->cTransform->pos.m_x - entity->cCollision->r <= 0 )                      // Left side
    {                                                                                                                   
        entity->cInput->left = false;
    }                                                                                                                     
    if ( entity->cTransform->pos.m_y + entity->cCollision->r >= m_windowConfig.height )  // Down side
    {                                                                                                                   
        entity->cInput->down = false;
    }                                                                                                                   
    if ( entity->cTransform->pos.m_y - entity->cCollision->r <= 0 )                      // Up side
    {                                                                                                                   
        entity->cInput->up = false;
    }
}

void Game::sMovement()
{
    // Entity movement mechanism
    for (auto e : m_entityManager.getEntities()) {
        if ( (!e->cInput) && e->cTransform ) {
            e->cTransform->pos += e->cTransform->velMag * e->cTransform->velAxis;                
        }
    }

    // player movement mechanism
    inputMovement(m_player);
}
void Game::inputMovement( std::shared_ptr<Entity> entity )
{
    // joystick movement
    if (m_player->cInput->jMove)
    {
        if (entity->cInput->up)     { entity->cTransform->velAxis.m_y = -1; }
        if (entity->cInput->left)   { entity->cTransform->velAxis.m_x = -1; }
        if (entity->cInput->down)   { entity->cTransform->velAxis.m_y =  1; }
        if (entity->cInput->right)  { entity->cTransform->velAxis.m_x =  1; }
        
        // Fixes the amplitude in diagonal movement
        if (entity->cInput->mFactor.distSq( Vec2(0.0) ) == 2)
        {
            entity->cTransform->velAxis.diagonalFix();
        }

        // Sample movement speed and rotation update
        entity->cTransform->pos += entity->cTransform->velMag * entity->cTransform->velAxis * entity->cInput->mFactor;

        if (!entity->cInput->up)    { entity->cTransform->velAxis.m_y =  0; }
        if (!entity->cInput->left)  { entity->cTransform->velAxis.m_x =  0; }
        if (!entity->cInput->down)  { entity->cTransform->velAxis.m_y =  0; }
        if (!entity->cInput->right) { entity->cTransform->velAxis.m_x =  0; }
    }

    // keyboard movement
    else if (entity->cInput->up || entity->cInput->left || entity->cInput->down || entity->cInput->right)
    {
        if (entity->cInput->up)     { entity->cTransform->velAxis.m_y = -1; }
        if (entity->cInput->left)   { entity->cTransform->velAxis.m_x = -1; }
        if (entity->cInput->down)   { entity->cTransform->velAxis.m_y =  1; }
        if (entity->cInput->right)  { entity->cTransform->velAxis.m_x =  1; }
        
        // Fixes the amplitude in diagonal movement
        if ( (entity->cInput->up   && ( entity->cInput->left || entity->cInput->right )) || 
             (entity->cInput->down && ( entity->cInput->left || entity->cInput->right )) )
        {
            entity->cTransform->velAxis.diagonalFix();
        }

        // movement speed and rotation update
        entity->cTransform->pos += entity->cTransform->velMag * entity->cTransform->velAxis;

        if (!entity->cInput->up)    { entity->cTransform->velAxis.m_y =  0; }
        if (!entity->cInput->left)  { entity->cTransform->velAxis.m_x =  0; }
        if (!entity->cInput->down)  { entity->cTransform->velAxis.m_y =  0; }
        if (!entity->cInput->right) { entity->cTransform->velAxis.m_x =  0; }
    }
}

void Game::sLifespan()
{
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->cLifeSpan && e->cShape)
        {
            if( e->cLifeSpan->remaining-- ) 
            {
                double k = (double) e->cLifeSpan->remaining / e->cLifeSpan->total;
                e->cShape->FILL.a    = 255*k;
                e->cShape->OUTLINE.a = 255*k;

                e->cShape->circle.setFillColor( e->cShape->FILL );
                e->cShape->circle.setOutlineColor( e->cShape->OUTLINE );
            }
            else { e->destroy(); }
        }
    }
}



void Game::RGBtoHSV(sf::Color& rgb, double& h, double& s, double& v)
{
    // R, G, B values are divided by 255
    // to change the range from 0..255 to 0..1
    double r = rgb.r / 255.0;
    double g = rgb.g / 255.0;
    double b = rgb.b / 255.0;

    // h, s, v = hue, saturation, value
    double cmax = r > g ? (r > b ? r : b) : (g > b ? g : b); // maximum of r, g, b
    double cmin = r < g ? (r < b ? r : b) : (g < b ? g : b); // minimum of r, g, b
    double diff = cmax - cmin; // diff of cmax and cmin.
    h = -1, s = -1;

    // if cmax and cmax are equal then h = 0
    if (cmax == cmin)
        h = 0;

    // if cmax equal r then compute h
    else if (cmax == r)
        h = fmod(60 * ((g - b) / diff) + 360, 360);

    // if cmax equal g then compute h
    else if (cmax == g)
        h = fmod(60 * ((b - r) / diff) + 120, 360);

    // if cmax equal b then compute h
    else if (cmax == b)
        h = fmod(60 * ((r - g) / diff) + 240, 360);

    // if cmax equal zero
    if (cmax == 0)
        s = 0;
    else
        s = (diff / cmax) * 100;

    // compute v
    v = cmax * 100;

    // This code is contributed by phasing17
}
void Game::sGUI() 
{
    ImGui::Begin("GeoCrisis");
    int i = 0;

    if (ImGui::BeginTabBar("MyTabBar"))
    {
        if (ImGui::BeginTabItem("System"))
        {
            ImGui::Checkbox("Enemy Spawning", &m_GuiEnemySpawner);
            ImGui::SliderInt("Spawn Interval", &m_spawnIntervalEnemy, 1, 512);
            if ( ImGui::Button("Manual Spawn") ) {
                spawnEnemy();
            }
            ImGui::Checkbox("Collison", &m_GuiCollision);
            ImGui::Checkbox("Movement", &m_GuiMovement);
            ImGui::Checkbox("Lifespan", &m_GuiLifespan);
            ImGui::Checkbox("Rendering", &m_GuiRender);
            if ( ImGui::Button("Delete All (except player)") ) {
                for (auto& e : m_entityManager.getEntities()) {
                    if (!e->cInput) e->destroy();
                }
            }
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Entity Manager"))
        {
            if ( ImGui::CollapsingHeader("Enemies by Tag") )
            {
                ImGui::Indent();

                if ( ImGui::CollapsingHeader("bullet") )
                {
                    i = 0;
                    for (auto& e : m_entityManager.getEntities("bullet"))
                    {
                        std::string str = "D##" + std::to_string(i++);
                        if (ImGui::Button(&str[0]))
                        {
                            e->destroy();
                        }
                        ImGui::SameLine();
                        ImGui::Text(" %3d  %10s    (%4d,%4d)", e->id(), e->tag().c_str(), (int)e->cTransform->pos.m_x, (int)e->cTransform->pos.m_y);
                    }

                }
                if ( ImGui::CollapsingHeader("enemy") )
                {
                    i = 0;
                    for (auto& e : m_entityManager.getEntities("enemy"))
                    {
                        std::string str = "D##" + std::to_string(i++);
                        if (ImGui::Button(&str[0]))
                        {
                            e->destroy();
                        }
                        ImGui::SameLine();
                        ImGui::Text(" %3d  %10s    (%4d,%4d)", e->id(), e->tag().c_str(), (int)e->cTransform->pos.m_x, (int)e->cTransform->pos.m_y);
                    }
                }
                if ( ImGui::CollapsingHeader("player") )
                {
                    i = 0;
                    for (auto& e : m_entityManager.getEntities("player"))
                    {
                        std::string str = "D##" + std::to_string(i++);
                        if (ImGui::Button(&str[0]))
                        {
                            e->destroy();
                        }
                        ImGui::SameLine();
                        ImGui::Text(" %3d  %10s    (%4d,%4d)", e->id(), e->tag().c_str(), (int)e->cTransform->pos.m_x, (int)e->cTransform->pos.m_y);
                    }
                }
                if ( ImGui::CollapsingHeader("enemySmall") )
                {
                    i = 0;
                    for (auto& e : m_entityManager.getEntities("enemySmall"))
                    {
                        std::string str = "D##" + std::to_string(i++);
                        if (ImGui::Button(&str[0]))
                        {
                            e->destroy();
                        }
                        ImGui::SameLine();
                        ImGui::Text(" %3d  %10s    (%4d,%4d)", e->id(), e->tag().c_str(), (int)e->cTransform->pos.m_x, (int)e->cTransform->pos.m_y);
                    }
                }

                ImGui::Unindent();
            }
            if ( ImGui::CollapsingHeader("All Entities") )
            {
                ImGui::Indent();
                i = 0;
                for (auto& e : m_entityManager.getEntities())
                {
                    ImGui::PushID(i++);
                    double h, s, v;
                    RGBtoHSV(e->cShape->FILL, h, s, v);
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(h/360, s/100, v/100));
                    if (ImGui::Button("D"))
                    {
                        e->destroy();
                    }
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::Text(" %3d  %10s    (%4d,%4d)", e->id(), e->tag().c_str(), (int)e->cTransform->pos.m_x, (int)e->cTransform->pos.m_y);
                }

                ImGui::Unindent();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Game::sRender()
{
    m_window.clear();
    for (auto& e : m_entityManager.getEntities())
    {
        if ( (!e->cInput) && e->cShape && e->cTransform)
        {
            e->cTransform->angle += 2;
            e->cShape->circle.setPosition( e->cTransform->pos.m_x, e->cTransform->pos.m_y );  
            e->cShape->circle.setRotation( e->cTransform->angle );
            m_window.draw( e->cShape->circle );
        }
    }
    m_player->cTransform->angle += 2;
    m_player->cShape->circle.setPosition( m_player->cTransform->pos.m_x, m_player->cTransform->pos.m_y );
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);
    m_window.draw(m_player->cShape->circle);
    
    // Score board
    std::string score = "Score: " + std::to_string(m_player->cScore->score);
    m_text.setString(score);
    m_text.setPosition(0, 0);
    m_window.draw(m_text);

    // special weapon cooldown
    double cooldown = m_frameSpecialWeapon / (double)m_powerMoveInterval;
    std::string str = "Bankai: " + std::to_string((int)(cooldown*100)) + "%";
    m_text.setString(str);
    m_text.setPosition(0, m_windowConfig.height - m_text.getCharacterSize() - 5);
    if (!m_paused) m_window.draw(m_text);
    
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sJoystick()
{
    // movement input
    double xAxis = 0, yAxis = 0;
    xAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X); if (-1 < xAxis && xAxis < 1) xAxis = 0.0;
    yAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y); if (-1 < yAxis && yAxis < 1) yAxis = 0.0;
    if (xAxis || yAxis) 
    { 
        m_player->cInput->jMove = true;
        m_player->cInput->mFactor = Vec2(abs(xAxis)/100, abs(yAxis)/100); 
    }
    else
    {
        m_player->cInput->jMove = false;
        m_player->cInput->mFactor = Vec2(0.0);
    }
    if (xAxis >  1) { m_player->cInput->right = true; } else m_player->cInput->right = false;
    if (xAxis < -1) { m_player->cInput->left  = true; } else m_player->cInput->left  = false;
    if (yAxis >  1) { m_player->cInput->down  = true; } else m_player->cInput->down  = false;
    if (yAxis < -1) { m_player->cInput->up    = true; } else m_player->cInput->up    = false;

    // shooting
    double z = 0, r = 0;
    z = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
    r = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
    if (z >  1 || z < -1 || r >  1 || r < -1)
    {
        m_player->cInput->shootJ = true;
        m_player->cInput->sFactor = m_player->cTransform->pos + Vec2(z, r);
    }
    else m_player->cInput->shootJ = false;

    // Misc
    if (sf::Joystick::isButtonPressed(0, 5)) { m_player->cInput->specialWeapon = true; }
    if (sf::Joystick::isButtonPressed(0, 6)) { m_running = false; }
    if (sf::Joystick::isButtonPressed(0, 7)) { m_paused ? m_paused = false : m_paused = true; }
}

void Game::sUserInput()
{
    sf::Event event;
    while ( m_window.pollEvent(event) )
    {
        // pass the event to ImGui to be parsed
        ImGui::SFML::ProcessEvent(m_window, event);

        // this event is triggered when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if ( event.type == sf::Event::KeyPressed )
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape: m_running = false; break;
                case sf::Keyboard::P: { m_paused ? (m_paused = false) : (m_paused = true); break; }
    
                case sf::Keyboard::W: { m_player->cInput->up    = true; break; }                
                case sf::Keyboard::A: { m_player->cInput->left  = true; break; }                
                case sf::Keyboard::S: { m_player->cInput->down  = true; break; }                
                case sf::Keyboard::D: { m_player->cInput->right = true; break; }
                default: break;
            }
        }

        // this event is triggered when a key is released
        if ( event.type == sf::Event::KeyReleased )
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W: { m_player->cInput->up    = false; break; }                
                case sf::Keyboard::A: { m_player->cInput->left  = false; break; }                
                case sf::Keyboard::S: { m_player->cInput->down  = false; break; }                
                case sf::Keyboard::D: { m_player->cInput->right = false; break; }                
                default: break;
            }
        }

        if ( event.type == sf::Event::MouseButtonPressed )
        {
            if ( ImGui::GetIO().WantCaptureMouse ) { continue; }

            if ( event.mouseButton.button == sf::Mouse::Left )
            {
                //std::cout << "Left Mouse Button clicked at " << "(" << event.mouseButton.x << "," << event.mouseButton.y << ")" << std::endl;
                m_player->cInput->shootK = true;
            }

            if ( event.mouseButton.button == sf::Mouse::Right )
            {
                //std::cout << "Right Mouse Button clicked at " << "(" << event.mouseButton.x << "," << event.mouseButton.y << ")" << std::endl;
                m_player->cInput->specialWeapon = true;
            }
        }

        if ( event.type == sf::Event::MouseButtonReleased )
        {
            if ( event.mouseButton.button == sf::Mouse::Left )
            {
                m_player->cInput->shootK = false;
            }
        }
    }
}
