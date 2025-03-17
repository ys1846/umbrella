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

string imgcompression(Mat& Img) //ʵ��ͼ����28*28��7*7��ѹ��
{
	vector<vector<int> > num;
	vector<int> num1;
	string Img_str = "";

	for (int r = 0; r < Img.rows; r++) //��ͼƬ���ش�����������ά��
	{
		for (int c = 0; c < Img.cols; c++) {
			num1.push_back(Img.at<uchar>(r, c));
		}
		num.push_back(num1);
		num1.clear();
	}

	for (int i = 0; i < Img.rows; i += 4) //��4*4��Сģ���������ֵ�����طָ���ֵ100��ģ��ķָ���ֵΪ6��16����������6����������ֵΪ1���4*4����Ϊ1������Ϊ0��
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

int string_compare(string& str1, string& str2) //����ŷʽ���루�Ƚ������ַ����ж��ٸ��ַ���ͬ��
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

	vector<String> train_Img_file;//�洢����ͼƬ
	vector<String> test_Img_file;

	string train_label = "";//�洢���Ƚ�ͼƬ��ǩ
	string test_label = "";

	Mat train_image;//�洢���Ƚ�ͼƬ
	Mat test_image;

	glob(train_folder, train_Img_file, false);
	glob(test_folder, test_Img_file, false);

	string test_str = "";//�洢ѹ����ͼƬ�Ķ�ֵ����
	string train_str = "";

	double total = 0;//����ͼƬ����
	int tt = 49;//��ȡ��С��ŷʽ����
	int dnum = 0;//�洢ŷʽ�����С
	int num = -1;//�洢ʶ������ֱ�ǩ
	int refuse_num = 0;//�ܾ�ʶ�����
	int false_num = 0;//����ʶ�����
	int true_num = 0;//��ȷʶ�����
	bool refuse = false;//�ж��Ƿ���ȷʶ��

	cv::Mat grayImage1;//�洢�ҶȻ����ͼ��
	Mat Img1;//�洢��ֵ�����ͼ��
	cv::Mat grayImage2;
	Mat Img2;

	int numtrue_num[10] = { 0 };//����ÿ������ͼƬʶ����ȷ����
	double total_num[10] = { 0 };//����ÿ������ͼƬ����

	for (const String& file1 : test_Img_file) {
		tt = 49;
		test_str = "";

		test_label = file1.substr(file1.find_last_of("\\") + 1, 1);//��ȡ��ǩ������һ��\������ݣ�
		test_image = imread(file1);

		cv::cvtColor(test_image, grayImage1, cv::COLOR_BGR2GRAY);//�ҶȻ�
		threshold(grayImage1, Img1, 50, 255, THRESH_BINARY);//��ֵ�ָ��ֵ��

		test_str = imgcompression(Img1);//��ͼ����28*28ѹ��Ϊ7*7

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
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t��ȷʶ��Ϊ��" << num << endl;
				true_num++;
				numtrue_num[std::stoi(test_label.c_str())]++;
			}
			else {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t����ʶ��Ϊ��" << num << endl;
				false_num++;
			}
		}
		else {
			std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t�ܾ�ʶ��" << endl;
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
		std::cout << "����" << i << "����ȷ�ʣ�" << a << endl;
	}

	std::cout << "��ȷ�ʣ�" << t << endl;
	std::cout << "�����ʣ�" << f << endl;
	std::cout << "�ܾ�ʶ���ʣ�" << r << endl;
}

int main() {

	Img_compare("C:\\Users\\27933\\Desktop\\MNIST_train\\*.jpg", "C:\\Users\\27933\\Desktop\\MNIST_test\\*.jpg");

	return 0;
}