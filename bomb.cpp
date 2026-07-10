#include "bomb.h"
#include <QDebug>

Bomb::Bomb()
    : m_x(0)
    , m_y(0)
    , m_currentFrame(0)
    , m_elapsedMs(0)
    , m_free(true)
{
    // 加载爆炸帧图片
    const char *bombPaths[BOMB_FRAME_COUNT] = {
        BOMB_PATH_1, BOMB_PATH_2, BOMB_PATH_3, BOMB_PATH_4,
        BOMB_PATH_5, BOMB_PATH_6, BOMB_PATH_7
    };

    for (int i = 0; i < BOMB_FRAME_COUNT; ++i)
    {
        m_frames[i].load(QString::fromUtf8(bombPaths[i]));
        if (m_frames[i].isNull())
        {
            qWarning() << "Failed to load bomb frame:" << bombPaths[i];
        }
    }
}

void Bomb::activate(int centerX, int centerY)
{
    m_free         = false;
    m_currentFrame = 0;
    m_elapsedMs    = 0;

    // 用第一帧的实际尺寸计算左上角坐标
    int frameW = m_frames[0].width();
    int frameH = m_frames[0].height();
    if (frameW <= 0) { frameW = 1; }
    if (frameH <= 0) { frameH = 1; }

    m_x = centerX - frameW / 2;
    m_y = centerY - frameH / 2;
}

void Bomb::updateAnimation(int deltaMs)
{
    if (m_free)
    {
        return;
    }
    if (deltaMs <= 0)
    {
        return;
    }

    m_elapsedMs += deltaMs;

    while (m_elapsedMs >= BOMB_FRAME_INTERVAL && !m_free)
    {
        m_elapsedMs -= BOMB_FRAME_INTERVAL;
        ++m_currentFrame;

        if (m_currentFrame >= BOMB_FRAME_COUNT)
        {
            reset();
        }
    }
}

void Bomb::reset()
{
    m_free         = true;
    m_currentFrame = 0;
    m_elapsedMs    = 0;
    m_x            = -100;
    m_y            = -100;
}
