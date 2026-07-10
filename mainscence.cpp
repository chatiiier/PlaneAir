#include "mainscence.h"
#include "ui_mainscence.h"
#include <QPainter>
#include <QIcon>
#include <QTime>

Mainscence::Mainscence(const QString &nickname, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Mainscence)
{
    ui->setupUi(this);

    // 设置主场景窗口
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle(GAME_TITLE);
    setWindowIcon(QIcon(GAME_ICON));

    // 初始化键盘状态
    m_keyUp    = false;
    m_keyDown  = false;
    m_keyLeft  = false;
    m_keyRight = false;

    // 初始化游戏状态
    m_state = StatePlaying;

    // 保存玩家昵称
    m_playerNickname = nickname;

    // 初始化敌机对象池
    m_enemySpawnElapsedMs = 0;
    resetEnemies();

    // 初始化爆炸和分数
    m_score = 0;
    resetBombs();

    // 初始化随机种子（只调用一次）
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    // 确保窗口能够接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // 设置定时器
    m_timer.setInterval(GAME_RATE);

    // 连接定时器信号（只连接一次）
    connect(&m_timer, &QTimer::timeout, [=](){
        if (m_state == StatePlaying)
        {
            updateHeroPosition();
            updatePostion();
            m_hero.updateWeapon(GAME_RATE);
            updateHeroBullets();

            // 敌机生成与移动
            m_enemySpawnElapsedMs += GAME_RATE;
            if (m_enemySpawnElapsedMs >= ENEMY_SPAWN_INTERVAL)
            {
                spawnEnemy();
                m_enemySpawnElapsedMs = 0;
            }
            updateEnemies();

            // 碰撞检测与爆炸
            checkHeroBulletEnemyCollisions();
            updateBombs();
        }
        update();
    });

    // 启动游戏
    playGame();
}

// 实现游戏启动
void Mainscence::playGame()
{
    m_timer.start();
}

// 更新主场景信息（地图滚动）
void Mainscence::updatePostion()
{
    map.MapPosition();
}

// 更新英雄子弹位置
void Mainscence::updateHeroBullets()
{
    for (int i = 0; i < HERO_BULLET_POOL_SIZE; ++i)
    {
        if (!m_hero.m_bullets[i].m_free)
        {
            m_hero.m_bullets[i].updatePosition();
        }
    }
}

// 地图绘制
void Mainscence::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 定义画家
    QPainter painter(this);

    // 1. 绘制地图
    painter.drawPixmap(0, map.m_map1_PosY, map.m_map1);
    painter.drawPixmap(0, map.m_map2_PosY, map.m_map2);

    // 2. 绘制敌机
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (!m_enemies[i].m_free)
        {
            painter.drawPixmap(
                m_enemies[i].m_x,
                m_enemies[i].m_y,
                m_enemies[i].m_pixmap
            );
        }
    }

    // 3. 绘制英雄子弹
    for (int i = 0; i < HERO_BULLET_POOL_SIZE; ++i)
    {
        if (!m_hero.m_bullets[i].m_free)
        {
            painter.drawPixmap(
                m_hero.m_bullets[i].m_x,
                m_hero.m_bullets[i].m_y,
                m_hero.m_bullets[i].m_bullet
            );
        }
    }

    // 4. 绘制爆炸动画
    for (int i = 0; i < BOMB_POOL_SIZE; ++i)
    {
        if (!m_bombs[i].m_free
            && m_bombs[i].m_currentFrame >= 0
            && m_bombs[i].m_currentFrame < BOMB_FRAME_COUNT)
        {
            const QPixmap &frame = m_bombs[i].m_frames[m_bombs[i].m_currentFrame];
            if (!frame.isNull())
            {
                painter.drawPixmap(m_bombs[i].m_x, m_bombs[i].m_y, frame);
            }
        }
    }

    // 5. 绘制英雄机
    painter.drawPixmap(m_hero.m_x, m_hero.m_y, m_hero.m_plane);

    // 6. 绘制 HUD（昵称、分数、弹药）
    {
        // 半透明底色
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, 72);

        // 昵称（第一行）
        painter.setPen(QColor(255, 255, 100));
        QFont hudFont;
        hudFont.setPixelSize(14);
        hudFont.setBold(true);
        painter.setFont(hudFont);
        QString nameText = QStringLiteral("玩家：%1").arg(m_playerNickname);
        painter.drawText(QRect(0, 2, GAME_WIDTH, 22),
                         Qt::AlignCenter, nameText);

        // 分数（第二行）
        painter.setPen(Qt::white);
        painter.setFont(hudFont);
        QString scoreText = QStringLiteral("分数：%1").arg(m_score);
        painter.drawText(QRect(0, 26, GAME_WIDTH, 22),
                         Qt::AlignCenter, scoreText);

        // 弹药（第三行）
        QString ammoText;
        if (m_hero.m_reloading)
        {
            ammoText = QStringLiteral("弹药：换弹中……");
        }
        else
        {
            ammoText = QStringLiteral("弹药：%1 / %2")
                           .arg(m_hero.m_currentAmmo)
                           .arg(HERO_MAGAZINE_CAPACITY);
        }
        painter.drawText(QRect(0, 48, GAME_WIDTH, 22),
                         Qt::AlignCenter, ammoText);
    }

    // 7. 暂停遮罩
    if (m_state == StatePaused)
    {
        // 半透明黑色遮罩
        painter.setBrush(QColor(0, 0, 0, 150));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

        // 白色文字
        painter.setPen(Qt::white);
        QFont font;
        font.setPixelSize(32);
        font.setBold(true);
        painter.setFont(font);

        // 第一行 "游戏已暂停"
        painter.drawText(QRect(0, 0, GAME_WIDTH, GAME_HEIGHT / 2),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         QStringLiteral("游戏已暂停"));

        // 第二行 "按 P 继续"
        font.setPixelSize(20);
        font.setBold(false);
        painter.setFont(font);
        painter.drawText(QRect(0, GAME_HEIGHT / 2, GAME_WIDTH, GAME_HEIGHT / 2),
                         Qt::AlignHCenter | Qt::AlignTop,
                         QStringLiteral("按 P 继续"));
    }
}

