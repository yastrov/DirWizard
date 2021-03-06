#ifndef EMPTYFOLDERTABLEMODEL_H
#define EMPTYFOLDERTABLEMODEL_H
#include "../hashfileinfostruct.h"
#include "basetablemodel.h"
#include <QFontMetrics>
#include <QStyleOptionComboBox>
#include <QApplication>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

class EmptyFolderTableModel : public BaseTableModel
{
    Q_OBJECT
public:
    explicit EmptyFolderTableModel(QSharedPtrListHFIS content, QObject *parent=nullptr)
        : BaseTableModel(content, parent) {}
    explicit EmptyFolderTableModel(QObject *parent=nullptr)
        : BaseTableModel(parent) {}
    virtual ~EmptyFolderTableModel() {}

    QVariant data(const QModelIndex &index,
                  int role=Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const  Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent=QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent=QModelIndex()) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole);
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) { return false; }
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
    enum Column {checked, fileName};

    QString getFileName(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // EMPTYFOLDERTABLEMODEL_H
