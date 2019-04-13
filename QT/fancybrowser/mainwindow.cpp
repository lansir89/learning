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
#include <QtWebEngineWidgets>
#include "mainwindow.h"

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result) {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

//! [1]

MainWindow::MainWindow(const QUrl& url)
{

    progress = 0;

    QFile file;                                                                                             //定义一个QFile对象，用来读取和写入文件
    file.setFileName(":/jquery.min.js");                                                 //设置文件名称
    file.open(QIODevice::ReadOnly);                                                        //打开文件
    jQuery = file.readAll();                                                                        //读取文件的所有信息，并返回给jQuery
    jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");             //追加内容到jQuery对象
    file.close();                                                                                           //关闭文件，此时jQuery对象里的内容是js文件里的内容
//! [1]

//! [2]
    view = new QWebEngineView(this);                                            //定义一个QWebEngineView对象
    view->load(url);                                                                            //加载url
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));               //加载完成后调用adjustLocation保存到编辑器中
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));                  //窗口标题发生变化时调用 adjustTitle
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));                   //显示加载百分比，也就是标题栏那个百分比
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));           //网页加载完成后调用finishLoading

    locationEdit = new QLineEdit(this);                                                                     //定义一个编辑器对象，这个是网页的地址栏
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());       //定义页面尺寸策略
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));         //按下回车后，调用changeLocation

    QToolBar *toolBar = addToolBar(tr("Navigation"));                                           //定义一个工具栏
    toolBar->addAction(view->pageAction(QWebEnginePage::Back));                     //以下几项是给工具栏增加项目
    toolBar->addAction(view->pageAction(QWebEnginePage::Forward));
    toolBar->addAction(view->pageAction(QWebEnginePage::Reload));
    toolBar->addAction(view->pageAction(QWebEnginePage::Stop));
    toolBar->addWidget(locationEdit);                                                    // 工具栏的最后内容是地址栏，这个就是增加地址栏
//! [2]

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));            //定义菜单栏view
    QAction* viewSourceAction = new QAction("Page Source", this);       //定义动作
    connect(viewSourceAction, SIGNAL(triggered()), SLOT(viewSource()));         //点击菜单后，调用viewSource
    viewMenu->addAction(viewSourceAction);                                  //给菜单栏增加新项目

//! [3]
    QMenu *effectMenu = menuBar()->addMenu(tr("&Effect"));              //定义菜单栏Effect
    effectMenu->addAction("Highlight all links", this, SLOT(highlightAllLinks()));      //给Effect增加新选项，并定义槽

    rotateAction = new QAction(this);                                       //定义一个新动作
    rotateAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));        //设置动作图标
    rotateAction->setCheckable(true);                    //设置可检查动作为真，可检查指的是有开头状态的动作
    rotateAction->setText(tr("Turn images upside down"));
    connect(rotateAction, SIGNAL(toggled(bool)), this, SLOT(rotateImages(bool)));       //如果菜单被选中，则调用rotateImages
    effectMenu->addAction(rotateAction);                        //增加

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));                //定义工具栏
    toolsMenu->addAction(tr("Remove GIF images"), this, SLOT(removeGifImages()));                //给Tools增加动作
    toolsMenu->addAction(tr("Remove all inline frames"), this, SLOT(removeInlineFrames()));
    toolsMenu->addAction(tr("Remove all object elements"), this, SLOT(removeObjectElements()));
    toolsMenu->addAction(tr("Remove all embedded elements"), this, SLOT(removeEmbeddedElements()));
    setCentralWidget(view);                         //将view设置为中央窗口部件

}
//! [3]

//! 函数功能：显示网页html源码
void MainWindow::viewSource()
{
    QTextEdit* textEdit = new QTextEdit(NULL);                      //定义一个新文本编辑框
    textEdit->setAttribute(Qt::WA_DeleteOnClose);               //关闭事件产生时关闭窗口
    textEdit->adjustSize();                                                         //调整窗口属性以适应页面内容
    textEdit->move(this->geometry().center() - textEdit->rect().center());
    textEdit->show();

    view->page()->toHtml(invoke(textEdit, &QTextEdit::setPlainText));           //保存窗口在父窗口的位置
}

//! [4]
//! 函数功能：保存网页内容
void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());          //将网页中的内容保存到编辑器里
}

//! 函数功能：加载用户在地址栏输入的网址
void MainWindow::changeLocation()
{
    QUrl url = QUrl::fromUserInput(locationEdit->text());               //获取用户输入的URL，并保存到url中
    view->load(url);                                                                        //加载URL
    view->setFocus();                                                                       //在输入框设置键盘光标
}
//! [4]

//! [5]
//!  函数功能：设置网页标题
void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)                   //如果progress在1和99之间，则表示progress是网页的加载百分比
        setWindowTitle(view->title());                  //设置网页标题
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));               //显示加载百分比
}

//!函数功能：将p传递给progress，然后调用adjustTitle
void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}
//! [5]

//! [6]
//! 函数功能：网页加载完成后，重新设置网页标题，原标题是加载进度百分比
void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();                                      //这里将页面标题设置为网页title
    view->page()->runJavaScript(jQuery);            //执行js文件

    rotateImages(rotateAction->isChecked());
}
//! [6]

//! [7]
void MainWindow::highlightAllLinks()
{
    // We append '; undefined' after the jQuery call here to prevent a possible recursion loop and crash caused by
    // the way the elements returned by the each iterator elements reference each other, which causes problems upon
    // converting them to QVariants.
    QString code = "qt.jQuery('a').each( function () { qt.jQuery(this).css('background-color', 'yellow') } ); undefined";
    view->page()->runJavaScript(code);
}
//! [7]

//! [8]
void MainWindow::rotateImages(bool invert)
{
    QString code;                   //定义一个字符串对象

    // We append '; undefined' after each of the jQuery calls here to prevent a possible recursion loop and crash caused by
    // the way the elements returned by the each iterator elements reference each other, which causes problems upon
    // converting them to QVariants.
    if (invert)
        code = "qt.jQuery('img').each( function () { qt.jQuery(this).css('-webkit-transition', '-webkit-transform 2s'); qt.jQuery(this).css('-webkit-transform', 'rotate(180deg)') } ); undefined";
    else
        code = "qt.jQuery('img').each( function () { qt.jQuery(this).css('-webkit-transition', '-webkit-transform 2s'); qt.jQuery(this).css('-webkit-transform', 'rotate(0deg)') } ); undefined";
    view->page()->runJavaScript(code);
}
//! [8]

//! [9]
void MainWindow::removeGifImages()
{
    QString code = "qt.jQuery('[src*=gif]').remove()";
    view->page()->runJavaScript(code);
}

void MainWindow::removeInlineFrames()
{
    QString code = "qt.jQuery('iframe').remove()";
    view->page()->runJavaScript(code);
}

void MainWindow::removeObjectElements()
{
    QString code = "qt.jQuery('object').remove()";
    view->page()->runJavaScript(code);
}

void MainWindow::removeEmbeddedElements()
{
    QString code = "qt.jQuery('embed').remove()";
    view->page()->runJavaScript(code);
}
//! [9]

