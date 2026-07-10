#ifndef HEROPLANE_H
#define HEROPLANE_H
#include<config.h>
#include<QPixmap>
class HeroPlane
{
public:
    HeroPlane();
    
    // 移动位置
    
    void setPostion(int x,int y);
    
    // 定义飞机的资源
    QPixmap m_plane; // 飞机图片
    
    int m_x;
    int m_y ; // 定义飞机的位置
    
    
    QRect m_rect; // 定义碰撞矩形框
    
    
    int recorder ; //发射子弹计时器
    
    
    void shoot(); // 子弹发射行为
    
    
};

#endif // HEROPLANE_H
