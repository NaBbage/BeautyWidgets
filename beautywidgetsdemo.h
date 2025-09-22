#include <QMainWindow>
#include "./ui_beautywidgetsdemo.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class BeautyWidgetsDemo;
}
QT_END_NAMESPACE

class BeautyWidgetsDemo : public QMainWindow
{
    Q_OBJECT

public:
    BeautyWidgetsDemo(QWidget *parent = nullptr);
    ~BeautyWidgetsDemo();

private:
    Ui::BeautyWidgetsDemo *ui;
};

BeautyWidgetsDemo::BeautyWidgetsDemo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BeautyWidgetsDemo)
{
    ui->setupUi(this);
}

BeautyWidgetsDemo::~BeautyWidgetsDemo()
{
    delete ui;
}
