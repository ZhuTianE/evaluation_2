#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QContextMenuEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include<QButtonGroup>
#include<cmath>


class VerticalButton : public QPushButton {
public:
    VerticalButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent) {}

protected:
    void paintEvent(QPaintEvent *event) override {
        QStylePainter painter(this);
        QStyleOptionButton option;
        initStyleOption(&option);

        painter.rotate(90);

        QRect rect = option.rect;
        rect.setRect(-rect.height(), rect.x(), rect.width(), rect.height());

        option.rect = rect;
        painter.drawControl(QStyle::CE_PushButton, option);
    }

    QSize sizeHint() const override {
        QSize size = QPushButton::sizeHint();
        return QSize(size.height(), size.width());
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    iniUI();
    setupNodeView();
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::onTreeWidgetItemClicked);


    //science_calculator
    //参数的初始化
    waittingForOperand = true;                      //等待操作数
    result = 0.0;
    value = 0.0;
    operatorFlag = false;
    modOperator = false;
    XYOperator = false;
    addOperator = false;
    minOperator = false;
    mulOperator = false;
    divOperator = false;

    //数字按键绑定0-9
    connect(ui->num0, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num1, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num2, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num3, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num4, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num5, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num6, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num7, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num8, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));
    connect(ui->num9, SIGNAL(clicked()), this, SLOT(buttonDigitalClicked()));

    //加减乘除
    connect(ui->add, SIGNAL(clicked()), this, SLOT(buttonAddClicked()));
    connect(ui->sub, SIGNAL(clicked()), this, SLOT(buttonMinClicked()));
    connect(ui->mul, SIGNAL(clicked()), this, SLOT(buttonMulClicked()));
    connect(ui->div, SIGNAL(clicked()), this, SLOT(buttonDivClicked()));

    connect(ui->point, SIGNAL(clicked()), this, SLOT(buttonPointClicked()));//小数点
    connect(ui->pi, SIGNAL(clicked()), this, SLOT(buttonPIClicked()));//pi
    connect(ui->del, SIGNAL(clicked()), this, SLOT(buttonDelClicked()));//回退

    connect(ui->C, SIGNAL(clicked()), this, SLOT(buttonClearClicked()));//清空
    ui->calculator_outputer->setAlignment(Qt::AlignRight);              //靠右显示
    ui->calculator_outputer->setReadOnly(true);                         //只读模式
    ui->calculator_outputer->setMinimumHeight(50);
    ui->calculator_outputer->setText("0");//初始化为0

    connect(ui->sin,SIGNAL(clicked()), this, SLOT(buttonSinClicked())); //三角函数sin
    connect(ui->cos,SIGNAL(clicked()), this, SLOT(buttonCosClicked())); //三角函数cos
    connect(ui->tan,SIGNAL(clicked()), this, SLOT(buttonTanClicked())); //三角函数tan

    connect(ui->x12, SIGNAL(clicked()), this, SLOT(buttonSqrtClicked()));//开方
    connect(ui->x2, SIGNAL(clicked()), this, SLOT(buttonPowClicked()));//平方
    connect(ui->x01, SIGNAL(clicked()), this, SLOT(buttonXClicked()));//倒数
    connect(ui->xy, SIGNAL(clicked()), this, SLOT(buttonXYClicked()));//x的y次方
    connect(ui->mod, SIGNAL(clicked()), this, SLOT(buttonModClicked()));//取余
    connect(ui->equal, SIGNAL(clicked()), this, SLOT(buttonEqualClicked()));//等于

}


