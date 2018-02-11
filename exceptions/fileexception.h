#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H

#include <QString>

class FileException {
	QString file;
public:
	FileException(const QString &url) : file(url){}
    QString getUrl() const{ return file; }
};

class FileOpenException : public FileException {
public:
    FileOpenException(const QString &url) : FileException(url){}
};

class JsonParsingException : public FileException{
public:
    JsonParsingException(const QString &url) : FileException(url){}
};

#endif
