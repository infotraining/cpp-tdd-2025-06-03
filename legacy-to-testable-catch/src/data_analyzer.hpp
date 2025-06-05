#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

struct StatResult
{
    std::string description;
    double value;

    StatResult(const std::string& desc, double val)
        : description(desc)
        , value(val)
    {
    }

    bool operator==(const StatResult& other) const
    {
        return description == other.description && value == other.value;
    }
};

using Data = std::vector<double>;
using Results = std::vector<StatResult>;

std::ostream& operator<<(std::ostream& out, const Results& r)
{
    out << "(";
    for (const auto& item : r)
    {
        out << "(" << item.description << " - " << item.value << ")";
    }

    return out;
}

namespace LegacyCode
{
    enum Statistics
    {
        avg,
        min_max,
        sum
    };

    class DataAnalyzer
    {
        Statistics stat_type_;
        Data data_;
        Results results_;

    public:
        DataAnalyzer(Statistics stat_type)
            : stat_type_{stat_type}
        {
        }

        void load_data(const std::string& file_name)
        {
            data_.clear();
            results_.clear();

            std::ifstream fin(file_name.c_str());
            if (!fin)
                throw std::runtime_error("File not opened!!!");

            double d;
            while (fin >> d)
            {
                data_.push_back(d);
            }

            std::cout << "File " << file_name << " has been loaded...\n";
        }

        void set_statistics(Statistics stat_type)
        {
            stat_type_ = stat_type;
        }

        void calculate()
        {
            if (stat_type_ == avg)
            {
                double sum = std::accumulate(data_.begin(), data_.end(), 0.0);
                double avg = sum / data_.size();

                StatResult result("Avg", avg);
                results_.push_back(result);
            }
            else if (stat_type_ == min_max)
            {
                double min = *(std::min_element(data_.begin(), data_.end()));
                double max = *(std::max_element(data_.begin(), data_.end()));

                results_.push_back(StatResult("Min", min));
                results_.push_back(StatResult("Max", max));
            }
            else if (stat_type_ == sum)
            {
                double sum = std::accumulate(data_.begin(), data_.end(), 0.0);

                results_.push_back(StatResult("Sum", sum));
            }
        }

        const Results& results() const
        {
            return results_;
        }

        void save_results(const std::string& file_name)
        {
            std::ofstream out{file_name};

            if (!out)
                throw std::runtime_error("File not opened!!!");

            for (const auto& rslt : results_)
                out << rslt.description << " = " << rslt.value << std::endl;
        }
    };
} // namespace LegacyCode

inline namespace Ver_1
{
    enum Statistics
    {
        avg,
        min_max,
        sum
    };

    namespace ClassicCpp
    {
        class DataReader
        {
        public:
            virtual Data read_data(const std::string&) = 0;
            virtual ~DataReader() = default;
        };

        class TextDataReader : public DataReader
        {
        public:
            Data read_data(const std::string& file_name)
            {
                Data data;

                std::ifstream fin(file_name.c_str());
                if (!fin)
                    throw std::runtime_error("File not opened!!!");

                double d;
                while (fin >> d)
                {
                    data.push_back(d);
                }

                return data;
            }
        };
    }

    using DataReader = std::function<Data(const std::string&)>;
    using DataWriter = std::function<void(const std::string&, const Results&)>;

    Data text_reader(const std::string& file_name)
    {
        Data data;

        std::ifstream fin(file_name.c_str());
        if (!fin)
            throw std::runtime_error("File not opened!!!");

        double d;
        while (fin >> d)
        {
            data.push_back(d);
        }

        return data;
    }

    auto text_writer = [](const std::string& file_name, const Results& results)
    {
        std::ofstream out{file_name};

        if (!out)
            throw std::runtime_error("File not opened!!!");

        for (const auto& rslt : results)
            out << rslt.description << " = " << rslt.value << std::endl;
    };

    class DataAnalyzer
    {
        Statistics stat_type_;
        Data data_;
        Results results_;
        DataReader reader_;
        DataWriter writer_;

    public:
        DataAnalyzer(Statistics stat_type, DataReader reader = text_reader, DataWriter writer = text_writer)
            : stat_type_{stat_type}
            , reader_{reader}
            , writer_{writer}
        {
        }

        void load_data(const std::string& file_name)
        {
            data_.clear();
            results_.clear();

            data_ = reader_(file_name);

            std::cout << "File " << file_name << " has been loaded...\n";
        }

        void set_statistics(Statistics stat_type)
        {
            stat_type_ = stat_type;
        }

        void calculate()
        {
            if (stat_type_ == avg)
            {
                double sum = std::accumulate(data_.begin(), data_.end(), 0.0);
                double avg = sum / data_.size();

                StatResult result("Avg", avg);
                results_.push_back(result);
            }
            else if (stat_type_ == min_max)
            {
                double min = *(std::min_element(data_.begin(), data_.end()));
                double max = *(std::max_element(data_.begin(), data_.end()));

                results_.push_back(StatResult("Min", min));
                results_.push_back(StatResult("Max", max));
            }
            else if (stat_type_ == sum)
            {
                double sum = std::accumulate(data_.begin(), data_.end(), 0.0);

                results_.push_back(StatResult("Sum", sum));
            }
        }

        const Results& results() const
        {
            return results_;
        }

        void save_results(const std::string& file_name)
        {
            writer_(file_name, results_);
        }
    };
} // namespace LegacyCode

