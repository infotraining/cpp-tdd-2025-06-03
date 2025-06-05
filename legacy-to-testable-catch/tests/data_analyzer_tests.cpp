#include <catch2/catch_test_macros.hpp>
#include <data_analyzer.hpp>
#include <sstream>

using namespace std;

std::string get_file_contents(const std::string& file_name)
{
    std::ifstream in{file_name};
    if (!in)
        throw std::runtime_error("File not opened!!!");

    std::string contents;
    std::string line;
    while (std::getline(in, line))
    {
        contents += line + "\n";
    }

    return contents;
}

TEST_CASE("DataAnalyzer - calculate stats", "[Integration]")
{
    DataAnalyzer data_analyzer(Statistics::avg);
    data_analyzer.load_data("data.dat");
    data_analyzer.calculate();
    data_analyzer.set_statistics(Statistics::min_max);
    data_analyzer.calculate();
    data_analyzer.set_statistics(Statistics::sum);
    data_analyzer.calculate();
    data_analyzer.save_results("results.txt");

    std::string expected_results = "Avg = 47.15\nMin = 1\nMax = 99\nSum = 4715\n";

    REQUIRE(get_file_contents("results.txt") == expected_results);
}

TEST_CASE("DataAnalyzer - test reading")
{
    auto testable_reader = [](const std::string& file_name) { 
        return Data{1, 2, 3, 4, 5};
    };

    Ver_1::DataAnalyzer data_analyzer(Statistics::sum, testable_reader);
    data_analyzer.calculate();

    REQUIRE(data_analyzer.results() == Results{{"Sum", 15.0}});
}

TEST_CASE("Avg")
{
    Data data = {1, 2, 3, 4, 5};

    Ver_2::Avg avg;
    REQUIRE(avg.calculate(data) == Results{{"Avg", 3.0}});
}