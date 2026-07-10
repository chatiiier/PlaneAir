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

// 游戏状态枚举，为后续 Boss 状态机预留扩展
enum GameState
{
    StatePlaying,
    StatePaused
};

namespace Ui {
class Mainscence;
}

class Mainscence : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainscence(QWidget *parent = 0);
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

    // 清空所有方向键状态
    void clearDirectionKeys();

    // 更新英雄机位置
    void updateHeroPosition();

    // 切换暂停/继续
    void togglePause();
};

#endif // MAINSCENCE_H
