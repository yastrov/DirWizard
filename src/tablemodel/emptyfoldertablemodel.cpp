#include "emptyfoldertablemodel.h"

namespace {
const int MaxColumns = 2;
QColor firstColor = QColor(Qt::white);
QColor secondColor = QColor(Qt::green);

QString getFileNameShort(const QString &path)
{
#ifdef NATIVE_PATH_SEP
    QString fname = QDir::toNativeSeparators(path);
    return  fname.section(QDir::separator(), -1);
#else
    return  path.section('/', -1);
#endif
}
}
/*
typedef struct {
    //QFileInfo i;
    QString fileName;
    QString hash;
    bool checked;
    uint groupID;
    qint64 size;
} HashFileInfoStruct;
*/

int EmptyFolderTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : items->count();
}


int EmptyFolderTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MaxColumns;
}

QVariant EmptyFolderTableModel::headerData(int section,
                                          Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case Column::checked: return tr("Remove?"); break;
        case Column::fileName: return tr("Folder"); break;
        default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

QVariant EmptyFolderTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() >= MaxColumns)
        return QVariant();
    const HashFileInfoStruct &item = items->at(index.row());
    switch(role) {
    case Qt::SizeHintRole: {
        QStyleOptionComboBox option;
        switch (index.column()) {
        case Column::checked: option.currentText = QString(""); break;
        case Column::fileName: option.currentText = getFileNameShort(item.fileName); break;
        default: Q_ASSERT(false);
        }
        QFontMetrics fontMetrics(data(index, Qt::FontRole)
                                 .value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText),
                   fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox,
                                               &option, size);
    }
    case Qt::DisplayRole: {
        switch (index.column()) {
        case Column::checked: return QVariant();
        case Column::fileName: return getFileNameShort(item.fileName);
        default: Q_ASSERT(false);
        }
    }
    case Qt::EditRole: {
        return QVariant();
    }
    case Qt::CheckStateRole: {
        if(index.column() == Column::checked) {
            return item.checked==true ? Qt::Checked : Qt::Unchecked;
        }
        return QVariant();
    }
    case Qt::ToolTipRole: {
        switch (index.column()) {
        case Column::checked: return tr("Remove?");
        case Column::fileName: return item.fileName;
        default: Q_ASSERT(false);
        }
    }
    case Qt::BackgroundColorRole: {
        QColor color = item.groupID % 2 == 0? firstColor : secondColor;
        return QVariant(color);
    }
    default: return QVariant();
    }
    return QVariant();
}

bool EmptyFolderTableModel::setData(const QModelIndex &index,
                                   const QVariant &value, int role)
{
    const int column = index.column();
    const int row = index.row();
    if (!index.isValid() ||
            row < 0 || row >= items->count() ||
            column < 0 || column > MaxColumns)
        return false;
    HashFileInfoStruct &item = (*items.data())[row];
    if (role == Qt::CheckStateRole)
    {
        if (value.toInt() == Qt::Checked)
        {
            item.checked = true;
        }
        else {
            item.checked = false;
        }
        emit dataChanged(index, index);
    }
    return false;
}

void EmptyFolderTableModel::sort(int column, Qt::SortOrder order)
{
#ifdef MYPREFIX_DEBUG
    if(order == Qt::AscendingOrder) {
        qDebug()<< "DuplicatesTableModel::sort Qt::AscendingOrder";
    } else {
        qDebug()<< "DuplicatesTableModel::sort Qt::DescendingOrder";
    }
#endif
    switch(column) {
    case Column::checked:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            if(v1.checked && !v2.checked)
                return true;
            return false;
        }); break;
    case Column::fileName:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.fileName > v2.fileName;
        });
        break;
    default: return;
    }
    if(order != Qt::AscendingOrder)
        std::reverse(items->begin(), items->end());
    emit dataChanged(QModelIndex(), QModelIndex());
}

QString EmptyFolderTableModel::getFileName(const QModelIndex &index) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() >= MaxColumns)
        return "";
    const HashFileInfoStruct &item = items->at(index.row());
    return item.fileName;
}
