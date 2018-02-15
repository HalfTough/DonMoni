#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H

#include "../project.h"
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

class FileParsingException : public FileException {
public:
    FileParsingException(const QString &url) : FileException(url){}
};

class SettingsParsingException{
    int number;
    QString line, file;
public:
    SettingsParsingException(QString file, QString line, int number)
        : file(file), line(line), number(number){}
    int getLineNumber() const { return number; }
    QString getLine() const { return line; }
    QString getFile() const { return file; }
};

class JsonParsingException {

};

class MoneyParsingException : public JsonParsingException{
    Money money;
public:
    MoneyParsingException(const Money money):money(money){}
    Money getMoney() const { return money; }
};

class DonationParsingException : public JsonParsingException{
    Payment donation;
public:
    DonationParsingException(const Payment &donation) : donation(donation){}
    Payment getDonation() const { return donation; }
};

class RecurParsingException : public JsonParsingException{
    RecurringDonation donation;
public:
    RecurParsingException(const RecurringDonation &donation) : donation(donation){}
    RecurringDonation getDonation() const { return donation; }
};

class ProjectParsingExeption : public JsonParsingException{
    Project project;
public:
    ProjectParsingExeption(const Project &project) : project(project){}
    Project getProject() const { return project; }
};

#endif
