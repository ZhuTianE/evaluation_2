#ifndef NODE_H
#define NODE_H
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QSpinBox>
#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QWidgetAction>
#include <QPushButton>
inline int node_x = 100;
inline int node_y = 30;
inline int x_gap = 75;
inline int y_gap = 40;

class Node : public QGraphicsItem {
public:
    int x;
    int y;
    double score;
    QString name;

    QList<Node*> children;
    Node* parent = nullptr;
    QGraphicsScene* scene;
    bool isRootNode;
    QGraphicsLineItem* verticalLine = nullptr;   // 用于存储垂直段的线
    QGraphicsLineItem* horizontalLine = nullptr; // 用于存储水平段的线
    // 构造函数，接受 QString, QGraphicsScene* 和可选的 Node* 父节点
    Node(QGraphicsScene* scene, Node* parent = nullptr)
        : scene(scene), parent(parent) {
        // 自动生成名称
        name = generateDefaultName();
        initializeNode();
    }

    Node(const QString& name, QGraphicsScene* scene, Node* parent = nullptr)
        : name(name), scene(scene), parent(parent) {
        // 使用传入的名称
        initializeNode();
    }

    int calculateTotalDescendants() const {
        int count = children.size(); // 直接子节点数量

        // 递归累加所有子节点的子节点数量
        for (const Node* child : children) {
            count += child->calculateTotalDescendants();
        }

        return count;
    }


    void initializeNode() {
        if (parent) {
            parent->addChild(this);
            x = parent->x + x_gap;
            int totalDescendants = parent->calculateTotalDescendants();
            y = parent->y + y_gap * totalDescendants;
            isRootNode = false;
            setFlag(QGraphicsItem::ItemIsSelectable, true);
            setFlag(QGraphicsItem::ItemIsFocusable, true);
            setFlag(QGraphicsItem::ItemIsMovable, false);

            Node* root = findRootNode();  // 找到根节点
            root->shiftNodesDown(y, this);  // 从根节点开始调整位置
        } else {
            x = 0;  // 设置根节点的初始位置
            y = 0;
            isRootNode = true;
            setFlag(QGraphicsItem::ItemIsSelectable, true);
            setFlag(QGraphicsItem::ItemIsFocusable, true);
            setFlag(QGraphicsItem::ItemIsMovable, false);
        }

        setPos(x, y);  // 设置 QGraphicsItem 的位置
        addConnectionLine();
        // 将节点添加到场景中
        if (scene) {
            scene->addItem(this);
        }
    }

    QString generateDefaultName() {
        if (parent == nullptr) {
            // 根节点名称
            return "****任务效能";
        }

        // 获取父节点当前的子节点数，以确定当前子节点的编号
        int childIndex = parent->children.size() + 1;

        if (parent->isRootNode) {
            // 如果父节点是根节点，生成格式为 "指标N"
            return QString("指标%1").arg(childIndex);
        } else {
            // 如果父节点不是根节点，生成格式为 "父节点名字-子节点编号"
            return QString("%1-%2").arg(parent->name).arg(childIndex);
        }
    }

    void addConnectionLine() {
        if (parent && scene) {
            // 使用虚线画笔
            QPen dashedPen(Qt::DashLine);
            dashedPen.setColor(Qt::black);
            dashedPen.setWidth(1);

            // 获取父节点的下半边中间位置
            QPointF parentPos = parent->scenePos() + QPointF(parent->boundingRect().width() / 2, parent->boundingRect().height());

            // 获取子节点的左边中间位置
            QPointF childPos = this->scenePos() + QPointF(0, this->boundingRect().height() / 2);

            // 计算折线的拐点
            QPointF intermediatePoint(parentPos.x(), childPos.y());

            // 创建并存储垂直段的线
            verticalLine = new QGraphicsLineItem(QLineF(parentPos, intermediatePoint));
            verticalLine->setPen(dashedPen);
            scene->addItem(verticalLine);

            // 创建并存储水平段的线
            horizontalLine = new QGraphicsLineItem(QLineF(intermediatePoint, childPos));
            horizontalLine->setPen(dashedPen);
            scene->addItem(horizontalLine);
        }
    }

    // 更新连接线位置的方法
    void updateConnectionLine() {
        if (verticalLine && horizontalLine && parent) {
            QPointF parentPos = parent->scenePos() + QPointF(parent->boundingRect().width() / 2, parent->boundingRect().height());
            QPointF childPos = this->scenePos() + QPointF(0, this->boundingRect().height() / 2);
            QPointF intermediatePoint(parentPos.x(), childPos.y());

            // 更新垂直段的线
            verticalLine->setLine(QLineF(parentPos, intermediatePoint));

            // 更新水平段的线
            horizontalLine->setLine(QLineF(intermediatePoint, childPos));
        }
    }

