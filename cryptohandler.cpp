#include "cryptohandler.h"
#include "aesencryption.h"
#include <QIcon>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDebug>

#include <chrono>

#include <openssl/blowfish.h>
#include <openssl/rc4.h>

namespace crypto
{

char CryptoHandler::mask = 0xAA;

bool CryptoHandler::crypto( QByteArray& array, Cipher type )
{
    if ( array.isEmpty() )
    {
        QMessageBox message(QMessageBox::Information , QString("Information"), QString("Empty file."));
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return false;
    }

    QByteArray hash = QCryptographicHash::hash( array, QCryptographicHash::Algorithm::Md5 );

    auto iter = mHashState.find( hash );

    qDebug() << "1_size" << mHashState.size();

    if ( mHashState.end() != iter && iter->second == State::Crypto )
    {
        QMessageBox message(QMessageBox::Information , QString("Information"), QString("File alredy crypted."));
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return false;
    }

    mHashState[ hash ] = State::RawData;

    startCrypto( array, type );

    QByteArray hash_cr = QCryptographicHash::hash( array, QCryptographicHash::Algorithm::Md5 );

    mHashState[ hash_cr ] = State::Crypto;

    qDebug() << "2_size" << mHashState.size();

    return true;
}


bool CryptoHandler::decrypto( QByteArray& array, Cipher type )
{
    if ( array.isEmpty() )
    {
        QMessageBox message(QMessageBox::Information , QString("Information"), QString("Empty file."));
        message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
        message.exec();
        return false;
    }

    QByteArray hash = QCryptographicHash::hash( array, QCryptographicHash::Algorithm::Md5 );

    qDebug() << "3_size" << mHashState.size();

    switch ( mHashState[ hash ] )
    {
        case Decrypto :
        {

            QMessageBox message(QMessageBox::Information , QString("Information"), QString("File alredy decrypted."));
            message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
            message.exec();
            break;
        }
        case RawData :
        {
            QMessageBox message(QMessageBox::Information , QString("Information"), QString("File is raw. First shoud crypt."));
            message.setWindowIcon( QIcon("C:/Users/ichiklikchi/Documents/Audio_decodev2/Icon.png") );
            message.exec();
            break;
        }
        case Crypto:
        {
            startDecrypto( array, type );
            mHashState[ hash ] = State::Decrypto;
            return true;
        }
        default:
            break;
    }

    return false;
}

static QString key = "1234567";
static BF_KEY key_bf;
static RC4_KEY key_rc4;

void CryptoHandler::startCrypto( QByteArray& array, Cipher type  )
{
    qDebug() << "C. size: " << array.size();

    auto start = std::chrono::high_resolution_clock::now();

    if ( Cipher::AES == type )
    {
        qDebug() << "AES crypto ";

        AESEncryption* cr = new AESEncryption( AESEncryption::Aes::AES_128, AESEncryption::Mode::ECB );

        QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);

        qDebug() << hashKey;

        array = cr->encode( array, hashKey );

        delete cr;
    }
    else if ( Cipher::Blowfish == type )
    {
        qDebug() << "Blowfish crypto ";

        unsigned char data[8] = "1234567";
        BF_set_key( &key_bf, 8, data );

        int i = 0;
        unsigned char out[8];
        unsigned char in[8];

        QByteArray result;
        for( char& item : array )
        {

            in[ i ] = reinterpret_cast< unsigned char& >( item );
            i++;

            if ( i == 8 )
            {
                i=0;
                BF_ecb_encrypt( in, out, &key_bf, BF_ENCRYPT );
                result.append( reinterpret_cast< char* >( out ), 8 );
            }
        }

        array = result;

    }
    else if ( Cipher::RC4 == type )
    {
        qDebug() << "RC4 crypto in :" << array.size();

        unsigned char data[8] = "1234567";
        RC4_set_key( &key_rc4, 8, data );
        std::string in = array.toStdString();
        unsigned char* outdata = new unsigned char [ in.size() + 1 ];
        RC4 ( &key_rc4, in.size() , (const unsigned char*) in.c_str(), outdata );
        std::string out( (char*) outdata, in.size() );
        QByteArray result( out.c_str(), out.size() );
        array = result;
        delete [] outdata;

        qDebug() << "RC4 crypto out :" << array.size();
    }
    else { }

    auto stop = std::chrono::high_resolution_clock::now();

    qDebug() <<  "Cipher time:" << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();

}

void CryptoHandler::startDecrypto( QByteArray& array, Cipher type  )
{
    auto start = std::chrono::high_resolution_clock::now();
    qDebug() << "D. size: " << array.size() ;

    if ( Cipher::AES == type )
    {
        qDebug() << "AES decrypto ";

        AESEncryption* cr = new AESEncryption( AESEncryption::Aes::AES_128, AESEncryption::Mode::ECB );

        QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);

        qDebug() << hashKey;

        array = cr->decode( array, hashKey );

        delete cr;
    }
    else if ( Cipher::Blowfish == type )
    {
        qDebug() << "Blowfish decrypto ";

        int i = 0;
        unsigned char out[8];
        unsigned char in[8];

        QByteArray result;
        for( char& item : array )
        {

            in[ i ] = reinterpret_cast< unsigned char& >( item );
            i++;

            if ( i == 8 )
            {
                i=0;
                BF_ecb_encrypt( in, out, &key_bf, BF_DECRYPT );
                result.append( reinterpret_cast< char* >( out ), 8 );
            }
        }
        array = result;
    }
    else if ( Cipher::RC4 == type )
    {
        qDebug() << "RC4 decrypto in :" << array.size();

        std::string in = array.toStdString();
        unsigned char* outdata = new unsigned char [ in.size() + 1 ];
        RC4 ( &key_rc4, in.size() , reinterpret_cast< const unsigned char* >( in.c_str() ), outdata );
        std::string out( reinterpret_cast< char* >( outdata ), in.size() );
        QByteArray result( out.c_str(), out.size() );
        array = result;
        delete [] outdata;

        qDebug() << "RC4 decrypto out :" << array.size();
    }
    else { }

    auto stop = std::chrono::high_resolution_clock::now();

    qDebug() <<  "Decipher time:" << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();
}


} // crypto
