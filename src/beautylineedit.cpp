#include "BeautyLineEdit.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QMouseEvent>

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
    setThemeColor("#228B22");
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
    m_bgColor = isEnabled() ? m_normalColor : m_disabledColor;
    update();
}

void BeautyLineEdit::setDisabledColor(const QColor &c) {
    m_disabledColor = c;
    if (!isEnabled()) {
        m_bgColor = c;
        update();
    }
}

void BeautyLineEdit::setBgColor(const QColor &c) {
    m_bgColor = c;
    update();
}

void BeautyLineEdit::setScale(qreal s) {
    m_scale = s;
    update();
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

    if (!hasFocus() && m_glowAlpha > 0.01 && m_cursorPos.x() > 0) {
        const qreal glowR = height() / 1.75;
        QRadialGradient glow(m_cursorPos, glowR);
        QColor c = m_themeColor;
        c.setAlphaF(m_glowAlpha);
        glow.setColorAt(0.0, c);
        glow.setColorAt(1.0, QColor(0,0,0,0));
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawEllipse(m_cursorPos, glowR, glowR);
    }

    p.save();
    p.translate(m_offset);
    p.translate(r.center());
    p.scale(m_scale, m_scale);
    p.translate(-r.center());

    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, radius, radius);

    const qreal outlineW = hasFocus() ? 2 : 0.8;
    QPen outline(m_themeColor, outlineW);
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

    auto *anim = new QPropertyAnimation(this, "glowAlpha");
    anim->setDuration(200);
    anim->setStartValue(m_glowAlpha);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::valueChanged, this, [this](const QVariant &v){
        m_glowAlpha = v.toReal();
        update();
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    m_cursorPos = QPointF(-1000, -1000);

    QLineEdit::focusOutEvent(event);
}

void BeautyLineEdit::mouseMoveEvent(QMouseEvent *event)
{
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
    } else {
        qreal minX = height() / 1.75;
        qreal maxX = width() - minX;
        qreal clampedX = qBound(minX, (qreal)event->pos().x(), maxX);

        qreal fixedY = rect().center().y() + 1;

        m_cursorPos = QPointF(clampedX, fixedY);

        update();

        if (m_glowAlpha <= 0.01) {
            auto *anim = new QPropertyAnimation(this, "glowAlpha");
            anim->setDuration(200);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
            anim->setEasingCurve(QEasingCurve::OutCubic);
            connect(anim, &QPropertyAnimation::valueChanged, this, [this](const QVariant &v){
                m_glowAlpha = v.toReal();
                update();
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }

    QLineEdit::mouseMoveEvent(event);
}

void BeautyLineEdit::enterEvent(QEnterEvent *event)
{
    if (!hasFocus()) {
        animateScale(kFocusScale);
        m_cursorPos = event->position();  // Qt6 用 position()，Qt5 用 pos()

        auto *anim = new QPropertyAnimation(this, "glowAlpha");
        anim->setDuration(200);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &QPropertyAnimation::valueChanged, this, [this](const QVariant &v){
            m_glowAlpha = v.toReal();
            update();
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QLineEdit::enterEvent(event);
}

void BeautyLineEdit::leaveEvent(QEvent *event)
{
    if (hasFocus()) {
        auto *back = new QPropertyAnimation(this, "offset");
        back->setDuration(180);
        back->setStartValue(m_offset);
        back->setEndValue(QPointF(0,0));
        back->setEasingCurve(QEasingCurve::OutCubic);
        back->start(QAbstractAnimation::DeleteWhenStopped);
    }else{
        animateScale(kRestScale);
        m_glowAlpha = 0.0;
        m_cursorPos = QPointF(-1000, -1000);
        update();
    }

    QLineEdit::leaveEvent(event);
}

void BeautyLineEdit::animateColor(const QColor &to)
{
    auto *anim = new QPropertyAnimation(this, "bgColor");
    anim->setDuration(200);
    anim->setStartValue(m_bgColor);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BeautyLineEdit::animateScale(qreal to)
{
    auto *anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(200);
    anim->setStartValue(m_scale);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
