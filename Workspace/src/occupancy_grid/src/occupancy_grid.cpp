/*
NOTE:
Duo Pointcloud2 data follows (z = forward, x = left, y = down)

*/

#include "ros/ros.h"
#include <ros/console.h>
#include <math.h>

//std and ext. lib msg types
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>
#include <nav_msgs/OccupancyGrid.h>

//custom msg types
#include "occupancy_grid/OccupancyGrid.h"
#include "occupancy_grid/OccupancyGridHeader.h"
#include "occupancy_grid/GridDataDimension.h"

//accessor for Occupancy Grid Message
float& oGridDataAccessor (occupancy_grid::OccupancyGrid& oGrid, unsigned int i, unsigned int j, unsigned int  k) {
	return oGrid.data[i*oGrid.dataDimension[0].stride + j*oGrid.dataDimension[1].stride + k*oGrid.dataDimension[2].stride];
}

//resolution is in terms of side length of a grid cell
//z, x max is the max range of the camera (in one direction)
typedef struct 
{
   float zMax;
   float xMax;
   float yOffset;
   float resolution;
   int rate;
   int queue_size;

   float mappingScalar;
   float mappingNormalizer;
} gridParams;


class OccupancyGrid {
	public:
		OccupancyGrid(){
			//get params from yaml file
			ros::param::get("zMax", m_gridParams.zMax);
			ros::param::get("xMax", m_gridParams.xMax);
			ros::param::get("yOffset", m_gridParams.yOffset);
			ros::param::get("resolution", m_gridParams.resolution);
			ros::param::get("rate", m_gridParams.rate);
			ros::param::get("queue_size", m_gridParams.queue_size);
			ros::param::get("debug", m_debug);
			ros::param::get("Scalar", m_gridParams.mappingScalar);
			ros::param::get("Normalizer", m_gridParams.mappingNormalizer);

			//hard coded values, need to fix later if figure out why rosparam does not work for these two params
			//m_gridParams.mappingScalar = 1.0;
			//m_gridParams.mappingNormalizer = 5.0;
			
			
			std::string pcl2TopicName;
			ros::param::get("PCL2TopicName", pcl2TopicName);

			//set grid params
			m_gridZSize = m_gridParams.zMax/m_gridParams.resolution + 1;
			m_gridXSize = m_gridParams.xMax*2/m_gridParams.resolution + 1;

			//make sure camera is in centre of a cell
			if (m_gridXSize % 2 == 0) {
				m_gridXSize++;	
			}
			m_gridCameraZ = 0;
			m_gridCameraX = m_gridXSize/2;
			
			//sub & pub
			m_sub = m_n.subscribe(pcl2TopicName, m_gridParams.queue_size, &OccupancyGrid::callback, this);
			m_pub = m_n.advertise<occupancy_grid::OccupancyGrid>("/OccupancyGrid", m_gridParams.queue_size);

			//Since rviz will flash back and forth between multiple messages published by the same topic, using an array of topics
			m_pub_rviz[0] = m_n.advertise<nav_msgs::OccupancyGrid>("/OccupancyGridCells1", m_gridParams.queue_size);
			m_pub_rviz[1] = m_n.advertise<nav_msgs::OccupancyGrid>("/OccupancyGridCells2", m_gridParams.queue_size);
			m_pub_rviz[2] = m_n.advertise<nav_msgs::OccupancyGrid>("/OccupancyGridCells3", m_gridParams.queue_size);
			m_pub_rviz[3] = m_n.advertise<nav_msgs::OccupancyGrid>("/OccupancyGridCells4", m_gridParams.queue_size);
		}

		void callback(const sensor_msgs::PointCloud2 input);
		
		gridParams getGridParams() const{
			return m_gridParams;
		}
		
	private:
		ros::NodeHandle m_n;	
		ros::Subscriber m_sub;
		ros::Publisher m_pub;	
		ros::Publisher m_pub_rviz[4];
				
		int m_gridZSize; 
		int m_gridXSize;
		int m_gridCameraZ;
		int m_gridCameraX;

		bool m_debug;

		gridParams m_gridParams;
};

