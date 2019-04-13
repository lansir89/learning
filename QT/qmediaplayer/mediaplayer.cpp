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

#define SLIDER_RANGE 8

#include "mediaplayer.h"
#include "ui_settings.h"

#ifdef Q_OS_SYMBIAN
#include <cdbcols.h>
#include <cdblen.h>
#include <commdb.h>
#endif

MediaVideoWidget::MediaVideoWidget(MediaPlayer *player, QWidget *parent) :
    Phonon::VideoWidget(parent), m_player(player), m_action(this)
{
    m_action.setCheckable(true);
    m_action.setChecked(false);
    m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
    m_action.setShortcutContext(Qt::WindowShortcut);
    connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
    addAction(&m_action);
    setAcceptDrops(true);
}

/* 全屏 */
void MediaVideoWidget::setFullScreen(bool enabled)
{
    Phonon::VideoWidget::setFullScreen(enabled);
    emit fullScreenChanged(enabled);                    /* 发送全屏信号 */
}

/* 鼠标双击事件 */
void MediaVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    Phonon::VideoWidget::mouseDoubleClickEvent(e);
    setFullScreen(!isFullScreen());                     /* 鼠标双击则全屏 */
}

/* 按钮事件 */
void MediaVideoWidget::keyPressEvent(QKeyEvent *e)
{
    if(!e->modifiers()) {
        // On non-QWERTY Symbian key-based devices, there is no space key.
        // The zero key typically is marked with a space character.
        // 0是因为有些系统没有空格键
        if (e->key() == Qt::Key_Space || e->key() == Qt::Key_0) {
            m_player->playPause();              /* 暂停 */
            e->accept();
            return;
        }

        // On Symbian devices, there is no key which maps to Qt::Key_Escape
        // On devices which lack a backspace key (i.e. non-QWERTY devices),
        // the 'C' key maps to Qt::Key_Backspace
        else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace) {
            setFullScreen(false);
            e->accept();
            return;
        }
    }
    Phonon::VideoWidget::keyPressEvent(e);
}

bool MediaVideoWidget::event(QEvent *e)
{
    switch(e->type())                               /* 获取事件类型 */
    {
    case QEvent::Close:
        //we just ignore the cose events on the video widget
        //this prevents ALT+F4 from having an effect in fullscreen mode
        e->ignore();
        return true;
    case QEvent::MouseMove:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        //fall through
    case QEvent::WindowStateChange:                 /* 状态即最大化、最小化 */
        {
            //we just update the state of the checkbox, in case it wasn't already
            m_action.setChecked(windowState() & Qt::WindowFullScreen);
            const Qt::WindowFlags flags = m_player->windowFlags();
            if (windowState() & Qt::WindowFullScreen) {
                m_timer.start(1000, this);
            } else {
                m_timer.stop();
#ifndef QT_NO_CURSOR
                unsetCursor();
#endif
            }
        }
        break;
    default:
        break;
    }

    return Phonon::VideoWidget::event(e);
}

/* 定时器事件 */
void MediaVideoWidget::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timer.timerId()) {            /* 判断定时器是否是指定定时器产生的 */
        //let's store the cursor shape
#ifndef QT_NO_CURSOR
        setCursor(Qt::BlankCursor);                     /* 隐藏光标 */
#endif
    }
    Phonon::VideoWidget::timerEvent(e);
}

void MediaVideoWidget::dropEvent(QDropEvent *e)
{
    m_player->handleDrop(e);
}

void MediaVideoWidget::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

