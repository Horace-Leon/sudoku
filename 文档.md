
## 项目名称
数独游戏 

## 版本
1.0

## 项目描述
这是一个数独游戏求解器，提供了基本的数独游戏功能，包括输入解析、有效性验证、候选值推导和深度复制等功能。该程序通过使用面向对象的设计思想，创建了`Grid`和`Sudoku`类，以管理数独棋盘和游戏逻辑。

## 技术栈
- 编程语言：Python 3.x
- 依赖库：
  - `copy`：用于深度复制对象
  - `json`：用于对象序列化和反序列化

## 设计概述

### 类及功能

#### 1. `Grid` 类
- **职责**：表示数独棋盘，提供棋盘的基本操作和验证。
- **主要方法**：
  - `__init__(self, grid_string)`：初始化棋盘并解析输入字符串。
  - `parse_grid(self, grid_string)`：将输入字符串转换为二维列表。
  - `validate_grid(self)`：验证棋盘的有效性。
  - `getRow(self, row)`：获取指定行的数字。
  - `getColumn(self, col)`：获取指定列的数字。
  - `getBox(self, row, col)`：获取指定位置的3x3方格的数字。
  - `clone(self)`：返回棋盘的深度副本。
  - `serialize(self)`：将棋盘转换为JSON格式。
  - `deserialize(cls, json_string)`：从JSON字符串创建棋盘对象。

#### 2. `Sudoku` 类
- **职责**：继承`Grid`类，提供数独特有的功能，如候选值推导。
- **主要方法**：
  - `__init__(self, grid_string=None, grid=None)`：初始化数独棋盘，支持通过字符串或已有棋盘初始化。
  - `getInference(self)`：推导当前棋盘的候选值。
  - `get_candidates(self, row, col)`：返回指定单元格的候选值。

## 使用说明

### 初始化
用户可以通过传入一个81位的数字字符串（`0`表示空白）来初始化一个数独棋盘：
```python
sudoku = Sudoku("530070000600195000098000060800060003400803001700020006060000280000419005000080079")
```

### 验证
程序会自动验证初始化的棋盘是否合法，如果发现不符合数独规则，会抛出相应的错误信息。

### 候选值推导
通过调用`getInference`方法，可以获取每个空白单元格的候选值：
```python
candidates = sudoku.getInference()
```

### 序列化与反序列化
可以将棋盘对象序列化为JSON字符串，或者从JSON字符串反序列化回棋盘对象：
```python
json_string = sudoku.serialize()
new_sudoku = Sudoku.deserialize(json_string)
```

## 错误处理
- 输入字符串长度不等于81位：抛出`ValueError`。
- 输入字符串包含非数字字符：抛出`ValueError`。
- 棋盘验证失败：抛出`ValueError`。
- 超出行列索引范围：抛出`IndexError`。

## 未来工作
- 实现数独求解算法，允许用户输入未完成的数独，并尝试求解。
- 提供图形用户界面（GUI），提升用户体验。
- 增加更复杂的错误处理和用户提示。


## 结论
本软件提供了一个基础的数独游戏框架，具备必要的功能和良好的代码结构，为后续扩展和功能实现提供了基础。