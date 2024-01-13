#include "cell.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <stack>
#include <string>

#include "sheet.h"

class Cell::Impl {
 public:
  Impl() = default;
  Impl(std::string_view str);
  virtual ~Impl() = default;
  virtual CellInterface::Value GetValue() = 0;
  virtual std::string GetText() const;
  virtual std::vector<Position> GetReferencedCells() const { return {}; }
  virtual bool IsCacheValid() const { return true; }
  virtual void InvalidateCache() {}

 protected:
  std::string text_;
};

class Cell::EmptyImpl : public Impl {
 public:
  EmptyImpl();
  CellInterface::Value GetValue() override;
};

class Cell::TextImpl : public Impl {
 public:
  TextImpl(std::string_view str);
  CellInterface::Value GetValue() override;

 private:
  std::string value_;
};

class Cell::FormulaImpl : public Impl {
 public:
  FormulaImpl(std::string_view str, const SheetInterface& sheet);
  CellInterface::Value GetValue() override;
  bool IsCacheValid() const override;
  void InvalidateCache() override;
  std::vector<Position> GetReferencedCells() const override;

 private:
  std::unique_ptr<FormulaInterface> formula_;
  const SheetInterface& sheet_;
  mutable std::optional<FormulaInterface::Value> cache_;
};

Cell::Impl::Impl(std::string_view str) : text_(str) {}

std::string Cell::Impl::GetText() const { return text_; }

Cell::EmptyImpl::EmptyImpl() : Impl("") {}

CellInterface::Value Cell::EmptyImpl::GetValue() { return text_; }

Cell::TextImpl::TextImpl(std::string_view str) : Impl(str), value_(str) {
  if (value_.front() == ESCAPE_SIGN) {
    value_ = value_.substr(1);
  }
}

CellInterface::Value Cell::TextImpl::GetValue() { return value_; }

Cell::FormulaImpl::FormulaImpl(std::string_view str,
                               const SheetInterface& sheet)
    : Impl(str),
      formula_(ParseFormula(std::string{str.substr(1, str.size())})),
      sheet_(sheet) {
  text_ = FORMULA_SIGN + formula_->GetExpression();
}

CellInterface::Value Cell::FormulaImpl::GetValue() {
  if (!cache_) cache_ = formula_->Evaluate(sheet_);

  auto value = formula_->Evaluate(sheet_);
  if (std::holds_alternative<double>(value)) return std::get<double>(value);

  return std::get<FormulaError>(value);
}

bool Cell::FormulaImpl::IsCacheValid() const { return cache_.has_value(); }

void Cell::FormulaImpl::InvalidateCache() { cache_.reset(); }

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
  return formula_->GetReferencedCells();
}

Cell::Cell(Sheet& sheet)
    : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
  std::unique_ptr<Impl> impl;
  if (text.empty()) {
    impl = std::make_unique<EmptyImpl>();
  } else if (text[0] == FORMULA_SIGN && text.size() > 1) {
    impl = std::make_unique<FormulaImpl>(text, sheet_);
  } else {
    impl = std::make_unique<TextImpl>(text);
  }
  if (IsCircularDependencyPossible(*impl)) {
    throw CircularDependencyException("Ошибка рекурсивной зависимости!");
  }
  impl_ = std::move(impl);
  for (Cell* outgoing : r_nodes_) {
    outgoing->l_nodes_.erase(this);
  }
  r_nodes_.clear();
  for (const Position& pos : impl_->GetReferencedCells()) {
    CellInterface* outgoing = sheet_.GetCell(pos);
    if (!outgoing) {
      sheet_.SetCell(pos, "");
      outgoing = sheet_.GetCell(pos);
    }
    r_nodes_.insert(dynamic_cast<Cell*>(outgoing));
    dynamic_cast<Cell*>(outgoing)->l_nodes_.insert(this);
  }
  InvalidateCache(true);
}

void Cell::Clear() { impl_ = std::make_unique<EmptyImpl>(); }

Cell::Value Cell::GetValue() const { return impl_->GetValue(); }
std::string Cell::GetText() const { return impl_->GetText(); }

bool Cell::IsCircularDependencyPossible(const Impl& impl) const {
  if (impl.GetReferencedCells().empty()) {
    return false;
  }
  std::unordered_set<const CellInterface*> referenced;
  for (const auto& cell : impl.GetReferencedCells()) {
    referenced.insert(sheet_.GetCell(cell));
  }
  std::unordered_set<const CellInterface*> visited;
  std::stack<const CellInterface*> to_visit;
  to_visit.push(this);
  while (!to_visit.empty()) {
    const CellInterface* current = to_visit.top();
    to_visit.pop();
    visited.insert(current);
    if (referenced.find(current) != referenced.end()) {
      return true;
    }
    for (const CellInterface* incoming :
         dynamic_cast<const Cell*>(current)->l_nodes_) {
      if (visited.find(incoming) == visited.end()) {
        to_visit.push(incoming);
      }
    }
  }
  return false;
}

void Cell::InvalidateCache(bool force) {
  if (impl_->IsCacheValid() || force) {
    impl_->InvalidateCache();
    for (Cell* incoming : l_nodes_) {
      incoming->InvalidateCache();
    }
  }
}

std::vector<Position> Cell::GetReferencedCells() const {
  return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !l_nodes_.empty();
}