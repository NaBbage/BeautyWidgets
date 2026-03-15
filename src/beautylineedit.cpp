#include "BeautyLineEdit.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QEvent>

BeautyLineEdit::BeautyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(0);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0,0,0,60));
    setGraphicsEffect(shadow);
    setFrame(false);

    setMinimumHeight(40);
    setStyleSheet(QStringLiteral(
        "QLineEdit { background: transparent; border: none; padding-left: %1px; padding-right: %1px; padding-top: 0px; padding-bottom: 0px; }")
                      .arg(kMargin + 6 + width() * 0.01));
    setThemeColor(QColor("#003494"));
    setScale(kRestScale);

}

static QColor mixWithWhite(const QColor &c, qreal factor) {
    return QColor::fromRgbF(
        c.redF()   * (1.0 - factor) + 1.0 * factor,
        c.greenF() * (1.0 - factor) + 1.0 * factor,
        c.blueF()  * (1.0 - factor) + 1.0 * factor,
        1.0
        );
}

void BeautyLineEdit::setThemeColor(const QColor &c) {
    m_themeColor  = c;
    m_normalColor = mixWithWhite(c, 0.96);
    m_activeColor = mixWithWhite(c, 0.99);
    QColor target = m_disabledColor;
    if (isEnabled()) {
        target = hasFocus() ? m_activeColor : m_normalColor;
    }
    // First-time initialization: avoid animating from an invalid/default (often black) bg.
    if (!m_bgColor.isValid()) {
        m_bgColor = target;
        update();
    } else {
        animateColor(target);
    }

    if (!qFuzzyIsNull(m_offset.x()) || !qFuzzyIsNull(m_offset.y())) {
        auto *back = new QPropertyAnimation(this, "offset", this);
        back->setDuration(180);
        back->setStartValue(m_offset);
        back->setEndValue(QPointF(0, 0));
        back->setEasingCurve(QEasingCurve::OutCubic);
        back->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void BeautyLineEdit::setDisabledColor(const QColor &c) {
    m_disabledColor = c;
    if (!isEnabled()) {
        m_bgColor = c;
        update();
    }
}

void BeautyLineEdit::setTextColor(const QColor &c)
{
    auto pal = palette();
    pal.setColor(QPalette::Text, c);
    pal.setColor(QPalette::PlaceholderText, c);
    setPalette(pal);
    update();
}

void BeautyLineEdit::setBgColor(const QColor &c) {
    m_bgColor = c;
    update();
}

void BeautyLineEdit::setScale(qreal s) {
    m_scale = s;
    update();
}

void BeautyLineEdit::changeEvent(QEvent *event)
{
    QLineEdit::changeEvent(event);

    if (event->type() != QEvent::EnabledChange) {
        return;
    }

    if (!isEnabled()) {
        m_savedFocusPolicy = focusPolicy();

        setFocusPolicy(Qt::NoFocus);
        clearFocus();
        const auto anims = findChildren<QAbstractAnimation*>();
        for (auto *a : anims) {
            if (a) a->stop();
        }
        setCursor(Qt::ArrowCursor);
        setOffset(QPointF(0, 0));
        setScale(kRestScale);
        setBgColor(m_disabledColor);

        if (auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect())) {
            auto *blur = new QPropertyAnimation(shadow, "blurRadius", this);
            blur->setDuration(150);
            blur->setStartValue(shadow->blurRadius());
            blur->setEndValue(0);
            blur->setEasingCurve(QEasingCurve::OutCubic);
            blur->start(QAbstractAnimation::DeleteWhenStopped);

            auto *offsetAnim = new QPropertyAnimation(shadow, "offset", this);
            offsetAnim->setDuration(150);
            offsetAnim->setStartValue(shadow->offset());
            offsetAnim->setEndValue(QPointF(0, 0));
            offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
            offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
        }
        return;
    }

    setFocusPolicy(m_savedFocusPolicy);
    setCursor(Qt::IBeamCursor);
    setOffset(QPointF(0, 0));
    setScale(underMouse() ? kFocusScale : kRestScale);
    if (hasFocus()) {
        animateColor(m_activeColor);
    } else {
        animateColor(m_normalColor);
    }
}

QSize BeautyLineEdit::sizeHint() const
{
    QSize base = QLineEdit::sizeHint();
    return base + QSize(kMargin * 2, kMargin * 2);
}

QRectF BeautyLineEdit::innerRect() const
{
    return QRectF(rect()).adjusted(kMargin, kMargin, -kMargin, -kMargin);
}

void BeautyLineEdit::setOffset(const QPointF &o)
{
    const QPointF clamped(qBound(-3.0, o.x(), 3.0),
                          qBound(-3.0, o.y(), 3.0));

    if (qFuzzyCompare(m_offset.x(), clamped.x()) && qFuzzyCompare(m_offset.y(), clamped.y()))
        return;

    m_offset = clamped;
    update();
}

void BeautyLineEdit::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF r = innerRect();
    const qreal radius = r.height() / 2.0;

    p.save();
    p.translate(m_offset);
    p.translate(r.center());
    p.scale(m_scale, m_scale);
    p.translate(-r.center());

    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, radius, radius);

    const qreal outlineW = hasFocus() ? 2 : 0.8;
    QPen outline(isEnabled()? m_themeColor : m_disabledColor, outlineW);
    p.setBrush(Qt::NoBrush);
    p.setPen(outline);
    p.drawRoundedRect(r, radius, radius);
    p.restore();

    setStyleSheet(QStringLiteral(
                    "QLineEdit { background: transparent; border: none; padding-left: %1px; padding-right: %1px; padding-top: 0px; padding-bottom: 0px; }")
                    .arg(kMargin + 6 + width() * 0.01));

    QLineEdit::paintEvent(event);
}