/* 一个窗口由好几个窗口组成 */
MediaPlayer::MediaPlayer() :
        playButton(0), nextEffect(0), settingsDialog(0), ui(0),
            m_AudioOutput(Phonon::VideoCategory),
            m_videoWidget(new MediaVideoWidget(this))
{
    setWindowTitle(tr("Media Player"));             /* 窗口标题 */
    setContextMenuPolicy(Qt::CustomContextMenu);    /* 设置上下文菜单 */
    m_videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
 
    QSize buttonSize(34, 28);                       /* 按钮大小 */

    QPushButton *openButton = new QPushButton(this);    /* 打开按键 */

    /* 设置按钮图标为标准图标里面的打开图标 */
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QPalette bpal;                                  /* 调色板 */
    QColor arrowcolor = bpal.buttonText().color();
    if (arrowcolor == Qt::black)
        arrowcolor = QColor(80, 80, 80);
    bpal.setBrush(QPalette::ButtonText, arrowcolor);
    openButton->setPalette(bpal);

    rewindButton = new QPushButton(this);           /* 上一首按钮 */
    rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    forwardButton = new QPushButton(this);          /* 下一首按钮 */
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    forwardButton->setEnabled(false);

    playButton = new QPushButton(this);             /* 播放按钮 */
    playIcon = style()->standardIcon(QStyle::SP_MediaPlay);     /* 一个按钮，两种状态，播放和暂停 */
    pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    playButton->setIcon(playIcon);                  /* 开始时设置为播放按钮 */

    slider = new Phonon::SeekSlider(this);          /* SeekSlider提供播放进度条 */
    slider->setMediaObject(&m_MediaObject);         /* 进度条和媒体对象绑定 */
    volume = new Phonon::VolumeSlider(&m_AudioOutput);
 
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(8, 8, 8, 8);        /* 设置边距 */

    QHBoxLayout *layout = new QHBoxLayout();

    info = new QLabel(this);                        /* 这个就是screen.png里面的那个文字 */
    info->setMinimumHeight(70);
    info->setAcceptDrops(false);                    /* 不接受放下事件 */
    info->setMargin(2);
    info->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    info->setLineWidth(2);
    info->setAutoFillBackground(true);              /* 自动填充背景 */

    QPalette palette;
    palette.setBrush(QPalette::WindowText, Qt::white);
#ifndef Q_WS_MAC
    openButton->setMinimumSize(54, buttonSize.height());    /* 设置最小尺寸 */
    rewindButton->setMinimumSize(buttonSize);
    forwardButton->setMinimumSize(buttonSize);
    playButton->setMinimumSize(buttonSize);
#endif
    info->setStyleSheet("border-image:url(:/images/screen.png) ; border-width:3px");
    info->setPalette(palette);
    info->setText(tr("<center>No media</center>"));

    volume->setFixedWidth(120);

    layout->addWidget(openButton);                          /* 布局 */
    layout->addWidget(rewindButton);
    layout->addWidget(playButton);
    layout->addWidget(forwardButton);

    layout->addStretch();
    layout->addWidget(volume);

    vLayout->addWidget(info);
    initVideoWindow();
    vLayout->addWidget(&m_videoWindow);
    QVBoxLayout *buttonPanelLayout = new QVBoxLayout();
    m_videoWindow.hide();                                   /* 隐藏m_videoWindow */
    buttonPanelLayout->addLayout(layout);
    
    timeLabel = new QLabel(this);                           /* 音乐总时间 */
    progressLabel = new QLabel(this);                       /* 播放时间 */
    QWidget *sliderPanel = new QWidget(this);
    QHBoxLayout *sliderLayout = new QHBoxLayout();          /* 布局 */
    sliderLayout->addWidget(slider);                        /* 进度条 */
    sliderLayout->addWidget(timeLabel);    
    sliderLayout->addWidget(progressLabel);    
    sliderLayout->setContentsMargins(0, 0, 0, 0);
    sliderPanel->setLayout(sliderLayout);                   /* 进度条和时间显示组成一个顶层窗口 */

    buttonPanelLayout->addWidget(sliderPanel);

    /* 将窗口sliderPanel加入布局，与隐藏的窗口组成一个窗口 */
    buttonPanelLayout->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_MAC
    layout->setSpacing(4);
    buttonPanelLayout->setSpacing(0);
    info->setMinimumHeight(100);
    info->setFont(QFont("verdana", 15));
    // QStyle *flatButtonStyle = new QWindowsStyle;
    openButton->setFocusPolicy(Qt::NoFocus);
    // openButton->setStyle(flatButtonStyle);
    // playButton->setStyle(flatButtonStyle);
    // rewindButton->setStyle(flatButtonStyle);
    // forwardButton->setStyle(flatButtonStyle);
 #endif
    QWidget *buttonPanelWidget = new QWidget(this);
    buttonPanelWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed); 
    buttonPanelWidget->setLayout(buttonPanelLayout);    
    vLayout->addWidget(buttonPanelWidget);                  /* 将组成的窗口加入布局 */
 
    QHBoxLayout *labelLayout = new QHBoxLayout();

    vLayout->addLayout(labelLayout);
    setLayout(vLayout);

    // Create menu bar:
    fileMenu = new QMenu(this);
    QAction *openFileAction = fileMenu->addAction(tr("Open &File..."));
    QAction *openUrlAction = fileMenu->addAction(tr("Open &Location..."));
#ifdef Q_OS_SYMBIAN
    QAction *selectIAPAction = fileMenu->addAction(tr("Select &IAP..."));
    connect(selectIAPAction, SIGNAL(triggered(bool)), this, SLOT(selectIAP()));
