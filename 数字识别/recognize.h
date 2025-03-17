#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>
#include<string.h>
#include<sstream>
#include<io.h>
#include<iterator>
#include<vector>
#include<fstream>
#include<filesystem>
#include<inttypes.h>
#include <Windows.h>
#include <regex>
#include <unordered_set>

using namespace std;
using namespace cv;

class Document //文件类
{
public:
	string text;
	int label;//标签0表示neg，1表示pos
	vector<string> tokens;
	map<string, double> tfidf;

	Document(string t, int l) :text(t), label(l) {
	}
};

class Population //种群类
{
public:
	vector<vector<bool> >individuals;
	vector<double> fitnesses;
	int size;
	size_t length;
	Population(int s, size_t l) :size(s), length(l) {
		individuals.resize(size);
		fitnesses.resize(size);
		for (int i = 0; i < size; i++) {
			individuals[i].resize(length);
			for (int j = 0; j < length; j++) {
				individuals[i][j] = rand() % 2;
			}
		}
	}
};

class Perceptron //感知器类
{
public:
	vector<double> weights;
	double bias;
	double learning_rate;//更新速度

	Perceptron(size_t n, double lr) :learning_rate(lr) {
		weights.resize(n);
		for (int i = 0; i < n; i++) {
			weights[i] = (double)rand() / RAND_MAX * 2 - 1;
		}
		bias = (double)rand() / RAND_MAX * 2 - 1;
	}
	int output(vector<double> input) {
		double sum = bias;
		for (int i = 0; i < input.size(); i++) {
			sum += weights[i] * input[i];
		}
		return sum > 0 ? 1 : 0;
	}
	void train(vector<double>input, int target) {
		double prediction = output(input);
		double error = target - prediction;
		for (int i = 0; i < input.size(); i++) {
			weights[i] += learning_rate * error * input[i];
		}
		bias += learning_rate * error;
	}
};


void Img_compare(const string& tr, const string& te);
void OneImg_compare(Mat& Img, const string& tr);
void Img_compare_Bayes(const string& tr, const string& te);
void Text_classification(const string& trpos, const string& trneg);

