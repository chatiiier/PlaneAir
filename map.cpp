#include "map.h"

Map::Map()
{

    //两张图上下拼接，图1在上图2在下

    m_map1.load(MAP_PATH);
    m_map2.load(MAP_PATH);

    // 设置图的位置

    m_map1_PosY =-GAME_HEIGHT; // 在屏幕的上方外

    m_map2_PosY = 0; // 在屏幕内

    m_scoll_speed =MAP_SCOLL_SPEED;




}

// 实现地图坐标更新
void Map::MapPosition(){

    //1、两张图同时向下滚动
    // 图1滚出屏幕底部，回到顶点
    // 图2滚出屏幕底部，回到原地
    m_map1_PosY+=m_scoll_speed;
    m_map2_PosY+=m_scoll_speed;


//    判断
    if(m_map1_PosY>=GAME_HEIGHT)
    {
        m_map1_PosY =-GAME_HEIGHT;
    }

    if(m_map2_PosY>=GAME_HEIGHT)
    {
        m_map2_PosY =-GAME_HEIGHT;
    }


}
