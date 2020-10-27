#include <iostream>
#include <time.h>
#include <numeric>
#include <functional>
#include "Header.hpp"



using namespace CSVmanager;

size_t int_pow(size_t const& x, size_t const& y) {
    return static_cast<size_t>(pow(x, y));
}

int rand_range(int const& min, int const& max) {
    return min + (rand() % (max - min + 1));
}

double rand_range(double const& min, double const& max) {
    if (min < INT16_MIN or max > INT16_MAX) {
        throw std::overflow_error("arg overflow");
    }
    return rand_range(min, max) + (rand() % (101)) / 100.0;
}

static double operator * 
    (std::vector<double> const& a, std::vector<double> const& b) 
{
    if (a.size() != b.size()) {
        throw std::exception("invalid args");
    }
    double result = 0;
    for (size_t i = 0; i < a.size(); i++)
    {
        result += a[i] * b[i];
    }
    return result;
}

static std::vector<double> operator - 
    (std::vector<double> const& a, std::vector<double> const& b) {
    if (a.size() != b.size()) {
        throw std::exception("invalid args");
    }
    std::vector<double> result;
    for (size_t i = 0; i < a.size(); i++)
    {
        result.push_back(a[i] - b[i]);
    }
    return result;

}


typedef std::vector<std::vector<double>> matrix;

class PCA {
private:
    
    matrix data;
    size_t col_num;
    size_t row_num;
    matrix components;
    std::vector<double> weights;

public:
    PCA(data_set data_, std::vector<std::string>& names_) : 
        col_num(names_.size()), 
        row_num(data_.cbegin()->second.size()) 
    {
        for (size_t i = 0; i < col_num; ++i) {
            data.push_back(data_.at(names_[i]));
            components.push_back(std::vector<double>());
        }

    }

    void compress() 
    {
        for (size_t k = 0; k < col_num; k++)
        {
            auto lim = int_pow(10, k);
            for (size_t i = 0; i < lim; i++)
            {
                for (size_t j = 0; j < row_num; j++)
                {
                    // if size < k then push_back
                    components[j][k] = calcualte_comp(
                        take_sample(j)
                    );
                    //calc weights
                    normalize_weights();
                }
            }
        }

    }

private:

    double calcualte_comp(std::vector<double> const& sample) {
        return sample * weights;
    }

    std::vector<double> take_sample(size_t const& index) {
        std::vector<double> result;
        for (auto& vec : data) {
            result.push_back(vec[index]);
        }
        return result;
    }
    

    void normalize_weights() {
        auto square_sum = [&](double x, double y) {return pow(x + y, 2); };
        auto norm = sqrt(
            std::accumulate(weights.cbegin(), weights.cend(), 0.0, square_sum)
        );
        auto normalize = [&norm](double x) {return x / norm; };
        std::transform(weights.begin(), weights.end(), weights.begin(), normalize);
    }

    void init_weight() {
        srand(time(0));
        
        for (size_t j = 0; j < col_num; ++j)
        {
            double val = rand_range(-1.0, 1.0);
            weights.push_back(val);
        }       
        normalize_weights();
    }

    void encode() {        
        for (auto& vec : data) {
            //auto& vec = it.second;
            auto x_max = *std::max_element(vec.cbegin(), vec.cend());
            auto x_min = *std::min_element(vec.cbegin(), vec.cend());
            auto mean = std::accumulate(vec.cbegin(), vec.cend(), 0.0) / row_num;
            for (auto& x : vec) {
                x = 2 * (x - x_min) / (x_max - x_min) - 1;
            }
        }

        for (auto& vec : data) {
            //auto& vec = it.second;
            auto mean = std::accumulate(vec.cbegin(), vec.cend(), 0.0) / row_num;
            for (auto& x : vec) {
                x -= mean;
            }
        }
    }

};

int main(void) {
    
	std::string path = "D:/Me/многа букаф/zmeya/DataMining/wine.data";
    std::vector<std::string> names = {
        "Cultivar",
        "Alcohol",
        "Malic acid",
        "Ash",
        "Alcalinity of ash",
        "Magnesium",
        "Total phenols",
        "Flavanoids",
        "Nonflavanoid phenols",
        "Proanthocyanins",
        "Color intensity",
        "Hue",
        "OD280/OD315 of diluted wines",
        "Proline"
    };
    reader read(path, names);
    auto data = read.read();
    auto culti = data.at("Cultivar");
    data.erase("Cultivar");
    for (auto iter : names) {
        std::cout << iter << ":\n";
        for (auto inner : data.at(iter)) {
            std::cout << inner << std::endl;
        }
        std::cout << "\n\n";
    }
	
	return 0;
}