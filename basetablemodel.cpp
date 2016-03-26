#include "basetablemodel.h"

Qt::ItemFlags BaseTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    // |Qt::ItemIsEditable
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|
                    Qt::ItemIsEnabled|Qt::ItemIsUserCheckable;
    return theFlags;
}

bool BaseTableModel::removeRow(int row, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row);
    items->removeAt(row);
    endRemoveRows();
    return true;
}

bool BaseTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        items->removeAt(row);
    endRemoveRows();
    return true;
}


void BaseTableModel::saveToFileFunc(const QString &fileName) const
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        QString delimeter = "\t";
        QString endOfLine = "\n";

        out << tr("Group ID") << delimeter;
        out << tr("Size") << delimeter;
        out << tr("Hash") << delimeter << tr("FileName");
        out << endOfLine;

        QListIterator<HashFileInfoStruct> it(*items);
        while(it.hasNext())
        {
            const HashFileInfoStruct &s = it.next();
            out << QString("%1").arg(s.groupID) << delimeter;
            out << s.size << delimeter << s.hash << delimeter << s.fileName << endOfLine;
        }
        file.close();
    }
}
void BaseTableModel::removeCheckedFunc()
{
    QListIterator<HashFileInfoStruct> it(*items);
    int removed = 0;
    int row = 0;
    QModelIndex index;
    while(it.hasNext())
    {
        const HashFileInfoStruct &strct = it.next();
        if(strct.checked)
        {
            if(QDir(strct.fileName).remove(strct.fileName)) {
                ++removed;
                removeRow(row, index);
            } else {
                QMessageBox msgBox;
                msgBox.setText(QObject::tr("Can't delete file: %1").arg(strct.fileName));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
            }
        }
        ++row;
    }
}

//Slots
void BaseTableModel::saveToFile(QString fileName)
{
#ifdef MYPREFIX_DEBUG
qDebug() << "BaseTableModel::saveToFile";
#endif
    saveToFileFunc(fileName);
}
void BaseTableModel::removeChecked(bool checked)
{
    Q_UNUSED(checked)
    removeChecked();
}
