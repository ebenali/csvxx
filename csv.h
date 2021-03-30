#pragma once

/**
 * C++17 CSV reader
 *  - Supports 'put-back' thanks to original application (one-shot csv ingestion script)
 *  - Returned rows may not outlive Reader object
 */

#include <cstring>
#include <istream>
#include <stdexcept>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace csv
{
using decomposed_line_t = std::vector<std::string>;

class Row {
    public:
	Row() = default;
	Row(decomposed_line_t row, decomposed_line_t const &hdr) : row_{ std::move(row) }, hdr_{ &hdr }
	{
	}
	const std::string &operator[](std::string const &at) const
	{
		auto hdr_at = find(hdr_->begin(), hdr_->end(), at);
		auto const idx = hdr_at - hdr_->begin();
		return row_[idx];
	}

	const std::string &at(std::string const &at) const
	{
		auto hdr_at = find(hdr_->begin(), hdr_->end(), at);
		if (hdr_at == hdr_->end())
			throw std::out_of_range{ std::string{ "csv row has no such col=" } + at };
		auto const idx = hdr_at - hdr_->begin();
		return row_[idx];
	}

	const std::string &operator[](off_t idx) const
	{
		return row_[idx];
	}

	const std::string &at(off_t idx) const
	{
		return row_.at(idx);
	}

	operator bool() const
	{
		return hdr_ != nullptr;
	}

	decomposed_line_t &line()
	{
		return row_;
	}

	decomposed_line_t const &line() const
	{
		return row_;
	}

	std::string str() const
	{
		std::string retv;
		return inner_product(hdr_->cbegin(), hdr_->cend(), row_.cbegin(), std::string{},
				     std::plus<std::string>{},
				     [](auto const &hdr, auto const &col) { return hdr + "=" + col + ","; });
	}

    private:
	decomposed_line_t row_;
	decomposed_line_t const *hdr_ = nullptr;
};

class Reader {
    private:
	std::istream *ist_ = nullptr;
	decomposed_line_t header_line_;
	decomposed_line_t saved_row_;

	std::pair<decomposed_line_t, bool> get_line()
	{
		std::string line;
		if (!ist_ || !std::getline((*ist_), line)) {
			return std::make_pair(decomposed_line_t{}, false);
		}
		decomposed_line_t retv_line;
		for (char *tok = const_cast<char *>(line.c_str()); tok = strtok(tok, ","); tok = nullptr)
			retv_line.push_back(tok);
		return std::make_pair(std::move(retv_line), true);
	}

    public:
	Reader(std::istream &ist) : ist_{ &ist }
	{
		if (!ist_)
			throw std::runtime_error{ "ist === nullptr" };

		auto [hdr_line, ok] = get_line();
		if (!ok) {
			throw std::runtime_error{ "can't read hdr" };
		}

		header_line_ = std::move(hdr_line);
	}

	~Reader() = default;
	Reader() = default;

	decomposed_line_t const &header() const
	{
		return header_line_;
	}

	std::string const &column_name(off_t at) const
	{
		return header_line_[at];
	}

	off_t column_index(std::string const &col_name) const
	{
		auto hdr_at = find(header_line_.begin(), header_line_.end(), col_name);
		return hdr_at == header_line_.end() ? -1 : hdr_at - header_line_.begin();
	}

	std::pair<Row, bool> get_row()
	{
		if (!saved_row_.empty())
			return { Row{ std::move(saved_row_), header_line_ }, true };
		auto [l, ok] = get_line();
		if (!ok)
			return { Row{}, false };
		return { Row{ std::move(l), header_line_ }, true };
	}

	void put_back(decomposed_line_t line)
	{
		saved_row_ = std::move(line);
	}
};
} // namespace csv