#endif
    QAction *const openLinkAction = fileMenu->addAction(tr("Open &RAM File..."));

    connect(openLinkAction, SIGNAL(triggered(bool)), this, SLOT(openRamFile()));

    fileMenu->addSeparator();  
    QMenu *aspectMenu = fileMenu->addMenu(tr("&Aspect ratio"));
    QActionGroup *aspectGroup = new QActionGroup(aspectMenu);
    connect(aspectGroup, SIGNAL(triggered(QAction*)), this, SLOT(aspectChanged(QAction*)));
    aspectGroup->setExclusive(true);
    QAction *aspectActionAuto = aspectMenu->addAction(tr("Auto"));
    aspectActionAuto->setCheckable(true);
    aspectActionAuto->setChecked(true);
    aspectGroup->addAction(aspectActionAuto);
    QAction *aspectActionScale = aspectMenu->addAction(tr("Scale"));
    aspectActionScale->setCheckable(true);
    aspectGroup->addAction(aspectActionScale);
    QAction *aspectAction16_9 = aspectMenu->addAction(tr("16/9"));
    aspectAction16_9->setCheckable(true);
    aspectGroup->addAction(aspectAction16_9);
    QAction *aspectAction4_3 = aspectMenu->addAction(tr("4/3"));
    aspectAction4_3->setCheckable(true);
    aspectGroup->addAction(aspectAction4_3);

    QMenu *scaleMenu = fileMenu->addMenu(tr("&Scale mode"));
    QActionGroup *scaleGroup = new QActionGroup(scaleMenu);
    connect(scaleGroup, SIGNAL(triggered(QAction*)), this, SLOT(scaleChanged(QAction*)));
    scaleGroup->setExclusive(true);
    QAction *scaleActionFit = scaleMenu->addAction(tr("Fit in view"));
    scaleActionFit->setCheckable(true);
    scaleActionFit->setChecked(true);
    scaleGroup->addAction(scaleActionFit);
    QAction *scaleActionCrop = scaleMenu->addAction(tr("Scale and crop"));
    scaleActionCrop->setCheckable(true);
    scaleGroup->addAction(scaleActionCrop);

    m_fullScreenAction = fileMenu->addAction(tr("Full screen video"));
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setEnabled(false); // enabled by hasVideoChanged
    bool b = connect(m_fullScreenAction, SIGNAL(toggled(bool)), m_videoWidget, SLOT(setFullScreen(bool)));
    Q_ASSERT(b);
    b = connect(m_videoWidget, SIGNAL(fullScreenChanged(bool)), m_fullScreenAction, SLOT(setChecked(bool)));
    Q_ASSERT(b);

    fileMenu->addSeparator();
    QAction *settingsAction = fileMenu->addAction(tr("&Settings..."));

    // Setup signal connections:
    connect(rewindButton, SIGNAL(clicked()), this, SLOT(rewind()));
    //connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    openButton->setMenu(fileMenu);

    connect(playButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    //connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(showSettingsDialog()));
    connect(openUrlAction, SIGNAL(triggered(bool)), this, SLOT(openUrl()));
    connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));

    connect(m_videoWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
    connect(&m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(updateInfo()));
    connect(&m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime()));
    connect(&m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime()));
    connect(&m_MediaObject, SIGNAL(finished()), this, SLOT(finished()));
    connect(&m_MediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(&m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
    connect(&m_MediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(hasVideoChanged(bool)));

    rewindButton->setEnabled(false);
    playButton->setEnabled(false);
    setAcceptDrops(true);

    m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
    Phonon::createPath(&m_MediaObject, m_videoWidget);

    resize(minimumSizeHint());
}

/* 播放器状态改变 */
void MediaPlayer::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    Q_UNUSED(oldstate);                     /* oldstate不再使用 */

    if (oldstate == Phonon::LoadingState) {     /* 如果oldstate为加载状态 */
//        QRect videoHintRect = QRect(QPoint(0, 0), m_videoWindow.sizeHint());
//        QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
        if (!m_smallScreen) {
            if (m_MediaObject.hasVideo()) {         /* 检查是否有视频资源 */
                // Flush event que so that sizeHint takes the
                // recently shown/hidden m_videoWindow into account:
                qApp->processEvents();
                resize(sizeHint());                 /* 有数据则设置窗口大小 */
            } else
                resize(minimumSize());              /* 否则设成最小化，这里人已经看不见了 */
        }
    }

    switch (newstate) {
        case Phonon::ErrorState:
            if (m_MediaObject.errorType() == Phonon::FatalError) {  /* 如果状态没出错 */
                playButton->setEnabled(false);
                rewindButton->setEnabled(false);
            } else {
                m_MediaObject.pause();
            }
            QMessageBox::warning(this, "Phonon Mediaplayer", m_MediaObject.errorString(), QMessageBox::Close);
            break;

        case Phonon::StoppedState:                  /* 停止状态 */
            m_videoWidget->setFullScreen(false);
            // Fall through
        case Phonon::PausedState:                   /* 暂停状态 */
            playButton->setIcon(playIcon);
            if (m_MediaObject.currentSource().type() != Phonon::MediaSource::Invalid){/* 资源无效 */
                playButton->setEnabled(true);
                rewindButton->setEnabled(true);
            } else {
                playButton->setEnabled(false);
                rewindButton->setEnabled(false);
            }
            break;
        case Phonon::PlayingState:
            playButton->setEnabled(true);
            playButton->setIcon(pauseIcon);
            if (m_MediaObject.hasVideo())
                m_videoWindow.show();
            // Fall through
        case Phonon::BufferingState:                /* 等待数据状态 */
            rewindButton->setEnabled(true);
            break;
        case Phonon::LoadingState:                  /* 加载状态 */
            rewindButton->setEnabled(false);
            break;
    }

}

