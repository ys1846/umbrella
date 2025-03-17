#include"recognize.h"



//ͼ���и�#############################
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


//ͼ��תstring###############################

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


//ģ��ƥ��ʵ��##############################################

void Img_compare(const string& tr, const string& te) {
	string train_folder = tr;
	string test_folder = te;

	vector<String> train_Img_file;//�洢����ͼƬ
	vector<String> test_Img_file;

	string train_label = "";//�洢���Ƚ�ͼƬ��ǩ
	string test_label = "";

	Mat train_image;//�洢���Ƚ�ͼƬ
	Mat test_image;

	glob(train_folder, train_Img_file, false);//���ļ��ж�ȡ�ļ�
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


//����ͼƬʶ��##########################################

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
		std::cout << "����һ�� " << numChannels << " ��ͨ����ͼ��" << std::endl;
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
		std::cout << "ʶ��Ϊ��" << num << endl;
	}
	else {
		std::cout << "�ܾ�ʶ��" << endl;
	}
}

//Bayesʵ��##################################################

void Img_compare_Bayes(const string& tr, const string& te) {
	string train_folder = tr;
	string test_folder = te;

	vector<String> train_Img_file;
	vector<String> test_Img_file;

	string train_label = "";
	string test_label = "";

	Mat train_image;
	Mat test_image;

	glob(train_folder, train_Img_file, false);//���ļ��ж�ȡͼƬ
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

	double PriorP[10] = { 0 }; //ÿ�����ֳ��ָ���
	double ConditionalP[10][49] = { 0 };//��ÿ����������ͼƬͳ��ÿ������Ϊ1�ĵĸ���
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

	for (int i = 0; i < 10; i++) //�����������
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


		for (const String& file : train_Img_file) //����������
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
		//�Ժ�����ʽ��й�һ��
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
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t��ȷʶ��Ϊ��" << num << "\t�������Ϊ��" << tt << endl;
				true_num++;
				numtrue_num[std::stoi(test_label.c_str())]++;
			}
			else {
				std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t����ʶ��Ϊ��" << num << "\t�������Ϊ��" << tt << endl;
				false_num++;
			}
		}
		else {
			std::cout << file1.substr(file1.find_last_of("\\") + 1) << "\t�ܾ�ʶ��" << endl;
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
		std::cout << "����" << i << "����ȷ�ʣ�" << a << endl;
	}
	std::cout << "��ȷ�ʣ�" << t << endl;
	std::cout << "�����ʣ�" << f << endl;
	std::cout << "�ܾ�ʶ���ʣ�" << r << endl;
}


//�ı�����######################################

vector<Document> read_dataset(const string& filename, int label) //��ȡ�ı�����
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


std::string removeHtmlTags(const std::string& text) //ȥ���ı��е�html��ǩ����
{
	std::regex html_tags("<[^>]*>");
	return regex_replace(text, html_tags, "");
}

string preprocess(string& text) //�ı���������ȥ�������ţ���дתСд��
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

vector<string> tokenize(string text) //�ı����ӷָ�Ϊһ�������ʺ���
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

double term_frequency(string token, Document doc) //����tf����
{
	int count = 0;
	for (string t : doc.tokens) {
		if (t == token) {
			count++;
		}
	}
	return (double)count / doc.tokens.size();
}

double inverse_document_frequency(string token, vector<Document> dataset) //����idf����
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

map<string, double>tf_idf(Document doc, vector<Document> dataset) //���㵥��tf-idf����
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