// 键盘按下事件
void Mainscence::keyPressEvent(QKeyEvent *event)
{
    // 方向键 / WASD：设置状态标志
    if (event->key() == Qt::Key_Up    || event->key() == Qt::Key_W) { m_keyUp    = true; return; }
    if (event->key() == Qt::Key_Down  || event->key() == Qt::Key_S) { m_keyDown  = true; return; }
    if (event->key() == Qt::Key_Left  || event->key() == Qt::Key_A) { m_keyLeft  = true; return; }
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) { m_keyRight = true; return; }

    // P 键：切换暂停，防止自动重复反复切换
    if (event->key() == Qt::Key_P && !event->isAutoRepeat())
    {
        togglePause();
        return;
    }

    // R 键：主动换弹，仅游戏进行中响应
    if (event->key() == Qt::Key_R && !event->isAutoRepeat())
    {
        if (m_state == StatePlaying)
        {
            m_hero.startReload();
        }
        return;
    }

    // 其他按键传递给基类
    QMainWindow::keyPressEvent(event);
}

// 键盘释放事件
void Mainscence::keyReleaseEvent(QKeyEvent *event)
{
    // 忽略自动重复产生的释放事件
    if (event->isAutoRepeat())
    {
        return;
    }

    // 方向键 / WASD：清除状态标志
    if (event->key() == Qt::Key_Up    || event->key() == Qt::Key_W) { m_keyUp    = false; return; }
    if (event->key() == Qt::Key_Down  || event->key() == Qt::Key_S) { m_keyDown  = false; return; }
    if (event->key() == Qt::Key_Left  || event->key() == Qt::Key_A) { m_keyLeft  = false; return; }
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) { m_keyRight = false; return; }

    // 其他按键传递给基类
    QMainWindow::keyReleaseEvent(event);
}

// 窗口失去焦点事件
void Mainscence::focusOutEvent(QFocusEvent *event)
{
    clearDirectionKeys();
    QMainWindow::focusOutEvent(event);
}

// 清空所有方向键状态
void Mainscence::clearDirectionKeys()
{
    m_keyUp    = false;
    m_keyDown  = false;
    m_keyLeft  = false;
    m_keyRight = false;
}

