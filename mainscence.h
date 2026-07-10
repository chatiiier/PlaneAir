#ifndef MAINSCENCE_H
#define MAINSCENCE_H
#include<QTimer>
#include <QMainWindow>
#include<config.h>
#include<map.h>
#include<heroplane.h>
#include<QKeyEvent>
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

    void updatePostion();

    // 定义地图
    Map map;



    // 定义英雄级

    HeroPlane m_hero;


    // 地图绘制
    void paintEvent(QPaintEvent *E);

    // 键盘事件

    void keyPressEvent(QKeyEvent *e);


private:
    Ui::Mainscence *ui;

    QTimer m_timer;


};

#endif // MAINSCENCE_H
