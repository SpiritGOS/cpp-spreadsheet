#pragma once

#include <functional>
#include <ostream>
#include <sstream>

#include "cell.h"
#include "common.h"

class Sheet : public SheetInterface {
 public:
  using Table = std::vector<std::vector<std::unique_ptr<Cell>>>;

  ~Sheet();

  void SetCell(Position pos, std::string text) override;

  const CellInterface* GetCell(Position pos) const override;
  CellInterface* GetCell(Position pos) override;

  void ClearCell(Position pos) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

 private:
  bool CheckPosition(Position pos) const;
  template <typename Function>
  void Print(std::ostream& output, Function func) const;
  Table table_;
};

template <typename Function>
inline void Sheet::Print(std::ostream& output, Function func) const {
  using namespace std::literals;
  Size table_size = GetPrintableSize();
  for (int row_index = 0; row_index < table_size.rows; ++row_index) {
    bool first_col = true;
    for (int col_index = 0; col_index < table_size.cols; ++col_index) {
      if (!first_col) {
        output << "\t"s;
      } else {
        first_col = false;
      }
      if (col_index < (int)table_[row_index].size() && table_[row_index][col_index]) {
        output << func(table_[row_index][col_index]);
      } else {
        output << ""s;
      }
    }
    output << std::endl;
  }
}
