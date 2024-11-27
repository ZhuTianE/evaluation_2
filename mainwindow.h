#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QTreeWidgetItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "node.h"
#include <QLineEdit>
#include <QSystemTrayIcon>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;

}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void onTreeWidgetItemClicked(QTreeWidgetItem *item, int column);

    void setupNodeView();



    void on_zhuan_pushButton_clicked();

    void on_cen_pushButton_clicked();

    void on_pushButton_clicked();

    void on_z_load_pushButton_clicked();

    void on_generate_weight_pushButton_clicked();

    void buttonDigitalClicked();                        //数字按键
    void buttonSinClicked();                            //三角函数sin
    void buttonCosClicked();                            //三角函数cos
    void buttonTanClicked();                            //三角函数tan
    void buttonSqrtClicked();                           //开方
    void buttonPowClicked();                            //平方
    void buttonXClicked();                              //倒数
    void buttonXYClicked();                             //X的Y次方
    void buttonModClicked();                            //取余
    void buttonEqualClicked();                          //等
    void buttonAddClicked();                            //加法
    void buttonMinClicked();                            //减法
    void buttonMulClicked();                            //乘法
    void buttonDivClicked();                            //除法
    void buttonPointClicked();                          //小数点
    void buttonPIClicked();                             //PI
    void buttonDelClicked();                            //回退
    void buttonClearClicked();                          //清空

private:
    QGraphicsScene *scene;
    Node *root;
    QGraphicsView *view;
    QGraphicsView *view_1;
    Ui::MainWindow *ui;
    QLabel *fLabCurFile;
    QProgressBar *progressBar1;
    QFontComboBox *comboFont;
    QSpinBox   *spinFontSize;
    QTreeWidget *treeWidget;
    QStackedWidget *stackedWidget;
    void iniUI();
    void clearSceneExceptRoot(Node* root);
    void createLayeredStructure(int layers, int nodesPerLayer, Node* parent = nullptr, int currentLayer = 1);
    void clearAllLines();
    void connectLinesRecursively(Node* parentNode);
    void connectAllLines();

    bool waittingForOperand;
    bool addOperator;                                //加法操作符
    bool minOperator;                                //减法操作符
    bool mulOperator;                                //乘法操作符
    bool divOperator;                                //除法操作符
    bool XYOperator;                                 //X的Y次方操作符
    bool modOperator;                                //余数操作符
    bool operatorFlag;                               //操作符标志
    bool calculate(double operand, const QString &operatorAct);
    bool calculate(double operand);
    void abortOperation();
    double value;
    double result;

};
#endif // MAINWINDOW_H