/* 初始化设置对话框 */
void MediaPlayer::initSettingsDialog()
{
    settingsDialog = new QDialog(this);
    ui = new Ui_settings();
    ui->setupUi(settingsDialog);

    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
    connect(ui->hueSlider, SIGNAL(valueChanged(int)), this, SLOT(setHue(int)));
    connect(ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(setSaturation(int)));
    connect(ui->contrastSlider , SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
    connect(ui->aspectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAspect(int)));
    connect(ui->scalemodeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setScale(int)));

    ui->brightnessSlider->setValue(int(m_videoWidget->brightness() * SLIDER_RANGE));
    ui->hueSlider->setValue(int(m_videoWidget->hue() * SLIDER_RANGE));
    ui->saturationSlider->setValue(int(m_videoWidget->saturation() * SLIDER_RANGE));
    ui->contrastSlider->setValue(int(m_videoWidget->contrast() * SLIDER_RANGE));
    ui->aspectCombo->setCurrentIndex(m_videoWidget->aspectRatio());
    ui->scalemodeCombo->setCurrentIndex(m_videoWidget->scaleMode());
    connect(ui->effectButton, SIGNAL(clicked()), this, SLOT(configureEffect()));

#ifdef Q_WS_X11
    //Cross fading is not currently implemented in the GStreamer backend
    ui->crossFadeSlider->setVisible(false);
    ui->crossFadeLabel->setVisible(false);
    ui->crossFadeLabel1->setVisible(false);
    ui->crossFadeLabel2->setVisible(false);
    ui->crossFadeLabel3->setVisible(false);
#endif
    ui->crossFadeSlider->setValue((int)(2 * m_MediaObject.transitionTime() / 1000.0f));
    
    // Insert audio devices:
    QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
    for (int i=0; i<devices.size(); i++){
        QString itemText = devices[i].name();
        if (!devices[i].description().isEmpty()) {
            itemText += QString::fromLatin1(" (%1)").arg(devices[i].description());
        }
        ui->deviceCombo->addItem(itemText);
        if (devices[i] == m_AudioOutput.outputDevice())
            ui->deviceCombo->setCurrentIndex(i);
    }

    // Insert audio effects:
    ui->audioEffectsCombo->addItem(tr("<no effect>"));
    QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
    Phonon::Effect *currEffect = currEffects.size() ? currEffects[0] : 0;
    QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
    for (int i=0; i<availableEffects.size(); i++){
        ui->audioEffectsCombo->addItem(availableEffects[i].name());
        if (currEffect && availableEffects[i] == currEffect->description())
            ui->audioEffectsCombo->setCurrentIndex(i+1);
    }
    connect(ui->audioEffectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(effectChanged()));

}

/* 设置音量 */
void MediaPlayer::setVolume(qreal volume)
{
    m_AudioOutput.setVolume(volume);
}

/* 最小化 */
void MediaPlayer::setSmallScreen(bool smallScreen)
{
    m_smallScreen = smallScreen;
}

void MediaPlayer::effectChanged()
{
    int currentIndex = ui->audioEffectsCombo->currentIndex();
    if (currentIndex) {

        /* 获取支持的音频效果 */
        QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
        Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

        QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
        Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;

        // Deleting the running effect will stop playback, it is deleted when removed from path
        if (nextEffect && !(currentEffect && (currentEffect->description().name() == nextEffect->description().name())))
            delete nextEffect;

        nextEffect = new Phonon::Effect(chosenEffect);
    }
    ui->effectButton->setEnabled(currentIndex);
}

