#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/for_each.h>
#include <thrust/transform.h>
#include <thrust/sequence.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>

template<typename Container>
cv::Mat copy_to_Mat(Container& container, int rows, int cols) {
    cv::Mat img(rows, cols, CV_32FC3);

    for(int i = 0; i != img.rows; ++i) {
        for(int j = 0; j != img.cols * 3; ++j) {
            img.at<float>(i,j) = container[img.cols * 3 * i + j];
        }
    }

    return img;
}

// Dear opencv why don't you do this for me?
cv::Mat transform_to_uchar(cv::Mat& img) {
    cv::Mat transformed(img.rows, img.cols, CV_8UC3);
    for(auto i = 0; i != img.rows; ++i) {
        for(auto j = 0; j != img.cols * 3; ++j) {
            transformed.at<unsigned char>(i, j) = img.at<float>(i, j) * 255.f;
        }
    }

    return img;
}

template<typename T>
__host__ __device__
void apply_mandel_brot(T* dest, int x, int y, int rows, int cols, const int iterations) {
    // calculate step
	const float x_scaler = 3.5f / static_cast<float>(cols * 3.f);
    const float y_scaler = 3.0f / static_cast<float>(rows);
    
    const float x0 = x * x_scaler - 2.5f;
	const float y0 = y * y_scaler - 1.5f;

	float real = 0.f;
	float img = 0.f;

	int iter = 0;
	while(real * real + img * img < 4.f && iter < iterations) {
        float real_temp = real * real - img * img + x0;
        img = 2.f * real * img + y0;
        real = real_temp;

        ++iter;
    }

    // coloring step
    const int base_index = cols * 3 * y + x;
    if(iter == iterations) {
        dest[base_index] = 0.f;
        dest[base_index + 1] = 0.f;
        dest[base_index + 2] = 0.f;
    }
    else {
        // random formula from the www; probably very slow
        const float value = (255.f - static_cast<float>(iter + 1.5 - std::log(std::log(real * real + img * img)))) / 255.f;
        dest[base_index] = 1.f;
        dest[base_index + 1] = value;
        dest[base_index + 2] = value;
    }
}

// for_each functor
struct apply_mandel {
    typedef thrust::device_vector<float> data_vector;

    apply_mandel(float* vec, int rows, int cols, const int max_iterations) 
        : dest(vec), rows(rows), cols(cols), iterations(max_iterations) {}

    __host__ __device__
    void operator()(int& id) {
        const int col = id % (cols * 3); 
        const int row = id / (cols * 3) ;
        apply_mandel_brot(dest, col, row, rows, cols, iterations);
    }
    
    float* dest;
    const int rows;
    const int cols;
    const int iterations;
};


thrust::host_vector<float> render_mandel_brot_gpu(int rows, int cols, const int iterations) {
	thrust::device_vector<float> d_vec(rows * cols * 3);
    apply_mandel worker(thrust::raw_pointer_cast(&d_vec[0]), rows, cols, iterations);

    thrust::device_vector<int> indices(rows * cols);
    thrust::sequence(indices.begin(), indices.end(), 0, 3);

    thrust::for_each(indices.begin(), indices.end(), worker);

    thrust::host_vector<float> h_vec = d_vec;
    return h_vec;
}

std::vector<float> render_mandel_brot_cpu(int rows, int cols, const int iterations) {
    std::vector<float> vec(rows * cols * 3);
    apply_mandel worker(&vec[0], rows, cols, iterations);

    std::vector<int> indices(rows * cols);
    thrust::sequence(indices.begin(), indices.end(), 0, 3);

    std::for_each(indices.begin(), indices.end(), worker);

    return vec;
}

int main()
{
    typedef std::chrono::high_resolution_clock hr_clock;
    typedef std::chrono::milliseconds ms;

    char mode = ' ';
    int iterations = 100;
    int rows = 4000;
    int cols = rows;

    std::cin >> mode;
    std::cin >> iterations;
    std::cin >> rows;

    cols = rows;

    cv::Mat img;

    if(mode == 'g') {
        std::cout << "running on GPU" << std::endl;
        auto begin = hr_clock::now();
        thrust::host_vector<float> data = render_mandel_brot_gpu(rows, cols, iterations);
        auto end = hr_clock::now();
        std::cout << "GPU took " << std::chrono::duration_cast<ms>(end - begin).count() << std::endl;

        img = copy_to_Mat(data, rows, cols);
    }
    else {
        std::cout << "running on CPU" << std::endl;
        auto begin = hr_clock::now();
        std::vector<float> data = render_mandel_brot_cpu(rows, cols, iterations);
        auto end = hr_clock::now();
        std::cout << "CPU took " << std::chrono::duration_cast<ms>(end - begin).count() << std::endl;
        
        img = copy_to_Mat(data, rows, cols);
    }

	cv::namedWindow("mega", 0); // 0 means resizeable
	cv::imshow("mega", img);
	cv::waitKey(0);

    std::cout << "Writing to file ..." << std::endl;
    cv::Mat transformed = transform_to_uchar(img);
    cv::imwrite("mandelbrot.jpeg", transformed);

    return 0;
}
