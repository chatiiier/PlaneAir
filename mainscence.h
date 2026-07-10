#ifndef MAINSCENCE_H
#define MAINSCENCE_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QPaintEvent>

#include "config.h"
#include "map.h"
#include "heroplane.h"
#include "enemyplane.h"
#include "enemybullet.h"
#include "bomb.h"

// 游戏状态枚举，为后续 Boss 状态机预留扩展
enum GameState
{
    StatePlaying,
    StatePaused,
    StateGameOver
};

namespace Ui {
class Mainscence;
}

class Mainscence : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainscence(const QString &nickname = QString(),
                        QWidget *parent = 0);
    ~Mainscence();

    // 初始化场景
    void mainscience();

    // 启动游戏
    void playGame();

    // 更新地图位置
    void updatePostion();

    // 定义地图
    Map map;

    // 定义英雄机
    HeroPlane m_hero;

    // 地图绘制
    void paintEvent(QPaintEvent *event);

    // 键盘按下事件
    void keyPressEvent(QKeyEvent *event);

    // 键盘释放事件
    void keyReleaseEvent(QKeyEvent *event);

    // 窗口失去焦点事件
    void focusOutEvent(QFocusEvent *event);

private:
    Ui::Mainscence *ui;

    QTimer m_timer;

    // 键盘方向状态
    bool m_keyUp;
    bool m_keyDown;
    bool m_keyLeft;
    bool m_keyRight;

    // 游戏状态
    GameState m_state;

    // 玩家昵称
    QString m_playerNickname;

    // 敌机对象池
    EnemyPlane m_enemies[ENEMY_POOL_SIZE];

    // 敌机生成计时
    int m_enemySpawnElapsedMs;

    // 爆炸对象池
    Bomb m_bombs[BOMB_POOL_SIZE];

    // 敌方子弹对象池
    EnemyBullet m_enemyBullets[ENEMY_BULLET_POOL_SIZE];

    // 分数
    int m_score;

    // 结算状态
    bool m_resultShown;
    bool m_rankSaved;

    // 清空所有方向键状态
    void clearDirectionKeys();

    // 更新英雄机位置
    void updateHeroPosition();

    // 更新英雄子弹位置
    void updateHeroBullets();

    // 敌机相关
    void spawnEnemy();
    void updateEnemies();
    void resetEnemies();

    // 碰撞与爆炸
    void checkHeroBulletEnemyCollisions();
    void checkEnemyBulletHeroCollisions();
    void checkEnemyHeroCollisions();
    void activateBomb(int centerX, int centerY);
    void updateBombs();
    void resetBombs();

    // 敌机射击
    void updateEnemyShooting();
    void spawnEnemyBullet(EnemyPlane &enemy);
    void updateEnemyBullets();
    void resetEnemyBullets();

    // 游戏结束
    void enterGameOver();
    void saveRankOnce();
    void showResultDialog();

    // 重置游戏（重新开始）
    void resetGame();

    // 切换暂停/继续
    void togglePause();
};

#endif // MAINSCENCE_H
