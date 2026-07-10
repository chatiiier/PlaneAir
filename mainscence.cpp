#include "mainscence.h"
#include "ui_mainscence.h"
#include <QPainter>
#include <QIcon>
#include <QTime>
#include <QApplication>
#include "resultdialog.h"
#include "rankmanager.h"

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
    m_stateBeforePause = StatePlaying;

    // 保存玩家昵称
    m_playerNickname = nickname;

    // 初始化敌机对象池
    m_enemySpawnElapsedMs = 0;
    resetEnemies();

    // 初始化爆炸和分数
    m_score = 0;
    m_resultShown = false;
    m_rankSaved   = false;
    resetBombs();
    resetEnemyBullets();

    // 初始化 Boss
    m_bossTriggered = false;
    m_bossWarningElapsedMs = 0;
    m_boss.reset();

    // 初始化随机种子（只调用一次）
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    // 确保窗口能够接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // 设置定时器
    m_timer.setInterval(GAME_RATE);

    // 连接定时器信号（只连接一次）
    connect(&m_timer, &QTimer::timeout, [=](){
        switch (m_state)
        {
        case StatePlaying:
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

            // 敌机射击
            updateEnemyShooting();
            updateEnemyBullets();

            // 碰撞检测
            checkHeroBulletEnemyCollisions();
            checkEnemyBulletHeroCollisions();
            checkEnemyHeroCollisions();

            // 英雄无敌状态更新
            m_hero.updateDamageState(GAME_RATE);

            // 爆炸动画
            updateBombs();

            // 检查 Boss 触发
            checkBossTrigger();

            // 检查玩家是否死亡
            if (m_hero.isDead())
            {
                enterGameOver();
            }
            break;
        }
        case StateBossWarning:
        {
            // WARNING 阶段：可移动和地图滚动，不射击不碰撞
            updateHeroPosition();
            updatePostion();
            m_hero.updateDamageState(GAME_RATE);
            updateBombs();
            updateBossWarning();
            break;
        }
        case StateBossFight:
        {
            updateHeroPosition();
            updatePostion();
            m_hero.updateWeapon(GAME_RATE);
            updateHeroBullets();
            updateBoss();
            m_hero.updateDamageState(GAME_RATE);
            updateBombs();

            // Boss 战暂时无碰撞和敌机

            if (m_hero.isDead())
            {
                enterGameOver();
            }
            break;
        }
        case StatePaused:
        case StateGameOver:
            // 不进行战斗更新
            break;
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

    // 2. 绘制 Boss（如果激活）
    if (m_boss.isActive())
    {
        painter.drawPixmap(m_boss.m_x, m_boss.m_y, m_boss.m_pixmap);
    }

    // 3. 绘制敌机（普通阶段）
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

    // 4. 绘制敌方子弹
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i)
    {
        if (!m_enemyBullets[i].m_free)
        {
            painter.drawPixmap(
                m_enemyBullets[i].m_x,
                m_enemyBullets[i].m_y,
                m_enemyBullets[i].m_pixmap
            );
        }
    }

    // 5. 绘制英雄子弹
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

    // 6. 绘制爆炸动画
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

    // 7. 绘制英雄机（无敌闪烁期间可能不绘制）
    if (m_hero.shouldDraw())
    {
        painter.drawPixmap(m_hero.m_x, m_hero.m_y, m_hero.m_plane);
    }

    // 8. 绘制 HUD（昵称、生命、分数、弹药）
    {
        // 半透明底色
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, 96);

        // 昵称（第一行）
        painter.setPen(QColor(255, 255, 100));
        QFont hudFont;
        hudFont.setPixelSize(14);
        hudFont.setBold(true);
        painter.setFont(hudFont);
        QString nameText = QStringLiteral("玩家：%1").arg(m_playerNickname);
        painter.drawText(QRect(0, 2, GAME_WIDTH, 22),
                         Qt::AlignCenter, nameText);

        // 生命（第二行）
        painter.setPen(QColor(0, 255, 100));
        painter.setFont(hudFont);
        QString hpText = QStringLiteral("生命：%1 / %2")
                             .arg(m_hero.m_hp)
                             .arg(m_hero.m_maxHp);
        painter.drawText(QRect(0, 26, GAME_WIDTH, 22),
                         Qt::AlignCenter, hpText);

        // 分数（第三行）
        painter.setPen(Qt::white);
        painter.setFont(hudFont);
        QString scoreText = QStringLiteral("分数：%1").arg(m_score);
        painter.drawText(QRect(0, 48, GAME_WIDTH, 22),
                         Qt::AlignCenter, scoreText);

        // 弹药（第四行）
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
        painter.drawText(QRect(0, 72, GAME_WIDTH, 22),
                         Qt::AlignCenter, ammoText);
    }

    // 9. Boss 血条（入场和战斗中显示）
    if (m_boss.isActive() && m_boss.m_maxHp > 0)
    {
        int barX = 10;
        int barY = 100;
        int barW = GAME_WIDTH - 20;
        int barH = 18;

        // 背景
        painter.setBrush(QColor(40, 40, 40));
        painter.setPen(QPen(QColor(80, 80, 80), 1));
        painter.drawRect(barX, barY, barW, barH);

        // 血量
        int hpW = barW * m_boss.m_hp / m_boss.m_maxHp;
        painter.setBrush(QColor(220, 30, 30));
        painter.setPen(Qt::NoPen);
        painter.drawRect(barX, barY, hpW, barH);

        // 文字
        painter.setPen(Qt::white);
        QFont barFont;
        barFont.setPixelSize(12);
        barFont.setBold(true);
        painter.setFont(barFont);
        QString bossHpText = QStringLiteral("BOSS HP %1 / %2")
                                 .arg(m_boss.m_hp)
                                 .arg(m_boss.m_maxHp);
        painter.drawText(QRect(barX, barY, barW, barH),
                         Qt::AlignCenter, bossHpText);
    }

    // 10. WARNING 遮罩
    if (m_state == StateBossWarning)
    {
        // 半透明红黑遮罩
        painter.setBrush(QColor(80, 0, 0, 180));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

        QFont warnFont;
        warnFont.setPixelSize(40);
        warnFont.setBold(true);
        painter.setFont(warnFont);

        // 闪烁效果（基于剩余时间）
        int remaining = BOSS_WARNING_TIME - m_bossWarningElapsedMs;
        bool visible = true;
        if (remaining > 0 && (remaining / 200) % 2 == 0)
        {
            visible = false;
        }

        if (visible)
        {
            painter.setPen(QColor(255, 60, 60));
            painter.drawText(QRect(0, GAME_HEIGHT / 2 - 80, GAME_WIDTH, 60),
                             Qt::AlignCenter,
                             QStringLiteral("WARNING"));

            QFont subFont;
            subFont.setPixelSize(22);
            subFont.setBold(false);
            painter.setFont(subFont);
            painter.setPen(Qt::white);
            painter.drawText(QRect(0, GAME_HEIGHT / 2, GAME_WIDTH, 40),
                             Qt::AlignCenter,
                             QStringLiteral("BOSS 即将出现"));
        }
    }

    // 11. 暂停遮罩
    if (m_state == StatePaused)
    {
        painter.setBrush(QColor(0, 0, 0, 150));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

        painter.setPen(Qt::white);
        QFont font;
        font.setPixelSize(32);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(QRect(0, 0, GAME_WIDTH, GAME_HEIGHT / 2),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         QStringLiteral("游戏已暂停"));

        font.setPixelSize(20);
        font.setBold(false);
        painter.setFont(font);
        painter.drawText(QRect(0, GAME_HEIGHT / 2, GAME_WIDTH, GAME_HEIGHT / 2),
                         Qt::AlignHCenter | Qt::AlignTop,
                         QStringLiteral("按 P 继续"));
    }

    // 12. 游戏结束遮罩
    if (m_state == StateGameOver)
    {
        painter.setBrush(QColor(0, 0, 0, 180));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT);

        painter.setPen(Qt::white);
        QFont font;
        font.setPixelSize(36);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(QRect(0, GAME_HEIGHT / 2 - 60, GAME_WIDTH, 50),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         QStringLiteral("游戏结束"));

        font.setPixelSize(20);
        font.setBold(false);
        painter.setFont(font);

        QString finalScoreText = QStringLiteral("最终分数：%1").arg(m_score);
        painter.drawText(QRect(0, GAME_HEIGHT / 2 - 10, GAME_WIDTH, 30),
                         Qt::AlignCenter, finalScoreText);
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

    // P 键：切换暂停（Playing/Warning/Fight 均可暂停，GameOver 无效）
    if (event->key() == Qt::Key_P && !event->isAutoRepeat())
    {
        if (m_state == StatePlaying
            || m_state == StateBossWarning
            || m_state == StateBossFight
            || m_state == StatePaused)
        {
            togglePause();
        }
        return;
    }

    // R 键：主动换弹，仅在 Playing 或 BossFight 时可使用
    if (event->key() == Qt::Key_R && !event->isAutoRepeat())
    {
        if (m_state == StatePlaying || m_state == StateBossFight)
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
    if (event->isAutoRepeat()) { return; }

    if (event->key() == Qt::Key_Up    || event->key() == Qt::Key_W) { m_keyUp    = false; return; }
    if (event->key() == Qt::Key_Down  || event->key() == Qt::Key_S) { m_keyDown  = false; return; }
    if (event->key() == Qt::Key_Left  || event->key() == Qt::Key_A) { m_keyLeft  = false; return; }
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) { m_keyRight = false; return; }

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

    if (m_keyUp)    { y -= HERO_SPEED; }
    if (m_keyDown)  { y += HERO_SPEED; }
    if (m_keyLeft)  { x -= HERO_SPEED; }
    if (m_keyRight) { x += HERO_SPEED; }

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
    if (m_state == StatePaused)
    {
        m_state = m_stateBeforePause;
    }
    else if (m_state != StateGameOver)
    {
        m_stateBeforePause = m_state;
        m_state = StatePaused;
        clearDirectionKeys();
    }
}

