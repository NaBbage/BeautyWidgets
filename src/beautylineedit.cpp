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
    setFrame(false); // 去掉自带边框
    setStyleSheet("QLineEdit { background: transparent; }");

    setMinimumHeight(40);
    setStyleSheet(QString("QLineEdit { background: transparent; padding-left: %1px; padding-right: %1px;}").arg(kMargin + 4));
    setThemeColor("#228B22");

}

static QColor mixWithWhite(const QColor &c, qreal factor) {
    // factor=0 → 原色，factor=1 → 白色
    return QColor::fromRgbF(
        c.redF()   * (1.0 - factor) + 1.0 * factor,
        c.greenF() * (1.0 - factor) + 1.0 * factor,
        c.blueF()  * (1.0 - factor) + 1.0 * factor,
        1.0
        );
}

void BeautyLineEdit::setThemeColor(const QColor &c) {
    m_themeColor  = c;                       // 边框用
    m_normalColor = mixWithWhite(c, 0.94);    // 平时背景
    m_activeColor = mixWithWhite(c, 0.97);   // 激活背景
    m_bgColor     = isEnabled() ? m_normalColor : m_disabledColor;
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

void BeautyLineEdit::setOffset(const QPointF &o)
{
    if (qFuzzyCompare(m_offset.x(), o.x()) && qFuzzyCompare(m_offset.y(), o.y()))
        return;

    m_offset = o;

    // 让文字跟着一起“漂”——用 text margins 轻轻平移
    const int dx = qBound(-3, qRound(o.x()), 3);
    const int dy = qBound(-3, qRound(o.y()), 3);

    // 方式A：只改左/上边距（不改变内容宽度就别动右/下）
    setTextMargins(m_baseMargins.left() + dx,
                   m_baseMargins.top()  + dy,
                   m_baseMargins.right(),
                   m_baseMargins.bottom());

    // 若想保持文本宽度不变，可对称调整（注意别调成负值）：
    // setTextMargins(m_baseMargins.left() + dx,
    //                m_baseMargins.top()  + dy,
    //                std::max(0, m_baseMargins.right() - dx),
    //                std::max(0, m_baseMargins.bottom() - dy));

    update();
}

void BeautyLineEdit::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF r = rect().adjusted(kMargin, kMargin, -kMargin, -kMargin);
    const qreal radius = r.height() / 2.0;

    // 底层光晕（不跟随 offset）
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

    // 只让“药丸”跟随 offset/scale
    p.save();
    p.translate(m_offset);
    p.translate(r.center());
    p.scale(m_scale, m_scale);
    p.translate(-r.center());

    // 背景
    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, radius, radius);

    // 轮廓：focus 时稍粗
    const qreal outlineW = hasFocus() ? 2 : 1.2;
    QPen outline(m_themeColor, outlineW);
    p.setBrush(Qt::NoBrush);
    p.setPen(outline);
    p.drawRoundedRect(r, radius, radius);
    p.restore();

    // 文字/光标交给基类（不受前面的平移影响）
    QLineEdit::paintEvent(event);
}

void BeautyLineEdit::focusInEvent(QFocusEvent *event)
{
    animateColor(m_activeColor);
    animateScale(1.0);

    // 阴影动画（照搬 BeautyPushButton::enterEvent）
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
        offsetAnim->setEndValue(QPointF(0, 3));   // ← 按钮的下移距离
        offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
        offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QLineEdit::focusInEvent(event);
}

void BeautyLineEdit::focusOutEvent(QFocusEvent *event)
{
    animateColor(m_normalColor);
    animateScale(1.0);

    // 阴影动画（照搬 BeautyPushButton::leaveEvent）
    if (auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect())) {
        auto *blur = new QPropertyAnimation(shadow, "blurRadius");
        blur->setDuration(150);
        blur->setStartValue(shadow->blurRadius());
        blur->setEndValue(0);    // ← 按钮的收回值
        blur->setEasingCurve(QEasingCurve::OutCubic);
        blur->start(QAbstractAnimation::DeleteWhenStopped);

        auto *offsetAnim = new QPropertyAnimation(shadow, "offset");
        offsetAnim->setDuration(150);
        offsetAnim->setStartValue(shadow->offset());
        offsetAnim->setEndValue(QPointF(0, 0));   // ← 回正
        offsetAnim->setEasingCurve(QEasingCurve::OutCubic);
        offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // 光晕照常收掉
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
        // 已激活 → 浮动偏移
        const QPointF c = rect().center();
        const QPointF diff = event->pos() - c;

        const qreal maxX = rect().width()  / 2.0;
        const qreal maxY = rect().height() / 2.0;

        qreal dx = qBound(-2.0, diff.x() / maxX * 2.0, 2.0);
        qreal dy = qBound(-2.0, diff.y() / maxY * 2.0, 2.0);

        setOffset({dx, dy / 4});
    } else {
        // 未激活时，光晕只横向移动
        qreal minX = height() / 1.75;                        // 左边界
        qreal maxX = width() - minX;              // 右边界
        qreal clampedX = qBound(minX, (qreal)event->pos().x(), maxX);

        // Y 固定在输入框的垂直中点
        qreal fixedY = rect().center().y() + 1;

        m_cursorPos = QPointF(clampedX, fixedY);

        update();

        // 如果第一次进入，做淡入动画
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
        // 鼠标进入时，记录当前位置
        m_cursorPos = event->position();  // Qt6 用 position()，Qt5 用 pos()

        // 做淡入动画
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
        // 只有激活时才需要动画回中
        auto *back = new QPropertyAnimation(this, "offset");
        back->setDuration(180);
        back->setStartValue(m_offset);
        back->setEndValue(QPointF(0,0));
        back->setEasingCurve(QEasingCurve::OutCubic);
        back->start(QAbstractAnimation::DeleteWhenStopped);
    }else{
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