    void addChild(Node* child) {
        children.append(child);
    }

    QRectF boundingRect() const override {
        // 定义节点的范围
        return QRectF(0, 0, 100, 30);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        painter->drawRect(0, 0, 100, 30);  // 仅绘制矩形边框
        painter->drawText(boundingRect(), Qt::AlignCenter, name);  // 在矩形中居中显示节点名称
    }

    int returnChildnum(){
        return children.size();
    }
    Node* findRootNode() {
        Node* current = this;
        while (current->parent != nullptr) {
            current = current->parent;
        }
        return current;  // 返回根节点
    }
    void shiftNodesDown(int y_threshold, Node* newNode) {
        // 如果当前节点是新创建的节点，则跳过
        if (this == newNode) {
            return;
        }

        // 如果当前节点的 y 值大于阈值，则将它的 y 值往下移动 y_gap
        if (this->y >= y_threshold) {
            this->y += y_gap;
            setPos(this->x, this->y);  // 更新 QGraphicsItem 的位置
            updateConnectionLine();
        }

        // 递归遍历所有子节点
        for (auto& child : this->children) {
            child->shiftNodesDown(y_threshold, newNode);  // 对子节点递归调用
        }
    }





protected:
    // 重写 contextMenuEvent 以在右键单击时显示上下文菜单
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override {
        QMenu menu;

        QWidget* widgetContainer = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widgetContainer);
        layout->setContentsMargins(28, 0, 0, 0);  // 设置边距

        // 创建 QLabel、QSpinBox 和 QPushButton
        QLabel* label = new QLabel("新建子节点", widgetContainer);
        QSpinBox* spinBox = new QSpinBox(widgetContainer);
        spinBox->setRange(1, 10);  // 设置数量范围
        spinBox->setValue(1);       // 默认值为 1
        QPushButton* applyButton = new QPushButton("应用", widgetContainer);

        // 将 QLabel、QSpinBox 和 QPushButton 添加到水平布局中
        layout->addWidget(label);
        layout->addWidget(spinBox);
        layout->addWidget(applyButton);

        // 创建 QWidgetAction 并设置其默认小部件
        QWidgetAction* widgetAction = new QWidgetAction(&menu);
        widgetAction->setDefaultWidget(widgetContainer);
        menu.addAction(widgetAction);  // 将 QWidgetAction 添加到菜单中

        QObject::connect(applyButton, &QPushButton::clicked, [&]() {
            int count = spinBox->value();  // 获取 QSpinBox 的值
            for (int i = 0; i < count; ++i) {
                Node* childNode = new Node(scene, this); // 创建子节点

            }
            menu.close();  // 关闭菜单
        });



        QAction* deleteAction = menu.addAction("删除(D)");
        QAction* renameAction = menu.addAction("重命名(R)");
        QAction* selectAction = menu.addAction("选中节点(S)");
        QAction* copyAction = menu.addAction("复制(C)");
        QAction* cutAction = menu.addAction("剪切(T)");
        QAction* pasteAction = menu.addAction("粘贴(P)");

        // 执行菜单，并获取被点击的动作
        QAction* selectedAction = menu.exec(event->screenPos());

        // 判断用户选择了哪个操作，并执行相应操作
        if (selectedAction == deleteAction) {
            // 执行删除操作
            deleteNode();
        } else if (selectedAction == renameAction) {
            // 执行重命名操作
            renameNode();
        } else if (selectedAction == selectAction) {
            // 执行选中节点操作
            selectNode();
        } else if (selectedAction == copyAction) {
            // 执行复制操作
            copyNode();
        } else if (selectedAction == cutAction) {
            // 执行剪切操作
            cutNode();
        } else if (selectedAction == pasteAction) {
            // 执行粘贴操作
            pasteNode();
        }
    }

    // 定义各项菜单操作的函数
    void createChildNode() {
        // 添加子节点的实现
    }

    void deleteNode() {
        // 删除节点的实现
    }

    void renameNode() {
        // 重命名节点的实现
    }

    void selectNode() {
        // 选中节点的实现
    }

    void copyNode() {
        // 复制节点的实现
    }

    void cutNode() {
        // 剪切节点的实现
    }

    void pasteNode() {
        // 粘贴节点的实现
    }

};


#endif // NODE_H
