
#include "global.h"
#include <chrono> // time utilities library
#include <cstdlib>

// global variables
#define TIME_LIMIT 50 // 50s time limit


//int chrono(){
//    auto start_time = std::chrono::high_resolution_clock :: now();
//    bool time_out = false;
//    while(!time_out){
//        myOptimizer();
//        //Fill in this section for my optimizer after we have written our own
//        auto current_time = std::chrono::high_resolution_clock ::now();
//        auto wall_clock = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - start_time);
//        //keep optimizing until within 10% of time limit
//        if (wall_clock.count() > 0.9 * time_out)
//            time_out = true;
//    }
//}



// structs of this cpp file
// This struct initializes the struct of Wave used in the Dijkstra algorithm

struct Wave{
    IntersectionIdx nodeID;
    StreetSegmentIdx edgeID; //ID of edge used to reach NodeID
    double travel_time; //Total travel time to reach node

    Wave (int n, int e, float t1) {nodeID = n; edgeID = e; travel_time = t1;}
};

// This struct is used to store the comparison result of speed between two ends of a single wave
struct CompareWave {
    bool operator()(const Wave& lhs, const Wave& rhs) const {
        // Compare based on my_idea_of_favourite_solution
        return lhs.travel_time > rhs.travel_time;
    }
};


// vectors defined for this cpp file
//depots + pickup + drop off
std::vector<IntersectionIdx> all_POIs;

//the first double represents the travel time, the second represent AllPOI index
//type, data, sorting method
std::vector< std::priority_queue< std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<> > > all_travel_time;

///XXX
double cost (std:: vector <int> path);
std:: vector <int> perturbation (std:: vector <int> current_path, double temp);
double reduceTemp (double Temp);
std:: vector <int> annealing_optimizer (std:: vector <int> current_path);
double time_checker (std::vector<int> current_path);

void explore_src(int srcID, double turn_penalty);
bool check_swap_legal(std::vector<int> path_input, int index1, int index2);
double find_path_travel_time(std::vector<int> path);
double find_shortest_travel_time_packages_POI(int orderID_from);
void init_travel_time(  const float turn_penalty,
                        const std::vector<DeliveryInf>& deliveries,
                        const std::vector<IntersectionIdx>& depots );
bool check_legal(int location_index, std::unordered_set<int> what_is_on_the_truck, std::unordered_set<int> picked_up_list);
std::vector<CourierSubPath> create_courier_path(double turn_time, std::vector<int> intersection_path);
std::vector<int> swap_point(std::vector<int> path_input);

double cost (std:: vector <int> path)  {
    return find_path_travel_time(path);
}

// yet to edit to switch order
std:: vector <int> perturbation (std:: vector <int> current_path, double temp) {
    // need to switch intersections/segments and return
    int num_intersections = current_path.size()-1;
    int seg1_intersection1 = std:: rand () % (num_intersections+1);
    int seg2_intersection1 = std:: rand () % (num_intersections+1);
    int seg1_intersection2 = seg1_intersection1+1;
    int seg2_intersection2 = seg2_intersection1+1;

    // when temp > 0.3, use street segments
    while (!check_swap_legal (current_path, seg1_intersection1, seg2_intersection1) || !check_swap_legal(current_path, seg1_intersection2, seg2_intersection2)) {
        seg1_intersection1 = std:: rand () % (num_intersections+1);
        seg2_intersection1 = std:: rand () % (num_intersections+1);
        seg1_intersection2 = seg1_intersection1+1;
        seg2_intersection2 = seg2_intersection1+1;
    }

    // when temp higher, better acceptance (use segments)
    if (temp > 0.3) {
        std:: swap (current_path[seg1_intersection1], current_path[seg2_intersection1]);
        std:: swap (current_path[seg2_intersection1], current_path[seg2_intersection2]);
    } else { // when temp lower, less acceptance, use intersections
        std:: swap (current_path[seg1_intersection1], current_path[seg2_intersection1]);
    }
    return current_path;
}

