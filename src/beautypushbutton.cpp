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
    setScale(0.98);


    connect(this, &QPushButton::toggled, this, [this](bool checked){
        if (checked) {
            animateColor(m_checkedColor);
        } else {
            animateColor(m_normalColor);
        }
    });
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
        m_bgColor = c;   // 如果当前就处于 checked 状态，立刻更新显示
        update();
    }
}

void BeautyPushButton::setDisabledColor(const QColor &c)
{
    m_disabledColor = c;
    if (!isEnabled()) {
        setBgColor(c);           // 如果要渐变：animateColor(c);
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
    // 给几何尺寸四周各多 6px 缓冲区（布局会按这个来放置）
    QSize s = QPushButton::sizeHint();
    return s + QSize(kMargin*2, kMargin*2);
}

QRectF BeautyPushButton::innerRect() const {
    // 真正绘制按钮的区域（中间的小一圈），留出透明边
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

    // 视觉浮动（只影响绘制，不动几何）
    p.translate(m_offset);

    // 以内矩形中心做缩放
    const QRectF r = innerRect();
    p.translate(r.center());
    p.scale(m_scale, m_scale);
    p.translate(-r.center());

    // 背景圆角
    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, 8, 8);

    // 文本（白色，居中在内矩形）
    p.setPen(Qt::white);
    p.setFont(font());
    p.drawText(r, Qt::AlignCenter, text());
}

void BeautyPushButton::mouseMoveEvent(QMouseEvent *event)
{
    // 根据鼠标相对内矩形中心的偏移来计算视觉平移，最大不超过 kMargin-1，避免出界被裁
    const QPointF c = innerRect().center();
    const QPointF diff = event->pos() - c;

    const qreal maxX = innerRect().width()  / 2.0;
    const qreal maxY = innerRect().height() / 2.0;

    int offset = 2;

    const qreal maxShift = qMax(1, offset); // 留一点安全边
    const qreal dx = qBound(-maxShift, diff.x() / maxX * maxShift, maxShift);
    const qreal dy = qBound(-maxShift, diff.y() / maxY * maxShift, maxShift);

    setOffset({dx, dy / 2}); // 触发重绘

    QPushButton::mouseMoveEvent(event);
}

void BeautyPushButton::enterEvent(QEnterEvent *event)
{
    if (!isEnabled()) {
        event->ignore();
        return;
    }
    animateScale(1.0);
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
        offsetAnim->setEndValue(QPointF(0, 3)); //阴影下移多少
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
    animateScale(0.98);
    // 视觉偏移回零（用属性动画）
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
        offsetAnim->setEndValue(QPointF(0, 0)); // 回正
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
