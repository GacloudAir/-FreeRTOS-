#pragma once

// ============ 横屏布局：212 x 104 ============
// 屏幕物理尺寸 104x212，rotation=3 后逻辑尺寸交换为 212x104
namespace Layout {

  // 两列选项框的左边界（左半边 / 右半边）
  constexpr int COL_X[2] = {9, 115};

  // 每列选项框宽度（恢复为两列布局前的大小）
  constexpr int COL_W = 88;

  // 三行选项，每行中心 y
  constexpr int ROW_Y[3] = {36, 62, 88};

  // Settings 页专用偏移
  constexpr int SETTINGS_X_SHIFT = -3;

  // 选项框高度
  constexpr int BOX_H = 20;

  // 字符相对框选区的偏移
  constexpr int TEXT_OFF_X = 10;
  constexpr int TEXT_OFF_Y = -4;

  // 一页最大项数（2 列 × 3 行）
  constexpr int ITEMS_PER_PAGE = 6;

  // 标题位置
  constexpr int TITLE_X = 8;
  constexpr int TITLE_Y = 14;
}