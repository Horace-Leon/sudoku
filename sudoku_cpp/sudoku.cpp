#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <set>
#include <map>
#include <utility>

class Grid {
public:
    static const int BOX_SIZE = 3;
    static const int GRID_SIZE = 9;

    Grid(const std::string& grid_string) {
        grid = parse_grid(grid_string);
        validate_grid();
    }

    // 构造函数用于反序列化
    Grid(const std::vector<std::vector<int>>& grid_) : grid(grid_) {
        validate_grid();
    }

    // 克隆构造函数
    Grid(const Grid& other) : grid(other.grid) {}

    // 获取指定行
    std::vector<int> getRow(int row) const {
        if (!(0 <= row && row < GRID_SIZE)) {
            throw std::out_of_range("行索引超出范围。");
        }
        return grid[row];
    }

    // 获取指定列
    std::vector<int> getColumn(int col) const {
        if (!(0 <= col && col < GRID_SIZE)) {
            throw std::out_of_range("列索引超出范围。");
        }
        std::vector<int> column(GRID_SIZE);
        for (int row = 0; row < GRID_SIZE; ++row) {
            column[row] = grid[row][col];
        }
        return column;
    }

    // 获取指定位置的3x3方格
    std::vector<int> getBox(int row, int col) const {
        if (!(0 <= row && row < GRID_SIZE && 0 <= col && col < GRID_SIZE)) {
            throw std::out_of_range("行或列索引超出范围。");
        }
        int box_row = (row / BOX_SIZE) * BOX_SIZE;
        int box_col = (col / BOX_SIZE) * BOX_SIZE;
        std::vector<int> box;
        for (int r = box_row; r < box_row + BOX_SIZE; ++r) {
            for (int c = box_col; c < box_col + BOX_SIZE; ++c) {
                box.push_back(grid[r][c]);
            }
        }
        return box;
    }

    // 克隆方法
    Grid clone() const {
        return Grid(*this);
    }

    // 序列化
    std::string serialize() const {
        std::ostringstream oss;
        for (const auto& row : grid) {
            for (const auto& num : row) {
                oss << num;
            }
        }
        return oss.str();
    }

    // 反序列化
    static Grid deserialize(const std::string& grid_string) {
        return Grid(grid_string);
    }

    // 重载输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Grid& grid) {
        for (const auto& row : grid.grid) {
            for (const auto& num : row) {
                os << num << ' ';
            }
            os << '\n';
        }
        return os;
    }

    // 重载相等运算符
    bool operator==(const Grid& other) const {
        return grid == other.grid;
    }

protected:
    std::vector<std::vector<int>> grid;

    // 解析输入字符串
    std::vector<std::vector<int>> parse_grid(const std::string& grid_string) {
        if (grid_string.length() != GRID_SIZE * GRID_SIZE) {
            throw std::invalid_argument("输入字符串长度必须为81位。");
        }
        if (!std::all_of(grid_string.begin(), grid_string.end(), ::isdigit)) {
            throw std::invalid_argument("输入字符串必须仅包含数字。");
        }

        std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE));
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                char c = grid_string[i * GRID_SIZE + j];
                grid[i][j] = c - '0';
            }
        }
        return grid;
    }

    // 验证棋盘是否合法
    void validate_grid() {
        // 检查行
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> row = getRow(i);
            std::vector<int> nums;
            for (int num : row) {
                if (num != 0) nums.push_back(num);
            }
            std::set<int> num_set(nums.begin(), nums.end());
            if (nums.size() != num_set.size()) {
                throw std::invalid_argument("第" + std::to_string(i + 1) + "行存在重复的数字。");
            }
        }
        // 检查列
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> col = getColumn(i);
            std::vector<int> nums;
            for (int num : col) {
                if (num != 0) nums.push_back(num);
            }
            std::set<int> num_set(nums.begin(), nums.end());
            if (nums.size() != num_set.size()) {
                throw std::invalid_argument("第" + std::to_string(i + 1) + "列存在重复的数字。");
            }
        }
        // 检查3x3方格
        for (int box_row = 0; box_row < GRID_SIZE; box_row += BOX_SIZE) {
            for (int box_col = 0; box_col < GRID_SIZE; box_col += BOX_SIZE) {
                std::vector<int> box = getBox(box_row, box_col);
                std::vector<int> nums;
                for (int num : box) {
                    if (num != 0) nums.push_back(num);
                }
                std::set<int> num_set(nums.begin(), nums.end());
                if (nums.size() != num_set.size()) {
                    throw std::invalid_argument("位置 (" + std::to_string(box_row + 1) + ", " + std::to_string(box_col + 1) + ") 的3x3方格存在重复的数字。");
                }
            }
        }
    }
};