void MainWindow::iniUI(){
    fLabCurFile = new QLabel;
    fLabCurFile->setMinimumWidth(150);
    fLabCurFile->setText("当前文件：");
    ui->statusbar->addWidget(fLabCurFile);

    progressBar1=new QProgressBar;
    progressBar1->setMaximumWidth(200);
    progressBar1->setMaximum(50);
    progressBar1->setMinimum(5);
    //progressBar1->setValue(ui->txtEdit->font().pointSize());
    ui->statusbar->addWidget(progressBar1);

    spinFontSize=new QSpinBox;
    spinFontSize->setMaximum(50);
    spinFontSize->setMinimum(5);
    //spinFontSize->setValue(ui->txtEdit->font().pointSize());
    spinFontSize->setMinimumWidth(50);

    ui->toolBar->addWidget(new QLabel("字体大小"));
    ui->toolBar->addWidget(spinFontSize);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(new QLabel("字体"));
    comboFont= new QFontComboBox;
    comboFont->setMinimumWidth(150);
    ui->toolBar->addWidget(comboFont);
    QVBoxLayout *layout = new QVBoxLayout(ui->side_z_c);

    for (int i = 0; i < 5; ++i) {
        VerticalButton *button = new VerticalButton(QString("Button %1").arg(i + 1), ui->side_z_c);
        layout->addWidget(button);
    }

    ui->side_z_c->setLayout(layout);
}

void MainWindow::onTreeWidgetItemClicked(QTreeWidgetItem *item, int column)
{
    QString itemText = item->text(column);

    // 根据点击的 treeWidget 项切换 stackedWidget 页面
    if (itemText == "指标体系设计") {
        ui->stackedWidget->setCurrentWidget(ui->index_page);
    } else if (itemText == "权重体系设计") {
        ui->stackedWidget->setCurrentWidget(ui->weight_page);
    } else if (itemText == "算法体系设计") {
        ui->stackedWidget->setCurrentWidget(ui->algorithm_page);//转到算法页
    }
}

MainWindow::~MainWindow()
{
    delete ui;

}

//生成对应界面的树（算法界面左下角）
void MainWindow::setupNodeView() {
    // 创建场景
    scene = new QGraphicsScene(this);

    // 初始化 view 并将其设置为 widget_2 的子控件
    view = new QGraphicsView(scene, ui->widget_2);
    view->setRenderHint(QPainter::Antialiasing);
    view->setContextMenuPolicy(Qt::DefaultContextMenu);

    view->setDragMode(QGraphicsView::NoDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 设置 view 的初始几何位置
    view->setGeometry(3, 3, ui->widget_2->width()-10, ui->widget_2->height()-10);
    root = new Node("****任务效能", scene);
    Node* child1= new Node(scene,root);
    Node* child2= new Node(scene,root);
    Node* child3= new Node(scene,child1);
    Node* child4= new Node(scene,child2);
    Node* child5= new Node(scene,root);


    view_1 = new QGraphicsView(scene, ui->left_widget_0);
    view_1->setRenderHint(QPainter::Antialiasing);
    view_1->setContextMenuPolicy(Qt::DefaultContextMenu);

    view_1->setDragMode(QGraphicsView::NoDrag);
    view_1->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_1->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 设置 view 的初始几何位置
    view_1->setGeometry(3, 3, ui->left_widget_0->width()-10, ui->left_widget_0->height()-10);


}




//********

void MainWindow::on_zhuan_pushButton_clicked()
{
    ui->weight_stack->setCurrentIndex(0);
}


void MainWindow::on_cen_pushButton_clicked()
{
    ui->weight_stack->setCurrentIndex(1);
}

void MainWindow::clearSceneExceptRoot(Node* root) {
    if (!scene) return;  // 检查 scene 是否为空

    // 遍历场景中的所有项
    for (QGraphicsItem* item : scene->items()) {
        // 如果 item 不是 root，就将其从场景中移除
        if (item != root) {
            scene->removeItem(item);
            delete item;  // 删除 item 以释放内存
        }
    }
    root->children.clear();
}

void MainWindow::clearAllLines() {
    // 遍历场景中的所有项
    for (auto item : scene->items()) {
        // 如果项是 QGraphicsLineItem，则将其删除
        if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
            scene->removeItem(lineItem);
            delete lineItem;
        }
    }
}