void OccupancyGrid::callback(const sensor_msgs::PointCloud2 input) {
	occupancy_grid::OccupancyGrid output;

	output.header.cameraZMax = m_gridParams.zMax;
	output.header.cameraXMax = m_gridParams.xMax;
	output.header.gridResolution = m_gridParams.resolution;
	output.header.gridCameraZ = m_gridCameraZ;
	output.header.gridCameraX = m_gridCameraX;
	output.header.cameraYOffset = m_gridParams.yOffset;

	output.dataDimension.push_back(occupancy_grid::GridDataDimension());
	output.dataDimension.push_back(occupancy_grid::GridDataDimension());
	output.dataDimension.push_back(occupancy_grid::GridDataDimension());

	output.dataDimension[0].label = "Z(Forward)";
	output.dataDimension[1].label = "X(Left)";
	output.dataDimension[2].label = "Points Detected // Avg. Height // Max Height // Min Height";

	output.dataDimension[0].size = m_gridZSize;
	output.dataDimension[1].size = m_gridXSize;
	output.dataDimension[2].size = 4;

	output.dataDimension[0].stride = output.dataDimension[1].size * output.dataDimension[2].size;
	output.dataDimension[1].stride = output.dataDimension[2].size;
	output.dataDimension[2].stride = 1;

	output.data.resize(output.dataDimension[0].size * output.dataDimension[1].size * output.dataDimension[2].size, 0);

	std::vector<std::vector<std::vector<float>>> oGridPoints;
	oGridPoints.resize(m_gridZSize, std::vector<std::vector<float>>(m_gridXSize));

	sensor_msgs::PointCloud2ConstIterator<float> iterX (input, "x");
	sensor_msgs::PointCloud2ConstIterator<float> iterY (input, "y");
	sensor_msgs::PointCloud2ConstIterator<float> iterZ (input, "z");

	ROS_INFO_STREAM(std::endl<<"New Frame Detected"<<std::endl);
	if (m_debug) {
		ROS_DEBUG_STREAM(std::endl << "Input Data & Conversion" << std::endl);
	}
	for (;iterZ != iterZ.end(); ++iterX, ++iterY, ++iterZ) {
		float height = -1 * (*iterY) + m_gridParams.yOffset;
		int convZ = (*iterZ)/m_gridParams.resolution;
		int convX = 1*(*iterX)/m_gridParams.resolution + (m_gridXSize/2.0);
		if (m_debug) {
			ROS_DEBUG_STREAM(std::fixed << std::setprecision(3) << "Z: " << *iterZ << "\tX: " << *iterX << "\tY: " << *iterY << "\tconvZ: " << convZ << "\tconvX: " << convX << "\tHeight: " << height << std::endl);
		}	
		if (convZ < m_gridZSize && convX < m_gridXSize && convZ >= 0 && convX >= 0 ) {      //invalid bounds error check
			oGridPoints[convZ][convX].push_back(height);
		}
		else{
			ROS_WARN_STREAM("Point Detected Out of Occupancy Grid Limits"<< std::endl);
		}
	}

	for(int z = 0; z < m_gridZSize; z++){
		for (int x = 0; x < m_gridXSize; x++){
			std::sort(oGridPoints[z][x].begin(), oGridPoints[z][x].end(), std::greater<float>());
			
			//point count
			oGridDataAccessor(output, z, x, 0) = oGridPoints[z][x].size();	

			float sum = 0;
			if (oGridDataAccessor(output, z, x, 0)!=0) {
				//avg height
				for(float a : oGridPoints[z][x]) {
					sum += a;
				}
				oGridDataAccessor(output, z, x, 1) = sum / oGridDataAccessor(output, z, x, 0);

				//max height
				sum = 0;
				for(int i = 0; i < oGridPoints[z][x].size() * 0.05; i++) {
					sum += oGridPoints[z][x][i];
				}
				oGridDataAccessor(output, z, x, 2) = sum / (unsigned int)(oGridPoints[z][x].size() * 0.05 + 1);

				//min height
				sum = 0;
				for(int i = 0, index = oGridPoints[z][x].size() - 1; i < oGridPoints[z][x].size() * 0.05; i++, index--) {
					sum += oGridPoints[z][x][index];
				}
				oGridDataAccessor(output, z, x, 3) = sum / (unsigned int)(oGridPoints[z][x].size() * 0.05 + 1); 
			}
		}
	}

	m_pub.publish(output);

	//ouput to rviz to visualize, publishes 4 messages, each corresponds to one element of the third dimension of the occupancy grid message(ie. point count, avg, max, min heights)
	for(int i = 0; i<output.dataDimension[2].size; i++)
	{
		nav_msgs::OccupancyGrid gridcells;
		gridcells.header.frame_id="/duo3d_camera";
		gridcells.header.stamp=ros::Time::now();
		gridcells.info.resolution = 1.0;
		gridcells.info.width=m_gridXSize;
		gridcells.info.height=m_gridZSize;
		//set the view to topdownortho in rviz, the display will be in the corretc orientation. Buttom left: point count, buttom right: avg, top left: max, top right: min
		gridcells.info.origin.position.x = i%2 * m_gridXSize + i%2*10.0;
	    	gridcells.info.origin.position.y = i/2 * m_gridZSize + i/2*10.0;
	    	gridcells.info.origin.position.z = 0.0;
	    	gridcells.info.origin.orientation.x = 0.0;
	    	gridcells.info.origin.orientation.y = 0.0;
	    	gridcells.info.origin.orientation.z = 0.0;
	    	gridcells.info.origin.orientation.w = 1.0;
		
		gridcells.data.resize(m_gridXSize * m_gridZSize, 0.0);
		//map the occupancy grid values to standard 0-100 value for display
		for(int x = 0; x < m_gridXSize; x++){
			for (int z = 0; z < m_gridZSize; z++){

				
				float cost = oGridDataAccessor(output, z, x, i);
				//gridcells.data[x*m_gridZSize + z] = cost;
				//ROS_ERROR_STREAM ( "1normalizer: "<< (float)m_gridParams.mappingNormalizer << " scalar: "<<m_gridParams.mappingScalar << " mpas to:"<< (cost/m_gridParams.mappingNormalizer/2.0*100*m_gridParams.mappingScalar) );

				

				//since number of points is typically very large
				if(i==0)
				{
					cost /= 1.0;
					int map_value = cost ; 
					
					if (map_value > 100)
						map_value = 100;

					gridcells.data[z*m_gridXSize + x] = map_value;
					
					
				}

				else
				{
					if(cost <=0)
						cost=0;
					else{
					//cost += m_gridParams.mappingNormalizer;
					cost = int ( cost/m_gridParams.mappingNormalizer *100*m_gridParams.mappingScalar);
					
					if(cost >= 100)
						cost = 100;
					gridcells.data[z*m_gridXSize + x] = cost;
					}
				} 
				
				
				
				
			}
		}
		
		m_pub_rviz[i].publish(gridcells);
	}


	std::stringstream debugString;
	debugString << std::fixed << std::setprecision(3);
	
	if (m_debug) {
		std::stringstream debugString;
		debugString << std::fixed << std::setprecision(3);
		
		debugString << std::endl << "Points Detected" << std::endl;
		for(int z = m_gridZSize-1; z >= 0; z--){
			for (int x = 0; x < m_gridXSize; x++){
				debugString << std::setw(8)<< (unsigned int)oGridDataAccessor(output, z, x, 0);			
			}
			debugString << std::endl;
		}
		ROS_DEBUG_STREAM(debugString.str());
	
		debugString.str("");
		debugString << std::endl << "Average Height" << std::endl;
		for(int z = m_gridZSize-1; z >= 0; z--){
			for (int x = 0; x < m_gridXSize; x++){
				debugString << oGridDataAccessor(output, z, x, 1) << "\t";
			}
			debugString << std::endl;
		}
		ROS_DEBUG_STREAM(debugString.str());
		
		debugString.str("");
		debugString << std::endl << "Average Max Height (Highest 5%)" << std::endl;
		for(int z = m_gridZSize-1; z >= 0; z--){
			for (int x = 0; x < m_gridXSize; x++){
				debugString << oGridDataAccessor(output, z, x, 2) << "\t";
			}
			debugString << std::endl;
		}
		ROS_DEBUG_STREAM(debugString.str());
		
		debugString.str("");
		debugString << std::endl << "Average Min Height (Lowest 5%)" << std::endl;
		for(int z = m_gridZSize-1; z >= 0; z--){
			for (int x = 0; x < m_gridXSize; x++){
				debugString << oGridDataAccessor(output, z, x, 3) << "\t";
			}
			debugString << std::endl;
		}
		ROS_DEBUG_STREAM(debugString.str());	
	}
}
	
int main(int argc, char **argv) {
	ros::init(argc, argv, "OccupancyGrid");

	OccupancyGrid o;

	ros::Rate rate(o.getGridParams().rate);
	
	if( ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug) ) {
   		ros::console::notifyLoggerLevelsChanged();
	}

	while(ros::ok()) {
		ros::spinOnce();
		//ros::Rate(rate).sleep();
	}
}
