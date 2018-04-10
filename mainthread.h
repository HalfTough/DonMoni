#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QThread>

#include "parser.h"


class MainProgram : public QObject{
    Q_OBJECT

    int ext=0;
    Parser *parser;
    void exitApp(int ret);
public:
    MainProgram(Parser *parser, QObject *parent = Q_NULLPTR) :QObject(parent), parser(parser){
    }
    int getExitCode(){ return ext; }
public slots:
    void run();
};

#endif