double reduceTemp (double Temp) {
    // need to write logic to accept/deny the solution
    return Temp * 0.975;
}
std:: vector <int> annealing_optimizer (std:: vector <int> current_path) {
    /*
    S = initialSolution // pick a random path or smarter path
    C = cost (S); // cost should be a fast function (low complexity) and should be travel time (might add additional conditions)
    T = temperature limit

    while (solution is changing) {
        for (# of perturbations) {
            S_new = perturb(S); // move a delivery order or swap order
            C_new = cost (S);
            delta_C = C_new - C;

            if (delta_C < 0 || random (0,1) < exp((-1) * delta_C / T)) {
                S = S_new;
                C = C_new;
            }
        } // end perturbations

        T = reduceTemp (T);
    }
     */
    // out while loop if solution is not changing/ run our of cpu time (50s)
    // should return the optimized path

    double Cost = cost (current_path);
    double Temp = 1;
    int num_perturb = current_path.size();
    std:: vector <int> path_new;

    while (path_new != current_path || num_perturb < 0) {
        for (int i = 0; i < num_perturb; ++i) {
             num_perturb--;
            path_new = perturbation (current_path, Temp);
            double Cost_new = cost(path_new);
            double delta_Cost = Cost_new - Cost;

            if (delta_Cost < 0 || (rand()/RAND_MAX < exp((-1) * delta_Cost / Temp))) {
                current_path = path_new;
                Cost = Cost_new;
            }
        }
        Temp = reduceTemp (Temp);
    }
    return path_new;
}

// This function checks if the algorithm meets the time limit of 50s
double time_checker (std::vector<int> current_path){
    auto start_time = std::chrono::high_resolution_clock :: now();
    bool time_out = false;
    double time;

    while(!time_out){
        annealing_optimizer(current_path);
        //Fill in this section for my optimizer after we have written our own
        auto current_time = std::chrono::high_resolution_clock ::now();
        auto wall_clock = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - start_time);

        //keep optimizing until within 10% of time limit
        if (wall_clock.count() > 0.9 * TIME_LIMIT) {
            time_out = true;
            time = wall_clock.count();
            return wall_clock.count();
        }
    }
    // fall out of while loop, valid solution, return value for this
    return time;
// pass in a solution


}

///XXX

//Helper functions below

//Find travel time between two intersections using dijkstra's algorithm. This func is a helper to the next one.
// Modify the dijkstra's algorithm in M3 where we run through the whole map once to get all travel time to POIs
// from one POI. Run for all POIs. Only need to run for order N complexity.
// Return the vector beck to travel_time.

//The destination ID is all POIs except the source
void explore_src(int srcID, double turn_penalty){

    for (int i=0; i<Nodes.size(); ++i){

        Nodes[i].best_route_time = 99999;
    }

    std::priority_queue<Wave, std::vector<Wave>, CompareWave> wave;
    wave.push(Wave (srcID, -1, 0) );


    while (wave.size()!=0){

        Wave curr = wave.top();
        wave.pop();
        int currID = curr.nodeID;
        int edgeID = curr.edgeID;

        // check if this path is the shortest route time
        if (curr.travel_time < Nodes[currID].best_route_time ){

            Nodes[currID].reaching_edge = edgeID;
            Nodes[currID].best_route_time =  curr.travel_time;

            // compute the travel time for this path
            for(int i=0; i<Nodes[currID].out_going_edge_to_Node.size(); ++i){

                StreetSegmentIdx out_edge = Nodes[currID].out_going_edge_to_Node[i].first;
                int to_node = Nodes[currID].out_going_edge_to_Node[i].second;
                double total_time =  curr.travel_time + travel_time(edgeID, out_edge, turn_penalty);

                wave.push(Wave(to_node,out_edge, total_time ));
            }
        }
    }
}

std::vector< std::vector< std::pair<double, int> > > time_reference_table;

