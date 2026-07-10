#include "mainscence.h"
#include "ui_mainscence.h"
#include<QPainter>
Mainscence::Mainscence(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Mainscence)
{
    ui->setupUi(this);

    // 设置主场景窗口

    setFixedSize(GAME_WIDTH,GAME_HEIGHT);

    setWindowTitle(GAME_TITLE);

    setWindowIcon(QIcon(GAME_ICON));

    // 设置定时器

    m_timer.setInterval(GAME_RATE);

    // 连接定时器信号（只连接一次）

    connect(&m_timer, &QTimer::timeout, [=](){
        updatePostion();
        update();
    });

    // 启动游戏

    playGame();


}

// 实现游戏启动
void Mainscence::playGame(){
    m_timer.start();
}
    // 更新主场景的信息

// 实现地图绘制
void Mainscence::paintEvent(QPaintEvent *E){

    // 定义画家
    QPainter painter(this);

    // 绘制地图
    painter.drawPixmap(0,map.m_map1_PosY,map.m_map1);
    painter.drawPixmap(0,map.m_map2_PosY,map.m_map2);


    painter.drawPixmap(m_hero.m_x,m_hero.m_y,m_hero.m_plane);



}
// 键盘事件
void Mainscence::keyPressEvent(QKeyEvent *e){

    // 获得英雄坐标
    int x =m_hero.m_x;
    int y = m_hero.m_y;
    // 键盘事件移动

    if(e->key()==Qt::Key_Left)
    {
        x-=HERO_SPEED;
    }
    else if(e->key()==Qt::Key_Right)
    {
        x+= HERO_SPEED;
    }

    // 月结判断
    if(x<0){
        x =0;
    }
    if(x>GAME_WIDTH-m_hero.m_plane.width())
        x = GAME_WIDTH-m_hero.m_plane.width();

    m_hero.setPostion(x,y);





}
void Mainscence::updatePostion(){
    map.MapPosition(); // 更新地图




}

Mainscence::~Mainscence()
{
    delete ui;
}
