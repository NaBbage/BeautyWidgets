# BeautyWidgets
美丽Qt组件库

## 简介 | Introduction

**BeautyWidgets** 是一个基于 **Qt** 的美丽自定义组件库。目前处于早期阶段（alpha 版本），只有按钮和条形输入框两个组件。  

**BeautyWidgets** is a **Qt-based custom widget library** designed to bring a beautiful user interface experience.  
Currently in **early alpha stage**, with only two components: a button and a pill-shaped line edit.  

---

## 组件 | Components

- **BeautyPushButton**  
  自带悬浮阴影与点击缩放动画的按钮。  
  A push button with hover shadow and click-scale animation.  

- **BeautyLineEdit**  
  药丸形输入框，带激活高亮、阴影动画、鼠标位置光晕。  
  A pill-shaped line edit with active highlight, shadow animation, and a cursor-following glow effect.  

---

## 已知问题 | Known Issues

目前组件的文字始终为白色，不适配较浅的主题色。  
Currently, the text color is always white, which does not adapt well to lighter theme colors.  

---

## 计划 | Roadmap

- [ ] Adapt components to lighter theme colors 组件适配较浅的主题色  
- [ ] Add more components 更多组件  

---

## 安装 | Installation

直接把源代码复制进你的项目。  
Simply copy the source code into your Qt project.  

---

## 使用 | Usage

使用 `setThemeColor(const QColor &c)` 来设置组件颜色。请设置为较深的颜色。其他方法和 Qt 原生组件方法相同。  
Use `setThemeColor(const QColor &c)` to set the component theme color. Please choose a relatively dark color. Other methods are the same as native Qt components.  

---

## 许可证 | License

本项目使用 **MIT License**。  
你可以在保留版权声明的前提下自由使用、修改和分发。  

This project is licensed under the **MIT License**.  
You are free to use, modify and distribute it with proper copyright notice.  

---

## 作者 | Author

BeautyWidgets by **NaBeauty** ✨
