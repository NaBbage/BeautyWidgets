#pragma once
#include <QPushButton>
#include <QColor>
#include <QPointF>

class BeautyPushButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor  bgColor READ bgColor  WRITE setBgColor)
    Q_PROPERTY(qreal   scale  READ scale    WRITE setScale)
    Q_PROPERTY(QPointF offset READ offset   WRITE setOffset)

public:
    explicit BeautyPushButton(QWidget *parent = nullptr);

    void setThemeColor(const QColor &base);
    void setCheckedColor(const QColor &c);
    void setDisabledColor(const QColor &c);

private:
    QColor  bgColor() const { return m_bgColor; }
    void    setBgColor(const QColor &c);
    qreal   scale()  const { return m_scale; }
    void    setScale(qreal s);
    QPointF offset() const { return m_offset; }
    void    setOffset(const QPointF &o) { m_offset = o; update(); }

protected:
    void changeEvent(QEvent *event) override;
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool hitButton(const QPoint &pos) const override; // 让点击只作用于“内矩形”

private:
    void animateColor(const QColor &to);
    void animateScale(qreal to);

    QRectF innerRect() const; // 可视按钮区域（居中留边）

private:
    QColor defaultColor { "#006400" };                         // 深绿
    QColor defaultPressedColor { defaultColor.darker(120) };   // 稍暗一点
    QColor defaultCheckedColor { defaultColor.darker(190) };
    QColor defaultDisabledColor { "#808080" };
private:
    QColor  m_bgColor { defaultColor };
    qreal   m_scale   { 1.0 };
    QPointF m_offset  { 0, 0 };
private:
    QColor m_normalColor { defaultColor };
    QColor m_pressedColor{ defaultPressedColor };
    QColor m_checkedColor{ defaultCheckedColor };
    QColor m_disabledColor{ defaultDisabledColor };
    static constexpr int kMargin = 6; // 四周留白，用于浮动/放大不被裁
};

// private:
//     QColor defaultColor = "#006400";

// private:
//     QColor  m_bgColor { "#006400" };
//     qreal   m_scale   { 1.0 };
//     QPointF m_offset  { 0, 0 };
// private:
//     QColor m_normalColor { "#006400" };
//     QColor m_pressedColor{ "#2e86c1" };
//     QColor m_checkedColor{ "#808080" };
//     QColor m_disabledColor{ "#808080" };
//     static constexpr int kMargin = 6; // 四周留白，用于浮动/放大不被裁
// };