void BeautyLineEdit::focusInEvent(QFocusEvent *event)
{
    if (!isEnabled()) {
        QLineEdit::focusInEvent(event);
        return;
    }
    animateColor(m_activeColor);
    animateScale(kFocusScale);

    if (auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect())) {
        auto *blur = new QPropertyAnimation(shadow, "blurRadius");
        blur->setDuration(150);
        blur->setStartValue(shadow->blurRadius());
        blur->setEndValue(30);   // ← 按钮的模糊半径
        blur->setEasingCurve(QEasingCurve::OutCubic);
        blur->start(QAbstractAnimation::DeleteWhenStopped);

        auto *offsetAnim = new QPropertyAnimation(shadow, "offset");
        offsetAnim->setDuration(150);
        offsetAnim->setStartValue(shadow->offset());
        offsetAnim->setEndValue(QPointF(0, 3));
        offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
        offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QLineEdit::focusInEvent(event);
}

void BeautyLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (!isEnabled()) {
        QLineEdit::focusOutEvent(event);
        return;
    }
    animateColor(m_normalColor);
    animateScale(underMouse() ? kFocusScale : kRestScale);

    if (auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect())) {
        auto *blur = new QPropertyAnimation(shadow, "blurRadius");
        blur->setDuration(150);
        blur->setStartValue(shadow->blurRadius());
        blur->setEndValue(0);
        blur->setEasingCurve(QEasingCurve::OutCubic);
        blur->start(QAbstractAnimation::DeleteWhenStopped);

        auto *offsetAnim = new QPropertyAnimation(shadow, "offset");
        offsetAnim->setDuration(150);
        offsetAnim->setStartValue(shadow->offset());
        offsetAnim->setEndValue(QPointF(0, 0));
        offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
        offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QLineEdit::focusOutEvent(event);
}

void BeautyLineEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    if (hasFocus()) {
        const QPointF c = rect().center();
        const QPointF diff = event->pos() - c;

        const qreal maxX = rect().width()  / 2.0;
        const qreal maxY = rect().height() / 2.0;

        int offset = 2;

        const qreal maxShift = qMax(1, offset);
        const qreal dx = qBound(-maxShift, diff.x() / maxX * maxShift, maxShift);
        const qreal dy = qBound(-maxShift, diff.y() / maxY * maxShift, maxShift);

        setOffset({dx, dy / 2});
    }

    QLineEdit::mouseMoveEvent(event);
}

void BeautyLineEdit::enterEvent(QEnterEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    if (!hasFocus()) {
        animateScale(kFocusScale);
    }

    QLineEdit::enterEvent(event);
}

void BeautyLineEdit::leaveEvent(QEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    if (hasFocus()) {
        auto *back = new QPropertyAnimation(this, "offset", this);
        back->setDuration(180);
        back->setStartValue(m_offset);
        back->setEndValue(QPointF(0,0));
        back->setEasingCurve(QEasingCurve::OutCubic);
        back->start(QAbstractAnimation::DeleteWhenStopped);
    }else{
        animateScale(kRestScale);
    }

    QLineEdit::leaveEvent(event);
}

void BeautyLineEdit::animateColor(const QColor &to)
{
    auto *anim = new QPropertyAnimation(this, "bgColor", this);
    anim->setDuration(200);
    anim->setStartValue(m_bgColor);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BeautyLineEdit::animateScale(qreal to)
{
    auto *anim = new QPropertyAnimation(this, "scale", this);
    anim->setDuration(200);
    anim->setStartValue(m_scale);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
