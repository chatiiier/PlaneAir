#include "herobullet.h"
#include <QDebug>

HeroBullet::HeroBullet()
{
    m_bullet.load(HERO_BULLET_PATH);
    if (m_bullet.isNull())
    {
        qWarning() << "Failed to load bullet image:" << HERO_BULLET_PATH;
    }
    m_speed = HERO_BULLET_SPEED;
    m_free  = true;
    m_x     = 0;
    m_y     = 0;
    m_rect.setWidth(m_bullet.width());
    m_rect.setHeight(m_bullet.height());
    m_rect.moveTo(-100, -100);
}

void HeroBullet::activate(int x, int y)
{
    m_free = false;
    m_x    = x;
    m_y    = y;
    m_rect.moveTo(m_x, m_y);
}

void HeroBullet::updatePosition()
{
    if (m_free)
    {
        return;
    }

    m_y -= m_speed;
    m_rect.moveTo(m_x, m_y);

    // 子弹完全飞出屏幕顶部时回收
    if (m_y + m_bullet.height() < 0)
    {
        reset();
    }
}

void HeroBullet::reset()
{
    m_free = true;
    m_rect.moveTo(-100, -100);
}