void init_travel_time(  const float turn_penalty,
                        const std::vector<DeliveryInf>& deliveries,
                        const std::vector<IntersectionIdx>& depots ){
    //this has to do with load map, may be unnecessary during auto tester
    init_nodes();
    std::vector<IntersectionIdx> ().swap(all_POIs);

    for (int i=0; i<deliveries.size(); ++i){

        IntersectionIdx pickUp = deliveries[i].pickUp;
        IntersectionIdx dropOff = deliveries[i].dropOff;
        all_POIs.push_back(pickUp);
        all_POIs.push_back(dropOff);
        //if is /2 then is pickup, if is not /2 than is drop off
    }

    for(int i=0; i<depots.size(); ++i){

        all_POIs.push_back(depots[i]);
        //If i>deliveries.size()*2, then they are all depots
    }
    std::vector< std::priority_queue< std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<> > > ().swap(all_travel_time);
    std::vector< std::vector< std::pair<double, int> > >().swap(time_reference_table);

    all_travel_time.resize(all_POIs.size());
    time_reference_table.resize(all_POIs.size());

    for(int i=0; i<all_POIs.size(); ++i){
        //Remember that this has return type bool, check for edge case in the future in case of path no found between intersections
        //explore_src also haven't implemented when to return true or false yet
        explore_src(all_POIs[i], turn_penalty);

        //from this pick up location to all POIs path
        for(int j=0; j<all_POIs.size();++j){

            std::vector<StreetSegmentIdx> path_to_this_POI = bfsTraceBack(all_POIs[j]);

            double time_to_this_poi = computePathTravelTime(turn_penalty, path_to_this_POI);
            time_reference_table[i].push_back(std::make_pair(time_to_this_poi,j));
            all_travel_time[i].push(std::make_pair(time_to_this_poi,j));
        }

    }

}


// This function returns the shortest travel time for a package at initial location
double find_shortest_travel_time_packages_POI(int orderID_from){
    return all_travel_time[orderID_from].top().second;
}


// This function returns the legality of path between 2 nodes
bool check_legal(int location_index, std::unordered_set<int> what_is_on_the_truck, std::unordered_set<int> picked_up_list){
    //Changed encoding of AllPOI time format.
    //0-delivery.size*2-1 is delivery range, where even is pickup, odd is drop off
    //delivery.size*2 - AllPOI_time.size() index are depots

    // if is pick up location, reached end of visited pickup location, valid
    if ((location_index % 2 == 0) && (picked_up_list.find(location_index) == picked_up_list.end())) {
        return true;
    } else if ((location_index % 2 == 0)) {
        return false;
    }

    // if is delivery location, delivery item == item on truck, valid

    for (int i = 0; i < what_is_on_the_truck.size(); ++i) {
        if (what_is_on_the_truck.find(location_index) != what_is_on_the_truck.end()) {
            return true;
        }
    }

    return false;
}

std::vector<CourierSubPath> create_courier_path(double turn_time, std::vector<int> intersection_path){

    std::vector<CourierSubPath> path;
    if (intersection_path.size()==0){
        return path;
    }

    IntersectionIdx start = all_POIs[intersection_path[0]];

    for (int i=1; i<intersection_path.size(); ++i){
        IntersectionIdx next = all_POIs[intersection_path[i]];
        if (start == next){
            continue;
        }
        CourierSubPath courier_path;
        courier_path.intersections=std::make_pair(start,next);
        courier_path.subpath = findPathBetweenIntersections(turn_time, std::make_pair(start, next));
        path.push_back(courier_path);
        start=next;
    }
    return path;
}

