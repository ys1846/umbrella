#include"recognize.h"



//图像切割#############################
void division(Mat& Img) {
	int r1 = 0, r2 = Img.rows - 1;
	int c1 = 0, c2 = Img.cols - 1;
	bool v = false;
	for (int i = 0; i < Img.rows; i++) {
		for (int j = 0; j < Img.cols; j++) {
			if (Img.at<uchar>(i, j) > 100) v = true;
		}
		if (v == true) {
			v = false;
			break;
		}
		r1++;
	}
	for (int i = Img.rows - 1; i > 0; i--) {
		for (int j = 0; j < Img.cols; j++) {
			if (Img.at<uchar>(i, j) > 100) v = true;
		}
		if (v == true) {
			v = false;
			break;
		}
		r2--;
	}
	for (int i = 0; i < Img.cols; i++) {
		for (int j = 0; j < Img.rows; j++) {
			if (Img.at<uchar>(j, i) > 100) v = true;
		}
		if (v == true) {
			v = false;
			break;
		}
		c1++;
	}
	for (int i = Img.cols - 1; i > 0; i--) {
		for (int j = 0; j < Img.rows; j++) {
			if (Img.at<uchar>(j, i) > 100) v = true;
		}
		if (v == true) {
			v = false;
			break;
		}
		c2--;
	}
	Img = Img(Range(r1, r2), Range(c1, c2));
}


//图像转string###############################

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


//模板匹配实现##############################################

void Img_compare(const string& tr, const string& te) {
	string train_folder = tr;
	string test_folder = te;

	vector<String> train_Img_file;//存储所有图片
	vector<String> test_Img_file;

	string train_label = "";//存储待比较图片标签
	string test_label = "";

	Mat train_image;//存储待比较图片
	Mat test_image;

	glob(train_folder, train_Img_file, false);//从文件中读取文件
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


//单张图片识别##########################################

void OneImg_compare(Mat& Img, const string& tr) {
	string train_folder = tr;
	vector<String> train_Img_file;
	string train_label = "";
	Mat train_image;
	glob(train_folder, train_Img_file, false);

	string train_str = "";
	string test_str = "";
	int tt = 49;
	int dnum = 0;
	int num = 0;
	bool refuse = false;

	cv::Mat grayImage1;
	Mat Img1;
	cv::Mat grayImage2;
	Mat Img2;

	int numChannels = Img.channels();

	if (numChannels == 1) {
		division(Img);
		resize(Img, Img, Size(28, 28), 0, 0, INTER_LINEAR);
		threshold(Img, Img1, 50, 255, THRESH_BINARY);

		test_str = imgcompression(Img1);
	}
	else if (numChannels == 3) {
		cv::cvtColor(Img, grayImage1, cv::COLOR_BGR2GRAY);
		division(Img);
		resize(Img, Img, Size(28, 28), 0, 0, INTER_LINEAR);
		threshold(grayImage1, Img1, 50, 255, THRESH_BINARY);

		test_str = imgcompression(Img1);
	}
	else {
		std::cout << "这是一张 " << numChannels << " 个通道的图像。" << std::endl;
	}

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
		std::cout << "识别为：" << num << endl;
	}
	else {
		std::cout << "拒绝识别" << endl;
	}
}

//Bayes实现##################################################