/* 显示设置对话框 */
void MediaPlayer::showSettingsDialog()
{
    const bool hasPausedForDialog = playPauseForDialog();

    if (!settingsDialog)
        initSettingsDialog();

    float oldBrightness = m_videoWidget->brightness();      /* 获取各种参数 */
    float oldHue = m_videoWidget->hue();
    float oldSaturation = m_videoWidget->saturation();
    float oldContrast = m_videoWidget->contrast();
    Phonon::VideoWidget::AspectRatio oldAspect = m_videoWidget->aspectRatio();
    Phonon::VideoWidget::ScaleMode oldScale = m_videoWidget->scaleMode();
    int currentEffect = ui->audioEffectsCombo->currentIndex();
    settingsDialog->exec();                                 /* 模态对话框 */
    
    if (settingsDialog->result() == QDialog::Accepted){     /* 如果接受作为模态对话框 */

        /* 定义两首歌之间的过渡时间 */
        m_MediaObject.setTransitionTime((int)(1000 * float(ui->crossFadeSlider->value()) / 2.0f));

        /* 获取音频设备 */
        QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();

        /* 绑定输出数据和设备 */
        m_AudioOutput.setOutputDevice(devices[ui->deviceCombo->currentIndex()]);

        /* 获取有哪些音乐效果对象 */
        QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();

        /* 返回音乐效果描述 */
        QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();

        if (ui->audioEffectsCombo->currentIndex() > 0){

            /* 默认为第一个音乐效果 */
            Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;    
            if (!currentEffect || currentEffect->description() != nextEffect->description()){
                foreach(Phonon::Effect *effect, currEffects) {
                    m_audioOutputPath.removeEffect(effect);
                    delete effect;
                }
                m_audioOutputPath.insertEffect(nextEffect);
            }
        } else {
            foreach(Phonon::Effect *effect, currEffects) {
                m_audioOutputPath.removeEffect(effect);
                delete effect;
                nextEffect = 0;
            }
        }
    } else {
        // Restore previous settings
        m_videoWidget->setBrightness(oldBrightness);
        m_videoWidget->setSaturation(oldSaturation);
        m_videoWidget->setHue(oldHue);
        m_videoWidget->setContrast(oldContrast);
        m_videoWidget->setAspectRatio(oldAspect);
        m_videoWidget->setScaleMode(oldScale);
        ui->audioEffectsCombo->setCurrentIndex(currentEffect);
    }

    if (hasPausedForDialog)
        m_MediaObject.play();
}

/* 初始化m_videoWindow */
void MediaPlayer::initVideoWindow()
{
    QVBoxLayout *videoLayout = new QVBoxLayout();
    videoLayout->addWidget(m_videoWidget);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    m_videoWindow.setLayout(videoLayout);
    m_videoWindow.setMinimumSize(100, 100);
}