class Sudoku : public Grid {
public:
    Sudoku(const std::string& grid_string) : Grid(grid_string) {}

    Sudoku(const std::vector<std::vector<int>>& grid_) : Grid(grid_) {}

    Sudoku(const Grid& grid_) : Grid(grid_) {}

    // 获取推理结果（候选值）
    std::map<std::pair<int, int>, std::vector<int>> getInference() const {
        std::map<std::pair<int, int>, std::vector<int>> candidates;
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (grid[row][col] == 0) {
                    std::vector<int> cell_candidates = get_candidates(row, col);
                    if (cell_candidates.empty()) {
                        throw std::invalid_argument("单元格 (" + std::to_string(row + 1) + ", " + std::to_string(col + 1) + ") 无可用候选值，数独无解。");
                    }
                    candidates[std::make_pair(row, col)] = cell_candidates;
                }
            }
        }
        return candidates;
    }

    // 获取指定单元格的候选值
    std::vector<int> get_candidates(int row, int col) const {
        std::set<int> used_vals;
        std::vector<int> row_vals = getRow(row);
        std::vector<int> col_vals = getColumn(col);
        std::vector<int> box_vals = getBox(row, col);
        for (int val : row_vals) {
            if (val != 0) used_vals.insert(val);
        }
        for (int val : col_vals) {
            if (val != 0) used_vals.insert(val);
        }
        for (int val : box_vals) {
            if (val != 0) used_vals.insert(val);
        }
        std::vector<int> candidates;
        for (int n = 1; n <= 9; ++n) {
            if (used_vals.find(n) == used_vals.end()) {
                candidates.push_back(n);
            }
        }
        return candidates;
    }
};

// 测试代码
void test_sudoku() {
    std::string input_string = "017903600000080000900000507072010430000402070064370250701000065000030000005601720";
    Sudoku sudoku(input_string);

    std::cout << "数独棋盘：" << std::endl;
    std::cout << sudoku << std::endl;

    std::map<std::pair<int, int>, std::vector<int>> inference;
    try {
        inference = sudoku.getInference();
    } catch (const std::exception& e) {
        std::cerr << "错误：" << e.what() << std::endl;
        return;
    }

    std::cout << "\n推理结果（候选值）：" << std::endl;
    for (std::map<std::pair<int, int>, std::vector<int>>::const_iterator it = inference.begin(); it != inference.end(); ++it) {
        const std::pair<int, int>& key = it->first;
        const std::vector<int>& value = it->second;
        std::cout << "单元格 (" << key.first + 1 << ", " << key.second + 1 << ") 的候选值: ";
        for (int val : value) {
            std::cout << val << ' ';
        }
        std::cout << std::endl;
    }

    // 克隆测试
    Sudoku cloned_sudoku = sudoku.clone();
    std::cout << "\n克隆后的数独棋盘：" << std::endl;
    std::cout << cloned_sudoku << std::endl;

    // 序列化和反序列化测试
    std::string serialized = sudoku.serialize();
    std::cout << "\n序列化的数独棋盘：" << std::endl;
    std::cout << serialized << std::endl;

    Sudoku deserialized_sudoku = Sudoku::deserialize(serialized);
    std::cout << "\n反序列化后的数独棋盘：" << std::endl;
    std::cout << deserialized_sudoku << std::endl;

    // 比较测试
    std::cout << "\n比较两个Sudoku对象是否相等：";
    std::cout << (sudoku == cloned_sudoku ? "相等" : "不相等") << std::endl;
}

int main() {
    test_sudoku();
    return 0;
}
