#include "mainscence.h"
#include "ui_mainscence.h"
#include <QPainter>
#include <QIcon>

Mainscence::Mainscence(QWidget *parent) :
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

    // 2. 绘制英雄子弹
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

    // 3. 绘制英雄机
    painter.drawPixmap(m_hero.m_x, m_hero.m_y, m_hero.m_plane);

    // 4. 绘制弹药 HUD
    {
        // 半透明底色
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, 30);

        painter.setPen(Qt::white);
        QFont hudFont;
        hudFont.setPixelSize(16);
        hudFont.setBold(true);
        painter.setFont(hudFont);

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
        painter.drawText(QRect(0, 0, GAME_WIDTH, 30),
                         Qt::AlignCenter, ammoText);
    }

    // 5. 暂停遮罩
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

Mainscence::~Mainscence()
{
    delete ui;
}
