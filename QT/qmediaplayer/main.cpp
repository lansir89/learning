/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
***************************************************************************/

#include <QtGui>
#include "mediaplayer.h"

const qreal DefaultVolume = -1.0;

int main (int argc, char *argv[])
{
    Q_INIT_RESOURCE(mediaplayer);                   /* 初始化资源文件 */
    QApplication app(argc, argv);
    app.setApplicationName("Media Player");         /* 设置应用名字和组织 */
    app.setOrganizationName("Qt");
    app.setQuitOnLastWindowClosed(true);            /* 无保留地退出 */

    QString fileName;
    qreal volume = DefaultVolume;                   /* qreal是double的别名 */
    bool smallScreen = false;
#ifdef Q_OS_SYMBIAN
    smallScreen = true;
#endif

    QStringList args(app.arguments());
    args.removeFirst();                             /* 移除可执行文件名 */
    while (!args.empty()) {
        const QString &arg = args.first();          /* 获取第一个参数 */
        if (QLatin1String("-small-screen") == arg || QLatin1String("--small-screen") == arg) {
            smallScreen = true;
        } else if (QLatin1String("-volume") == arg || QLatin1String("--volume") == arg) {
            if (!args.empty()) {
                args.removeFirst();
                volume = qMax(qMin(args.first().toFloat(), float(1.0)), float(0.0));
            }
        } else if (fileName.isNull()) {
            fileName = arg;
        }
        args.removeFirst();
    }

    MediaPlayer player;
    player.setSmallScreen(smallScreen);
    if (DefaultVolume != volume)
        player.setVolume(volume);
    if (!fileName.isNull())
        player.setFile(fileName);

    if (smallScreen)
        player.showMaximized();
    else
        player.show();

    return app.exec();
}