void MainWindow::connectAllLines() {
    // 找到根节点（假设场景中只有一个根节点）

    if (root) {
        // 从根节点开始递归连接
        connectLinesRecursively(root);
    }
}

void MainWindow::on_pushButton_clicked()
{
    int layers = ui->spinBox->value();
    int nodesPerLayer = ui->spinBox_2->value();

    // 清空场景中除 root 以外的所有节点
    clearSceneExceptRoot(root);
    // 调用函数在现有场景中添加新的节点层级结构
    createLayeredStructure( layers, nodesPerLayer);
}

void MainWindow::connectLinesRecursively(Node* parentNode) {
    QPen dashedPen(Qt::black);
    dashedPen.setStyle(Qt::DashLine);
    dashedPen.setWidth(1);

    // 遍历父节点的所有子节点
    for (auto childNode : parentNode->children) {
        // 获取父节点的下半边中间位置
        QPointF parentPos = parentNode->scenePos() + QPointF(parentNode->boundingRect().width() / 2, parentNode->boundingRect().height());

        // 获取子节点的左边中间位置
        QPointF childPos = childNode->scenePos() + QPointF(0, childNode->boundingRect().height() / 2);

        // 计算折线的拐点
        QPointF intermediatePoint(parentPos.x(), childPos.y());

        // 绘制从父节点下边中间到拐点的垂直线
        scene->addLine(QLineF(parentPos, intermediatePoint), dashedPen);

        // 绘制从拐点到子节点左边中间的水平线
        scene->addLine(QLineF(intermediatePoint, childPos), dashedPen);

        // 递归连接子节点
        connectLinesRecursively(childNode);
    }
}

void MainWindow::createLayeredStructure(int layers, int nodesPerLayer, Node* parent , int currentLayer ) {
    // 如果到达了最大层数，则停止
    if (currentLayer > layers) return;

    // 如果是初始调用，没有传入父节点，则使用根节点
    if (parent == nullptr) {
        parent = root;
    }

    // 为当前层的每个节点生成 nodesPerLayer 个子节点
    for (int i = 0; i < nodesPerLayer; ++i) {
        // 创建新的节点，父节点为传入的 parent
        Node* childNode = new Node(scene, parent);


        // 递归调用，为每个子节点生成下一层的子节点
        createLayeredStructure(layers, nodesPerLayer, childNode, currentLayer + 1);
    }
}

void MainWindow::on_z_load_pushButton_clicked()
{
    bool ok;
    int expertCount = QInputDialog::getInt(this, "输入专家数量", "请输入需要几位专家：", 1, 1, 10, 1, &ok);
    if (!ok) return;

    QStringList expertNames;
    QStringList filePaths;
    for (int i = 0; i < expertCount; ++i) {
        QString expertName = QInputDialog::getText(this, tr("输入专家姓名"), tr("专家 %1 姓名：").arg(i + 1), QLineEdit::Normal, "", &ok);
        if (!ok || expertName.isEmpty()) {
            QMessageBox::warning(this, "警告", "专家姓名不能为空。");
            return;
        }
        expertNames << expertName;

        QString defaultPath = "C:/默认路径/";
        QString filePath = QFileDialog::getOpenFileName(this, tr("导入专家评分文件"), defaultPath, tr("所有文件 (*.*);;文本文件 (*.txt);;Excel 文件 (*.xlsx)"));
        if (filePath.isEmpty()) {
            QMessageBox::warning(this, "警告", "未选择文件。");
            return;
        }
        filePaths << filePath;
    }

    QMessageBox::information(this, "导入成功", tr("已成功导入 %1 位专家的评分文件。").arg(expertCount));

}