bool check_swap_legal(std::vector<int> path_input, int index1, int index2) {
    bool cond1 = false;
    bool cond2 = false;

    //check if it is pick up or drop off
    if (path_input[index1] % 2 == 0 ) {
        //is pick up
        auto index1_pickup_location = path_input.begin() + index2;
        int index1_dropoff_value = path_input[index1] + 1;
        auto index1_dropoff_location = std::find(path_input.begin(), path_input.end(), index1_dropoff_value);
        if (index1_pickup_location < index1_dropoff_location){
            cond1 = true;
        }

    } else {
        //is drop off
        auto index1_dropoff_location = path_input.begin() + index2;
        int index1_pickup_value = path_input[index1] - 1;
        auto index1_pickup_location = std::find(path_input.begin(), path_input.end(), index1_pickup_value);
        if (index1_pickup_location < index1_dropoff_location) {
            cond1 = true;
        }
    }

    if (path_input[index2] % 2 == 0) {
        //is pick up

        auto index2_pickup_location = path_input.begin() + index1;
        int index2_dropoff_value = path_input[index2] + 1;
        auto index2_dropoff_location = std::find(path_input.begin(), path_input.end(), index2_dropoff_value);
        if (index2_pickup_location < index2_dropoff_location){
            cond2 = true;
        }

    } else {
        //is drop off
        auto index2_dropoff_location = path_input.begin() + index1;
        int index2_pickup_value = path_input[index2] - 1;
        auto index2_pickup_location = std::find(path_input.begin(), path_input.end(), index2_pickup_value);
        if (index2_pickup_location < index2_dropoff_location) {
            cond2 = true;
        }
    }

    if (cond2 && cond1){
        return true;
    }
    return false;
}

//randomly to swap two points
std::vector<int> swap_point(std::vector<int> path_input){
    // Check if the vector has enough elements to swap (excluding the first and last elements)
    if (path_input.size() <= 3) {
        return path_input;
    }

    std::srand(std::time(0));

    bool legal = false;
    while (!legal) {
        // Generate two random indices, excluding the first and last elements
        int idx1 = 1 + std::rand() % (path_input.size() - 2);
        int idx2 = 1 + std::rand() % (path_input.size() - 2);
        while (idx1 == idx2) {  // Ensure idx2 is different from idx1
            idx2 = 1 + std::rand() % (path_input.size() - 2);
        }

        // Swap the elements at idx1 and idx2
        std::swap(path_input[idx1], path_input[idx2]);

        // Check if the swap is legal
        legal = check_swap_legal(path_input, idx1, idx2);

        // If not legal, swap them back
        if (!legal) {
            std::swap(path_input[idx1], path_input[idx2]);
        }
    }

    return path_input;
}

double find_path_travel_time(std::vector<int> path){
    double total_time;
    int prev_idx = path[0];
    for(int i=1; i<path.size(); ++i){
        int next_idx = path[i];
        total_time += time_reference_table[prev_idx][next_idx].first;
    }
    return total_time;
}