double evaluate(vector<bool> individual, Population population, vector<Document> train_set, vector<Document> test_set) //���������Ӧ�Ⱥ�����������֪����ѵ����
{
	vector<string> features;
	for (int i = 0; i < individual.size(); i++) {
		features.push_back(population.individuals[0][i] ? "1" : "0");
	}

	Perceptron classifier(features.size(), 0.05);//�����֪������������Ϊ0.05

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

vector<bool> select(Population population) //��ĸѡ�������������̶��㷨����ȷ��Ϊ���ʣ�
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

pair<vector<bool>, vector<bool>> crossover(vector<bool>parent1, vector<bool>parent2, double crossover_rate) //���򽻲溯�����򵥽��棬ѡȡ������һ�㽻����ĸ�õ�ǰ��Ļ���
{
	double r = (double)rand() / RAND_MAX;
	if (r < crossover_rate) //С�ڽ������򽻲�
	{
		int point = rand() % (parent1.size() - 1) + 1;//����������򽻲��
		vector<bool> offspring1(parent1.begin(), parent1.begin() + point);
		offspring1.insert(offspring1.end(), parent2.begin() + point, parent2.end());
		vector<bool> offspring2(parent2.begin(), parent2.begin() + point);
		offspring2.insert(offspring2.end(), parent1.begin() + point, parent1.end());
		return make_pair(offspring1, offspring2);
	}
	else return make_pair(parent1, parent2);
}

vector<bool>mutate(vector<bool>individual, double mutation_rate) //����ͻ�亯��
{
	for (int i = 0; i < individual.size(); i++) {
		double r = (double)rand() / RAND_MAX;
		if (r < mutation_rate) //С��ͻ������ͻ��
		{
			individual[i] = !individual[i];
		}
	}
	return individual;
}

void replace(Population& population, Population new_population) //��Ⱥ���º�����������һ����Ѹ������²����ĸ�����Ϊ����Ⱥ��
{
	vector<pair<vector<bool>, double>> sorted_population;
	for (int i = 0; i < population.size; i++) {
		sorted_population.push_back(make_pair(population.individuals[i], population.fitnesses[i]));
	}
	sort(sorted_population.begin(), sorted_population.end(), [](auto a, auto b) {return a.second > b.second; });//������Ӧ�ȶԾ���Ⱥ����
	vector<pair<vector<bool>, double>> sorted_new_population;
	for (int i = 0; i < new_population.size; i++) {
		sorted_new_population.push_back(make_pair(new_population.individuals[i], new_population.fitnesses[i]));
	}
	sort(sorted_new_population.begin(), sorted_new_population.end(), [](auto a, auto b) {return a.second > b.second; });//������Ⱥ����
	population.individuals[0] = sorted_population[0].first;
	population.fitnesses[0] = sorted_population[0].second;
	for (int i = 1; i < population.size; i++) {
		population.individuals[i] = sorted_new_population[i - 1].first;
		population.fitnesses[i] = sorted_new_population[i - 1].second;
	}
}

vector<bool>genetic_algorithm(vector<Document>train_set, vector<Document>test_set, int population_size, double crossover_rate, double mutation_rate, int iterations, size_t m_size) //��Ⱥ��������
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
		cout << "��������:" << i + 1 << "\t��Ӧ�ȣ� ";
		for (int k = 0; k < population.size; k++) {
			cout << population.fitnesses[k] << " ";
		}
		cout << endl;
		i++;
	}
	return population.individuals[0];//������ѻ���
}

void Text_classification(const string& trpos, const string& trneg) {
	srand(static_cast<unsigned>(time(0)));
	vector<Document> dataset = read_dataset(trpos, 1);
	vector<Document> dataset0 = read_dataset(trneg, 0);
	dataset.insert(dataset.end(), dataset0.begin(), dataset0.end());
	dataset0.clear();
	
	size_t max_num = 0;//�洢����ı���������ȷ������������

	for (Document& doc : dataset)//�ı�����
	{
		doc.text = removeHtmlTags(doc.text);
		doc.text = preprocess(doc.text);
		doc.tokens = tokenize(doc.text);
		if (doc.tokens.size() > max_num) max_num = doc.tokens.size();
	}


	for (Document& doc : dataset)//����tf-idf
	{
		doc.tfidf = tf_idf(doc, dataset);
	}

	vector<Document> train_set(dataset.begin(), dataset.begin() + 20);//�����ݷ�Ϊ���Լ���ѵ����
	vector<Document> test_set(dataset.begin() + 20, dataset.end());

	int population_size = 14;//��Ⱥ��С
	double crossover_rate = 0.8;//������
	double mutation_rate = 0.02;//ͻ����
	int iterations = 50;//��������
	vector<bool>best_individual = genetic_algorithm(train_set, test_set, population_size, crossover_rate, mutation_rate, iterations, max_num);
	cout << "���ŵ��������:";
	for (bool gene : best_individual) {
		cout << gene << " ";
	}
	cout << endl;

}