/* 配置音乐效果 */
void MediaPlayer::configureEffect()
{
    if (!nextEffect)
        return;


    QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
    const QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
    if (ui->audioEffectsCombo->currentIndex() > 0) {
        Phonon::EffectDescription chosenEffect = availableEffects[ui->audioEffectsCombo->currentIndex() - 1];

        QDialog effectDialog;
        effectDialog.setWindowTitle(tr("Configure effect"));
        QVBoxLayout *topLayout = new QVBoxLayout(&effectDialog);

        QLabel *description = new QLabel("<b>Description:</b><br>" + chosenEffect.description(), &effectDialog);
        description->setWordWrap(true);                         /* 文本设置为有包装 */
        topLayout->addWidget(description);

        QScrollArea *scrollArea = new QScrollArea(&effectDialog);       /* 定义一个滚动条区域 */
        topLayout->addWidget(scrollArea);

        QVariantList savedParamValues;                                  /* 定义链表 */
        foreach(Phonon::EffectParameter param, nextEffect->parameters()) {  /* 迭代每一个参数 */
            savedParamValues << nextEffect->parameterValue(param);      /* 参数值保存到list */
        }

        QWidget *scrollWidget = new Phonon::EffectWidget(nextEffect);   /* 定义一个效果窗口 */
        scrollWidget->setMinimumWidth(320);
        scrollWidget->setContentsMargins(10, 10, 10,10);
        scrollArea->setWidget(scrollWidget);

        QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
        connect(bbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
        connect(bbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
        topLayout->addWidget(bbox);

        effectDialog.exec();

        if (effectDialog.result() != QDialog::Accepted) {
            //we need to restore the parameters values
            int currentIndex = 0;
            foreach(Phonon::EffectParameter param, nextEffect->parameters()) {
                nextEffect->setParameterValue(param, savedParamValues.at(currentIndex++));
            }

        }
    }
}

void MediaPlayer::handleDrop(QDropEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if (e->proposedAction() == Qt::MoveAction){         /* 如果动作是移动数据 */
        // Just add to the queue:
        for (int i=0; i<urls.size(); i++)
            m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
    } else {
        // Create new queue:
        m_MediaObject.clearQueue();
        if (urls.size() > 0) {
            QString fileName = urls[0].toLocalFile();       /* 将url转换为本地目录 */
            QDir dir(fileName);
            if (dir.exists()) {                             /* 如果拖动的数据是个目录 */
                dir.setFilter(QDir::Files);                 /* 设置过滤器 */
                QStringList entries = dir.entryList();      /* 过滤 */
                if (entries.size() > 0) {
                    setFile(fileName + QDir::separator() +  entries[0]);    /* 设置要播放的歌曲 */
                    for (int i=1; i< entries.size(); ++i)   /* 将拖动的数据保存到队列中 */
                        m_MediaObject.enqueue(fileName + QDir::separator() + entries[i]);
                }
            } else {                                        /* 如果拖动的数据是个文件 */
                setFile(fileName);
                for (int i=1; i<urls.size(); i++)
                    m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
            }        
        }
    }
    forwardButton->setEnabled(m_MediaObject.queue().size() > 0);
    m_MediaObject.play();
}

/* 鼠标放下事件 */
void MediaPlayer::dropEvent(QDropEvent *e)
{
    /* 被拖动的数据有Url并且动作不是创建链表，这个url应该是目录 */
    if (e->mimeData()->hasUrls() && e->proposedAction() != Qt::LinkAction) {
        e->acceptProposedAction();
        handleDrop(e);
    } else {
        e->ignore(); 
    }
}

/* 鼠标进入事件 */
void MediaPlayer::dragEnterEvent(QDragEnterEvent *e)
{
    dragMoveEvent(e);
}

/* 鼠标拖动事件 */
void MediaPlayer::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        if (e->proposedAction() == Qt::CopyAction || e->proposedAction() == Qt::MoveAction){
            e->acceptProposedAction();
        }
    }
}

/* 暂停 */
void MediaPlayer::playPause()
{
    if (m_MediaObject.state() == Phonon::PlayingState)      /* 如果播放器状态为播放状态 */
        m_MediaObject.pause();
    else {
        if (m_MediaObject.currentTime() == m_MediaObject.totalTime())   /* 如果已经播放完毕 */
            m_MediaObject.seek(0);                          /* 重定位到开头 */
        m_MediaObject.play();                               /* 播放 */
    }
}

void MediaPlayer::setFile(const QString &fileName)
{
    setWindowTitle(fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1));
    m_MediaObject.setCurrentSource(Phonon::MediaSource(fileName));      /* 设置要播放的文件名 */
    m_MediaObject.play();
}

/* 设置Url，拖动的时候应该是拖动一个url */
void MediaPlayer::setLocation(const QString& location)
{
    setWindowTitle(location.right(location.length() - location.lastIndexOf('/') - 1));
    m_MediaObject.setCurrentSource(Phonon::MediaSource(QUrl::fromEncoded(location.toUtf8())));
    m_MediaObject.play();
}

bool MediaPlayer::playPauseForDialog()
{
    // If we're running on a small screen, we want to pause the video when
    // popping up dialogs. We neither want to tamper with the state if the
    // user has paused.
    if (m_smallScreen && m_MediaObject.hasVideo()) {        /* 如果是最小化而且有音乐在流中 */
        if (Phonon::PlayingState == m_MediaObject.state()) {    /* 如果播放器状态为播放，则停止 */
            m_MediaObject.pause();
            return true;
        }
    }
    return false;
}

/* 打开文件 */
void MediaPlayer::openFile()
{
    const bool hasPausedForDialog = playPauseForDialog();

    QStringList fileNames = QFileDialog::getOpenFileNames(this, QString(),
                                                          QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    if (hasPausedForDialog)
        m_MediaObject.play();

    m_MediaObject.clearQueue();                         /* 清空队列 */
    if (fileNames.size() > 0) {
        QString fileName = fileNames[0];
        setFile(fileName);
        for (int i=1; i<fileNames.size(); i++)
            m_MediaObject.enqueue(Phonon::MediaSource(fileNames[i]));
    }
    forwardButton->setEnabled(m_MediaObject.queue().size() > 0);    /* 下一首设置为可用 */
}

/* 缓冲区状态 */
void MediaPlayer::bufferStatus(int percent)
{
    if (percent == 100)                                             /* 如果缓冲区已满 */
        progressLabel->setText(QString());
    else {
        QString str = QString::fromLatin1("(%1%)").arg(percent);
        progressLabel->setText(str);
    }
}

/* 设置饱和度。这个函数和下面几个函数都不是递归，只是个简单的封装函数 */
void MediaPlayer::setSaturation(int val)
{
    m_videoWidget->setSaturation(val / qreal(SLIDER_RANGE));
}

/* 设置色彩 */
void MediaPlayer::setHue(int val)
{
    m_videoWidget->setHue(val / qreal(SLIDER_RANGE));
}

/* 设置方向比例 */
void MediaPlayer::setAspect(int val)
{
    m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(val));
}

