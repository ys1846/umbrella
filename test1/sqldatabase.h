#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>

#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>

struct Data
{
    int areaNum;
    int engine[6];
};

class Sqldatabase : public QObject
{
    Q_OBJECT
public:
    explicit Sqldatabase(QObject *parent = nullptr);

    void databaseInit();
    void databaseCreateTable();
    bool databaseInsert(Data data);
    bool databaseUpdate(Data data);
    Data databaseGetdata(int areaNum);

signals:

public slots:
};

#endif // SQLDATABASE_H
