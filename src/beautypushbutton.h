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
    void setNormalColor(const QColor &c);
    void setPressedColor(const QColor &c);
    void setCheckedColor(const QColor &c);
    void setDisabledColor(const QColor &c);
    void setTextColor(const QColor &c);
    QColor  bgColor() const { return m_bgColor; }
    void    setBgColor(const QColor &c);

private:
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
    bool hitButton(const QPoint &pos) const override;

private:
    void animateColor(const QColor &to);
    void animateScale(qreal to);

    QRectF innerRect() const;

private:
    QColor defaultColor { "#003494" };
    QColor defaultPressedColor { defaultColor.darker(120) };
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
    QColor m_textColor{ Qt::white };
    static constexpr int kMargin = 6;
};
