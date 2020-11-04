#include <iostream>
#include <time.h>
#include <numeric>
#include <functional>
#include <algorithm>
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

	return rand_range(static_cast<int>(min), static_cast<int>(max)) + (rand() % (101)) / 100.0;
}

static matrix multi(std::vector<double> const& a, std::vector<double> const& b) {
	matrix result;
	if (a.size() < b.size()) {
		throw std::invalid_argument("incompatible vectors");
	}

	size_t row = a.size(), col = b.size();

	for (size_t i = 0; i < col; i++)
	{
		result.push_back(std::vector<double>(row, 0.0));
	}

	for (size_t i = 0; i < col; i++) {

		for (size_t j = 0; j < row; j++)
		{
			result[i][j] = a[j] * b[i];
		}
	}
	return result;

}

static double operator *
(std::vector<double> const& a, std::vector<double> const& b)
{
	if (a.size() != b.size()) {
		throw  std::invalid_argument("incompatible vectors");
	}
	double result = 0;
	for (size_t i = 0; i < a.size(); i++)
	{
		result += a[i] * b[i];
	}
	return result;
}

static std::vector<double> operator *
(std::vector<double> const& vec, double const& val) {
	std::vector<double> result;
	for (auto& elem : vec) {
		result.push_back(elem * val);
	}
	return result;
}

static std::vector<double> operator *
(double const& val, std::vector<double> const& vec) {
	return vec * val;
}

static std::vector<double> operator -
(std::vector<double> const& a, std::vector<double> const& b) {
	if (a.size() != b.size()) {
		throw std::invalid_argument("incompatible vectors");
	}
	std::vector<double> result;
	for (size_t i = 0; i < a.size(); i++)
	{
		result.push_back(a[i] - b[i]);
	}
	return result;

}

static matrix operator -
(matrix const& a, matrix const& b) {
	size_t a_col = a.size(), a_row = a[0].size(), b_col = b.size(), b_row = b[0].size();
	if (a_col != b_col or a_row != b_row) {
		throw std::invalid_argument("incompatible matrices");
	}
	matrix result(a);
	size_t col = a_col, row = a_row;
	for (size_t i = 0; i < col; i++)
	{
		for (size_t j = 0; j < row; j++)
		{
			result[i][j] -= b[i][j];
		}
	}
	return result;
}

static std::vector<double> operator +
(std::vector<double> const& a, std::vector<double> const& b) {
	if (a.size() != b.size()) {
		throw std::exception("invalid args");
	}
	std::vector<double> result;
	for (size_t i = 0; i < a.size(); i++)
	{
		result.push_back(a[i] + b[i]);
	}
	return result;

}



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
			components.push_back(std::vector<double>(row_num, 0.0));
		}
		init_weight();

	}

	matrix compress()
	{

		for (size_t k = 0; k < col_num; k++)
		{

			auto lim = int_pow(10, k + 1);
			for (size_t i = 0; i < lim; i++)
			{
				for (size_t j = 0; j < row_num; j++)
				{
					auto cur_row = take_sample(j);
					auto cur_comp = calcualte_comp(cur_row);
					components[k][j] = cur_comp;
					//calc weights
					recalc_weights(cur_comp, j + 1, cur_row);
					normalize_weights();
				}


			}

			data = data - multi(components[k], weights);

		}

		return components;

	}

private:

	void recalc_weights(double const& comp, int cur_row_num, std::vector<double> const& sample) {
		double denum = (1.0 / cur_row_num * comp);
		auto delta = (sample - (comp * weights)) * denum;
		weights = weights + delta;

	}

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
		auto square_sum = [&](double x, double y) {return x + pow(y, 2); };
		auto sum = std::accumulate(weights.cbegin(), weights.cend(), 0.0, square_sum);
		auto norm = sqrt(sum);
		auto normalize = [&norm](double x) {return x / norm; };
		std::transform(weights.begin(), weights.end(), weights.begin(), normalize);
	}

	void init_weight() {
		srand(time(0));

		for (size_t j = 0; j < col_num; ++j)
		{
			double val = rand_range(-1.0, 1.0);
			if (weights.size() < col_num) {
				weights.push_back(val);
			}
			else {
				weights[j] = val;
			}
		}
		normalize_weights();
	}

};



int main(void) {

	std::string path = "D:/Me/многа букаф/zmeya/DataMining/iris.data.processed";
	std::vector<std::string> names = {
		"sepal length in cm",
		"sepal width in cm",
		"petal length in cm",
		"petal width in cm"
	};
	reader read(path, names);
	auto data = read.read();
	PCA compr(data, names);
	auto new_data = compr.compress();
	path = "D:/Me/многа букаф/zmeya/DataMining/iris.data.compressed";
	writer wr(new_data, path);
	wr.write();
	return 0;
}