// 更新英雄机位置
void Mainscence::updateHeroPosition()
{
    int x = m_hero.m_x;
    int y = m_hero.m_y;

    // 根据按键状态计算新坐标
    if (m_keyUp)    { y -= HERO_SPEED; }
    if (m_keyDown)  { y += HERO_SPEED; }
    if (m_keyLeft)  { x -= HERO_SPEED; }
    if (m_keyRight) { x += HERO_SPEED; }

    // 边界限制
    if (x < 0) { x = 0; }
    if (x > GAME_WIDTH - m_hero.m_plane.width())
        { x = GAME_WIDTH - m_hero.m_plane.width(); }
    if (y < 0) { y = 0; }
    if (y > GAME_HEIGHT - m_hero.m_plane.height())
        { y = GAME_HEIGHT - m_hero.m_plane.height(); }

    m_hero.setPostion(x, y);
}

// 切换暂停/继续
void Mainscence::togglePause()
{
    if (m_state == StatePlaying)
    {
        m_state = StatePaused;
        clearDirectionKeys();
    }
    else
    {
        m_state = StatePlaying;
    }
}

// 生成敌机
void Mainscence::spawnEnemy()
{
    // 寻找空闲敌机槽位
    int freeIndex = -1;
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (m_enemies[i].m_free)
        {
            freeIndex = i;
            break;
        }
    }

    if (freeIndex == -1)
    {
        return; // 对象池已满
    }

    // 随机决定类型
    int randomValue = qrand() % 100;
    EnemyType type = (randomValue < LIGHT_ENEMY_PROBABILITY)
                         ? EnemyLight : EnemyHeavy;

    // 通过预加载的 pixmap 计算合法 X 范围
    int pixW = m_enemies[freeIndex].pixmapWidthForType(type);
    int maxX = GAME_WIDTH - pixW;
    if (maxX < 0)
    {
        maxX = 0;
    }
    int x = qrand() % (maxX + 1);
    int y = -pixW; // 从屏幕顶部外出现
    if (y > 0)
    {
        y = 0;
    }

    m_enemies[freeIndex].activate(type, x, y);
}

// 更新所有活动敌机位置
void Mainscence::updateEnemies()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (!m_enemies[i].m_free)
        {
            m_enemies[i].updatePosition();
        }
    }
}

// 重置所有敌机为初始空闲状态
void Mainscence::resetEnemies()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        m_enemies[i].reset();
    }
}

// 英雄子弹 vs 敌机 碰撞检测
void Mainscence::checkHeroBulletEnemyCollisions()
{
    for (int b = 0; b < HERO_BULLET_POOL_SIZE; ++b)
    {
        if (m_hero.m_bullets[b].m_free)
        {
            continue;
        }

        bool bulletUsed = false;

        for (int e = 0; e < ENEMY_POOL_SIZE; ++e)
        {
            if (m_enemies[e].m_free)
            {
                continue;
            }

            // QRect 碰撞检测
            if (m_hero.m_bullets[b].m_rect.intersects(m_enemies[e].m_rect))
            {
                // 保存敌机信息（reset 前）
                int centerX = m_enemies[e].m_x
                              + m_enemies[e].m_pixmap.width() / 2;
                int centerY = m_enemies[e].m_y
                              + m_enemies[e].m_pixmap.height() / 2;
                int scoreValue = m_enemies[e].m_scoreValue;

                // 子弹回收
                m_hero.m_bullets[b].reset();
                bulletUsed = true;

                // 敌机扣血
                m_enemies[e].takeDamage(HERO_BULLET_DAMAGE);

                // 敌机死亡处理
                if (m_enemies[e].isDead())
                {
                    activateBomb(centerX, centerY);
                    m_score += scoreValue;
                    m_enemies[e].reset();
                }

                break; // 子弹已回收，跳出内层循环
            }
        }

        Q_UNUSED(bulletUsed);
    }
}

// 激活爆炸动画
void Mainscence::activateBomb(int centerX, int centerY)
{
    for (int i = 0; i < BOMB_POOL_SIZE; ++i)
    {
        if (m_bombs[i].m_free)
        {
            m_bombs[i].activate(centerX, centerY);
            return;
        }
    }
    // 对象池已满，丢弃本次爆炸视觉
}

// 更新所有活动爆炸动画
void Mainscence::updateBombs()
{
    for (int i = 0; i < BOMB_POOL_SIZE; ++i)
    {
        if (!m_bombs[i].m_free)
        {
            m_bombs[i].updateAnimation(GAME_RATE);
        }
    }
}

// 重置所有爆炸对象
void Mainscence::resetBombs()
{
    for (int i = 0; i < BOMB_POOL_SIZE; ++i)
    {
        m_bombs[i].reset();
    }
}

Mainscence::~Mainscence()
{
    delete ui;
}