// ============ 敌机相关 ============

void Mainscence::spawnEnemy()
{
    int freeIndex = -1;
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (m_enemies[i].m_free) { freeIndex = i; break; }
    }
    if (freeIndex == -1) { return; }

    int randomValue = qrand() % 100;
    EnemyType type = (randomValue < LIGHT_ENEMY_PROBABILITY) ? EnemyLight : EnemyHeavy;

    int pixW = m_enemies[freeIndex].pixmapWidthForType(type);
    int maxX = GAME_WIDTH - pixW;
    if (maxX < 0) { maxX = 0; }
    int x = qrand() % (maxX + 1);
    int y = -pixW;
    if (y > 0) { y = 0; }

    m_enemies[freeIndex].activate(type, x, y);
}

void Mainscence::updateEnemies()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (!m_enemies[i].m_free) { m_enemies[i].updatePosition(); }
    }
}

void Mainscence::resetEnemies()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i) { m_enemies[i].reset(); }
}

// ============ 碰撞 ============

void Mainscence::checkHeroBulletEnemyCollisions()
{
    for (int b = 0; b < HERO_BULLET_POOL_SIZE; ++b)
    {
        if (m_hero.m_bullets[b].m_free) { continue; }
        bool bulletUsed = false;

        for (int e = 0; e < ENEMY_POOL_SIZE; ++e)
        {
            if (m_enemies[e].m_free) { continue; }

            if (m_hero.m_bullets[b].m_rect.intersects(m_enemies[e].m_rect))
            {
                int centerX = m_enemies[e].m_x + m_enemies[e].m_pixmap.width() / 2;
                int centerY = m_enemies[e].m_y + m_enemies[e].m_pixmap.height() / 2;
                int scoreValue = m_enemies[e].m_scoreValue;

                m_hero.m_bullets[b].reset();
                bulletUsed = true;
                m_enemies[e].takeDamage(HERO_BULLET_DAMAGE);

                if (m_enemies[e].isDead())
                {
                    activateBomb(centerX, centerY);
                    m_score += scoreValue;
                    m_enemies[e].reset();
                }
                break;
            }
        }
        Q_UNUSED(bulletUsed);
    }
}

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
}

