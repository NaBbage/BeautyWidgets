#pragma once

#include <QLineEdit>
#include <QColor>
#include <QPointF>

class BeautyLineEdit : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(QColor  bgColor READ bgColor WRITE setBgColor)
    Q_PROPERTY(qreal   scale   READ scale   WRITE setScale)
    Q_PROPERTY(QPointF offset  READ offset  WRITE setOffset)

public:
    explicit BeautyLineEdit(QWidget *parent = nullptr);

    void setThemeColor(const QColor &c);
    void setDisabledColor(const QColor &c);

private:
    QColor  bgColor() const { return m_bgColor; }
    void    setBgColor(const QColor &c);
    qreal   scale()  const { return m_scale; }
    void    setScale(qreal s);
    QPointF offset() const { return m_offset; }
    void    setOffset(const QPointF &o);
    QPointF m_cursorPos { -1000, -1000 };
    qreal   m_glowAlpha { 0.0 };
    QMargins m_baseMargins;

protected:
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void animateColor(const QColor &to);
    void animateScale(qreal to);

private:
    QColor  m_themeColor;
    QColor  m_bgColor;
    QColor  m_normalColor;
    QColor  m_activeColor;
    QColor  m_disabledColor;
    qreal   m_scale   { 1.0 };
    QPointF m_offset  { 0, 0 };
    static constexpr int kMargin = 6;
};
