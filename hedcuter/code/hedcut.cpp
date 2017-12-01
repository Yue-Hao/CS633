#include "hedcut.h"
#include <time.h>
#include "DBSCAN.hpp"
#include <map>

using Eigen::MatrixXf;
using Eigen::VectorXf;

Hedcut::Hedcut()
{
	//control flags
	disk_size = 1;        //if uniform_disk_size is true, all disks have radius=disk_size,
	                      //othewise, the largest disks will have their radii=disk_size 

	uniform_disk_size = false; //true if all disks have the same size. disk_size is used when uniform_disk_size is true.
	black_disk = false;        //true if all disks are black ONLY

	//cvt control flags
	cvt_iteration_limit = 100; //max number of iterations when building cvf
	max_site_displacement = 1.01f; //max tolerable site displacement in each iteration. 
	average_termination = false;
	gpu = false;
	subpixels = 1;

	debug = false;
    use_weighted_color = false;
    use_area_radius = false;
}

bool Hedcut::build(cv::Mat & input_image, cv::Mat & stipple_image) {
    cv::Mat grayscale;
	cv::cvtColor(input_image, grayscale, CV_BGR2GRAY);

	//sample n points
	std::vector<cv::Point2d> pts;
	int n = sample_initial_points(grayscale,pts);

	//initialize cvt
	CVT cvt;

	cvt.iteration_limit = 1;//this->cvt_iteration_limit;
	cvt.max_site_displacement = this->max_site_displacement;
	cvt.average_termination = this->average_termination;
	cvt.gpu = this->gpu;
	cvt.subpixels = this->subpixels;
	cvt.debug = this->debug;

	clock_t startTime, endTime;
	startTime = clock();

	//compute weighted centroidal voronoi tessellation
	if (cvt.gpu)
		cvt.compute_weighted_cvt_GPU(input_image, pts);
	else
		cvt.compute_weighted_cvt(grayscale, pts);	//*****

	endTime = clock();
	std::cout << "Total time: "<< ((double)(endTime - startTime)) / CLOCKS_PER_SEC << std::endl;

	if (debug) cv::waitKey();

	//create disks
    if (use_weighted_color) {
        create_disks_weighted_color(input_image, cvt);
    } else {
        create_disks(input_image, cvt);
    }
	return true;
}

bool Hedcut::build(cv::Mat & input_image, int n)
{
	cv::Mat grayscale;
	cv::cvtColor(input_image, grayscale, CV_BGR2GRAY);

	//sample n points
	std::vector<cv::Point2d> pts;
	sample_initial_points(grayscale, n, pts);

	//initialize cvt
	CVT cvt;

	cvt.iteration_limit = this->cvt_iteration_limit;
	cvt.max_site_displacement = this->max_site_displacement;
	cvt.average_termination = this->average_termination;
	cvt.gpu = this->gpu;
	cvt.subpixels = this->subpixels;
	cvt.debug = this->debug;

	clock_t startTime, endTime;
	startTime = clock();

	//compute weighted centroidal voronoi tessellation
	if (cvt.gpu)
		cvt.compute_weighted_cvt_GPU(input_image, pts);
	else
		cvt.compute_weighted_cvt(grayscale, pts);	//*****

	endTime = clock();
	std::cout << "Total time: "<< ((double)(endTime - startTime)) / CLOCKS_PER_SEC << std::endl;

	if (debug) cv::waitKey();

	//create disks
    if (use_weighted_color) {
        create_disks_weighted_color(input_image, cvt);
    } else if (use_area_radius) {
        create_disks_area_radius(input_image, cvt);
    } else {
        create_disks(input_image, cvt);
    }
	return true;
}


void Hedcut::sample_initial_points(cv::Mat & img, int n, std::vector<cv::Point2d> & pts)
{
	//create n points that spread evenly that are in areas of black points...
	int count = 0;

cv::RNG rng_uniform(0);
cv::RNG rng_gaussian(0);
//cv::RNG rng_uniform(time(NULL));
//cv::RNG rng_gaussian(time(NULL));
	cv::Mat visited(img.size(), CV_8U, cv::Scalar::all(0)); //all unvisited

	while (count < n)
	{
		//generate a random point
		int c = (int)floor(img.size().width*rng_uniform.uniform(0.f, 1.f));
		int r = (int)floor(img.size().height*rng_uniform.uniform(0.f, 1.f));

		//decide to keep basic on a probability (black has higher probability)
		float value = img.at<uchar>(r, c)*1.0/255; //black:0, white:1
		float gr = fabs(rng_gaussian.gaussian(0.8));
		if ( value < gr && visited.at<uchar>(r, c) ==0) //keep
		{
			count++;
			pts.push_back(cv::Point(r, c));
			visited.at<uchar>(r,c)=1;
		}
	}

	if (debug)
	{
		cv::Mat tmp = img.clone();
		for (auto& c : pts)
		{
			cv::circle(tmp, cv::Point(c.y, c.x), 2, CV_RGB(0, 0, 255), -1);
		}
		cv::imshow("samples", tmp);
		cv::waitKey();
	}
}

