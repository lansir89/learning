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
#include <QtWebKitWidgets>

#include "window.h"

//! [Window constructor]
Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);                              //初始化ui文件
}
//! [Window constructor]

//! [set URL]
void Window::setUrl(const QUrl &url)
{    
    webView->setUrl(url);               //设置QWebView的网址
}
//! [set URL]
    
//! [begin document inspection]
void Window::on_webView_loadFinished()
{
    treeWidget->clear();                        //清除树视图类

    QWebFrame *frame = webView->page()->mainFrame();
    QWebElement document = frame->documentElement();                //获取网页对象

    examineChildElements(document, treeWidget->invisibleRootItem());
}
//! [begin document inspection]

//! [traverse document]         遍历文件
void Window::examineChildElements(const QWebElement &parentElement,
                                  QTreeWidgetItem *parentItem)
{
    QWebElement element = parentElement.firstChild();
    while (!element.isNull()) {

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, element.tagName());
        parentItem->addChild(item);                             //为treeWidget添加条目

        examineChildElements(element, item);            //嵌套调用

        element = element.nextSibling();                    //下一个
    }
}
//! [traverse document]