void Mainscence::updateBombs()
{
    for (int i = 0; i < BOMB_POOL_SIZE; ++i)
    {
        if (!m_bombs[i].m_free) { m_bombs[i].updateAnimation(GAME_RATE); }
    }
}

void Mainscence::resetBombs()
{
    for (int i = 0; i < BOMB_POOL_SIZE; ++i) { m_bombs[i].reset(); }
}

// ============ 敌机射击 ============

void Mainscence::updateEnemyShooting()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (m_enemies[i].m_free) { continue; }
        if (m_enemies[i].updateAndCheckShoot(GAME_RATE))
        {
            spawnEnemyBullet(m_enemies[i]);
        }
    }
}

void Mainscence::spawnEnemyBullet(EnemyPlane &enemy)
{
    if (enemy.m_free) { return; }

    int freeIndex = -1;
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i)
    {
        if (m_enemyBullets[i].m_free) { freeIndex = i; break; }
    }
    if (freeIndex == -1) { return; }

    EnemyBulletType bulletType = (enemy.m_type == EnemyHeavy) ? EnemyBulletHeavy : EnemyBulletLight;

    int bulletX = enemy.m_x + enemy.m_pixmap.width() / 2;
    int bulletY = enemy.m_y + enemy.m_pixmap.height();

    m_enemyBullets[freeIndex].activate(bulletType, bulletX, bulletY);

    if (m_enemyBullets[freeIndex].m_pixmap.width() > 0)
    {
        m_enemyBullets[freeIndex].m_x = bulletX - m_enemyBullets[freeIndex].m_pixmap.width() / 2;
        m_enemyBullets[freeIndex].m_rect.moveTo(m_enemyBullets[freeIndex].m_x, m_enemyBullets[freeIndex].m_y);
    }
}

void Mainscence::updateEnemyBullets()
{
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i)
    {
        if (!m_enemyBullets[i].m_free) { m_enemyBullets[i].updatePosition(); }
    }
}