void Hedcut::create_disks(cv::Mat & img, CVT & cvt)
{
	cv::Mat grayscale;
	cv::cvtColor(img, grayscale, CV_BGR2GRAY);

	disks.clear();

	//create disks from cvt
	for (auto& cell : cvt.getCells())
	{
		//compute avg intensity
		unsigned int total = 0;
		unsigned int r = 0, g = 0, b = 0;

		for (auto & resizedPix : cell.coverage)
		{
			cv::Point pix(resizedPix.x / subpixels, resizedPix.y / subpixels);
			total += grayscale.at<uchar>(pix.x, pix.y);
			r += img.at<cv::Vec3b>(pix.x, pix.y)[2];
			g += img.at<cv::Vec3b>(pix.x, pix.y)[1];
			b += img.at<cv::Vec3b>(pix.x, pix.y)[0];

		}
		float avg_v = floor(total * 1.0f/ cell.coverage.size());
        if(disks.size()==100) {
            std::cout << "c = " << cell.coverage.size() << std::endl;
            std::cout << r << std::endl;
        }

		r = floor(r / cell.coverage.size());
		g = floor(g / cell.coverage.size());
		b = floor(b / cell.coverage.size());


		//create a disk
		HedcutDisk disk;
		disk.center.x = cell.site.y; //x = col
		disk.center.y = cell.site.x; //y = row
		disk.color = (black_disk) ? cv::Scalar::all(0) : cv::Scalar(r, g, b, 0.0);
		disk.radius = (uniform_disk_size) ? disk_size : (100 * disk_size / (avg_v + 100));

		//remember
		this->disks.push_back(disk);

	}//end for cell
}

    void Hedcut::create_disks_weighted_color(cv::Mat & img, CVT & cvt)
    {
        cv::Mat grayscale;
        cv::cvtColor(img, grayscale, CV_BGR2GRAY);

        disks.clear();

        unsigned int max_disk_size = 0;
        for (auto& cell : cvt.getCells())
        {
            if (cell.coverage.size() > max_disk_size) {
                max_disk_size = cell.coverage.size();
            }
        }
        //create disks from cvt
        for (auto& cell : cvt.getCells())
        {
            //compute avg intensity
            unsigned int total = 0;
            unsigned int r = 0, g = 0, b = 0;
            float rf = 0.0, gf = 0.0, bf = 0.0;
            float total_weight = 0.0;
            for (auto & resizedPix : cell.coverage)
            {
                cv::Point pix(resizedPix.x / subpixels, resizedPix.y / subpixels);
                total += grayscale.at<uchar>(pix.x, pix.y);

                float w = sqrt(pow(cell.site.x-pix.x,2.0)+pow(cell.site.y-pix.y,2.0));
                float weight = 1.0;
                if(w > 1.0) {
                    weight = 1.0/w;
                }
                //std::cout << weight << std::endl;
                rf += weight*img.at<cv::Vec3b>(pix.x, pix.y)[2];
                gf += weight*img.at<cv::Vec3b>(pix.x, pix.y)[1];
                bf += weight*img.at<cv::Vec3b>(pix.x, pix.y)[0];
                total_weight += weight;
            }
            float avg_v = floor(total * 1.0f/ cell.coverage.size());
            if(disks.size()==100) {
                std::cout << "w = " << total_weight << std::endl;
                std::cout << rf << std::endl;
            }


            r = floor(rf / total_weight);
            g = floor(gf / total_weight);
            b = floor(bf / total_weight);


            //create a disk
            HedcutDisk disk;
            disk.center.x = cell.site.y; //x = col
            disk.center.y = cell.site.x; //y = row
            disk.color = (black_disk) ? cv::Scalar::all(0) : cv::Scalar(r, g, b, 0.0);
            disk.radius = (uniform_disk_size) ? disk_size : (100 * disk_size / (avg_v + 100));

            //remember
            this->disks.push_back(disk);

        }//end for cell
    }
	//done

