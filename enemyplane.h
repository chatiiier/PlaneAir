#ifndef ENEMYPLANE_H
#define ENEMYPLANE_H

#include <QPixmap>
#include <QRect>
#include "config.h"

// 敌机类型
enum EnemyType
{
    EnemyLight,
    EnemyHeavy
};

// 普通敌机（不继承 QObject，无 Q_OBJECT，无 QTimer）
class EnemyPlane
{
public:
    EnemyPlane();

    void activate(EnemyType type, int x, int y);
    void updatePosition();
    void takeDamage(int damage);
    bool isDead() const;
    void reset();

    // 射击计时
    bool updateAndCheckShoot(int deltaMs);
    void resetShootTimer();

    // 根据类型返回预加载图片的宽度（用于 spawnEnemy 计算合法 X）
    int pixmapWidthForType(EnemyType type) const;

    EnemyType m_type;

    QPixmap m_pixmap;
    QPixmap m_pixmapLight;
    QPixmap m_pixmapHeavy;

    QRect m_rect;

    int m_x;
    int m_y;

    int m_hp;
    int m_maxHp;
    int m_speed;
    int m_scoreValue;

    bool m_free;

    int m_horizontalDirection;
    int m_moveRecorder;

    // 射击计时（每架敌机独立）
    int m_shootElapsedMs;
    int m_shootInterval;
};

#endif // ENEMYPLANE_H
