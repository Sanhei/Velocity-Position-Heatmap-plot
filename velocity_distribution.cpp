#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <iterator>
#include <cmath>
#include <cstdio>
#include <algorithm>
namespace fs = std::experimental::filesystem;

int Count_variable_inbins(std::vector<double> variable, double bin_bottom, double bin_up)
{
        int count = 0;
        for(unsigned int i=0; i<variable.size(); i++){
                if(bin_bottom<variable[i] & variable[i]<bin_up)
                {
                        count += 1;
                }
        }
        return count;
}


int main(int argc, char **argv)
{
        // Read trajectory file.
        //std::cout<<ss.substr(0, 8);
        double time_interval = 0.001;
        std::cout<<"Time interval is "<<time_interval<<std::endl;

        // Read trajectory
        std::ifstream inputfile;
        inputfile.open("./timestep_changing/0.000100.txt");
        if(!inputfile)
        {
                std::cout<<"unable to openfile";
                exit(1);
        }
        double x, y;
        //Since in the trajectory one column is time, another is position;
        std::vector<double> trajectory;
        std::vector<double> velocity;
        std::cout<<"Reading trajectory"<<std::endl;

        while(inputfile>>x)
        {
                trajectory.push_back(x);
        }
        // The system needs to be relaxed, we collect data after a certain time
        int relaxation_time = 300;
        trajectory.erase(trajectory.begin(), 
                        trajectory.size()>relaxation_time ? trajectory.begin() + relaxation_time : trajectory.end());
        // In case the relaxation time is bigger than the whole traj

        //Velocity: calculate the middle point
        for(unsigned int i=0; i<trajectory.size()-2; i++)
        {
                velocity.push_back((trajectory[i+2]-trajectory[i])/time_interval/2);
        }
        std::cout<<"Velocity size is "<<velocity.size()<<std::endl;

        // Start heatmaps
        // For x bins and size;
        // discritize from -2 to 2 into 100 bins;

        std::vector<double> position_bins;
        for(int i = 0; i<101; i++)
        {
                position_bins.push_back(-2+0.04*i);
        }
        // 0.04 is the binsize of position

        // Put the velocity into each bins
        std::vector<std::vector<double>> v_IN_xBins(100);

        // This 2D vector will arrange the velocity into position bins
        /*
         * The trajectory always start at x=-1;
         * So we directly assign the start bin where -1 is
         */
        std::cout<<"Velocities assign into the position bins"<<std::endl;
        int start_bins = 0;
        for(int i=0; i<position_bins.size(); i++)
        {
               if(position_bins[start_bins]<trajectory[1])
                       start_bins += 1;
        }

        std::cout<<"First bin index is "<<start_bins<<std::endl;
        int bin_index = start_bins;
        
        std::cout<<"Save velocity into position bins"<<std::endl;
        for(unsigned int i=1; i<trajectory.size()-1; i++)
        {
                if(trajectory[i]-trajectory[i-1]>0){
                        while(position_bins[bin_index]<trajectory[i])
                        {
                                bin_index += 1;
                        }
                }
                else{
                        while(trajectory[i]<position_bins[bin_index])
                        {
                                bin_index -= 1;
                        }
                }
                v_IN_xBins[bin_index].push_back(velocity[i-1]);
        }
        
        // Velocity bins construct
        std::vector<double> velocity_bins;
        double max_velocity = *std::max_element(velocity.begin(), velocity.end());
        double min_velocity = *std::min_element(velocity.begin(), velocity.end());
        if (-min_velocity>max_velocity)
        {
                max_velocity = -min_velocity;
        }
        else
        {
                min_velocity = -max_velocity;}

        // Set the symmetry boundary
        int bin_num_v = 100;
        double size_of_velocity_bins = (max_velocity-min_velocity)/bin_num_v;
        // Because the boundary, need to add 2 additional data points
        for(int i=0; i<bin_num_v+1; i++)
        {
                velocity_bins.push_back(min_velocity + i*size_of_velocity_bins);
        }

        // Save the file

        bool plot_V_in_P = true;
        std::vector<std::vector<int>> heatmap(100);
        // Plot a heatmap
        int count_map = 0;
        if(plot_V_in_P)
        {
               for(unsigned int x=0; x<100; x++)
                /* x means position
                 * v means velocity
                 */
               {
                        for(unsigned int v=0; v<bin_num_v-1; v++)
                        // Need to be minus 2, because boundary is -1
                        {
                                count_map = Count_variable_inbins(v_IN_xBins[x], velocity_bins[v], velocity_bins[v+1]);
                                if(count_map<0)
                                        count_map=0;
                                heatmap[x].push_back(count_map);
                        }
               }
        }

        
        std::ofstream V_BINS;
        V_BINS.open("./V_bins.txt");
        for(int v=0; v<bin_num_v+1; v++)
        {
                V_BINS<<velocity_bins[v]<<std::endl;
        }
        V_BINS.close();

 
        std::ofstream X_BINS;
        X_BINS.open("./X_bins.txt");
        for(int x=0; x<position_bins.size(); x++)
        {
                X_BINS<<position_bins[x]<<std::endl;
        }
        X_BINS.close();






        std::ofstream HEATMAP_OUT;
        HEATMAP_OUT.open("./heat_out.txt");
        for(int x=0; x<100; x++)
        {
                for(int v=0; v<bin_num_v; v++)
                {
                        HEATMAP_OUT<<heatmap[x][v]<<" ";
                }
                HEATMAP_OUT<<std::endl;
        }
        HEATMAP_OUT.close();

        /*
        std::vector<double> velocity_bins;
        double max_velocity = *std::max_element(velocity.begin(), velocity.end());
        double min_velocity = *std::min_element(velocity.begin(), velocity.end());
        if (abs(min_velocity)>max_velocity)
                max_velocity = -min_velocity;
        else
                min_velocity = -max_velocity;

        // Set the symmetry boundary
        double size_of_velocity_bins = (max_velocity-min_velocity)/100;
        for(int i=0; i<101; i++)
                velocity_bins.push_back(min_velocity + i*size_of_velocity_bins);

        */
}
