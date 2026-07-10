#include "heroplane.h"

HeroPlane::HeroPlane()



{



    m_plane.load(HERO_PLANE); // 加载资源

    m_x = GAME_WIDTH*0.5 - m_plane.width()*0.5;
    m_y = GAME_HEIGHT-m_plane.height();

    // 将飞机移动到矩形框中
    m_rect.setWidth(m_plane.width());
    m_rect.setHeight(m_plane.height());

    m_rect.moveTo(m_x,m_y);

    recorder =0;





}

// 实现移动
void HeroPlane::setPostion(int x, int y){

    m_x = x;
    m_y =y;
    m_rect.moveTo(m_x,m_y);



}
