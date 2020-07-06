#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QAudioRecorder>

#include "cryptohandler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

private slots:

    void on_setPath_clicked( );

    void on_Encrypt_clicked( );

    void on_Decrypt_clicked( );

    void on_Start_clicked( );

    void on_Stop_clicked( );

    void on_AES_clicked(bool checked);

    void on_RC4_clicked(bool checked);

    void on_Blowfish_clicked(bool checked);


private:
    bool wasSetTypeCipher( bool );
    void proccesBar( int );

    Ui::MainWindow*             mUI = nullptr;
    QString                     mPath = {};
    QFile                       mSound = {};
    crypto::CryptoHandler       mCryptoHandler = {};
    QAudioRecorder*             mAudio = nullptr;

    bool                        mHasRecord = false;
    crypto::Cipher              mTypeCipher;
};

#endif // MAINWINDOW_H
