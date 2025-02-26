/*********************
Name: Darren Bowers
main.h - k-Means Clustering
Purpose: A simple k-means clustering algorithm, kmeans, using the Euclidean distance for 2-dimensional numerical data. program should be executed as follows:
kmeans k input.txt
where input parameter k > 1 is an integer, specifying the number of clusters. input.txt is an input file containing many 2-dimensional data points, with a singly x and y per row
**********************/

#include <iostream> // cout, cerr, endl.
#include <fstream> // For reading from and writing to files.
#include <sstream> // Allows treating strings as input streams
#include <vector> // Dynamic array that can change size as needed.
#include <string> // For handling and manipulating strings.
#include <cstdlib> // Grants atoi, which converts a string to an integer.
#include <cmath> // For mathematical operations
#include <limits> // for numberic_limits
#include <algorithm> // For the shuffle operation
#include <random> // To make a pseudorandom number generator for use in shuffling

using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::vector;
using std::string;
using std::ofstream;
using std::istringstream;
using std::numeric_limits;
using std::random_device;
using std::mt19937; 

const int MAXITERATIONS = 1000;

struct Point {
    double x, y;
    int cluster; // Cluster index (0-indexed internally)
};

struct Centroid {
    double x, y;
};

double squaredEuclideanDistance(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    return dx * dx + dy * dy;
}

int main(int argc, char* argv[]) {
    // Check for proper command line arguments
    int k = atoi(argv[1]);
    if (argc == 3 && k >= 1) {
        // Open input file
        ifstream infile(argv[2]);
        if (infile) {
            // Read points from the input file
            vector<Point> points;
            string line;
            while (getline(infile, line)) {
                if(line.empty())
                    continue; // Skip empty lines
                istringstream iss(line);
                Point p;
                if (!(iss >> p.x >> p.y))
                    continue; // Skip lines that don't have two numbers
                p.cluster = -1;
                points.push_back(p);
            }
            infile.close();
            
            if (points.size() >= static_cast<size_t>(k)) {
                // Create a random number generator and shuffle the points to create better centroids
                random_device rd;
                mt19937 g(rd());
                shuffle(points.begin(), points.end(), g);
                
                // Initialize centroids by choosing the first k points
                vector<Centroid> centroids(k);
                for (int i = 0; i < k; i++) {
                    centroids[i].x = points[i].x;
                    centroids[i].y = points[i].y;
                }
                // Initialize loop conditions
                bool changed = true;
                int iterations = 0;
                
                // Main k-means loop
                while (changed && iterations < MAXITERATIONS) {
                    changed = false;
                    // Assignment step: assign each point to the closest centroid
                    for (auto &p : points) {
                        int bestCluster = -1;
                        double bestDist = numeric_limits<double>::max();
                        for (int i = 0; i < k; i++) {
                            double dist = squaredEuclideanDistance(p.x, p.y, centroids[i].x, centroids[i].y);
                            if (dist < bestDist) {
                                bestDist = dist;
                                bestCluster = i;
                            }
                        }
                        if (p.cluster != bestCluster) {
                            p.cluster = bestCluster;
                            changed = true;
                        }
                    }
                    // Update step: recalculate centroids as the mean of all points in each cluster
                    vector<double> sumX(k, 0.0), sumY(k, 0.0);
                    vector<int> count(k, 0);
                    for (const auto &p : points) {
                        sumX[p.cluster] += p.x;
                        sumY[p.cluster] += p.y;
                        count[p.cluster]++;
                    }
                    for (int i = 0; i < k; i++) {
                        if (count[i] > 0) {
                            centroids[i].x = sumX[i] / count[i];
                            centroids[i].y = sumY[i] / count[i];
                        }
                    }
                    iterations++;
                }
                // Write the clustered data to output.txt
                ofstream outfile("output.txt");
                if (outfile) {
                    for (const auto &p : points) {
                        // Output x, y, and cluster label (1-indexed for user display)
                        outfile << p.x << "\t" << p.y << "\t" << (p.cluster + 1) << "\n";
                    }
                    outfile.close();
                    cout << "Clustering complete. Output written to output.txt" << endl;
                } else {
                    cerr << "Error: Unable to open output file." << endl;
                }     
            } else{
                cerr << "Error: The number of points is less than k." << endl;
            }
        }else{
            cerr << "Error: Unable to open input file " << argv[2] << endl;
        }
    } else{
        cerr << "Usage: " << argv[0] << " k input.txt" << endl << "where k is a number above 1" << endl;
    }
    return 0;
}