void Img_compare_Bayes(const string& tr, const string& te) {
	string train_folder = tr;
	string test_folder = te;

	vector<String> train_Img_file;
	vector<String> test_Img_file;

	string train_label = "";
	string test_label = "";

	Mat train_image;
	Mat test_image;

	glob(train_folder, train_Img_file, false);//从文件中读取图片
	glob(test_folder, test_Img_file, false);

	string test_str = "";
	string train_str = "";

	double sum = 0;
	double total = 0;
	double tt = 0;
	int num = -1;
	int refuse_num = 0;
	int false_num = 0;
	int true_num = 0;
	bool refuse = false;

	cv::Mat grayImage1;
	Mat Img1;
	cv::Mat grayImage2;
	Mat Img2;

	int numtrue_num[10] = { 0 };
	double total_num[10] = { 0 };

	double PriorP[10] = { 0 }; //每个数字出现个数
	double ConditionalP[10][49] = { 0 };//对每个数字所有图片统计每个像素为1的的个数
	int pp = 0;
	double num_P[10] = { 0 };

	for (const String& file : train_Img_file) {
		train_str = "";

		train_label = file.substr(file.find_last_of("\\") + 1, 1);
		train_image = imread(file);

		cv::cvtColor(train_image, grayImage2, cv::COLOR_BGR2GRAY);
		threshold(grayImage2, Img2, 50, 255, THRESH_BINARY);

		train_str = imgcompression(Img2);

		pp = std::stoi(train_label.c_str());
		PriorP[pp]++;

		for (int i = 0; i < 49; i++) {
			if (train_str[i] == '1') ConditionalP[pp][i]++;
		}
	}

	for (int i = 0; i < 10; i++) //计算先验概率
	{
		for (int j = 0; j < 49; j++) {
			ConditionalP[i][j] = (ConditionalP[i][j] + 1) / (PriorP[i] + 2);
		}
		PriorP[i] /= 100.0;
	}


	for (const String& file1 : test_Img_file) {
		tt = 49;
		test_str = "";

		test_label = file1.substr(file1.find_last_of("\\") + 1, 1);


		test_image = imread(file1);

		cv::cvtColor(test_image, grayImage1, cv::COLOR_BGR2GRAY);
		threshold(grayImage1, Img1, 50, 255, THRESH_BINARY);

		test_str = imgcompression(Img1);


		for (const String& file : train_Img_file) //计算后验概率
		{
			
			for (int y = 0; y < 10; y++) num_P[y] = PriorP[y];
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 49; j++) {
					if (test_str[j] == '1') {
						num_P[i] *= ConditionalP[i][j];
					}
					else {
						num_P[i] *= 1 - ConditionalP[i][j];
					}
				}
			}

		}
		//对后验概率进行归一化
		sum = 0;
		for (int i = 0; i < 10; i++) {
			sum += num_P[i];
		}
		for (int i = 0; i < 10; i++) {
			num_P[i] /= sum;
		}

		refuse = false;
		tt = 0;
		num = -1;

		for (int i = 0; i < 10; i++) {
			if (num_P[i] > tt) {
				tt = num_P[i];
				num = i;
				refuse = false;
			}
			else if (num_P[i] == tt) {
				refuse = true;
			}
		}
		if (!refuse) {
			if (num == std::stoi(test_label.c_str())) {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t正确识别为：" << num << "\t后验概率为：" << tt << endl;
				true_num++;
				numtrue_num[std::stoi(test_label.c_str())]++;
			}
			else {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t错误识别为：" << num << "\t后验概率为：" << tt << endl;
				false_num++;
			}
		}
		else {
			std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t拒绝识别" << endl;
			refuse_num++;
		}
		total_num[std::stoi(test_label.c_str())]++;
		total++;
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


//文本分类######################################

vector<Document> read_dataset(const string& filename, int label) //读取文本函数
{
	vector<Document> dataset;
	ifstream file;


	for (const auto& entry : filesystem::directory_iterator(filename)) {
		if (entry.path().extension() == ".txt") {
			file.open(entry.path().string(), ios::in);
			if (file.is_open()) {
				string line = "";
				getline(file, line);
				dataset.push_back(Document(line, label));
			}
			else {
				cerr << "cant't open file" << endl;
				//perror("Error opening file");
			}
			file.close();
		}
	}
	return dataset;
}


std::string removeHtmlTags(const std::string& text) //去除文本中的html标签函数
{
	std::regex html_tags("<[^>]*>");
	return regex_replace(text, html_tags, "");
}

string preprocess(string& text) //文本处理函数（去除标点符号，大写转小写）
{
	string result = "";
	for (wchar_t c : text) {
			if (iswalpha(c)) {
				result += tolower(c);
			}
			else if (iswpunct(c) || iswdigit(c)) {
				continue;
			}
			else {
				result += c;
			}
	}
	return result;
}

