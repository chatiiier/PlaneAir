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

#endif // CONFIG_H