namespace Ver_2
{
    class IStatistics
    {
    public:
        virtual Results calculate(const Data& data) = 0;
        virtual ~IStatistics() = default;
    };

    class Avg : public IStatistics
    {
    public:
        Results calculate(const Data& data) override
        {
            Results results;
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double avg = sum / data.size();

            StatResult result("Avg", avg);
            results.push_back(result);

            return results;
        }
    };

    class MinMax : public IStatistics
    {
    public:
        Results calculate(const Data& data) override
        {
            Results results;
            double min = *(std::min_element(data.begin(), data.end()));
            double max = *(std::max_element(data.begin(), data.end()));

            results.push_back(StatResult("Min", min));
            results.push_back(StatResult("Max", max));
            return results;
        }
    };

    class Sum : public IStatistics
    {
    public:
        Results calculate(const Data& data) override
        {
            Results results;
            double sum = std::accumulate(data.begin(), data.end(), 0.0);

            results.push_back(StatResult("Sum", sum));

            return results;
        }
    };

    namespace Statistics
    {
        auto avg = std::make_shared<Avg>();
        auto min_max = std::make_shared<MinMax>();
        auto sum = std::make_shared<Sum>();
    }

    using DataReader = std::function<Data(const std::string&)>;
    using DataWriter = std::function<void(const std::string&, const Results&)>;

    Data text_reader(const std::string& file_name)
    {
        Data data;

        std::ifstream fin(file_name.c_str());
        if (!fin)
            throw std::runtime_error("File not opened!!!");

        double d;
        while (fin >> d)
        {
            data.push_back(d);
        }

        return data;
    }

    auto text_writer = [](const std::string& file_name, const Results& results)
    {
        std::ofstream out{file_name};

        if (!out)
            throw std::runtime_error("File not opened!!!");

        for (const auto& rslt : results)
            out << rslt.description << " = " << rslt.value << std::endl;
    };

    class DataAnalyzer
    {
        std::shared_ptr<IStatistics> stat_type_;
        Data data_;
        Results results_;
        DataReader reader_;
        DataWriter writer_;

    public:
        DataAnalyzer(std::shared_ptr<IStatistics> stat_type, DataReader reader = text_reader, DataWriter writer = text_writer)
            : stat_type_{stat_type}
            , reader_{reader}
            , writer_{writer}
        {
        }

        void load_data(const std::string& file_name)
        {
            data_.clear();
            results_.clear();

            data_ = reader_(file_name);

            std::cout << "File " << file_name << " has been loaded...\n";
        }

        void set_statistics(std::shared_ptr<IStatistics> stat_type)
        {
            stat_type_ = stat_type;
        }

        void calculate()
        {
            Results current_results = stat_type_->calculate(data_);

            results_.insert(results_.end(), current_results.begin(), current_results.end());
        }

        const Results& results() const
        {
            return results_;
        }

        void save_results(const std::string& file_name)
        {
            writer_(file_name, results_);
        }
    };
}

#endif // SOURCE_HPP