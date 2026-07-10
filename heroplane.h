#ifndef HEROPLANE_H
#define HEROPLANE_H

#include <QPixmap>
#include <QRect>
#include "config.h"
#include "herobullet.h"

class HeroPlane
{
public:
    HeroPlane();

    // 移动位置
    void setPostion(int x, int y);

    // 子弹发射行为
    void shoot();

    // 更新武器状态（每帧调用，deltaMs 为距上次调用的毫秒数）
    void updateWeapon(int deltaMs);

    // 主动换弹
    void startReload();

    // 重置武器状态
    void resetWeapon();

    // 飞机资源
    QPixmap m_plane;

    int m_x;
    int m_y;

    QRect m_rect;

    // 英雄子弹对象池
    HeroBullet m_bullets[HERO_BULLET_POOL_SIZE];

    // 武器状态
    int  m_currentAmmo;
    bool m_reloading;
    int  m_shootElapsedMs;
    int  m_reloadElapsedMs;
};

#endif // HEROPLANE_H
