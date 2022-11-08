enum IDS { PLAYER, BULLET, ENEMY , SCOPE };

typedef struct 
{
    int ID;
    int x;
    int y;
    int lives;
    int speed;
    int boundx;
    int boundy;
    int score;
} SpaceShip;

typedef struct
{
    int ID;
    int x;
    int y;
    bool live;
    int speed;
} Bullet;

typedef struct
{
    int ID;
    int x;
    int y;
    bool live;
    int lifetime;
    //int speed;
    int boundx;
    int boundy;
} Comet;

typedef struct
{
    int ID;
    int x;
    int y;
    bool live;
    int clickx;
    int clicky;
    int score;
    int lives;
} mouseScope;