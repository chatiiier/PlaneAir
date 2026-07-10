#ifndef BOMB_H
#define BOMB_H

#include <QPixmap>
#include "config.h"

// 爆炸动画（不继承 QObject，无 Q_OBJECT，无 QTimer）
class Bomb
{
public:
    Bomb();

    void activate(int centerX, int centerY);
    void updateAnimation(int deltaMs);
    void reset();

    QPixmap m_frames[BOMB_FRAME_COUNT];

    int m_x;
    int m_y;
    int m_currentFrame;
    int m_elapsedMs;

    bool m_free;
};

#endif // BOMB_H
