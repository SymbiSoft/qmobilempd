#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QObject>
#include <QtNetwork>
#include "mpdalbum.h"
#include "mpdartist.h"
#include "mpdtrack.h"
#include "mpdfileentry.h"
#include "common.h"
#include "commondebug.h"
#include "networkstatusupdater.h"

class MpdAlbum;
class MpdArtist;
class MpdTrack;
class MpdFileEntry;


class NetworkAccess : public QThread
{
    Q_OBJECT
    
public:
    enum State {PAUSE,PLAYING,STOP};
    explicit NetworkAccess(QObject *parent = 0);
    Q_INVOKABLE bool connectToHost(QString hostname, quint16 port,QString password);
    bool savePlaylist(QString name);
    QList<MpdAlbum*> *getAlbums();
    QList<MpdArtist*> *getArtists();
    QList<MpdTrack*> *getTracks();
    QList<MpdAlbum*> *getArtistsAlbums(QString artist);
    QList<MpdTrack*> *getAlbumTracks(QString album);
    QList<MpdTrack*> *getAlbumTracks(QString album, QString cartist);
    QList<MpdTrack*> *getCurrentPlaylistTracks();
    QList<MpdTrack*> *getPlaylistTracks(QString name);
    QStringList *getSavedPlaylists();
    void addPlaylist(QString name);
    bool authenticate(QString passwort);
    void suspendUpdates();
    void resumeUpdates();
    void setUpdateInterval(quint16 ms);
    QList<MpdFileEntry*> *getDirectory(QString path);
    Q_INVOKABLE bool connected();
    void seekPosition(int id,int pos);
    status_struct getStatus();


signals:
    void connectionestablished();
    void disconnected();
    void statusUpdate(status_struct status);
    void connectionError(QString);
    void currentPlayListReady(QList<QObject*>*);
    void artistsReady(QList<QObject*>*);
    void albumsReady(QList<QObject*>*);
    void filesReady(QList<QObject*>*);
    void artistAlbumsReady(QList<QObject*>*);
    void albumTracksReady(QList<QObject*>*);
public slots:
    void addTrackToPlaylist(QString fileuri);
    void addAlbumToPlaylist(QString album);
    void addArtistAlbumToPlaylist(QString artist,QString album);
    void addArtist(QString artist);
    void playTrack(QString fileuri);
    void playTrackByNumber(int nr);
    void socketConnected();
    void pause();
    void next();
    void previous();
    void stop();
    void clearPlaylist();
    void setVolume(int volume);
    void setRandom(bool);
    void setRepeat(bool);
    void disconnect();
    quint32 getPlayListVersion();


protected slots:
    void connectedtoServer();
    void disconnectedfromServer();
    void updateStatusInternal();
    void errorHandle();
    
protected:
//   void run();


private:

    QString hostname;
    quint16 port;
    QString password;
    QTcpSocket* tcpsocket;
    QString mpdversion;
    QTimer *statusupdater;
    quint16 updateinterval;
    quint32 mPlaylistversion;
    NetworkStatusUpdater *updater;




};

#endif // NETWORKACCESS_H