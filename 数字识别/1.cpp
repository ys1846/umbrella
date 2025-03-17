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

string imgcompression(Mat& Img) //实现图像由28*28到7*7的压缩
{
	vector<vector<int> > num;
	vector<int> num1;
	string Img_str = "";

	for (int r = 0; r < Img.rows; r++) //将图片像素存入容器（二维）
	{
		for (int c = 0; c < Img.cols; c++) {
			num1.push_back(Img.at<uchar>(r, c));
		}
		num.push_back(num1);
		num1.clear();
	}

	for (int i = 0; i < Img.rows; i += 4) //用4*4大小模板遍历像素值，像素分割阈值100，模板的分割阈值为6（16个像素若有6个以上像素值为1则该4*4区域为1，否则为0）
	{
		for (int j = 0; j < Img.cols; j += 4) {
			int total = 0;
			for (int m = 0; m < 4; m++) {
				for (int n = 0; n < 4; n++) {
					if (num[i + m][j + n] > 100) total++;
				}
			}
			if (total > 6) Img_str += "1";
			else Img_str += "0";
		}
	}

	return Img_str;
}

int string_compare(string& str1, string& str2) //计算欧式距离（比较两个字符串有多少个字符不同）
{
	int count = 0;
	for (int i = 0; i < str1.length(); i++)
	{
		if (str1[i] != str2[i]) count++;
	}
	return count;
}

void Img_compare(const string& tr, const string& te) {
	string train_folder = tr;
	string test_folder = te;

	vector<String> train_Img_file;//存储所有图片
	vector<String> test_Img_file;

	string train_label = "";//存储待比较图片标签
	string test_label = "";

	Mat train_image;//存储待比较图片
	Mat test_image;

	glob(train_folder, train_Img_file, false);
	glob(test_folder, test_Img_file, false);

	string test_str = "";//存储压缩后图片的二值像素
	string train_str = "";

	double total = 0;//计数图片个数
	int tt = 49;//存取最小的欧式距离
	int dnum = 0;//存储欧式距离大小
	int num = -1;//存储识别的数字标签
	int refuse_num = 0;//拒绝识别个数
	int false_num = 0;//错误识别个数
	int true_num = 0;//正确识别个数
	bool refuse = false;//判断是否正确识别

	cv::Mat grayImage1;//存储灰度化后的图像
	Mat Img1;//存储二值化后的图像
	cv::Mat grayImage2;
	Mat Img2;

	int numtrue_num[10] = { 0 };//计数每个数字图片识别正确个数
	double total_num[10] = { 0 };//计数每个数字图片个数

	for (const String& file1 : test_Img_file) {
		tt = 49;
		test_str = "";

		test_label = file1.substr(file1.find_last_of("\\") + 1, 1);//读取标签（最有一个\后的内容）
		test_image = imread(file1);

		cv::cvtColor(test_image, grayImage1, cv::COLOR_BGR2GRAY);//灰度化
		threshold(grayImage1, Img1, 50, 255, THRESH_BINARY);//阈值分割二值化

		test_str = imgcompression(Img1);//将图像由28*28压缩为7*7

		for (const String& file : train_Img_file) {
			train_str = "";

			train_label = file.substr(file.find_last_of("\\") + 1, 1);
			train_image = imread(file);

			cv::cvtColor(train_image, grayImage2, cv::COLOR_BGR2GRAY);
			threshold(grayImage2, Img2, 50, 255, THRESH_BINARY);

			train_str = imgcompression(Img2);

			dnum = string_compare(test_str, train_str);
			if (dnum < tt) {
				tt = dnum;
				refuse = false;
				num = std::stoi(train_label.c_str());
			}
			else if (dnum == tt && num != std::stoi(train_label.c_str())) {
				refuse = true;
			}

		}
		if (!refuse) {
			if (num == std::stoi(test_label.c_str())) {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t正确识别为：" << num << endl;
				true_num++;
				numtrue_num[std::stoi(test_label.c_str())]++;
			}
			else {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t错误识别为：" << num << endl;
				false_num++;
			}
		}
		else {
			std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t拒绝识别" << endl;
			refuse_num++;
		}
		total++;
		total_num[std::stoi(test_label.c_str())]++;
	}

	train_Img_file.clear();
	test_Img_file.clear();

	double t, f, r;
	t = true_num / total;
	f = false_num / total;
	r = refuse_num / total;
	for (int i = 0; i < 10; i++) {
		double a = numtrue_num[i] / total_num[i];
		std::cout << "数字" << i << "的正确率：" << a << endl;
	}

	std::cout << "正确率：" << t << endl;
	std::cout << "错误率：" << f << endl;
	std::cout << "拒绝识别率：" << r << endl;
}

int main() {

	Img_compare("C:\\Users\\27933\\Desktop\\MNIST_train\\*.jpg", "C:\\Users\\27933\\Desktop\\MNIST_test\\*.jpg");

	return 0;
}