void Hedcut::create_disks_area_radius(cv::Mat & img, CVT & cvt)
    {
        cv::Mat grayscale;
        cv::cvtColor(img, grayscale, CV_BGR2GRAY);

        disks.clear();

        unsigned int max_disk_size = 0;
        for (auto& cell : cvt.getCells())
        {
            if (cell.coverage.size() > max_disk_size) {
                max_disk_size = cell.coverage.size();
            }
        }
        //create disks from cvt
        for (auto& cell : cvt.getCells())
        {
            //compute avg intensity
            unsigned int total = 0;
            unsigned int r = 0, g = 0, b = 0;
            float rf = 0.0, gf = 0.0, bf = 0.0;
            float total_weight = 0.0;
            for (auto & resizedPix : cell.coverage)
            {

                cv::Point pix(resizedPix.x / subpixels, resizedPix.y / subpixels);
                total += grayscale.at<uchar>(pix.x, pix.y);
                r += img.at<cv::Vec3b>(pix.x, pix.y)[2];
                g += img.at<cv::Vec3b>(pix.x, pix.y)[1];
                b += img.at<cv::Vec3b>(pix.x, pix.y)[0];
            }

            float avg_v = floor(total * 1.0f/ cell.coverage.size());
            r = floor(r / cell.coverage.size());
            g = floor(g / cell.coverage.size());
            b = floor(b / cell.coverage.size());

            //create a disk
            HedcutDisk disk;
            disk.center.x = cell.site.y; //x = col
            disk.center.y = cell.site.x; //y = row
            disk.color = (black_disk) ? cv::Scalar::all(0) : cv::Scalar(r, g, b, 0.0);
            //disk.radius = (uniform_disk_size) ? disk_size : (100 * disk_size / (avg_v + 100));
            float rate = float(cell.coverage.size())/float(max_disk_size);
            //std::cout << avg_v << std::endl;
            //disk.radius = (uniform_disk_size) ? disk_size : (1.0 * disk_size / (1.01-rate  ));
            disk.radius = (uniform_disk_size) ? disk_size :  disk_size *(1.0-rate)*0.5;

            //remember
            this->disks.push_back(disk);

        }//end for cell
    }
	//done


int Hedcut::sample_initial_points(cv::Mat & img, std::vector<cv::Point2d> & pts) {

    std::vector<float> x;
    std::vector<float> y;

    //list<Point> P;
    for (unsigned int i = 0; i < img.size().width; i++) {
        for (unsigned int j = 0; j < img.size().height; j++) {
            int value = img.at<uchar>(i,j);
            //std::cout << value << std::endl;
            if(value < 120) {
                //std::cout << value << std::endl;
                x.push_back(i*1.0);
                y.push_back(j*1.0);
            }
        }
    }

    MatrixXf data(x.size(),2);
    for(int i = 0; i < x.size(); i++) {
        data(i,0) = x[i];
        data(i,1) = y[i];
    }
    std::cout << x.size() << std::endl;


    clustering::DBSCAN<VectorXf,MatrixXf> dbscan(0.005, 2);
    dbscan.fit(data);

    auto labels = dbscan.get_labels();

    std::map<int,float> cx;
    std::map<int,float> cy;
    std::map<int,int> n;

    for(int i = 0; i < x.size(); i++) {
        int l = labels[i];
        if(l < 0)
            continue;
        //std::cout << l << std::endl;
        if(cx.count(l) == 0) {
            cx[l] = x[i];
            cy[l] = y[i];
            n[l] = 1;
        } else {
            cx[l] = cx[l] + x[i];
            cy[l] = cy[l] + y[i];
            n[l] = n[l] + 1;
        }
    }

    int p_count = 0;
    for(auto it = n.begin(); it != n.end() ; it++) {
        int c = (int)floor(cx[it->first]/it->second);
        int r = (int)floor(cy[it->first]/it->second);
        //std::cout << c << " " << r << std::endl;
        pts.push_back(cv::Point(c,r));
        p_count++;
    }

    //std::cout << p_count << std::endl;

    if (debug)
	{
		cv::Mat tmp = img.clone();
		for (auto& c : pts)
		{
			cv::circle(tmp, cv::Point(c.y, c.x), 2, CV_RGB(0, 0, 255), -1);
		}
		cv::imshow("samples", tmp);
		cv::waitKey();
	}
    std::cout << p_count << std::endl;
    return p_count;
}
