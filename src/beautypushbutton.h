#pragma once
#include <QPushButton>
#include <QColor>
#include <QPointF>
#include <Qt>

class BeautyPushButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor  bgColor READ bgColor  WRITE setBgColor)
    Q_PROPERTY(qreal   scale  READ scale    WRITE setScale)
    Q_PROPERTY(QPointF offset READ offset   WRITE setOffset)
    Q_PROPERTY(bool floatingOnChecked READ floatingOnChecked WRITE setFloatingOnChecked)
    Q_PROPERTY(bool borderEnabled READ borderEnabled WRITE setBorderEnabled)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(qreal borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment)

public:
    explicit BeautyPushButton(QWidget *parent = nullptr);

    void setThemeColor(const QColor &base);
    void setNormalColor(const QColor &c);
    void setPressedColor(const QColor &c);
    void setCheckedColor(const QColor &c);
    void setDisabledColor(const QColor &c);
    void setTextColor(const QColor &c);
    bool    floatingOnChecked() const { return m_floatingOnChecked; }
    void    setFloatingOnChecked(bool enabled);
    bool    borderEnabled() const { return m_borderEnabled; }
    void    setBorderEnabled(bool enabled);
    QColor  borderColor() const { return m_borderColor; }
    void    setBorderColor(const QColor &c);
    qreal   borderWidth() const { return m_borderWidth; }
    void    setBorderWidth(qreal width);
    Qt::Alignment textAlignment() const { return m_textAlignment; }
    void    setTextAlignment(Qt::Alignment alignment);
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
    void animateShadow(qreal blurRadius, const QPointF &offset);
    void syncShadowState();
    bool shouldKeepFloating() const;

    QRectF innerRect() const;

private:
    QColor defaultColor { 210, 245, 210 };
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
    QColor m_textColor{ Qt::black };
    bool m_floatingOnChecked { false };
    bool m_borderEnabled { false };
    QColor m_borderColor { Qt::black };
    qreal m_borderWidth { 1.0 };
    Qt::Alignment m_textAlignment { Qt::AlignCenter };
    static constexpr int kMargin = 6;
};
