#include "sheet.h"

#include <algorithm>
#include <functional>
#include <optional>

#include "cell.h"
#include "common.h"

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("Invalid position");
  }
  // Расширение таблицы, если таблица недостаточного размера
  if ((int)(table_.size()) <= pos.row) {
    table_.resize(pos.row + 1);
  }
  if ((int)(table_[pos.row].size()) <= pos.col) {
    table_[pos.row].resize(pos.col + 1);
  }
  // Проверка выделена ли память под ячейку
  if (!table_[pos.row][pos.col]) {
    table_[pos.row][pos.col] = std::make_unique<Cell>(*this);
  }
  table_[pos.row][pos.col]->Set(text);
}

const CellInterface* Sheet::GetCell(Position pos) const {
  if (!pos.IsValid()) {
    throw InvalidPositionException("Invalid position");
  }
  if (!CheckPosition(pos)) {
    return nullptr;
  }
  return table_[pos.row][pos.col].get();
}
CellInterface* Sheet::GetCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("Invalid position");
  }
  if (!CheckPosition(pos)) {
    return nullptr;
  }
  return table_[pos.row][pos.col].get();
}

void Sheet::ClearCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("Invalid position");
  }
  if (!CheckPosition(pos)) {
    return;
  }
  table_[pos.row][pos.col].reset();
}

Size Sheet::GetPrintableSize() const {
  int max_row = 0;
  int max_col = 0;
  for (int row_index = 0; row_index < (int)table_.size(); ++row_index) {
    for (int col_index = 0; col_index < (int)table_[row_index].size();
         ++col_index) {
      if (table_[row_index][col_index] &&
          !(table_[row_index][col_index]->GetText().empty())) {
        max_row = row_index + 1 > max_row ? row_index + 1 : max_row;
        max_col = col_index + 1 > max_col ? col_index + 1 : max_col;
      }
    }
  }
  return Size{max_row, max_col};
}

void Sheet::PrintValues(std::ostream& output) const {
  Print(output, [](const std::unique_ptr<Cell>& cell_ptr) {
    std::stringstream ss;
    if (std::holds_alternative<std::string>(cell_ptr->GetValue())) {
      ss << std::get<std::string>(cell_ptr->GetValue());
    } else if (std::holds_alternative<double>(cell_ptr->GetValue())) {
      ss << std::get<double>(cell_ptr->GetValue());
    } else {
      ss << "#ARITHM!"s;
    }
    return ss.str();
  });
}
void Sheet::PrintTexts(std::ostream& output) const {
  Print(output, [](const std::unique_ptr<Cell>& cell_ptr) {
    return cell_ptr->GetText();
  });
}

bool Sheet::CheckPosition(Position pos) const {
  return (int)table_.size() > 0 && pos.row < (int)table_.size() &&
         pos.col < (int)table_[pos.row].size();
}

std::unique_ptr<SheetInterface> CreateSheet() {
  return std::make_unique<Sheet>();
}