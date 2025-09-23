#include "BeautyPushButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

BeautyPushButton::BeautyPushButton(QWidget *parent)
    : QPushButton(parent)
{
#ifdef Q_OS_MAC
    setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground, true);

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(0);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 100));
    setGraphicsEffect(shadow);
    setScale(1);

    connect(this, &QPushButton::toggled, this, [this](bool checked){
        if (checked) {
            animateColor(m_checkedColor);
        } else {
            animateColor(m_normalColor);
        }
    });
    setStyleSheet("QPushButton { border: 7px solid; border-radius: 8px; background: transparent; }");
}

void BeautyPushButton::setThemeColor(const QColor &base)
{
    m_normalColor  = base;
    m_pressedColor = base.darker(120);
    m_checkedColor = base.darker(150);
    QColor target;
    if (!isEnabled()) {
        target = m_disabledColor;
    } else if (isCheckable() && isChecked()) {
        target = m_checkedColor;
    } else {
        target = m_normalColor;
    }
    animateColor(target);
    update();
}

void BeautyPushButton::setCheckedColor(const QColor &c)
{
    m_checkedColor = c;
    if (isChecked()) {
        m_bgColor = c;
        update();
    }
}

void BeautyPushButton::setDisabledColor(const QColor &c)
{
    m_disabledColor = c;
    if (!isEnabled()) {
        setBgColor(c);
    }
}

void BeautyPushButton::changeEvent(QEvent *event)
{
    QPushButton::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        if (!isEnabled()) {
            setBgColor(m_disabledColor);
            return;
        }
        if (isCheckable() && isChecked()) {
            animateColor(m_checkedColor);
        } else {
            animateColor(m_normalColor);
        }
    }
}

QSize BeautyPushButton::sizeHint() const {
    QSize s = QPushButton::sizeHint();
    return s + QSize(kMargin*2, kMargin*2);
}

QRectF BeautyPushButton::innerRect() const {
    return QRectF(rect()).adjusted(kMargin, kMargin, -kMargin, -kMargin);
}

void BeautyPushButton::setBgColor(const QColor &c){
    m_bgColor = c;
    update();
}

void BeautyPushButton::setScale(qreal s){
    m_scale = s;
    update();
}

void BeautyPushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.translate(m_offset);

    const QRectF r = innerRect();
    p.translate(r.center());
    p.scale(m_scale, m_scale);
    p.translate(-r.center());

    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, 8, 8);

    p.setPen(Qt::white);
    p.setFont(font());
    p.drawText(r, Qt::AlignCenter, text());
}

void BeautyPushButton::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF c = innerRect().center();
    const QPointF diff = event->pos() - c;

    const qreal maxX = innerRect().width()  / 2.0;
    const qreal maxY = innerRect().height() / 2.0;

    int offset = 2;

    const qreal maxShift = qMax(1, offset);
    const qreal dx = qBound(-maxShift, diff.x() / maxX * maxShift, maxShift);
    const qreal dy = qBound(-maxShift, diff.y() / maxY * maxShift, maxShift);

    setOffset({dx, dy / 2});

    QPushButton::mouseMoveEvent(event);
}

void BeautyPushButton::enterEvent(QEnterEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    animateScale(1.01);
    if (auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect())) {
        auto *blur = new QPropertyAnimation(shadow, "blurRadius");
        blur->setDuration(150);
        blur->setStartValue(shadow->blurRadius());
        blur->setEndValue(30);
        blur->setEasingCurve(QEasingCurve::OutCubic);
        blur->start(QAbstractAnimation::DeleteWhenStopped);
        auto *offsetAnim = new QPropertyAnimation(shadow, "offset");
        offsetAnim->setDuration(150);
        offsetAnim->setStartValue(shadow->offset());
        offsetAnim->setEndValue(QPointF(0, 3));
        offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
        offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    QPushButton::enterEvent(event);
}

void BeautyPushButton::leaveEvent(QEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    animateScale(1);
    auto *back = new QPropertyAnimation(this, "offset");
    back->setDuration(180);
    back->setStartValue(m_offset);
    back->setEndValue(QPointF(0,0));
    back->setEasingCurve(QEasingCurve::OutCubic);
    back->start(QAbstractAnimation::DeleteWhenStopped);

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

    QPushButton::leaveEvent(event);
}

void BeautyPushButton::mousePressEvent(QMouseEvent *event)
{
    animateColor(QColor(m_pressedColor));
    animateScale(0.95);

    QPushButton::mousePressEvent(event);
}

void BeautyPushButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    animateColor(QColor(m_normalColor));
    animateScale(1.0);
    if (isCheckable() && isChecked()) {
        animateColor(m_checkedColor);
    } else {
        animateColor(m_normalColor);
    }
    QPushButton::mouseReleaseEvent(event);
}

bool BeautyPushButton::hitButton(const QPoint &pos) const
{
    QPainterPath path;
    path.addRoundedRect(innerRect(), 8, 8);
    return path.contains(pos);
}

void BeautyPushButton::animateColor(const QColor &to)
{
    auto *anim = new QPropertyAnimation(this, "bgColor");
    anim->setDuration(150);
    anim->setStartValue(m_bgColor);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void BeautyPushButton::animateScale(qreal to)
{
    auto *anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(150);
    anim->setStartValue(m_scale);
    anim->setEndValue(to);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