void MainWindow::on_generate_weight_pushButton_clicked()
{
    QXlsx::Document xlsx;

    // 设置标题格式
    QXlsx::Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    titleFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    titleFormat.setBorderStyle(QXlsx::Format::BorderThin);

    // 设置普通单元格格式
    QXlsx::Format cellFormat;
    cellFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    cellFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    cellFormat.setBorderStyle(QXlsx::Format::BorderThin);

    // 设置列标题
    xlsx.write("A1", "一级能力", titleFormat);
    xlsx.write("B1", "二级能力", titleFormat);
    xlsx.write("C1", "三级能力", titleFormat);
    xlsx.write("D1", "指标项", titleFormat);
    xlsx.write("E1", "权重", titleFormat);

    // 示例数据填充
    xlsx.write("A2", "任务效率", cellFormat);
    xlsx.write("B2", "一级能力1", cellFormat);
    xlsx.write("C2", "三级能力1-1", cellFormat);
    xlsx.write("D2", "指标项1-1-a", cellFormat);
    xlsx.write("E2", "25%", cellFormat);

    xlsx.write("D3", "指标项1-1-b", cellFormat);
    xlsx.write("E3", "33%", cellFormat);

    xlsx.write("C4", "三级能力1-2", cellFormat);
    xlsx.write("D4", "指标项1-2-a", cellFormat);
    xlsx.write("E4", "20%", cellFormat);

    xlsx.write("D5", "指标项1-2-b", cellFormat);
    xlsx.write("E5", "27%", cellFormat);

    // 合并单元格
    xlsx.mergeCells("A2:A5", cellFormat);  // 合并一级能力
    xlsx.mergeCells("B2:B5", cellFormat);  // 合并二级能力1

    // 添加更多数据
    xlsx.write("B6", "一级能力2", cellFormat);
    xlsx.write("C6", "三级能力2-1", cellFormat);
    xlsx.write("D6", "指标项2-1-a", cellFormat);
    xlsx.write("E6", "38%", cellFormat);

    xlsx.write("D7", "指标项2-1-b", cellFormat);
    xlsx.write("E7", "62%", cellFormat);

    xlsx.mergeCells("A6:A7", cellFormat);  // 合并一级能力2
    xlsx.mergeCells("B6:B7", cellFormat);  // 合并二级能力2

    // 保存 Excel 文件
    xlsx.saveAs("D:\\MyCode\\Qt\\evaluation_sys\\evaluation\\weight.xlsx");
    QMessageBox::information(this,tr("提示"),tr("模板生成成功！"));
}

//science_calculator

//数字按键输入数字
void MainWindow::buttonDigitalClicked()
{
    QPushButton *buttonClicked = qobject_cast<QPushButton *>(sender());     //判断是那个键值被摁下
    int buttonValue = buttonClicked->text().toInt();                        //将键值转化为整型赋值给value

    if(waittingForOperand)
    {
        ui->calculator_outputer->clear();
        waittingForOperand = false;
    }
    ui->calculator_outputer->setText(ui->calculator_outputer->text()+QString::number(buttonValue));         //计算器显示之前的值加上现在的值
}

void MainWindow::buttonSinClicked()//三角函数sin
{
    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        text = std::sin(text*3.141592654/180);
        ui->calculator_outputer->setText(QString::number(text));
    }
    waittingForOperand = true;
}

void MainWindow::buttonCosClicked()//三角函数cos
{
    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        text = std::cos(text*3.141592654/180);
        ui->calculator_outputer->setText(QString::number(text));
    }
    waittingForOperand = true;
}

void MainWindow::buttonTanClicked()//三角函数tan
{
    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        text = std::tan(text*3.141592654/180);
        ui->calculator_outputer->setText(QString::number(text));
    }
    waittingForOperand = true;
}

void MainWindow::buttonSqrtClicked()//开方
{
    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        if(text < 0)
            abortOperation();
        else
        {
            text = std::sqrt(text);
            ui->calculator_outputer->setText(QString::number(text));
        }
    }
    waittingForOperand = true;
}

