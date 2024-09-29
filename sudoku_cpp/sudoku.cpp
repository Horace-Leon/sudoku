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

    // ���캯�����ڷ����л�
    Grid(const std::vector<std::vector<int>>& grid_) : grid(grid_) {
        validate_grid();
    }

    // ��¡���캯��
    Grid(const Grid& other) : grid(other.grid) {}

    // ��ȡָ����
    std::vector<int> getRow(int row) const {
        if (!(0 <= row && row < GRID_SIZE)) {
            throw std::out_of_range("������������Χ��");
        }
        return grid[row];
    }

    // ��ȡָ����
    std::vector<int> getColumn(int col) const {
        if (!(0 <= col && col < GRID_SIZE)) {
            throw std::out_of_range("������������Χ��");
        }
        std::vector<int> column(GRID_SIZE);
        for (int row = 0; row < GRID_SIZE; ++row) {
            column[row] = grid[row][col];
        }
        return column;
    }

    // ��ȡָ��λ�õ�3x3����
    std::vector<int> getBox(int row, int col) const {
        if (!(0 <= row && row < GRID_SIZE && 0 <= col && col < GRID_SIZE)) {
            throw std::out_of_range("�л�������������Χ��");
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

    // ��¡����
    Grid clone() const {
        return Grid(*this);
    }

    // ���л�
    std::string serialize() const {
        std::ostringstream oss;
        for (const auto& row : grid) {
            for (const auto& num : row) {
                oss << num;
            }
        }
        return oss.str();
    }

    // �����л�
    static Grid deserialize(const std::string& grid_string) {
        return Grid(grid_string);
    }

    // ������������
    friend std::ostream& operator<<(std::ostream& os, const Grid& grid) {
        for (const auto& row : grid.grid) {
            for (const auto& num : row) {
                os << num << ' ';
            }
            os << '\n';
        }
        return os;
    }

    // ������������
    bool operator==(const Grid& other) const {
        return grid == other.grid;
    }

protected:
    std::vector<std::vector<int>> grid;

    // ���������ַ���
    std::vector<std::vector<int>> parse_grid(const std::string& grid_string) {
        if (grid_string.length() != GRID_SIZE * GRID_SIZE) {
            throw std::invalid_argument("�����ַ������ȱ���Ϊ81λ��");
        }
        if (!std::all_of(grid_string.begin(), grid_string.end(), ::isdigit)) {
            throw std::invalid_argument("�����ַ���������������֡�");
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

    // ��֤�����Ƿ�Ϸ�
    void validate_grid() {
        // �����
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> row = getRow(i);
            std::vector<int> nums;
            for (int num : row) {
                if (num != 0) nums.push_back(num);
            }
            std::set<int> num_set(nums.begin(), nums.end());
            if (nums.size() != num_set.size()) {
                throw std::invalid_argument("��" + std::to_string(i + 1) + "�д����ظ������֡�");
            }
        }
        // �����
        for (int i = 0; i < GRID_SIZE; ++i) {
            std::vector<int> col = getColumn(i);
            std::vector<int> nums;
            for (int num : col) {
                if (num != 0) nums.push_back(num);
            }
            std::set<int> num_set(nums.begin(), nums.end());
            if (nums.size() != num_set.size()) {
                throw std::invalid_argument("��" + std::to_string(i + 1) + "�д����ظ������֡�");
            }
        }
        // ���3x3����
        for (int box_row = 0; box_row < GRID_SIZE; box_row += BOX_SIZE) {
            for (int box_col = 0; box_col < GRID_SIZE; box_col += BOX_SIZE) {
                std::vector<int> box = getBox(box_row, box_col);
                std::vector<int> nums;
                for (int num : box) {
                    if (num != 0) nums.push_back(num);
                }
                std::set<int> num_set(nums.begin(), nums.end());
                if (nums.size() != num_set.size()) {
                    throw std::invalid_argument("λ�� (" + std::to_string(box_row + 1) + ", " + std::to_string(box_col + 1) + ") ��3x3��������ظ������֡�");
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

    // ��ȡ����������ѡֵ��
    std::map<std::pair<int, int>, std::vector<int>> getInference() const {
        std::map<std::pair<int, int>, std::vector<int>> candidates;
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (grid[row][col] == 0) {
                    std::vector<int> cell_candidates = get_candidates(row, col);
                    if (cell_candidates.empty()) {
                        throw std::invalid_argument("��Ԫ�� (" + std::to_string(row + 1) + ", " + std::to_string(col + 1) + ") �޿��ú�ѡֵ�������޽⡣");
                    }
                    candidates[std::make_pair(row, col)] = cell_candidates;
                }
            }
        }
        return candidates;
    }

    // ��ȡָ����Ԫ��ĺ�ѡֵ
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

// ���Դ���
void test_sudoku() {
    std::string input_string = "017903600000080000900000507072010430000402070064370250701000065000030000005601720";
    Sudoku sudoku(input_string);

    std::cout << "�������̣�" << std::endl;
    std::cout << sudoku << std::endl;

    std::map<std::pair<int, int>, std::vector<int>> inference;
    try {
        inference = sudoku.getInference();
    } catch (const std::exception& e) {
        std::cerr << "����" << e.what() << std::endl;
        return;
    }

    std::cout << "\n����������ѡֵ����" << std::endl;
    for (std::map<std::pair<int, int>, std::vector<int>>::const_iterator it = inference.begin(); it != inference.end(); ++it) {
        const std::pair<int, int>& key = it->first;
        const std::vector<int>& value = it->second;
        std::cout << "��Ԫ�� (" << key.first + 1 << ", " << key.second + 1 << ") �ĺ�ѡֵ: ";
        for (int val : value) {
            std::cout << val << ' ';
        }
        std::cout << std::endl;
    }

    // ��¡����
    Sudoku cloned_sudoku = sudoku.clone();
    std::cout << "\n��¡����������̣�" << std::endl;
    std::cout << cloned_sudoku << std::endl;

    // ���л��ͷ����л�����
    std::string serialized = sudoku.serialize();
    std::cout << "\n���л����������̣�" << std::endl;
    std::cout << serialized << std::endl;

    Sudoku deserialized_sudoku = Sudoku::deserialize(serialized);
    std::cout << "\n�����л�����������̣�" << std::endl;
    std::cout << deserialized_sudoku << std::endl;

    // �Ƚϲ���
    std::cout << "\n�Ƚ�����Sudoku�����Ƿ���ȣ�";
    std::cout << (sudoku == cloned_sudoku ? "���" : "�����") << std::endl;
}

int main() {
    test_sudoku();
    return 0;
}
