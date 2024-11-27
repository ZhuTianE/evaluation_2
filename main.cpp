#include "mainwindow.h"

#include <QApplication>
#include <QTreeWidget>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>



class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CustomTreeWidget(QWidget *parent = nullptr) : QTreeWidget(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        QTreeWidgetItem *selectedItem = itemAt(event->pos());
        if (selectedItem)
        {
            QMenu contextMenu(this);

            QAction *addAction = new QAction("新增子节点", this);
            QAction *deleteAction = new QAction("删除", this);
            QAction *renameAction = new QAction("重命名", this);

            // 添加动作到右键菜单
            contextMenu.addAction(addAction);
            contextMenu.addAction(deleteAction);
            contextMenu.addAction(renameAction);

            // 显示右键菜单
            contextMenu.exec(event->globalPos());

            connect(addAction, &QAction::triggered, this, [this, selectedItem]() {
                QTreeWidgetItem *newItem = new QTreeWidgetItem(selectedItem);
                newItem->setText(0, "新节点");
                selectedItem->addChild(newItem);
            });
            connect(deleteAction, &QAction::triggered, this, [this, selectedItem]() {
                delete selectedItem;
            });
            connect(renameAction, &QAction::triggered, this, [this, selectedItem]() {
                selectedItem->setFlags(selectedItem->flags() | Qt::ItemIsEditable);
                editItem(selectedItem);
            });
        }
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
