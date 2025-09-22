#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QColor>

#include <memory>

#include "beautypushbutton.h"
#include "beautylineedit.h"
#include "ui_beautywidgetsdemo.h"

class BeautyWidgetsDemo final : public QMainWindow {
    Q_OBJECT

public:
    explicit BeautyWidgetsDemo(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , ui(std::make_unique<Ui::BeautyWidgetsDemo>()) {
        ui->setupUi(this);
        setWindowTitle(QStringLiteral("BeautyWidgets Demo"));
    }

private:
    std::unique_ptr<Ui::BeautyWidgetsDemo> ui;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    BeautyWidgetsDemo window;
    window.show();
    return app.exec();
}

#include "main.moc"
