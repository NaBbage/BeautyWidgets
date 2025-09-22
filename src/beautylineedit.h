#pragma once

#include <QLineEdit>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QSize>

class BeautyLineEdit : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(QColor  bgColor READ bgColor WRITE setBgColor)
    Q_PROPERTY(qreal   scale   READ scale   WRITE setScale)
    Q_PROPERTY(QPointF offset  READ offset  WRITE setOffset)
    Q_PROPERTY(qreal glowAlpha READ glowAlpha WRITE setGlowAlpha)

public:
    explicit BeautyLineEdit(QWidget *parent = nullptr);

    void setThemeColor(const QColor &c);
    void setDisabledColor(const QColor &c);
    QSize sizeHint() const override;

private:
    QColor  bgColor() const { return m_bgColor; }
    void    setBgColor(const QColor &c);
    qreal   scale()  const { return m_scale; }
    void    setScale(qreal s);
    QPointF offset() const { return m_offset; }
    void    setOffset(const QPointF &o);
    QPointF m_cursorPos { -1000, -1000 };
    qreal   m_glowAlpha { 0.0 };

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
    qreal glowAlpha() const { return m_glowAlpha; }
        void setGlowAlpha(qreal alpha) {
            if (!qFuzzyCompare(m_glowAlpha, alpha)) {
                m_glowAlpha = alpha;
                update();
            }
        }

    QRectF innerRect() const;

private:
    QColor  m_themeColor;
    QColor  m_bgColor;
    QColor  m_normalColor;
    QColor  m_activeColor;
    QColor  m_disabledColor;
    qreal   m_scale   { 0.5 };
    QPointF m_offset  { 0, 0 };
    static constexpr int kMargin = 5;
    static constexpr qreal kRestScale = 0.98;
    static constexpr qreal kFocusScale = 1.0;
};
