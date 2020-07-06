#ifndef CRYPTOHANDLER_H
#define CRYPTOHANDLER_H

#include <map>
#include <QByteArray>

namespace crypto {

enum class Cipher
{
    AES = 0,
    RC4,
    Blowfish,
    Undefined
};

class CryptoHandler
{

    enum State
    {
        RawData = 0,
        Crypto,
        Decrypto
    };

public:

    /*
     * This metod crypto provided darta by key
     */

    bool crypto( QByteArray&, Cipher );


    /*
     * This metod decrypto provided darta by key
     */

    bool decrypto( QByteArray&, Cipher );

private:

    void startCrypto( QByteArray&, Cipher );
    void startDecrypto( QByteArray&, Cipher );

    static char mask;

    std::map< QByteArray, State  > mHashState;
};

} // crypto

#endif // CRYPTOHANDLER_H
