#ifndef MAP_H
#define MAP_H
#include <QPixmap>
#include<config.h>
class Map
{
public:
    Map();


    //  定义坐标更新

    void MapPosition();

    // 重置地图到初始位置
    void resetPosition();


    QPixmap m_map1; // 第一张背景图

    QPixmap m_map2; // 第二章背景图

    // 定义坐标

    int m_map1_PosY; // 定义第一张图的Y坐标
    int m_map2_PosY;// 定义第二张图的Y的坐标

    int m_scoll_speed; // 滚动速度







    //
};

#endif // MAP_H
