#include "heroplane.h"

HeroPlane::HeroPlane()
{
    m_plane.load(HERO_PLANE);

    m_x = GAME_WIDTH * 0.5 - m_plane.width() * 0.5;
    m_y = GAME_HEIGHT - m_plane.height();

    // 碰撞矩形
    m_rect.setWidth(m_plane.width());
    m_rect.setHeight(m_plane.height());
    m_rect.moveTo(m_x, m_y);

    // 初始化武器
    resetWeapon();
}

// 移动
void HeroPlane::setPostion(int x, int y)
{
    m_x = x;
    m_y = y;
    m_rect.moveTo(m_x, m_y);
}

// 重置武器状态
void HeroPlane::resetWeapon()
{
    m_currentAmmo    = HERO_MAGAZINE_CAPACITY;
    m_reloading      = false;
    m_shootElapsedMs = 0;
    m_reloadElapsedMs = 0;

    for (int i = 0; i < HERO_BULLET_POOL_SIZE; ++i)
    {
        m_bullets[i].reset();
    }
}

// 每帧更新武器
void HeroPlane::updateWeapon(int deltaMs)
{
    // 换弹中
    if (m_reloading)
    {
        m_reloadElapsedMs += deltaMs;
        if (m_reloadElapsedMs >= HERO_RELOAD_TIME)
        {
            m_currentAmmo     = HERO_MAGAZINE_CAPACITY;
            m_reloading       = false;
            m_reloadElapsedMs = 0;
            m_shootElapsedMs  = 0;
        }
        return;
    }

    // 弹药耗尽，自动换弹
    if (m_currentAmmo <= 0)
    {
        startReload();
        return;
    }

    // 射击计时
    m_shootElapsedMs += deltaMs;
    if (m_shootElapsedMs >= HERO_SHOOT_INTERVAL)
    {
        shoot();
    }
}

// 发射子弹
void HeroPlane::shoot()
{
    // 换弹中不射击
    if (m_reloading)
    {
        return;
    }

    // 弹药耗尽，触发换弹
    if (m_currentAmmo <= 0)
    {
        startReload();
        return;
    }

    // 射击间隔未到
    if (m_shootElapsedMs < HERO_SHOOT_INTERVAL)
    {
        return;
    }

    // 寻找空闲子弹
    int freeIndex = -1;
    for (int i = 0; i < HERO_BULLET_POOL_SIZE; ++i)
    {
        if (m_bullets[i].m_free)
        {
            freeIndex = i;
            break;
        }
    }

    // 没有空闲子弹
    if (freeIndex == -1)
    {
        return;
    }

    // 计算子弹发射位置（英雄机顶部中心）
    int bulletX = m_x + m_plane.width() / 2
                  - m_bullets[freeIndex].m_bullet.width() / 2;
    int bulletY = m_y - m_bullets[freeIndex].m_bullet.height();

    // 激活子弹
    m_bullets[freeIndex].activate(bulletX, bulletY);

    m_currentAmmo--;
    m_shootElapsedMs = 0;

    // 射击后弹药耗尽，自动换弹
    if (m_currentAmmo == 0)
    {
        startReload();
    }
}

// 主动换弹
void HeroPlane::startReload()
{
    // 正在换弹中，忽略
    if (m_reloading)
    {
        return;
    }

    // 弹药已满，不需要换弹
    if (m_currentAmmo >= HERO_MAGAZINE_CAPACITY)
    {
        return;
    }

    m_reloading       = true;
    m_reloadElapsedMs = 0;
    m_shootElapsedMs  = 0;
}
