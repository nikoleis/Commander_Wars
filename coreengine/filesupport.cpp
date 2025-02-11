#include "coreengine/filesupport.h"
#include "coreengine/settings.h"

#include <QCryptographicHash>
#include <QDirIterator>
#include <QCoreApplication>

QByteArray Filesupport::getHash(const QStringList & filter, const QStringList & folders)
{
    QCryptographicHash myHash(QCryptographicHash::Sha3_512);
    QStringList fullList;
    for (const auto & folder : qAsConst(folders))
    {
        fullList.append(oxygine::Resource::RCC_PREFIX_PATH + folder);
        fullList.append(Settings::getUserPath() + folder);
    }
    for (const auto & folder : qAsConst(fullList))
    {
        QString path =  folder;
        QDirIterator dirIter(path, filter, QDir::Files, QDirIterator::Subdirectories);
        while (dirIter.hasNext())
        {
            dirIter.next();
            QFile file(dirIter.filePath());
            file.open(QIODevice::ReadOnly | QIODevice::Truncate);
            myHash.addData(&file);
            file.close();
        }
    }
    return myHash.result();
}

QByteArray Filesupport::getRuntimeHash(const QStringList & mods)
{
    QStringList folders = mods;
    folders.append("/resources");
    QStringList filter = {"*.js", "*.txt", "*.csv", "*.xml"};
    return getHash(filter, folders);
}

void Filesupport::writeByteArray(QDataStream& stream, const QByteArray& array)
{
    stream << static_cast<qint32>(array.size());
    for (qint32 i = 0; i < array.size(); i++)
    {
        stream << static_cast<qint8>(array[i]);
    }
}

void Filesupport::writeBytes(QDataStream& stream, const QByteArray& array)
{
    for (qint32 i = 0; i < array.size(); i++)
    {
        stream << static_cast<qint8>(array[i]);
    }
}

QByteArray Filesupport::readByteArray(QDataStream& stream)
{
    QByteArray array;
    qint32 size = 0;
    stream >> size;
    for (qint32 i = 0; i < size; i++)
    {
        qint8 value = 0;
        stream >> value;
        array.append(value);
    }
    return array;
}

void Filesupport::storeList(const QString & file, const QStringList & items, const QString & folder)
{
    QDir dir(folder);
    dir.mkpath(".");
    QFile dataFile(folder + file + ".bl");
    dataFile.open(QIODevice::WriteOnly);
    QDataStream stream(&dataFile);
    stream << file;
    stream << static_cast<qint32>(items.size());
    for (qint32 i = 0; i < items.size(); i++)
    {
        stream << items[i];
    }
}

Filesupport::StringList Filesupport::readList(const QString & file, const QString & folder)
{
    return readList(folder + file);
}

Filesupport::StringList Filesupport::readList(const QString & file)
{
    QFile dataFile(file);
    dataFile.open(QIODevice::ReadOnly);
    QDataStream stream(&dataFile);
    StringList ret;
    stream >> ret.name;
    qint32 size = 0;
    stream >> size;
    for (qint32 i = 0; i < size; i++)
    {
        QString name;
        stream >> name;
        ret.items.append(name);
    }
    return ret;
}
