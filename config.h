#ifndef CONFIG_H
#define CONFIG_H

// 集中配置飞机大战的详细参数及资源路径

// 主场景的配置
#define GAME_WIDTH  512
#define GAME_HEIGHT 768
#define GAME_TITLE  "飞机大战V1.0"
#define GAME_ICON   ":/res/app.ico"

// 地图配置
#define MAP_PATH        ":/res/img_bg_level_1.jpg"
#define MAP_SCOLL_SPEED 2
#define GAME_RATE       10

// 英雄机的配置
#define HERO_PLANE ":/res/hero2.png"
#define HERO_SPEED 2

// 英雄子弹配置
#define HERO_BULLET_PATH      ":/res/hero_bullet_7.png"
#define HERO_BULLET_SPEED     8
#define HERO_BULLET_POOL_SIZE 50

// 弹夹配置
#define HERO_MAGAZINE_CAPACITY 20
#define HERO_SHOOT_INTERVAL    150
#define HERO_RELOAD_TIME       1000

#define HERO_BULLET_DAMAGE 1

// 玩家生命与无敌配置
#define HERO_MAX_HP          3
#define HERO_INVINCIBLE_TIME 1000
#define HERO_BLINK_INTERVAL  100

// 敌方子弹配置
#define ENEMY_BULLET_POOL_SIZE 80

#define LIGHT_ENEMY_BULLET_PATH  ":/res/bullet_4.png"
#define LIGHT_ENEMY_SHOOT_INTERVAL 700
#define LIGHT_ENEMY_BULLET_SPEED   6
#define LIGHT_ENEMY_BULLET_DAMAGE  1

#define HEAVY_ENEMY_BULLET_PATH  ":/res/bullet_6.png"
#define HEAVY_ENEMY_SHOOT_INTERVAL 1600
#define HEAVY_ENEMY_BULLET_SPEED   4
#define HEAVY_ENEMY_BULLET_DAMAGE  2

// 爆炸动画配置
#define BOMB_POOL_SIZE     20
#define BOMB_FRAME_COUNT   7
#define BOMB_FRAME_INTERVAL 60

#define BOMB_PATH_1 ":/res/bomb-1.png"
#define BOMB_PATH_2 ":/res/bomb-2.png"
#define BOMB_PATH_3 ":/res/bomb-3.png"
#define BOMB_PATH_4 ":/res/bomb-4.png"
#define BOMB_PATH_5 ":/res/bomb-5.png"
#define BOMB_PATH_6 ":/res/bomb-6.png"
#define BOMB_PATH_7 ":/res/bomb-7.png"

// 敌机配置
#define ENEMY_POOL_SIZE        20
#define ENEMY_SPAWN_INTERVAL   700

#define LIGHT_ENEMY_PATH       ":/res/img-plane_1.png"
#define LIGHT_ENEMY_HP         2
#define LIGHT_ENEMY_SPEED      4
#define LIGHT_ENEMY_SCORE      10
#define LIGHT_ENEMY_PROBABILITY 70

#define HEAVY_ENEMY_PATH       ":/res/img-plane_7.png"
#define HEAVY_ENEMY_HP         8
#define HEAVY_ENEMY_SPEED      2
#define HEAVY_ENEMY_SCORE      30

#endif // CONFIG_H

