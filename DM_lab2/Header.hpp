#pragma once
#include <regex>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator> 
#include <map> 


namespace CSVmanager
{
	typedef std::map<std::string, std::vector<double>> data_set;
	typedef std::vector<std::vector<double>> matrix;

	class reader {
	private:
		std::string path = "";
		const std::vector<std::string> names;
		size_t col_num = 0;
	public:
		reader() {}
		reader(std::string& path_) : path(path_) { count_cols(); }
		reader(const char* path_) : path(path_) { count_cols(); }
		reader(std::string& path_, std::vector<std::string>& names_) 
			: path(path_), names(names_), col_num(names_.size()) 
		{
			
		}
		reader(const char* path_, std::vector<std::string>& names_) 
			: path(path_), names(names_), col_num(names_.size()) 
		{
			
		}

		data_set read() {
			
			std::fstream file;
			std::string line;
			std::vector<std::vector<double>> values;
			for (size_t i = 0; i < col_num; i++)
			{
				values.push_back(std::vector<double>());
			}
			file.open(path, std::ios::in);
			for (; std::getline(file, line); )
			{				
				std::stringstream inner(line);
				std::string val;
				for (size_t j = 0; std::getline(inner, val, ','); ++j) {
					try {
						values[j].push_back(std::stod(val));
					}
					catch (std::invalid_argument e) {/*skip*/}
				}
			}
			file.close();
			return fabriacate_data_set(values);
		}
		

		~reader() {
			
		}


	private:
		void count_cols() {
			std::fstream file;
			file.open(path, std::ios::in);
			std::string str;
			std::getline(file, str);
			file.close();
			std::smatch match;
			std::regex_search(str, match, std::regex(","));
			col_num = match.size();
		}

		data_set fabriacate_data_set(std::vector<std::vector<double>>& values) {
			data_set result;
			for (size_t i = 0; i < col_num; ++i) {
				result.insert(std::pair<std::string, std::vector<double>>(names[i], values[i]));
			}
			return result;

		}
	
	
	};


	class writer
	{

	public:
		writer() {}
		writer(matrix& data_) : data(data_) { }
		writer(matrix& data_, std::string path_) : data(data_), path(path_) { }
		void write() {
			std::fstream file;
			file.open(path, std::ios::out);
			std::string result = "";
			size_t col = data.size(), row = data[0].size();
			
			for (size_t j = 0; j < row; j++)
			{
				for (size_t i = 0; i < col; i++)
				{
					result += std::to_string(data[i][j]) + (i != col - 1 ? "," : "\n");
				}
				
			}
			
			
			file << result;
			file.close();
		}
		~writer() {}

	private:
		std::string path = "data.csv";
		matrix data;

	};


}