void Mainscence::resetEnemyBullets()
{
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i) { m_enemyBullets[i].reset(); }
}

void Mainscence::checkEnemyBulletHeroCollisions()
{
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i)
    {
        if (m_enemyBullets[i].m_free) { continue; }
        if (m_enemyBullets[i].m_rect.intersects(m_hero.m_rect))
        {
            int damage = m_enemyBullets[i].m_damage;
            m_enemyBullets[i].reset();
            m_hero.takeDamage(damage);
            if (m_hero.isDead()) { return; }
        }
    }
}

void Mainscence::checkEnemyHeroCollisions()
{
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        if (m_enemies[i].m_free) { continue; }
        if (m_enemies[i].m_rect.intersects(m_hero.m_rect))
        {
            int centerX = m_enemies[i].m_x + m_enemies[i].m_pixmap.width() / 2;
            int centerY = m_enemies[i].m_y + m_enemies[i].m_pixmap.height() / 2;
            activateBomb(centerX, centerY);
            m_enemies[i].reset();
            m_hero.takeDamage(1);
            if (m_hero.isDead()) { return; }
        }
    }
}

// ============ Boss 相关 ============

void Mainscence::checkBossTrigger()
{
    if (!m_bossTriggered && m_score >= BOSS_TRIGGER_SCORE)
    {
        enterBossWarning();
    }
}

void Mainscence::enterBossWarning()
{
    if (m_bossTriggered) { return; }
    m_bossTriggered = true;

    m_bossWarningElapsedMs = 0;
    m_state = StateBossWarning;
    clearDirectionKeys();
    clearNormalEnemiesForBoss();
    update();
}

void Mainscence::clearNormalEnemiesForBoss()
{
    // 清除所有普通敌机（不产生爆炸，不加分）
    for (int i = 0; i < ENEMY_POOL_SIZE; ++i)
    {
        m_enemies[i].reset();
    }
    // 清除所有敌方子弹
    for (int i = 0; i < ENEMY_BULLET_POOL_SIZE; ++i)
    {
        m_enemyBullets[i].reset();
    }
    // 清除所有英雄子弹
    for (int i = 0; i < HERO_BULLET_POOL_SIZE; ++i)
    {
        m_hero.m_bullets[i].reset();
    }
    m_enemySpawnElapsedMs = 0;
}

void Mainscence::updateBossWarning()
{
    m_bossWarningElapsedMs += GAME_RATE;
    if (m_bossWarningElapsedMs >= BOSS_WARNING_TIME)
    {
        startBossFight();
    }
}

void Mainscence::startBossFight()
{
    m_bossWarningElapsedMs = BOSS_WARNING_TIME;
    m_boss.activate();
    m_state = StateBossFight;
    clearDirectionKeys();
    update();
}

void Mainscence::updateBoss()
{
    m_boss.updatePosition();
}

// ============ 游戏结束 ============

void Mainscence::enterGameOver()
{
    if (m_state == StateGameOver) { return; }

    m_state = StateGameOver;
    clearDirectionKeys();
    m_hero.m_visible = true;

    saveRankOnce();
    showResultDialog();
}

void Mainscence::saveRankOnce()
{
    if (m_rankSaved) { return; }
    if (m_playerNickname.isEmpty()) { return; }
    if (m_score < 0) { return; }

    m_rankSaved = true;

    RankManager manager;
    manager.updateBestScore(m_playerNickname, m_score);
}

void Mainscence::showResultDialog()
{
    if (m_resultShown) { return; }
    m_resultShown = true;

    RankManager manager;
    int bestScore = manager.bestScoreFor(m_playerNickname);

    ResultDialog dialog(ResultDefeat, m_playerNickname, m_score, bestScore, this);
    int action = dialog.exec();

    if (action == ActionRestart)
    {
        resetGame();
    }
    else if (action == ActionExitGame)
    {
        QApplication::quit();
    }
    else
    {
        close();
    }
}

// ============ 重新开始 ============

void Mainscence::resetGame()
{
    m_state       = StatePlaying;
    m_stateBeforePause = StatePlaying;
    m_resultShown = false;
    m_rankSaved   = false;

    clearDirectionKeys();
    m_score = 0;

    m_hero.setPostion(GAME_WIDTH * 0.5 - m_hero.m_plane.width() * 0.5,
                      GAME_HEIGHT - m_hero.m_plane.height());
    m_hero.resetWeapon();
    m_hero.resetHealth();

    map.resetPosition();

    m_enemySpawnElapsedMs = 0;
    resetEnemies();
    resetEnemyBullets();
    resetBombs();

    // Boss 重置
    m_bossTriggered = false;
    m_bossWarningElapsedMs = 0;
    m_boss.reset();

    setFocus();
    update();
}

Mainscence::~Mainscence()
{
    delete ui;
}
