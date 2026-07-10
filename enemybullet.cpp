#include "enemybullet.h"
#include <QDebug>

EnemyBullet::EnemyBullet()
    : m_type(EnemyBulletLight)
    , m_x(0)
    , m_y(0)
    , m_speed(0)
    , m_damage(0)
    , m_free(true)
{
    m_lightPixmap.load(LIGHT_ENEMY_BULLET_PATH);
    if (m_lightPixmap.isNull())
    {
        qWarning() << "Failed to load light enemy bullet:" << LIGHT_ENEMY_BULLET_PATH;
    }
    m_heavyPixmap.load(HEAVY_ENEMY_BULLET_PATH);
    if (m_heavyPixmap.isNull())
    {
        qWarning() << "Failed to load heavy enemy bullet:" << HEAVY_ENEMY_BULLET_PATH;
    }
    m_rect.moveTo(-100, -100);
}

void EnemyBullet::activate(EnemyBulletType type, int x, int y)
{
    m_type = type;

    if (type == EnemyBulletLight)
    {
        m_pixmap = m_lightPixmap;
        m_speed  = LIGHT_ENEMY_BULLET_SPEED;
        m_damage = LIGHT_ENEMY_BULLET_DAMAGE;
    }
    else
    {
        m_pixmap = m_heavyPixmap;
        m_speed  = HEAVY_ENEMY_BULLET_SPEED;
        m_damage = HEAVY_ENEMY_BULLET_DAMAGE;
    }

    m_x    = x;
    m_y    = y;
    m_free = false;

    m_rect.setWidth(m_pixmap.width());
    m_rect.setHeight(m_pixmap.height());
    m_rect.moveTo(m_x, m_y);
}

void EnemyBullet::updatePosition()
{
    if (m_free)
    {
        return;
    }

    m_y += m_speed;
    m_rect.moveTo(m_x, m_y);

    // 完全飞出屏幕底部时回收
    if (m_y >= GAME_HEIGHT)
    {
        reset();
    }
}

void EnemyBullet::reset()
{
    m_free = true;
    m_rect.moveTo(-100, -100);
    m_x = 0;
    m_y = 0;
}
