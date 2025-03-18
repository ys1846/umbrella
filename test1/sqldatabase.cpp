#include "sqldatabase.h"
#include<QDebug>

Sqldatabase::Sqldatabase(QObject *parent) : QObject(parent)
{
    databaseInit();
}

void Sqldatabase::databaseInit()
{
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE");
    sql.setDatabaseName("mydatabase.db");

    if(sql.isOpen()){
        qDebug() << "数据库已经打开" ;
    }
    else {
        if(sql.open()){
            qDebug() << "数据库打开成功";
            databaseCreateTable();

        }
        else{
            qDebug() << "数据库打开失败";
        }
    }
}

void Sqldatabase::databaseCreateTable()
{
    QSqlQuery query;
    QString sql =
        QString("create table if not exists EngineTable(areaNum integer primary key, en0 integer, en1 integer, en2 integer, en3 integer, en4 integer, en5 integer)");
    if(query.exec(sql)){
        qDebug() << "create table success";
    }
    else{
        qDebug() << "create table failure:" << query.lastError().text();
    }

}

bool Sqldatabase::databaseInsert(Data data)
{
    QSqlQuery query;
    QString sql =
        QString("insert into EngineTable values(%1, %2, %3, %4, %5, %6, %7)")
            .arg(data.areaNum)
            .arg(data.engine[0]).arg(data.engine[1])
            .arg(data.engine[2]).arg(data.engine[3])
            .arg(data.engine[4]).arg(data.engine[5]);
    if(query.exec(sql)){
        qDebug() << "insert data success";
        return true;
    }
    else{
        //qDebug() << "insert data failure:" << query.lastError().text();
        if(query.lastError().nativeErrorCode() == "19"){
            qDebug() << "修改数据";
           return databaseUpdate(data);
        }

    }

}

bool Sqldatabase::databaseUpdate(Data data)
{
    QSqlQuery query;
    QString sql =
        QString("update EngineTable set en0 = %1, en1 = %2, en2 = %3, en3 = %4, en4 = %5, en5 = %6 where areaNum = %7")
            .arg(data.engine[0]).arg(data.engine[1])
            .arg(data.engine[2]).arg(data.engine[3])
            .arg(data.engine[4]).arg(data.engine[5])
            .arg(data.areaNum);
    if(query.exec(sql)){
        qDebug() << "updata data success";
        return true;

    }
    else{
        qDebug() << "updata data failure:" << query.lastError().text();
        return false;

    }

}

Data Sqldatabase::databaseGetdata(int areaNum)
{
    Data data;
    memset(&data, 0, sizeof(data));
    data.areaNum = -1;
    QSqlQuery query;
    QString sql =
        QString("select * from EngineTable where areaNum = %1").arg(areaNum);
    if(query.exec(sql)){
        while(query.next()){
            data.areaNum = query.value("areaNum").toInt();
            data.engine[0] = query.value("en0").toInt();
            data.engine[1] = query.value("en1").toInt();
            data.engine[2] = query.value("en2").toInt();
            data.engine[3] = query.value("en3").toInt();
            data.engine[4] = query.value("en4").toInt();
            data.engine[5] = query.value("en5").toInt();
        }

        qDebug() << "get data success";

    }
    else{
        qDebug() << "get data failure:" << query.lastError().text();

    }
    return data;
}
