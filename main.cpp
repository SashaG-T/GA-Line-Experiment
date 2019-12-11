#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int fitness(float* data, int size) {

	int retVal(0); //fitness score
	int streak(1);
	//increase fitness score for incremental streaks
	for (int i = 1; i < size; i++) {
		retVal += (data[i - 1] < data[i] ? (streak *= 2, streak) + (int)(0.000000001 / (data[i]*1.0 - data[i - 1]*1.0)) : (streak = 1, 0));
	}
	return retVal;
}

void mutate(float* data, int size) {

	//randomly change one value in array to some other random value...
	data[rand() % size] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void crossover(float* d1, float* d2, int entitySize) {

	float* tmp = new float[entitySize];
	int pivot((rand() % entitySize));
	int size(entitySize);

	if (!(rand() % 4)) {
		memcpy(tmp, d1, size * sizeof(float));
		memcpy(d1, d2 + pivot, (size - pivot) * sizeof(float));
		memcpy(d1 + (size - pivot), d2, (pivot) * sizeof(float));
		memcpy(d2, tmp + pivot, (size - pivot) * sizeof(float));
		memcpy(d2 + (size - pivot), tmp, (pivot) * sizeof(float));
	}
	else {
		memcpy(tmp, d1, pivot * sizeof(float));
		memcpy(d1, d2, pivot * sizeof(float));
		memcpy(d2, tmp, pivot * sizeof(float));
	}
	delete[] tmp;

}

void generateEntity(float* data, int size) {
	for (int i = 0; i < size; i++) {
		data[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
}

void createEntityList(float** data, int entityCount, int entitySize) {

	for (int i = 0; i < entityCount; i++) {
		data[i] = new float[entitySize];
	}

}

void destroyEntityList(float** data, int entityCount) {

	for (int i = 0; i < entityCount; i++) {
		delete[] data[i], data[i] = 0;
	}

}


int main(void) {

	const int entityCount(200);
	const int entitySize(300);
	const int generations(INT_MAX);
	const int reproduction(20);
	const int pixelSize = 4;

	char field_window[] = "Field";
	cv::Mat nnt = cv::Mat::zeros(entityCount*pixelSize, entitySize*pixelSize, CV_8UC4);

	int f[entityCount];
	float* entityList[entityCount];
	createEntityList(entityList, entityCount, entitySize);

	for (int i = 0; i < entityCount; i++) {
		generateEntity(entityList[i], entitySize);
		f[i] = fitness(entityList[i], entitySize);
	}

	//keep fittest, mutate less fittest
	for (;;) {
		for(int repro = 0; repro < reproduction; repro++) {
			std::vector<int> iVec;
			for (int i = 0; i < entityCount; i++) {
				iVec.push_back(i);
			}
			std::sort(iVec.begin(), iVec.end(), [&f](const int& lhs, const int& rhs) { return f[lhs] < f[rhs];});

			int c1;
			int c2;
			for (c1 = 0; rand() % 4; c1++) {}
			for (c2 = c1 + 1; rand() % 4; c2++) {}

			int c3;
			int c4;
			for (c3 = 0; rand() % 4; c3++) {}
			for (c4 = c3 + 1; rand() % 4; c4++) {}

			if (c1 > entitySize) {
				c1 = entitySize;
			}
			if (c2 > entitySize) {
				c2 = entitySize;
			}
			if (c3 > entitySize) {
				c3 = entitySize;
			}
			if (c4 > entitySize) {
				c4 = entitySize;
			}

			int h1(iVec.at(iVec.size() - c1 - 1));
			int h2(iVec.at(iVec.size() - c2 - 1));
			int l1(iVec.at(c3));
			int l2(iVec.at(c4));

			memmove(entityList[l1], entityList[h1], entitySize * sizeof(float));
			memmove(entityList[l2], entityList[h2], entitySize * sizeof(float));
			crossover(entityList[l1], entityList[l2], entitySize);
			mutate(entityList[l1], entitySize);
			mutate(entityList[l2], entitySize);
			f[l1] = fitness(entityList[l1], entitySize);
			f[l2] = fitness(entityList[l2], entitySize);
		}

		CV_Assert(nnt.channels() == 4);
		for (int i = 0; i < nnt.rows; i++) {
			for (int j = 0; j < nnt.cols; j++) {
				int x(i / pixelSize);
				int y(j / pixelSize);
				cv::Vec4b& bgra = nnt.at<cv::Vec4b>(i, j);
				bgra[0] = UCHAR_MAX;
				bgra[1] = entityList[x][y] * UCHAR_MAX;
				bgra[2] = entityList[x][y] * UCHAR_MAX;
				bgra[3] = entityList[x][y] * UCHAR_MAX;
				//bgra[1] = cv::saturate_cast<uchar>((float(nnt.cols - j)) / ((float)nnt.cols) * UCHAR_MAX);
				//bgra[2] = cv::saturate_cast<uchar>((float(nnt.rows - i)) / ((float)nnt.rows) * UCHAR_MAX);
				//bgra[3] = cv::saturate_cast<uchar>(0.5 * (bgra[1] + bgra[2]));
			}
		}
		cv::imshow(field_window, nnt);
		cv::waitKey(1);
	}

	destroyEntityList(entityList, entityCount);

	cv::imshow(field_window, nnt);

	cv::waitKey(0);
	return 0;
}