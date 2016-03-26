#ifndef BASETABLEMODEL_H
#define BASETABLEMODEL_H
#include <QObject>
#include <QAbstractTableModel>
#include "hashfileinfostruct.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

class BaseTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BaseTableModel(QSharedPtrListHFIS content, QObject *parent=nullptr)
        : QAbstractTableModel(parent), items(content) {}
    virtual ~BaseTableModel() {}
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool removeRow(int row, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);
    virtual void saveToFileFunc(const QString &fileName) const;
    virtual void removeCheckedFunc();
public slots:
    void saveToFile(QString fileName);
    void removeChecked(bool checked=false);
protected:
    QSharedPtrListHFIS items;
};

#endif // BASETABLEMODEL_H
