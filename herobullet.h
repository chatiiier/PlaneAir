#ifndef HEROBULLET_H
#define HEROBULLET_H

#include <QPixmap>
#include <QRect>
#include "config.h"

class HeroBullet
{
public:
    HeroBullet();

    void activate(int x, int y);
    void updatePosition();
    void reset();

    QPixmap m_bullet;
    QRect m_rect;

    int m_x;
    int m_y;
    int m_speed;

    bool m_free;
};

#endif // HEROBULLET_H
