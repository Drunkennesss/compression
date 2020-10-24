#include <iostream>
#include <time.h>
#include <numeric>
#include <functional>
#include "Header.hpp"

using namespace CSVmanager;

int rand_range(int const& min, int const& max) {
    return min + (rand() % (max - min + 1));
}

double rand_range(double const& min, double const& max) {
    if (min < INT16_MIN or max > INT16_MAX) {
        throw std::overflow_error("arg overflow");
    }
    return rand_range(min, max) + (rand() % (101)) / 100.0;
}

class PCA {
private:
    data_set data;
    const std::vector<std::string> names;
    std::vector<std::vector<double>> components;
    std::vector<double> weights;

public:
    PCA(data_set data_, std::vector<std::string>& names_) : data(data_), names(names_) {       

    }

private:

    void normalize_vect() {
        auto square_sum = [&](double x, double y) {return pow(x + y, 2); };
        auto norm = sqrt(std::accumulate(weights.cbegin(), weights.cend(), 0.0, square_sum));
        auto normalize = [&norm](double x) {return x / norm; };
        std::transform(weights.begin(), weights.end(), weights.begin(), normalize);
    }

    void init_weight() {
        srand(time(0));
        
        for (size_t j = 0; j < names.size(); ++j)
        {
            double val = rand_range(-1.0, 1.0);
            weights.push_back(val);
        }       
        normalize_vect();
    }

    void encode() {        
        for (auto it : data) {
            auto& vec = it.second;
            auto x_max = *std::max_element(vec.cbegin(), vec.cend());
            auto x_min = *std::min_element(vec.cbegin(), vec.cend());
            auto mean = std::accumulate(vec.cbegin(), vec.cend(), 0.0) / vec.size();
            for (auto x : vec) {
                x = 2 * (x - x_min) / (x_max - x_min) - 1;
            }
        }

        for (auto it : data) {
            auto& vec = it.second;
            auto mean = std::accumulate(vec.cbegin(), vec.cend(), 0.0) / vec.size();
            for (auto x : vec) {
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