vector<string> tokenize(string text) //文本句子分割为一个个单词函数
{
	vector<string> tokens;
	size_t start = 0;
	size_t end = 0;
	while ((end = text.find(' ', start)) != string::npos) {
		string token = text.substr(start, end - start);
		tokens.push_back(token);
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

double term_frequency(string token, Document doc) //计算tf函数
{
	int count = 0;
	for (string t : doc.tokens) {
		if (t == token) {
			count++;
		}
	}
	return (double)count / doc.tokens.size();
}

double inverse_document_frequency(string token, vector<Document> dataset) //计算idf函数
{
	int count = 0;
	for (const Document& doc : dataset) {
		vector<string> tokens = doc.tokens;
		for (string t : tokens) {
			if (t == token) {
				count++;
				break;
			}
		}
	}

	return log((double)dataset.size() / (count + 1));
}

map<string, double>tf_idf(Document doc, vector<Document> dataset) //计算单词tf-idf函数
{
	map<string, double> vector1;
	set<string>unique_tokens(doc.tokens.begin(), doc.tokens.end());
	set<string>::iterator it = unique_tokens.begin();
	it++;
	for (; it != unique_tokens.end(); ++it) {
		string token = *it;
		double tf = term_frequency(token, doc);
		double idf = inverse_document_frequency(token, dataset);
		vector1[token] = tf * idf;
	}
	return vector1;
}


double evaluate(vector<bool> individual, Population population, vector<Document> train_set, vector<Document> test_set) //计算个体适应度函数（包含感知器的训练）
{
	vector<string> features;
	for (int i = 0; i < individual.size(); i++) {
		features.push_back(population.individuals[0][i] ? "1" : "0");
	}

	Perceptron classifier(features.size(), 0.05);//定义感知器，更新速率为0.05

	for (size_t i = 0; i < train_set.size(); i++) {
		vector<double> input;
		for (size_t j = 0; j < train_set[i].tokens.size(); j++) {
			if (features[j] == "1") {
				input.push_back(train_set[i].tfidf[train_set[i].tokens[j]]);
			}
			else {
				input.push_back(0);
			}
		}
		classifier.train(input, train_set[i].label);
	}

	int correct = 0;
	for (size_t i = 0; i < test_set.size(); i++) {
		vector<double> input;
		for (size_t j = 0; j < test_set[i].tokens.size(); j++) {
			if (features[j] == "1") {
				input.push_back(test_set[i].tfidf[test_set[i].tokens[j]]);
			}
			else {
				input.push_back(0);
			}
		}
		int prediction = classifier.output(input);
		if (prediction == test_set[i].label) {
			correct++;
		}
	}

	return (double)correct / test_set.size();
}

vector<bool> select(Population population) //父母选择函数（采用轮盘赌算法，正确率为概率）
{
	double total_fitness = 0;
	for (double fitness : population.fitnesses) {
		total_fitness += fitness;
	}
	double r = (double)rand() / RAND_MAX * total_fitness;
	double sum = 0;
	for (int i = 0; i < population.size; i++) {
		sum += population.fitnesses[i];
		if (sum >= r) {
			return population.individuals[i];
		}
	}
	return population.individuals[population.size - 1];
}

pair<vector<bool>, vector<bool>> crossover(vector<bool>parent1, vector<bool>parent2, double crossover_rate) //基因交叉函数（简单交叉，选取基因上一点交换父母该点前后的基因）
{
	double r = (double)rand() / RAND_MAX;
	if (r < crossover_rate) //小于交叉率则交叉
	{
		int point = rand() % (parent1.size() - 1) + 1;//随机产生基因交叉点
		vector<bool> offspring1(parent1.begin(), parent1.begin() + point);
		offspring1.insert(offspring1.end(), parent2.begin() + point, parent2.end());
		vector<bool> offspring2(parent2.begin(), parent2.begin() + point);
		offspring2.insert(offspring2.end(), parent1.begin() + point, parent1.end());
		return make_pair(offspring1, offspring2);
	}
	else return make_pair(parent1, parent2);
}

vector<bool>mutate(vector<bool>individual, double mutation_rate) //基因突变函数
{
	for (int i = 0; i < individual.size(); i++) {
		double r = (double)rand() / RAND_MAX;
		if (r < mutation_rate) //小于突变率则突变
		{
			individual[i] = !individual[i];
		}
	}
	return individual;
}

void replace(Population& population, Population new_population) //种群更新函数（保留上一代最佳个体与新产生的个体作为新种群）
{
	vector<pair<vector<bool>, double>> sorted_population;
	for (int i = 0; i < population.size; i++) {
		sorted_population.push_back(make_pair(population.individuals[i], population.fitnesses[i]));
	}
	sort(sorted_population.begin(), sorted_population.end(), [](auto a, auto b) {return a.second > b.second; });//根据适应度对旧种群排序
	vector<pair<vector<bool>, double>> sorted_new_population;
	for (int i = 0; i < new_population.size; i++) {
		sorted_new_population.push_back(make_pair(new_population.individuals[i], new_population.fitnesses[i]));
	}
	sort(sorted_new_population.begin(), sorted_new_population.end(), [](auto a, auto b) {return a.second > b.second; });//对新种群排序
	population.individuals[0] = sorted_population[0].first;
	population.fitnesses[0] = sorted_population[0].second;
	for (int i = 1; i < population.size; i++) {
		population.individuals[i] = sorted_new_population[i - 1].first;
		population.fitnesses[i] = sorted_new_population[i - 1].second;
	}
}

vector<bool>genetic_algorithm(vector<Document>train_set, vector<Document>test_set, int population_size, double crossover_rate, double mutation_rate, int iterations, size_t m_size) //种群迭代函数
{
	Population population(population_size, m_size);
	int i = 0;
	
	for (int j = 0; j < population.size; j++) {
		population.fitnesses[j] = evaluate(population.individuals[j], population, train_set, test_set);
	}
	while (i < iterations) {
		Population new_population(population.size, m_size);
		for (int j = 0; j < new_population.size; j += 2) {
			vector<bool> parent1 = select(population);
			vector<bool> parent2 = select(population);
			pair<vector<bool>, vector<bool>> offspring = crossover(parent1, parent2, crossover_rate);
			offspring.first = mutate(offspring.first, mutation_rate);
			offspring.second = mutate(offspring.second, mutation_rate);
			new_population.individuals[j] = offspring.first;
			new_population.individuals[j + 1] = offspring.second;
		}
		for (int j = 0; j < new_population.size; j++) {
			new_population.fitnesses[j] = evaluate(new_population.individuals[j], new_population, train_set, test_set);
		}
		replace(population, new_population);
		cout << "迭代次数:" << i + 1 << "\t适应度： ";
		for (int k = 0; k < population.size; k++) {
			cout << population.fitnesses[k] << " ";
		}
		cout << endl;
		i++;
	}
	return population.individuals[0];//返回最佳基因
}

void Text_classification(const string& trpos, const string& trneg) {
	srand(static_cast<unsigned>(time(0)));
	vector<Document> dataset = read_dataset(trpos, 1);
	vector<Document> dataset0 = read_dataset(trneg, 0);
	dataset.insert(dataset.end(), dataset0.begin(), dataset0.end());
	dataset0.clear();
	
	size_t max_num = 0;//存储最长的文本单词数（确定基因数量）

	for (Document& doc : dataset)//文本处理
	{
		doc.text = removeHtmlTags(doc.text);
		doc.text = preprocess(doc.text);
		doc.tokens = tokenize(doc.text);
		if (doc.tokens.size() > max_num) max_num = doc.tokens.size();
	}


	for (Document& doc : dataset)//计算tf-idf
	{
		doc.tfidf = tf_idf(doc, dataset);
	}

	vector<Document> train_set(dataset.begin(), dataset.begin() + 20);//将数据分为测试集和训练集
	vector<Document> test_set(dataset.begin() + 20, dataset.end());

	int population_size = 14;//种群大小
	double crossover_rate = 0.8;//交叉率
	double mutation_rate = 0.02;//突变率
	int iterations = 50;//迭代次数
	vector<bool>best_individual = genetic_algorithm(train_set, test_set, population_size, crossover_rate, mutation_rate, iterations, max_num);
	cout << "最优的特征组合:";
	for (bool gene : best_individual) {
		cout << gene << " ";
	}
	cout << endl;

}