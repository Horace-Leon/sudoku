import copy
import json

class Grid:
    BOX_SIZE = 3
    GRID_SIZE = 9
    def __init__(self, grid_string):
        self.grid = self.parse_grid(grid_string)
        self.validate_grid()

    def parse_grid(self, grid_string):
        if len(grid_string) != self.GRID_SIZE * self.GRID_SIZE:
            raise ValueError("输入字符串长度必须为81位。")
        if not grid_string.isdigit():
            raise ValueError("输入字符串必须仅包含数字。")
        grid = [[int(grid_string[i * self.GRID_SIZE + j]) for j in range(self.GRID_SIZE)] for i in
                range(self.GRID_SIZE)]
        return grid

    def validate_grid(self):
        """验证初始棋盘是否符合数独规则。"""
        for i in range(self.GRID_SIZE):
            row = [num for num in self.getRow(i) if num != 0]
            if len(row) != len(set(row)):
                raise ValueError(f"第{i + 1}行存在重复的数字。")

            col = [num for num in self.getColumn(i) if num != 0]
            if len(col) != len(set(col)):
                raise ValueError(f"第{i + 1}列存在重复的数字。")

        for box_row in range(0, self.GRID_SIZE, self.BOX_SIZE):
            for box_col in range(0, self.GRID_SIZE, self.BOX_SIZE):
                box = [num for num in self.getBox(box_row, box_col) if num != 0]
                if len(box) != len(set(box)):
                    raise ValueError(f"位置 ({box_row + 1}, {box_col + 1}) 的3x3方格存在重复的数字。")

    def getRow(self, row):
        """返回指定行的数据"""
        if not 0 <= row < self.GRID_SIZE:
            raise IndexError("行索引超出范围。")
        return self.grid[row]

    def getColumn(self, col):
        """返回指定列的数据"""
        if not 0 <= col < self.GRID_SIZE:
            raise IndexError("列索引超出范围。")
        return [self.grid[row][col] for row in range(self.GRID_SIZE)]

    def getBox(self, row, col):
        """返回指定位置的3x3方格数据"""
        if not (0 <= row < self.GRID_SIZE and 0 <= col < self.GRID_SIZE):
            raise IndexError(f"行或列索引超出范围：row={row}, col={col}")
        box_row = (row // self.BOX_SIZE) * self.BOX_SIZE
        box_col = (col // self.BOX_SIZE) * self.BOX_SIZE
        return [self.grid[r][c] for r in range(box_row, box_row + self.BOX_SIZE)
                for c in range(box_col, box_col + self.BOX_SIZE)]

    def __str__(self):
        """外表化：输出友好的字符串表示形式"""
        return '\n'.join([' '.join(map(str, row)) for row in self.grid])

    def __eq__(self, other):
        """比较两个Grid对象是否相等"""
        if isinstance(other, Grid):
            return self.grid == other.grid
        return False

    def __hash__(self):
        """生成哈希值"""
        return hash(tuple(tuple(row) for row in self.grid))

    def clone(self):
        """克隆：返回一个Grid对象的深度副本"""
        return copy.deepcopy(self)

    def serialize(self):
        """串行化：将Grid对象转换为JSON格式"""
        return json.dumps(self.grid)

    @classmethod
    def deserialize(cls, json_string):
        """反序列化：从JSON字符串创建一个Grid对象"""
        grid_list = json.loads(json_string)
        grid_string = ''.join(str(num) for row in grid_list for num in row)
        return cls(grid_string)

    def __iter__(self):
        return iter(self.grid)


class Sudoku(Grid):
    def __init__(self, grid_string=None, grid=None):
        if grid is not None:
            self.grid = grid
            self.validate_grid()
        elif grid_string is not None:
            super().__init__(grid_string)
        else:
            raise ValueError("必须提供grid_string或grid之一。")


    def getInference(self):
        """推理棋盘，返回每个单元格的候选值"""
        candidates = {}
        for row in range(self.GRID_SIZE):
            for col in range(self.GRID_SIZE):
                if self.grid[row][col] == 0:
                    cell_candidates = self.get_candidates(row, col)
                    if not cell_candidates:
                        raise ValueError(f"单元格 ({row + 1}, {col + 1}) 无可用候选值，数独无解。")
                    candidates[(row, col)] = cell_candidates
        return candidates

    def get_candidates(self, row, col):
        """返回指定单元格的候选值"""
        row_vals = set(self.getRow(row))
        col_vals = set(self.getColumn(col))
        box_vals = set(self.getBox(row, col))
        used_vals = row_vals | col_vals | box_vals
        return [n for n in range(1, 10) if n not in used_vals]

    def __str__(self):
        """外表化：输出数独棋盘的字符串表示"""
        return super().__str__()

    def __eq__(self, other):
        """比较两个Sudoku对象是否相等"""
        return super().__eq__(other)

    def clone(self):
        """克隆：返回一个Sudoku对象的深度副本"""
        return copy.deepcopy(self)

    def serialize(self):
        """串行化：将Sudoku对象转换为JSON格式"""
        return super().serialize()

    @classmethod
    def deserialize(cls, json_string):
        grid = json.loads(json_string)
        return cls(grid=grid)


# 测试代码
def test_sudoku():
    input_string = "017903600000080000900000507072010430000402070064370250701000065000030000005601720"
    sudoku = Sudoku(input_string)

    print("数独棋盘：")
    print(sudoku)

    inference = sudoku.getInference()
    print("\n推理结果（候选值）：")
    for key, value in inference.items():
        print(f"单元格 {key} 的候选值: {value}")

    # 克隆测试
    cloned_sudoku = sudoku.clone()
    print("\n克隆后的数独棋盘：")
    print(cloned_sudoku)

    # 序列化和反序列化测试
    serialized = sudoku.serialize()
    print("\n序列化的数独棋盘：")
    print(serialized)

    deserialized_sudoku = Sudoku.deserialize(serialized)
    print("\n反序列化后的数独棋盘：")
    print(deserialized_sudoku)

    input_string = "017903600000080000900000507072010430000402070064370250701000065000030000005601720"
    sudoku2 = Sudoku(input_string)

    # 比较测试
    print("\n比较两个Sudoku对象是否相等：", sudoku == sudoku2)


if __name__ == "__main__":
# 调用测试代码
    test_sudoku()
