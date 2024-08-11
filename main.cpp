#include <algorithm>
#include <iostream>
#include <vector>
//#include <cstdarg>
#include <math.h>
#include <random>
#include <thread>
#include <chrono>

void vecsum_partial(std::vector<int> vec1,  std::vector<int> vec2, std::vector<int> res, int start, int stop){
    for(int i = start; i < stop; i++){
        res[i] = vec1[i] + vec2[i];
    }
}

std::vector<int> vecsum_parr(std::vector<int> vec1,  std::vector<int> vec2, int num_threads){
//void vecsum_parr(std::vector<int> vec1,  std::vector<int> vec2, int num_threads){
    int const length = vec1.size();
    int const block_size=length/num_threads;
    std::vector<int> res(length);
    std::vector<std::thread> threads(num_threads-1);//ONE OF THE THREADS IS MAIN THREAD, NEEDS NO STD::THREAD OBJECT
    for(int i = 0; i < num_threads-1; i++){
        threads[i] = std::thread(vecsum_partial, vec1, vec2, res, i*block_size, (i+1)*block_size);
    }
    for(auto& thr : threads){
        thr.join();
    }
    vecsum_partial( vec1, vec2, res, (num_threads-1)*block_size, length); //LAST BLOCK IS LEFT TO MAIN THREAD
    return res;
}

int main()
{
    std::cout << "Количество аппаратных ядер - " << std::thread::hardware_concurrency() << std::endl;
    std::vector<int> thrnums{1,2,4,8,16};
    //PRINT OUT FIRST LINE
    std::printf("%10s", " ");
    for(int i = 0; i<4; i++){
        std::printf("%13.0f", pow(10,3+i));
    }
    std::cout << std::endl;
    //PRINT OUT LINES WITH SECOND COUNTS
    for(auto thrnum : thrnums){
        std::printf("%2d потоков", thrnum);
        for(int i = 0; i<4; i++){
            std::vector<int> v1(pow(10,3+i));
            std::vector<int> v2(pow(10,3+i));
            std::vector<int> res(pow(10,3+i));
            std::mt19937 gen(time(nullptr));
            std::uniform_int_distribution<int> dis(0, 1'000'000);
            auto rand_num([=]() mutable {return dis(gen); });
            std::generate(v1.begin(), v1.end(), rand_num);
            gen.seed(time(nullptr)+10);
            auto rand_num2([=]() mutable {return dis(gen); });
            std::generate(v2.begin(), v2.end(), rand_num2);

            auto start = std::chrono::high_resolution_clock::now();
            //vecsum_parr(v1, v2, thrnum);
            res = vecsum_parr(v1, v2, thrnum);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> time = end - start;
            std::printf("%12fs", time.count());
        }
        std::cout << std::endl;
    }

    return 0;
}
