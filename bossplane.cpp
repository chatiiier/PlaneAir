#include "bossplane.h"
#include <QDebug>

BossPlane::BossPlane()
    : m_x(0)
    , m_y(0)
    , m_hp(0)
    , m_maxHp(0)
    , m_horizontalDirection(1)
    , m_moveState(BossInactive)
{
    QPixmap original;
    original.load(BOSS_PLANE_PATH);
    if (original.isNull())
    {
        qWarning() << "Failed to load boss image:" << BOSS_PLANE_PATH;
    }
    else
    {
        // 只缩放一次
        int w = original.width() * BOSS_SCALE_PERCENT / 100;
        int h = original.height() * BOSS_SCALE_PERCENT / 100;
        m_pixmap = original.scaled(w, h,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);
    }

    reset();
}

void BossPlane::activate()
{
    m_hp    = BOSS_MAX_HP;
    m_maxHp = BOSS_MAX_HP;

    m_horizontalDirection = 1;

    // 水平居中，顶部外
    m_x = GAME_WIDTH / 2 - m_pixmap.width() / 2;
    m_y = -m_pixmap.height();

    m_moveState = BossEntering;

    m_rect.setWidth(m_pixmap.width());
    m_rect.setHeight(m_pixmap.height());
    m_rect.moveTo(m_x, m_y);
}

void BossPlane::updatePosition()
{
    if (m_moveState == BossInactive)
    {
        return;
    }

    if (m_moveState == BossEntering)
    {
        m_y += BOSS_ENTER_SPEED;
        if (m_y >= BOSS_TARGET_Y)
        {
            m_y = BOSS_TARGET_Y;
            m_moveState = BossActive;
        }
        m_rect.moveTo(m_x, m_y);
        return;
    }

    // BossActive：水平往返
    m_x += BOSS_MOVE_SPEED * m_horizontalDirection;

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

    m_rect.moveTo(m_x, m_y);
}

void BossPlane::reset()
{
    m_moveState = BossInactive;
    m_hp        = BOSS_MAX_HP;
    m_maxHp     = BOSS_MAX_HP;
    m_horizontalDirection = 1;

    m_x = GAME_WIDTH / 2 - m_pixmap.width() / 2;
    m_y = -m_pixmap.height();

    m_rect.setWidth(m_pixmap.width());
    m_rect.setHeight(m_pixmap.height());
    m_rect.moveTo(m_x, m_y);
}

bool BossPlane::isActive() const
{
    return m_moveState != BossInactive;
}

bool BossPlane::hasEntered() const
{
    return m_moveState == BossActive;
}
