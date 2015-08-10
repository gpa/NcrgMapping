#pragma once

#include <string>
#include <regex>

namespace ncrg
{
	template <typename T = char>
	class NcrgMapping
	{
		static constexpr char removal_placeholder = '\xf6';
		using string = std::basic_string<T>;
		using exprIter = typename string::iterator;
		using keyValueContainer = typename std::vector<std::pair<string, string>>;

	public:
		NcrgMapping(string ncrgRegex);

		keyValueContainer getValuesForMatch(const std::smatch &) const;
		inline const std::regex &getStdRegex() const { return m_regex; };

	private:
		exprIter parseCapturingGroups(exprIter begin, exprIter end);
		void registerCapturingGroup(exprIter begin, exprIter end);

		std::vector<string> m_groups;
		std::regex m_regex;
	};

	template <typename T>
	NcrgMapping<T>::NcrgMapping(string expr)
	{
		auto iter = expr.begin();
		while ((iter = parseCapturingGroups(iter, expr.end())) != expr.end())
		{
		}

		expr.erase(std::remove(expr.begin(), expr.end(), 
			NcrgMapping::removal_placeholder), expr.end());

		try
		{
			m_regex.assign(expr);
		}
		catch (const std::regex_error& e)
		{
			m_groups.clear();
			throw std::invalid_argument(e.what());
		}
	}

	template <typename T>
	typename NcrgMapping<T>::exprIter
	NcrgMapping<T>::parseCapturingGroups(exprIter begin, exprIter end)
	{
		if (*begin != '(')
			begin = std::find(begin, end, '(');

		if (begin != end)
			++begin;

		for (auto iter = begin; iter != end; ++iter)
		{
			if (*iter == '(')
			{
				iter = parseCapturingGroups(iter, end);
			}
			else if (*iter == ')')
			{
				registerCapturingGroup(begin, iter);
				return iter;
			}
		}

		return end;
	}

	template <typename T>
	void NcrgMapping<T>::registerCapturingGroup(exprIter begin, exprIter end)
	{
		auto nameBegin = *begin == '<' ? begin + 1 : end;
		auto nameEnd = std::find(nameBegin, end, '>');

		if (nameEnd == end)
		{
			m_groups.emplace_back();
			return;
		}

		auto isNameValid = std::find_if(nameBegin, nameEnd, [](char ch) {
			return isalnum(ch) || ch == '_';
			}) != nameEnd;

		if (!isNameValid)
		{
			m_groups.emplace_back();
			return;
		}

		m_groups.emplace_back(nameBegin, nameEnd);
		std::fill(nameBegin - 1, nameEnd + 1, NcrgMapping::removal_placeholder);
	}

	template <typename T>
	typename NcrgMapping<T>::keyValueContainer
	NcrgMapping<T>::getValuesForMatch(const std::smatch &results) const
	{
		if (m_groups.empty())
			return {};

		keyValueContainer container;
		for (unsigned i = 1; i < results.size(); ++i)
		{
			unsigned n = i - 1;
			bool wanted = !m_groups[n].empty();
			if (wanted && results[i].matched)
			{
				container.push_back(std::make_pair(m_groups[n], results[i].str()));
			}
		}

		return container;
	}
}