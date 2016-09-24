#ifndef FILELISTTABLEMODEL_H
#define FILELISTTABLEMODEL_H
#include "../hashfileinfostruct.h"
#include "basetablemodel.h"
#include <QFontMetrics>
#include <QStyleOptionComboBox>

class FileListTableModel : public BaseTableModel
{
    Q_OBJECT
public:
    explicit FileListTableModel(QSharedPtrListHFIS content, QObject *parent=nullptr)
        : BaseTableModel(content, parent) {}
    virtual ~FileListTableModel() {}
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
    enum Column {fileName, hash, size};


    QString getFileName(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // FILELISTTABLEMODEL_H