/* 设置大小模式，如果video和widget大小不等，设置这个 */
void MediaPlayer::setScale(int val)
{
    m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(val));
}

/* 设置亮度 */
void MediaPlayer::setBrightness(int val)
{
    m_videoWidget->setBrightness(val / qreal(SLIDER_RANGE));
}

/* 设置对比度 */
void MediaPlayer::setContrast(int val)
{
    m_videoWidget->setContrast(val / qreal(SLIDER_RANGE));
}

/* 这个应该是更新歌曲信息，也就是图片中间那个显示文件的label */
void MediaPlayer::updateInfo()
{
    int maxLength = 30;
    QString font = "<font color=#ffeeaa>";      /* 这个是label中title的显示格式 */
    QString fontmono = "<font family=\"monospace,courier new\" color=#ffeeaa>";

    /* m_MediaObject.metaData()返回的是歌曲信息，包括歌手名、歌名等等 */
    QMap <QString, QString> metaData = m_MediaObject.metaData();
    QString trackArtist = metaData.value("ARTIST");     /* 返回ARTIST键关联的值，这个是保存歌手名 */
    if (trackArtist.length() > maxLength)
        trackArtist = trackArtist.left(maxLength) + "...";
    
    QString trackTitle = metaData.value("TITLE");       /* 返回TITLE保存的值，保存歌名 */
    int trackBitrate = metaData.value("BITRATE").toInt();

    QString fileName;                               /* 保存资源文件名，应该包括路径 */
    if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) { /* 如果媒体源的类型为URL */
        fileName = m_MediaObject.currentSource().url().toString();
    } else {
        fileName = m_MediaObject.currentSource().fileName();
        fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
        if (fileName.length() > maxLength)
            fileName = fileName.left(maxLength) + "...";
    }
    
    QString title;    
    if (!trackTitle.isEmpty()) {            /* 如果trackTitle非空 */
        if (trackTitle.length() > maxLength)    /* 并且长度大于最大长度 */
            trackTitle = trackTitle.left(maxLength) + "...";
        title = "Title: " + font + trackTitle + "<br></font>";
    } else if (!fileName.isEmpty()) {
        if (fileName.length() > maxLength)
            fileName = fileName.left(maxLength) + "...";
        title = font + fileName + "</font>";
        if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) {
            title.prepend("Url: ");             /* title前面加上字符串Url: */
        } else {
            title.prepend("File: ");
        }
    }
        
    QString artist;
    if (!trackArtist.isEmpty())
        artist = "Artist:  " + font + trackArtist + "</font>";

    QString bitrate;
    if (trackBitrate != 0)
        bitrate = "<br>Bitrate:  " + font + QString::number(trackBitrate/1000) + "kbit</font>";

    info->setText(title + artist + bitrate);        /* 证据显示，这个bitrate好像没啥用，没显示什么东西 */
}

void MediaPlayer::updateTime()
{
    long len = m_MediaObject.totalTime();           /* 总时间，单位是毫秒 */
    long pos = m_MediaObject.currentTime();         /* 当前时间 */
    QString timeString;    
    if (pos || len)
    {
        int sec = pos/1000;                         /* 计算当前时间 */
        int min = sec/60;
        int hour = min/60;
        int msec = pos;
        QTime playTime(hour%60, min%60, sec%60, msec%1000); /* 使用当前时间创建一个时间对象 */

        sec = len / 1000;                           /* 计算返回时间 */
        min = sec / 60;
        hour = min / 60;
        msec = len;
        QTime stopTime(hour%60, min%60, sec%60, msec%1000); /* 总时间也就是停止时间 */

        QString timeFormat = "m:ss";
        if (hour > 0)
            timeFormat = "h:mm:ss";        
        timeString = playTime.toString(timeFormat);         /* 转换成字符串 */
        if (len)
            timeString += " / " + stopTime.toString(timeFormat);    /* 注意这个"/" */
    }
    timeLabel->setText(timeString);
}

/* 重新定位到开头 */
void MediaPlayer::rewind()
{
    m_MediaObject.seek(0);
}

