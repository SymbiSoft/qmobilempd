// checksum 0xfd0b version 0x20001
/*
  This file was generated by the Mobile Qt Application wizard of Qt Creator.
  MainWindow is a convenience class containing mobile device specific code
  such as screen orientation handling.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>

#if defined(Q_OS_SYMBIAN) && defined(ORIENTATIONLOCK)
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif // Q_OS_SYMBIAN && ORIENTATIONLOCK

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netaccess = new NetworkAccess(this);



    //Load settings
    //TODO cleanup
    loadSettings();



    CommonDebug("Start creating context view");
    //Create context view

    contextview = new Ui_ContextView(this,netaccess);
    ui->verticalLayout_2->addWidget(contextview,1);
    ui->verticalLayout_2->setEnabled(true);
    contextview->show();
    //Create Menu

    ui->menuBar->clear();
    QFont tempfont = QFont("Arial",8);
    tempfont.setBold(false);


    //FIXME
    //WORKAROUND no scrollbar on popup menu see QTBUG-17633
    menuscroller = new QScrollArea(NULL);
    viewmenu = new QMenu(this);
#ifdef Q_OS_SYMBIAN
    viewmenu->setFont(tempfont);
    viewmenu->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
#endif
    QWidget::updateGeometry();
    menuscroller->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    CommonDebug("geometry menuscroller:"+QString::number(viewmenu->sizeHint().width()+20).toAscii()+":"+QString::number(ui->btnNext->geometry().y()).toAscii());
 //   menuscroller->setWidget(viewmenu);

    menuscroller->hide();

    menuscroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    connectacion = viewmenu->addAction(tr("Connect to host"));

    QAction *settingsaction = viewmenu->addAction(tr("Settings"));
    //QAction *trackaction = viewmenu->addAction(QString("Tracks"));
    QAction *artistaction = viewmenu->addAction(tr("Artists"));
    QAction *albumaction = viewmenu->addAction(tr("Albums"));
    QAction *currentplaylistaction = viewmenu->addAction(tr("Playlist"));
    QAction *filesaction = viewmenu->addAction(tr("Files"));
    QAction *aboutaction = viewmenu->addAction(tr("About"));
    QAction *exitaction = viewmenu->addAction("Exit");
    //Connect Menubar Signals
    //connect(trackaction,SIGNAL(triggered()),contextview,SLOT(showTracks()));

    connect(connectacion,SIGNAL(triggered()),this,SLOT(tryConnect()));
    connect(settingsaction,SIGNAL(triggered()),this,SLOT(openSettings()));
    connect(artistaction,SIGNAL(triggered()),contextview,SLOT(showArtists()));
    connect(albumaction,SIGNAL(triggered()),contextview,SLOT(showAlbums()));
    connect(currentplaylistaction,SIGNAL(triggered()),contextview,SLOT(showCurrentPlaylist()));
    connect(filesaction,SIGNAL(triggered()),this,SLOT(showFiles()));
    connect(aboutaction,SIGNAL(triggered()),this,SLOT(showAbout()));
    connect(exitaction,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->btnPlay,SIGNAL(clicked()),netaccess,SLOT(pause()));
    connect(ui->btnStop,SIGNAL(clicked()),netaccess,SLOT(stop()));
    connect(ui->btnNext,SIGNAL(clicked()),netaccess,SLOT(next()));
    connect(ui->btnPrev,SIGNAL(clicked()),netaccess,SLOT(previous()));
    connect(ui->btn_Menu,SIGNAL(clicked()),this,SLOT(showMenu()));
    connect(ui->btnVolume,SIGNAL(clicked()),this,SLOT(toggleVolumeSlider()));
    connect(ui->btnRepeat,SIGNAL(clicked(bool)),netaccess,SLOT(setRepeat(bool)));
    connect(ui->btnShuffle,SIGNAL(clicked(bool)),netaccess,SLOT(setRandom(bool)));
    connect(contextview,SIGNAL(exitrequested()),this,SLOT(close()));
    connect(netaccess,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(netaccess,SIGNAL(statusUpdate(status_struct)),this,SLOT(updateStatusLabel(status_struct)));
    connect(contextview,SIGNAL(requestMaximised(bool)),this,SLOT(setMaximised(bool)));
    connect(viewmenu,SIGNAL(aboutToHide()),menuscroller,SLOT(hide()));


    //connect(viewmenu,SIGNAL(aboutToHide()),this,SLOT(sh));
    this->setWindowTitle("qMobileMPD");

    //Volume slider
    volumeslider = new QSlider(Qt::Vertical,this);
    volumeslider->setTickInterval(1);
    volumeslider->setMaximum(100);
    volumeslider->setMinimum(0);
    volumeslider->hide();

    connect(volumeslider,SIGNAL(sliderMoved(int)),this,SLOT(changeServerVolume(int)));

    showExpanded();
    currentstatusfield = StatusField_Title;

    ui->btnShuffle->setAutoExclusive(false);
    ui->btnRepeat->setAutoExclusive(false);
    ui->btnRepeat->setChecked(false);
    ui->btnShuffle->setChecked(false);


   // this->setOrientation(ScreenOrientationLockPortrait);

    if(searchDefaultServer())
    {
      //  tryConnect();
    }
    else{
        setMaximised(false);
        QMessageBox::information(this,tr("Default Server"),tr("Please define a default server under server properties which is used for connection."),QMessageBox::Ok,QMessageBox::NoButton);
        setMaximised(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#ifdef Q_OS_SYMBIAN
    if (orientation != ScreenOrientationAuto) {
#if defined(ORIENTATIONLOCK)
        const CAknAppUiBase::TAppUiOrientation uiOrientation =
                (orientation == ScreenOrientationLockPortrait) ? CAknAppUi::EAppUiOrientationPortrait
                    : CAknAppUi::EAppUiOrientationLandscape;
        CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
        TRAPD(error,
            if (appUi)
                appUi->SetOrientationL(uiOrientation);
        );
        Q_UNUSED(error)
#else // ORIENTATIONLOCK
        qWarning("'ORIENTATIONLOCK' needs to be defined on Symbian when locking the orientation.");
#endif // ORIENTATIONLOCK
    }
#elif defined(Q_WS_MAEMO_5)
    Qt::WidgetAttribute attribute;
    switch (orientation) {
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_Maemo5PortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_Maemo5LandscapeOrientation;
        break;
    case ScreenOrientationAuto:
    default:
        attribute = Qt::WA_Maemo5AutoOrientation;
        break;
    }
    setAttribute(attribute, true);

#else // Q_OS_SYMBIAN
    Q_UNUSED(orientation);
#endif // Q_OS_SYMBIAN
}

void MainWindow::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
    //showMaximized();
#elif defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    showMaximized();
#elif defined(QT_SIMULATOR)
    showFullScreen();
#else
    show();
#endif
}


void MainWindow::updateStatusLabel(status_struct tempstruct)
{
    ui->pgr_time->setValue(tempstruct.percent);
    switch(currentstatusfield) {
    case StatusField_Artist:{ui->pgr_time->setFormat(tempstruct.artist);break;}
    case StatusField_Album:{ui->pgr_time->setFormat(tempstruct.album);break;}
    case StatusField_SongInfo:{ui->pgr_time->setFormat(tempstruct.info);break;}
    case StatusField_Title:{ui->pgr_time->setFormat(tempstruct.title);break;}
    };
    currentstatusfield = (currentstatusfield+1)%NR_StatusFieldShown;
    ui->btnShuffle->setChecked(tempstruct.shuffle);
    ui->btnRepeat->setChecked(tempstruct.repeat);
    updateVolumeSlider(tempstruct.volume);
    if(tempstruct.playing==false)
    {
        ui->btnPlay->setIcon(QIcon(":/icons/media-playback-start.png"));
    }
    else
    {
       ui->btnPlay->setIcon(QIcon(":/icons/media-playback-pause.png"));
    }
}


//Settings
void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("server_properties");
    int size = settings.beginReadArray("profiles");
    CommonDebug(QString::number(size).toAscii()+" Settings found");
    for(int i = 0;i<size;i++)
    {
        serverprofile tempstruct;
        settings.setArrayIndex(i);
        tempstruct.hostname = settings.value("hostname").toString();
        tempstruct.password = settings.value("password").toString();
        tempstruct.profilename = settings.value("profilename").toString();
        tempstruct.port = settings.value("port").toUInt();
        tempstruct.defaultprofile = settings.value("default").toBool();
        connectionprofiles.append(tempstruct);
        CommonDebug("loaded Settings:"+tempstruct.hostname.toAscii()+":"+QString::number(tempstruct.port).toAscii()+":"+tempstruct.password.toAscii());
    }
    settings.endArray();
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("server_properties");
    settings.beginWriteArray("profiles");
    for(int i=0;i<connectionprofiles.length();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("hostname",connectionprofiles.at(i).hostname);
        settings.setValue("password",connectionprofiles.at(i).password);
        settings.setValue("profilename",connectionprofiles.at(i).profilename);
        settings.setValue("port",connectionprofiles.at(i).port);
        settings.setValue("default",connectionprofiles.at(i).defaultprofile);
        CommonDebug("wrote setting:"+QString::number(i).toAscii());
    }
    settings.endArray();
    settings.endGroup();
    CommonDebug("Settings written");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::showAbout()
{
    setMaximised(false);
   QMessageBox::about(this,tr("qmobilempd"),tr("qMobileMPD Version:") + QString(VERSION) + tr(" Copyright 2011 by Hendrik Borghorst, licensed under GPLv3 for more information visit: <a href='http://code.google.com/p/qmobilempd'>Homepage</a> . This applications uses <a href='http://www.tango-project.org'>Tango icons</a>."));
    setMaximised(true);
}

void MainWindow::showMenu()
{
    //setMaximised(false);
    if(!viewmenu->isVisible())
    {
        viewmenu->show();
    }
    else{
        menuscroller->hide();
        viewmenu->hide();
    }
}


void MainWindow::toggleVolumeSlider()
{
    if(volumeslider->isVisible())
    {
        volumeslider->hide();
        CommonDebug("hide volume slider");
    }
    else {
        CommonDebug("show volume slide");
        int x,y;
        x =  ui->btnVolume->geometry().x();
        y = (ui->btnVolume->geometry().y())- 200;
        CommonDebug("sliderpos:"+QString::number(x).toAscii()+":"+QString::number(y).toAscii());
        volumeslider->setGeometry(x,y,50,200);
        volumeslider->show();
        volumeslider->setVisible(true);
        volumeslider->setFocus();
    }
}

void MainWindow::updateVolumeSlider(quint8 volume)
{
    if(volume>=85)
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-high.png"));
    }
    else if((volume<85)&&(volume>=40))
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-medium.png"));
    }
    else if((volume<40)&&(volume>0))
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-low.png"));
    }
    else if(volume<=0)
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-muted.png"));
    }
    volumeslider->setValue(volume);
}

void MainWindow::changeServerVolume(int volume)
{
    if(volume>=85)
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-high.png"));
    }
    else if((volume<85)&&(volume>=40))
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-medium.png"));
    }
    else if((volume<40)&&(volume>0))
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-low.png"));
    }
    else if(volume<=0)
    {
        ui->btnVolume->setIcon(QIcon(":/icons/audio-volume-muted.png"));
    }
    netaccess->setVolume(volume);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
#ifdef Q_OS_SYMBIAN

    switch (event->nativeVirtualKey())
            {
            case 63552:
                {
                // TODO
                netaccess->setVolume(volumeslider->value()+2);
                volumeslider->setValue(volumeslider->value()+2);
                break;
                }
            case 63553:
                {
                // TODO
                netaccess->setVolume(volumeslider->value()-2);
                volumeslider->setValue(volumeslider->value()-2);
                break;
                }

            default:
                {
                break;
                }
            };
#endif
}




void MainWindow::tryConnect()
{
    //Get default host and connect
    if(searchDefaultServer()){
        bool success = netaccess->connectToHost(hostname,port);
        if(success)
        {
            CommonDebug("sucessfully connected to valid mpd,try authentication if any");
            if(password!="")
            {
                bool pwsuccess = netaccess->authenticate(password);
               /* if(!pwsuccess) {
                    bool tryanother=true;
                    while((tryanother)&&(!pwsuccess)) {
                        password = QInputDialog::getText(this,tr("Wrong password"),"Enter password",QLineEdit::Password,"",&tryanother);
                        netaccess->authenticate(password);
                    }
                    if((!tryanother)&&(!pwsuccess)) {
                        QMessageBox::warning(this,tr("Not authenticated"),tr("Functions could be limited without password"),QMessageBox::Ok,QMessageBox::NoButton);
                    }
                }*/
                if(!pwsuccess)
                {
                    QMessageBox::warning(this,tr("Not authenticated"),tr("Functions could be limited without password, check settings and reconnect"),QMessageBox::Ok,QMessageBox::NoButton);
                }
            }
            //get Playlist

            connectacion->setText("Disconnect");
            disconnect(connectacion,SIGNAL(triggered()),0,0);
            connect(connectacion,SIGNAL(triggered()),this,SLOT(tryDisconnect()));
        }
        else
        {
            QMessageBox::critical(this,tr("Connection error"),tr("Could not connect to default host check settings"));
        }
    }
}