void MainWindow::buttonPowClicked()//平方
{

    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        text = std::pow(text,2);
        ui->calculator_outputer->setText(QString::number(text));
    }
    waittingForOperand = true;
}

void MainWindow::buttonXClicked()//倒数
{
    if(operatorFlag)
        return;
    else
    {
        double text = ui->calculator_outputer->text().toDouble();
        if(text == 0.0)
            abortOperation();
        else
        {
            text = 1/text;
            ui->calculator_outputer->setText(QString::number(text));
            waittingForOperand = true;
        }
    }
}


void MainWindow::buttonXYClicked()//X的Y次方
{
    if(!operatorFlag)
    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }

    XYOperator = true;
}

void MainWindow::buttonModClicked()//取余
{
    if(!operatorFlag)
    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }
    modOperator = true;
}

void MainWindow::buttonEqualClicked()//等于
{

    if(!operatorFlag)
        return;
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
        waittingForOperand = true;
        operatorFlag = false;
    }
}


void MainWindow::buttonAddClicked()//加
{
    if(!operatorFlag)

    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }
    addOperator = true;
}

void MainWindow::buttonMinClicked()//减
{
    if(!operatorFlag)
    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }
    minOperator = true;
}

void MainWindow::buttonMulClicked()//乘
{
    if(!operatorFlag)
    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else

    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }
    mulOperator = true;
}

void MainWindow::buttonDivClicked()//除
{
    if(!operatorFlag)
    {
        result = ui->calculator_outputer->text().toDouble();
        operatorFlag = true;
        waittingForOperand = true;
    }
    else
    {
        value = ui->calculator_outputer->text().toDouble();
        if(calculate(value)==false)
        {
            abortOperation();
        }
    }
    divOperator = true;
}

void MainWindow::buttonPointClicked()//小数点
{
    //首先判断当前显示内容是否已经包含“.”符号
    if(ui->calculator_outputer->text().contains('.'))
        return;
    else
        ui->calculator_outputer->setText(ui->calculator_outputer->text()+tr("."));
}

void MainWindow::buttonPIClicked()//pi值
{
    //判断是否是进行输入状态
    if(waittingForOperand)
        ui->calculator_outputer->setText(QString::number(3.141592654));
    else

    return;
}

void MainWindow::buttonDelClicked()//回退
{
    //判断当前是否属于数值输入模式
    if(waittingForOperand)
        return;
    QString text = ui->calculator_outputer->text();
    text.chop(1);

    //判断删除后数据是否为空
    if(text.isEmpty())
    {
        text = "0";
        waittingForOperand = true;
    }
    ui->calculator_outputer->setText(text);
}

void MainWindow::buttonClearClicked()//归零
{
    waittingForOperand = true;                      //等待操作数
    result = 0.0;
    value = 0.0;
    operatorFlag = false;
    modOperator = false;
    XYOperator = false;
    addOperator = false;
    minOperator = false;
    mulOperator = false;
    divOperator = false;
    ui->calculator_outputer->setText("0");
}


bool MainWindow::calculate(double operand)//运算结果
{
    if(addOperator)
    {
        result += operand;
        addOperator = false;
    }
    else if(minOperator)
    {

        result -= operand;
        minOperator = false;
    }
    else if(mulOperator)
    {
        result *= operand;
        mulOperator = false;
    }
    else if(divOperator)
    {
        if(operand - 0 < 10e-6 )
            return false;
        result /= operand;
        divOperator = false;
    }
    else if(modOperator)
    {
        if(operand - 0 < 10e-6 )
            return false;
        result = (int)result%(int)operand;
        modOperator = false;
    }
    else if(XYOperator)
    {
        result = pow(result,operand);
        XYOperator = false;
    }
    waittingForOperand = true;
    ui->calculator_outputer->setText(QString::number(result));
    return true;
}


void MainWindow::abortOperation()//出现错误
{
    buttonClearClicked();
    ui->calculator_outputer->setText(tr("ERROR!!!"));
}
















