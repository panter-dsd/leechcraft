/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2012  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "fdodesktopparser.h"
#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted.hpp>
#include <QtDebug>

namespace LeechCraft
{
namespace Launchy
{
	namespace
	{
		typedef boost::variant<std::string, std::vector<std::string>> FieldVal_t;
		typedef boost::optional<std::string> Lang_t;
		struct Field
		{
			std::string Name_;
			Lang_t Lang_;
			FieldVal_t Val_;
		};
		typedef std::vector<Field> Fields_t;

		struct Item
		{
			Fields_t Fields_;
		};
	}
}
}

BOOST_FUSION_ADAPT_STRUCT (LeechCraft::Launchy::Field,
		(std::string, Name_)
		(LeechCraft::Launchy::Lang_t, Lang_)
		(LeechCraft::Launchy::FieldVal_t, Val_));

BOOST_FUSION_ADAPT_STRUCT (LeechCraft::Launchy::Item,
		(LeechCraft::Launchy::Fields_t, Fields_));

namespace LeechCraft
{
namespace Launchy
{
	namespace
	{
		namespace ascii = boost::spirit::ascii;
		namespace qi = boost::spirit::qi;
		namespace phoenix = boost::phoenix;

		template<typename Iter>
		struct Parser : qi::grammar<Iter, Item ()>
		{
			qi::rule<Iter, Item ()> Start_;
			qi::rule<Iter, std::string ()> Lang_;
			qi::rule<Iter, void ()> KeyValSep_;
			qi::rule<Iter, std::string ()> LineValSingle_;
			qi::rule<Iter, FieldVal_t ()> LineVal_;
			qi::rule<Iter, Field ()> Line_;
			qi::rule<Iter, void ()> Comment_;

			Parser ()
			: Parser::base_type (Start_)
			{
				auto eol = qi::lit ("\n");
				Comment_ %= qi::lit ("#") >> *(qi::char_) >> eol;

				Lang_ %= '[' >> qi::lexeme[+(qi::char_ ("a-zA-Z0-9"))] >> ']';

				KeyValSep_ %= *(qi::lit (' ')) >> '=' >> *(qi::lit (' '));

				LineValSingle_ %= qi::lexeme[+((qi::lit ("\\;") | (qi::char_ - ';' - '\r' - '\n')))];
				LineVal_ %= LineValSingle_ | *(LineValSingle_ >> ';');

				Line_ %= qi::lexeme[+(qi::char_ ("a-zA-Z0-9"))] >>
						-Lang_ >>
						KeyValSep_ >>
						LineVal_ >>
						eol;

				Start_ %= *Comment_ >>
						qi::lit ("[Desktop Entry]") >> eol >>
						*(Comment_ | Line_);
				qi::on_error<qi::fail> (Start_,
						std::cout << phoenix::val ("Error! Expecting") << qi::_4
								<< phoenix::val (" here: \"") << phoenix::construct<std::string> (qi::_3, qi::_2)
								<< phoenix::val ("\"") << std::endl);
			}
		};

		template<typename Iter>
		Item Parse (Iter begin, Iter end)
		{
			Item res;
			qDebug () << qi::parse (begin, end, Parser<Iter> (), res);
			return res;
		}

		QString ToUtf8 (const std::string& str)
		{
			return QString::fromUtf8 (str.c_str ());
		}
	}

	FDODesktopParser::Result_t FDODesktopParser::operator() (const QByteArray& data)
	{
		const auto& item = Parse (data.begin (), data.end ());

		Result_t result;
		qDebug () << data;
		qDebug () << Q_FUNC_INFO << item.Fields_.size ();
		for (const auto& field : item.Fields_)
		{
			qDebug () << ToUtf8 (field.Name_) << (field.Lang_ ? ToUtf8 (*field.Lang_) : "no lang") << field.Val_.which ();
			//result [ToUtf8 (field.Name_)];
		}
		return result;
	}
}
}