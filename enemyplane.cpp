#include "enemyplane.h"
#include <QDebug>

EnemyPlane::EnemyPlane()
    : m_type(EnemyLight)
    , m_x(0)
    , m_y(0)
    , m_hp(0)
    , m_maxHp(0)
    , m_speed(0)
    , m_scoreValue(0)
    , m_free(true)
    , m_horizontalDirection(1)
    , m_moveRecorder(0)
    , m_shootElapsedMs(0)
    , m_shootInterval(0)
{
    m_pixmapLight.load(LIGHT_ENEMY_PATH);
    if (m_pixmapLight.isNull())
    {
        qWarning() << "Failed to load light enemy image:" << LIGHT_ENEMY_PATH;
    }
    m_pixmapHeavy.load(HEAVY_ENEMY_PATH);
    if (m_pixmapHeavy.isNull())
    {
        qWarning() << "Failed to load heavy enemy image:" << HEAVY_ENEMY_PATH;
    }
    m_rect.moveTo(-100, -100);
}

int EnemyPlane::pixmapWidthForType(EnemyType type) const
{
    if (type == EnemyHeavy)
    {
        return m_pixmapHeavy.width();
    }
    return m_pixmapLight.width();
}

void EnemyPlane::activate(EnemyType type, int x, int y)
{
    m_type = type;

    if (type == EnemyLight)
    {
        m_pixmap        = m_pixmapLight;
        m_hp            = LIGHT_ENEMY_HP;
        m_maxHp         = LIGHT_ENEMY_HP;
        m_speed         = LIGHT_ENEMY_SPEED;
        m_scoreValue    = LIGHT_ENEMY_SCORE;
        m_shootInterval = LIGHT_ENEMY_SHOOT_INTERVAL;
    }
    else
    {
        m_pixmap        = m_pixmapHeavy;
        m_hp            = HEAVY_ENEMY_HP;
        m_maxHp         = HEAVY_ENEMY_HP;
        m_speed         = HEAVY_ENEMY_SPEED;
        m_scoreValue    = HEAVY_ENEMY_SCORE;
        m_shootInterval = HEAVY_ENEMY_SHOOT_INTERVAL;
    }

    m_x = x;
    m_y = y;
    m_free = false;

    m_horizontalDirection = 1;
    m_moveRecorder  = 0;
    m_shootElapsedMs = 0;

    m_rect.setWidth(m_pixmap.width());
    m_rect.setHeight(m_pixmap.height());
    m_rect.moveTo(m_x, m_y);
}

void EnemyPlane::updatePosition()
{
    if (m_free)
    {
        return;
    }

    // 垂直移动
    m_y += m_speed;

    // 轻型敌机：水平摆动
    if (m_type == EnemyLight)
    {
        m_moveRecorder++;
        if (m_moveRecorder % 3 == 0)
        {
            m_x += m_horizontalDirection;

            // 到达左右边界时反转方向
            if (m_x <= 0)
            {
                m_x = 0;
                m_horizontalDirection = 1;
            }
            else if (m_x >= GAME_WIDTH - m_pixmap.width())
            {
                m_x = GAME_WIDTH - m_pixmap.width();
                m_horizontalDirection = -1;
            }
        }
    }

    m_rect.moveTo(m_x, m_y);

    // 完全飞出屏幕底部时回收
    if (m_y >= GAME_HEIGHT)
    {
        reset();
    }
}

void EnemyPlane::takeDamage(int damage)
{
    if (m_free)
    {
        return;
    }
    if (damage <= 0)
    {
        return;
    }
    m_hp -= damage;
}

bool EnemyPlane::isDead() const
{
    return !m_free && m_hp <= 0;
}

void EnemyPlane::reset()
{
    m_free = true;
    m_hp   = 0;
    m_rect.moveTo(-100, -100);
    m_x = 0;
    m_y = 0;
    m_shootElapsedMs = 0;
}

bool EnemyPlane::updateAndCheckShoot(int deltaMs)
{
    if (m_free)
    {
        return false;
    }

    // 敌机尚未进入可见区域时不射击
    if (m_y + m_pixmap.height() < 0)
    {
        return false;
    }

    m_shootElapsedMs += deltaMs;
    if (m_shootElapsedMs >= m_shootInterval)
    {
        m_shootElapsedMs = 0;
        return true;
    }
    return false;
}

void EnemyPlane::resetShootTimer()
{
    m_shootElapsedMs = 0;
}
