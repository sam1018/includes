#include <stack>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void split_line(const string& input, int& space_count, string& output)
{
	space_count = 0;

	while (input.size() != space_count && input[space_count] == ' ')
		++space_count;

	output = input.substr(space_count);
}

template< typename ... Args >
std::string stringer(Args const& ... args)
{
	std::ostringstream stream;
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0) ...
	};

	return stream.str();
}

class outputter
{
	ofstream outfile;

public:
	outputter(const string& outfilename) : outfile(outfilename)
	{
		outfile << "<root>\n";
	}

	~outputter()
	{
		outfile << "</root>\n";
	}

	void print_opening(const string& name, const string& src, bool has_child)
	{
		auto end = has_child ? ">" : "/>";
		outfile << "<" << name << " src=\"" << src << "\"" << end << "\n";
	}

	void print_closing(const string& name)
	{
		outfile << "</" << name << ">\n";
	}
};

class output
{
	outputter out;
	stack<string> outstack;
	string lastline;

	void pop_lines(int count)
	{
		for (int i = 0; i < count; ++i)
		{
			out.print_closing(outstack.top());
			outstack.pop();
		}
	}

	void process_lastline(const string& _lastline, const string& _currentline)
	{
		auto lastline = string{};
		auto lastdepth = int{};
		split_line(_lastline, lastdepth, lastline);
		auto lastname = stringer("depth", lastdepth);

		auto currentline = string{};
		auto currentdepth = int{};
		split_line(_currentline, currentdepth, currentline);

		if (currentdepth > lastdepth)
		{
			out.print_opening(lastname, lastline, true);
			outstack.push(lastname);
		}
		else if (currentdepth < lastdepth)
		{
			out.print_opening(lastname, lastline, false);
			pop_lines(lastdepth - currentdepth);
		}
		else
			out.print_opening(lastname, lastline, false);
	}

public:
	output(const string& outfilename) : out(outfilename)
	{
	}

	~output()
	{
		pop_lines(outstack.size());
	}

	void add(const string& line)
	{
		if(lastline != "")
			process_lastline(lastline, line);

		lastline = line;
	}
};

int main()
{
	auto infile = ifstream("input.txt");
	auto line = string{};
	auto checker1 = string("Note: including file: ");
	auto checker2 = string(".cpp");
	output out("output.xml");

	while (getline(infile, line))
	{
		auto ind1 = line.find(checker1);
		auto ind2 = line.rfind(checker2);

		if (ind1 != string::npos)
			out.add(" " + line.substr(ind1 + checker1.size()));
		else if (ind2 != string::npos && ind2 + checker2.size() == line.size())
			out.add(line.substr(line.find_last_of(' ') + 1));
	}
}
