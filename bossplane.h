#ifndef BOSSPLANE_H
#define BOSSPLANE_H

#include <QPixmap>
#include <QRect>
#include "config.h"

// Boss 移动状态
enum BossMoveState
{
    BossInactive,
    BossEntering,
    BossActive
};

// Boss（不继承 QObject，无 Q_OBJECT，无 QTimer）
class BossPlane
{
public:
    BossPlane();

    void activate();
    void updatePosition();
    void reset();

    bool isActive() const;
    bool hasEntered() const;

    QPixmap m_pixmap;
    QRect m_rect;

    int m_x;
    int m_y;

    int m_hp;
    int m_maxHp;

    int m_horizontalDirection;

    BossMoveState m_moveState;
};

#endif // BOSSPLANE_H