/* 下一首 */
void MediaPlayer::forward()
{
    QList<Phonon::MediaSource> queue = m_MediaObject.queue();   /* 获取媒体文件的队列 */
    if (queue.size() > 0) {
        m_MediaObject.setCurrentSource(queue[0]);
        forwardButton->setEnabled(queue.size() > 1);    /* 如果文件数量大于1则使能下一首按钮 */
        m_MediaObject.play();
    }
}

/* 打开链接 */
void MediaPlayer::openUrl()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QString sourceURL = settings.value("location").toString();
    bool ok = false;

    /* 这个打开openLocation对话框并获取用户的输入内容 */
    sourceURL = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"), QLineEdit::Normal, sourceURL, &ok);
    if (ok && !sourceURL.isEmpty()) {
        setLocation(sourceURL);
        settings.setValue("location", sourceURL);       /* 将地址写到setting */
    }
}

/*!
 \since 4.6
 */
void MediaPlayer::openRamFile()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));

    const QStringList fileNameList(QFileDialog::getOpenFileNames(this,
                                                                  QString(),
                                                                  settings.value("openRamFile").toString(),
                                                                  QLatin1String("RAM files (*.ram)")));

    if (fileNameList.isEmpty())
        return;

    QFile linkFile;
    QList<QUrl> list;
    QByteArray sourceURL;
    for (int i = 0; i < fileNameList.count(); i++ ) {
        linkFile.setFileName(fileNameList[i]);
        if (linkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!linkFile.atEnd()) {
                sourceURL = linkFile.readLine().trimmed();
                if (!sourceURL.isEmpty()) {
                    const QUrl url(QUrl::fromEncoded(sourceURL));
                    if (url.isValid())
                        list.append(url);
                }
            }
            linkFile.close();
        }
    }

    if (!list.isEmpty()) {
        m_MediaObject.clearQueue();
        setLocation(list[0].toString());
        for (int i = 1; i < list.count(); i++)
            m_MediaObject.enqueue(Phonon::MediaSource(list[i]));
        m_MediaObject.play();
    }

    forwardButton->setEnabled(!m_MediaObject.queue().isEmpty());
    settings.setValue("openRamFile", fileNameList[0]);
}

void MediaPlayer::finished()
{
}

/* 显示菜单，即openFile那个菜单 */
void MediaPlayer::showContextMenu(const QPoint &p)
{
    /* 在指定的位置显示菜单，如果是全屏 */
    fileMenu->popup(m_videoWidget->isFullScreen() ? p : mapToGlobal(p));
}

/* 设置widget和video的大小比例 */
void MediaPlayer::scaleChanged(QAction *act)
{
    if (act->text() == tr("Scale and crop"))
        m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
    else 
        m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);    
}

/* 改变video的宽和高的比例 */
void MediaPlayer::aspectChanged(QAction *act)
{
    if (act->text() == tr("16/9"))
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
    else if (act->text() == tr("Scale"))
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
    else if (act->text() == tr("4/3"))
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
    else
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);    
}

void MediaPlayer::hasVideoChanged(bool bHasVideo)
{
    info->setVisible(!bHasVideo);
    m_videoWindow.setVisible(bHasVideo);
    m_fullScreenAction->setEnabled(bHasVideo);
}

#ifdef Q_OS_SYMBIAN
void MediaPlayer::selectIAP()
{
    TRAPD(err, selectIAPL());
    if (KErrNone != err)
        QMessageBox::warning(this, "Phonon Mediaplayer", "Error selecting IAP", QMessageBox::Close);
}

void MediaPlayer::selectIAPL()
{
    QVariant currentIAPValue = m_MediaObject.property("InternetAccessPointName");
    QString currentIAPString = currentIAPValue.toString();
    bool ok = false;
    CCommsDatabase *commsDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commsDb);
    commsDb->ShowHiddenRecords();
    CCommsDbTableView* view = commsDb->OpenTableLC(TPtrC(IAP));
    QStringList items;
    TInt currentIAP = 0;
    for (TInt l = view->GotoFirstRecord(), i = 0; l != KErrNotFound; l = view->GotoNextRecord(), i++) {
       TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
       view->ReadTextL(TPtrC(COMMDB_NAME), iapName);
       QString iapString = QString::fromUtf16(iapName.Ptr(), iapName.Length());
       items << iapString;
       if (iapString == currentIAPString)
           currentIAP = i;
    }
    currentIAPString = QInputDialog::getItem(this, tr("Select Access Point"), tr("Select Access Point"), items, currentIAP, false, &ok);
    if (ok)
        m_MediaObject.setProperty("InternetAccessPointName", currentIAPString);
    CleanupStack::PopAndDestroy(2); //commsDB, view
}
#endif
