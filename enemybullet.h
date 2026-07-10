#ifndef ENEMYBULLET_H
#define ENEMYBULLET_H

#include <QPixmap>
#include <QRect>
#include "config.h"

// 敌方子弹类型
enum EnemyBulletType
{
    EnemyBulletLight,
    EnemyBulletHeavy
};

// 敌方子弹（不继承 QObject，无 Q_OBJECT，无 QTimer）
class EnemyBullet
{
public:
    EnemyBullet();

    void activate(EnemyBulletType type, int x, int y);
    void updatePosition();
    void reset();

    EnemyBulletType m_type;

    QPixmap m_pixmap;
    QRect m_rect;

    int m_x;
    int m_y;
    int m_speed;
    int m_damage;

    bool m_free;

private:
    QPixmap m_lightPixmap;
    QPixmap m_heavyPixmap;
};

#endif // ENEMYBULLET_H
