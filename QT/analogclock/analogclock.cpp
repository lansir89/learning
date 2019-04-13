/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "analogclock.h"

AnalogClock::AnalogClock(QWidget *parent)
    : QWidget(parent)
{
    QTimer *timer = new QTimer(this);                                           //定义一个定时器
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));        //定义器超时后更新
    timer->start(1000);                                                                 //启动定时器，时间间隔为1000ms

    setWindowTitle(tr("Analog Clock"));
    resize(200, 200);                                                   //重设窗口大小
}

//!重载绘画事件，这里描绘窗口，同时更新槽update()调用时这个函数也调用
void AnalogClock::paintEvent(QPaintEvent *)
{
    static const QPoint hourHand[3] = {                  //时针线，由三个点连接而线
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };
    static const QPoint minuteHand[3] = {           //分针线，由三个点连接而成
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };

    QColor hourColor(127, 0, 127);                  //时针线的RGB颜色值
    QColor minuteColor(0, 127, 127, 191);       //分针线的RGB颜色值，同时还有191的alpha分量，这意味着它的75％不透明

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();                  //返回系统的当前时间值，并保存到time

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  //Antialiasing 指示引擎如果可能的话应该边缘反走样,即通过使用不同的颜色强度平滑边缘
    painter.translate(width() / 2, height() / 2);           //移动原点到窗口中心位置
    painter.scale(side / 200.0, side / 200.0);              //缩放坐标系统，因为side被 “resize(200, 200); ”一句设置成200，所以这里的side/200=1

    painter.setPen(Qt::NoPen);                                 //设置画笔
    painter.setBrush(hourColor);                            //设置画刷

    painter.save();                                                     //保存,防止坐标系跑偏了

     /* time.minute() / 60.0＊30指这个小时应该旋转多少度，time.hour*30指过去几个小时应该旋转多少度 */
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 3);                              //使用hourHand里面的3个点描绘出时针线
    painter.restore();

    painter.setPen(hourColor);

    //这里画出12条线，每条线代表一个小时
    for (int i = 0; i < 12; ++i) {
        painter.drawLine(0, 88, 0, 96);                         //画线
        painter.drawText(-8, -85, 15, 75,
                      Qt::AlignHCenter | Qt::AlignTop,
                      QString::number(i));
       painter.rotate(30.0);                                           //画笔旋转30度

    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    painter.setPen(minuteColor);

    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter.drawLine(0, 92, 0, 96);
        painter.rotate(6.0);
    }
    painter.rotate(90);
}