//This is the function called by the auto tester, returning the courier path
std::vector<CourierSubPath> travelingCourier(
        const float turn_penalty,
        const std::vector<DeliveryInf>& deliveries,
        const std::vector<IntersectionIdx>& depots){

    std::vector<std::vector<int>> all_paths_multi_start;
    std::vector<int> aultimate_courier_path;

    //initialize orders
    init_travel_time(turn_penalty, deliveries, depots);

    //Loop through all starting points, for each starting point, we find the closest delivery POI to it
    int smallest_time = 999999999;
    int last_idx = 0;

    for(int i=deliveries.size()*2; i < all_POIs.size(); ++i){
        std::vector<int> path_from_this_starting_depot;
        std::vector<std::pair<double, int>> disgard_from_search;
        double travel_time = 0;
        int start_index = i;
        int my_index = i;
        path_from_this_starting_depot.push_back(my_index);

        std::vector<int> visited(all_POIs.size(), 1);
        std::vector<bool> picked_or_dropped(all_POIs.size(), false);

//        std::cout<<"------------Starting from " << all_POIs[my_index] << "--------------"<<std::endl;

        //keep searching for next closest legal point
        while (visited.size()>depots.size()){
            bool valid = false;
            int to_index = all_travel_time[my_index].top().second;
            //Even index, is Pick up
            if ( (to_index< deliveries.size()*2) && to_index % 2 == 0){
                int pick_up_idx = to_index;
                if (!picked_or_dropped[pick_up_idx]){
                    picked_or_dropped[pick_up_idx] = true;
                    valid = true;
                    path_from_this_starting_depot.push_back(pick_up_idx);
                    visited.pop_back();

                    //Find travel time
                    travel_time += time_reference_table[my_index][pick_up_idx].first;

                    //Put what we disgard during the last check back into check database, and clear the disgard list
                    for(int j=0; j<disgard_from_search.size(); ++j){
                        all_travel_time[my_index].push(disgard_from_search[j]);
                    }
                    std::vector<std::pair<double, int>>().swap(disgard_from_search);

                    //Break if we found all points to deliver
                    if (visited.size()==depots.size()){
                        last_idx = pick_up_idx;
                        break;
                    }

                    my_index = to_index;
//
                }
            //Odd index, is drop off
            }else if ( (to_index< deliveries.size()*2) && to_index % 2 == 1){
                int pick_up_idx = to_index -1;
                int drop_index = to_index;
                if (picked_or_dropped[pick_up_idx] && !picked_or_dropped[drop_index]){
                    picked_or_dropped[drop_index] = true;
                    valid = true;
                    path_from_this_starting_depot.push_back(drop_index);
                    visited.pop_back();

                    //Find travel time
                    travel_time += time_reference_table[my_index][drop_index].first;

                    for(int j=0; j<disgard_from_search.size(); ++j){
                        all_travel_time[my_index].push(disgard_from_search[j]);
                    }
                    std::vector<std::pair<double, int>>().swap(disgard_from_search);
                    //Break if we found all points to deliver
                    if (visited.size()==depots.size()){
                        last_idx = pick_up_idx;
                        break;
                    }

                    my_index = to_index;
//
                }
            }
            //If not valid, pop and check next
            if (!valid){
                disgard_from_search.push_back(all_travel_time[my_index].top());
                all_travel_time[my_index].pop();
            }
        }
        //Form back close loop
        path_from_this_starting_depot.push_back(start_index);
        travel_time += time_reference_table[last_idx][start_index].first;
        //Cout the path

        if (travel_time<smallest_time){
            smallest_time = travel_time;
            aultimate_courier_path = path_from_this_starting_depot;
        }
//        for(int j=0; j< path_from_this_starting_depot.size(); ++j){
//            std::cout<<all_POIs[path_from_this_starting_depot[j]]<<std::endl;
//        }

        all_paths_multi_start.push_back(path_from_this_starting_depot);

    }




    //This is easier to run in parallel
    for(int i=0; i<all_paths_multi_start.size(); ++i){
        int iteration =0;
        while(/*time not reached (45s - used )/number of start point*/iteration<50){
            std::vector<int> path = all_paths_multi_start[i];
            double old_time = find_path_travel_time(path);
            std::vector<int> new_path = swap_point(path);
            double new_time = find_path_travel_time(new_path);
            if(new_time<old_time){
                all_paths_multi_start[i] = new_path;
                iteration++;
                std::cout<<"Found better time "<< new_time<<std::endl;
            }
            if(new_time<smallest_time){
                aultimate_courier_path = new_path;
            }
        }

    }

    //now we have 5 seconds left, we compare path travel time and return the least

//    for(int i=0; i<all_paths_multi_start.size(); ++i){
//        double time = find_path_travel_time(all_paths_multi_start[i]);
//        if (time<smallest_time){
//            smallest_time = time;
//            aultimate_courier_path = all_paths_multi_start[i];
//        }
//    }





    std::vector<CourierSubPath> solution_path = create_courier_path(turn_penalty, aultimate_courier_path);

    return solution_path;
//    //initialize orders
//    init_travel_time(turn_penalty,deliveries);
//    //We simulate start from all depots
//    for (int i=0; i<depots.size(); ++i){
//
//        // we find closes delivery POI from the depot
//    }
//    std:: vector <CourierSubPath> current_path;
//    if (time_checker()) {
//        // return solution if time of run is within 50s
//    }

}
