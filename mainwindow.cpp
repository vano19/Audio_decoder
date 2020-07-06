#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <future>

#include <QMessageBox>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow( QWidget* parent ) :
    QMainWindow ( parent ),
    mUI ( new Ui::MainWindow ),
    mAudio ( new QAudioRecorder ),
    mTypeCipher ( crypto::Cipher::Undefined )
{
    mUI->setupUi( this );
    mUI->progressBar->setValue( 0 );
    setWindowTitle("Cipher handler for sounds.");
    setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec( "audio/mp3" );
    audioSettings.setQuality( QMultimedia::VeryHighQuality );
    audioSettings.setEncodingMode( QMultimedia::ConstantQualityEncoding );

    mAudio->setEncodingSettings( audioSettings );

    if ( mAudio->setOutputLocation(QUrl::fromLocalFile("voice_test") ) )
    {
        qDebug() << mAudio->outputLocation();
    }

    if ( mAudio->isAvailable() )
    {
         qDebug() << "Available!";
    }

    qDebug() << mAudio->state() << " " << mAudio->status() << " " << mAudio->error();

}

MainWindow::~MainWindow( )
{
    delete mUI;
    delete mAudio;
}


void MainWindow::on_setPath_clicked( )
{
    mPath = mUI->lineEdit->text( );
    mUI->progressBar->setValue( 0 );

    /*   If was opened just close   */
    if ( mSound.size( ) )
        mSound.close( );

    mSound.setFileName( mPath );
    if ( !mSound.open( QFile::ReadOnly ) )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Not correct path.") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
    }
    else
    {
        QMessageBox message( QMessageBox::Information , QString("OK"), QString("This fild have found.") );
        message.exec();
    }
}

bool MainWindow::wasSetTypeCipher( bool shouldBlock )
{
    switch( mTypeCipher )
    {
        case crypto::Cipher::RC4:  mUI->RC4->setEnabled( shouldBlock ); return true;
        case crypto::Cipher::AES:  mUI->AES->setEnabled( shouldBlock ); return true;
        case crypto::Cipher::Blowfish: mUI->Blowfish->setEnabled( shouldBlock ); return true;
        default:
        {
            QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Type of cipher hasn't choised.") );
            message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
            message.exec();
            return false;
        }
    }
}

void MainWindow::on_Encrypt_clicked()
{
    if ( !mPath.size() )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Path for file wasn't setted.") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return;
    }

    if ( !wasSetTypeCipher( true ) )
        return;

    QString name = mPath;
    QString name_type = name.right( 4 );
    int index = ( name.size() - 11 ) > 0 ? name.size() - 11 : 0 ;

    if ( index != 0 && name[index] == '_' )
    {
        name.remove(name.size()-11, name.size());
        name.append("_decripto");
        name.append(name_type);
    }
    else
    {
        name.remove(name.size()-4, name.size());
        name.append("_cripto");
        name.append(name_type);
    }

    QByteArray arr = mSound.readAll( );

    proccesBar( 5 );

    if ( mCryptoHandler.crypto( arr, mTypeCipher ) )
    {
        QFile sound_cryptho( name );

        if ( !sound_cryptho.open(QFile::WriteOnly) )
        {
            QMessageBox message (QMessageBox::Critical , QString("Error"), QString("Cann't create file!") );
            message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
            message.exec( );
            return;
        }

        proccesBar( 10 );

        sound_cryptho.write( arr );
        sound_cryptho.close( );
    }
    else
    {
        mUI->progressBar->setValue( 0 );
    }

    mSound.close();
    mPath.clear();
}

void MainWindow::proccesBar( int index )
{
    std::vector<int> numbers( index );
    std::iota( numbers.begin( ), numbers.end( ), 1 );

    for ( auto i : numbers )
    {
        mUI->progressBar->setValue( i * 10 );
    }
}

void MainWindow::on_Decrypt_clicked()
{
    if ( !mPath.size() )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Path for file wasn't setted.") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec( );
        return;
    }

    if ( !wasSetTypeCipher( false ) )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Type of cipher hasn't choised.") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return;
    }

    QString name = mPath;

    // handler name

    QByteArray arr = mSound.readAll( );

    proccesBar( 5 );

    if ( mCryptoHandler.decrypto( arr, mTypeCipher ) )
    {
        QFile sound_cryptho( name );

        if ( !sound_cryptho.open( QFile::WriteOnly ) )
        {
            QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Cann't create file!") );
            message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
            message.exec( );
            return;
        }

        proccesBar( 10 );

        sound_cryptho.write( arr );
        sound_cryptho.close();
    }
    else
    {
        mUI->progressBar->setValue( 0 );
    }

    mSound.close();
    mPath.clear();
}

void MainWindow::on_Start_clicked( )
{
    if ( mHasRecord )
    {
        QMessageBox message(QMessageBox::Information , QString("Information"), QString("Recording to file has already been starting."));
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return;
    }


    if ( !wasSetTypeCipher( true ) )
        return;


    mHasRecord = true;

    proccesBar( 2 );

    mAudio->record();
    qDebug() << mAudio->state() << " " << mAudio->status() << " " << mAudio->error();
}

void MainWindow::on_Stop_clicked( )
{
    if ( !mHasRecord )
    {
        QMessageBox message(QMessageBox::Information , QString("Information"), QString("Recording to file hasn't started."));
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return;
    }

    if ( !wasSetTypeCipher( false ) )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Type of cipher hasn't choised.") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return;
    }

    proccesBar( 6 );

    mHasRecord = false;

    mAudio->stop();

    qDebug() << mAudio->state() << " " << mAudio->status() << " " << mAudio->error( );

    QString name = mAudio->outputLocation().toString();

    for ( int i = 0; i < name.size(); i++ )
        if ( "C" == name[i] )
        {
            name = name.right( name.size() - i );
            break;
        }

    qDebug() << name;

    QFile sound_cryptho( name );

    if ( !sound_cryptho.open( QFile::ReadWrite ) )
    {
        QMessageBox message( QMessageBox::Critical , QString("Error"), QString("Cann't create file!") );
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec( );
        return;
    }

    QByteArray arr = sound_cryptho.readAll( );

    if ( mCryptoHandler.crypto( arr, mTypeCipher ) )
    {
        sound_cryptho.resize( 0 );

        sound_cryptho.write( arr );

        proccesBar( 10 );
    }
    else
    {
        mUI->progressBar->setValue( 0 );
    }


    sound_cryptho.close();

}


void MainWindow::on_AES_clicked(bool checked)
{
   mUI->RC4->setEnabled( !checked );
   mUI->Blowfish->setEnabled( !checked );

   mTypeCipher = checked ? crypto::Cipher::AES : crypto::Cipher::Undefined ;

}

void MainWindow::on_RC4_clicked(bool checked)
{
    mUI->AES->setEnabled( !checked );
    mUI->Blowfish->setEnabled( !checked );

    mTypeCipher = checked ? crypto::Cipher::RC4 : crypto::Cipher::Undefined ;
}

void MainWindow::on_Blowfish_clicked(bool checked)
{
    mUI->AES->setEnabled( !checked );
    mUI->RC4->setEnabled( !checked );

    mTypeCipher = checked ? crypto::Cipher::Blowfish : crypto::Cipher::Undefined ;
}