void MainWindow::tryDisconnect()
{
    netaccess->disconnect();
}

void MainWindow::disconnected()
{
    connectacion->setText("Connect");
    disconnect(connectacion,SIGNAL(triggered()),0,0);
    connect(connectacion,SIGNAL(triggered()),this,SLOT(tryConnect()));
    //Clear old content
    ui->pgr_time->setFormat("");
    ui->pgr_time->setValue(0);
}

void MainWindow::openSettings()
{
    settingsdialogobject = new SettingsDialog(connectionprofiles,this);
    settingsdialogobject->showFullScreen();
    connect(settingsdialogobject,SIGNAL(okRequested(QList<serverprofile>)),this,SLOT(applySettings(QList<serverprofile>)));
    connect(settingsdialogobject,SIGNAL(cancelRequested()),this,SLOT(discardSettings()));
}

void MainWindow::applySettings(QList<serverprofile> profiles)
{
    connectionprofiles = profiles;
    discardSettings();
}

void MainWindow::discardSettings()
{
    settingsdialogobject->hide();
    delete settingsdialogobject;
}

bool MainWindow::searchDefaultServer()
{
        for(int i=0;i<connectionprofiles.length();i++)
        {
            if(connectionprofiles.at(i).defaultprofile)
            {
                hostname = connectionprofiles.at(i).hostname;
                password = connectionprofiles.at(i).password;
                port = connectionprofiles.at(i).port;
                return true;
            }

    }
        return false;
}


bool MainWindow::event(QEvent *event)
{
    if(event->type()==QEvent::WindowDeactivate||event->type()==QEvent::Leave)
    {
        netaccess->suspendUpdates();
    }
    if(event->type()==QEvent::WindowActivate||event->type()==QEvent::Enter)
    {
        netaccess->resumeUpdates();
    }
    if(event->type()==QEvent::Resize)
    {
        menuscroller->hide();
    }
    return QMainWindow::event(event);
}


void MainWindow::focusInEvent(QFocusEvent *event)
{
    showExpanded();
    QMainWindow::focusInEvent(event);
}

void MainWindow::showFiles()
{
    contextview->showFiles("");
}

void MainWindow::setMaximised(bool value)
{
    CommonDebug("set maximised called");
    if(!value)
    {
        showMaximized();
        ui->centralWidget->showMaximized();
    }
    else
    {
        showExpanded();
    }
}
