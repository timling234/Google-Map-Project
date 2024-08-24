//
// Created by huan3409 on 3/22/24.
//

#include "global.h"

// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given turn_penalty (in seconds) per turn implied by the path.  If there is
// no turn, then there is no penalty. Note that whenever the street id changes
// (e.g. going from Bloor Street West to Bloor Street East) we have a turn.
#define NO_EDGE -1
#define NO_TIME 0

// This function computes the total travel time of the indicated path
double computePathTravelTime(const double turn_penalty,
                             const std::vector<StreetSegmentIdx>& path){
    //initial the variables here
    double total_time = 0;
    int num_of_turns = 0;
    StreetIdx prev_street = segment_time[path[0]].first;

    //compute the path travel time
    for (int i=0; i<path.size(); ++i){

        //time increment
        total_time = total_time + segment_time[path[i]].second;
        if(prev_street != segment_time[path[i]].first){
            num_of_turns ++;
        }
        //update the previous streetID
        prev_street = segment_time[path[i]].first;
    }
    //return total time
    total_time = total_time + turn_penalty * num_of_turns;
    return total_time;
}




// This function computes the travel time from one edge to the other edge of a street segment
double travel_time(StreetSegmentIdx last_edge, StreetSegmentIdx next_edge, double turn_penalty){
    StreetIdx street_last;

    // defensive coding
    if(last_edge != NO_EDGE){
        street_last = getStreetSegmentInfo(last_edge).streetID;
    }else{
        street_last = getStreetSegmentInfo(next_edge).streetID;
    }


    StreetIdx street_next = getStreetSegmentInfo(next_edge).streetID;

    double travel_time = segment_time[next_edge].second;

    // condition for turn
    if (street_last!=street_next){
        travel_time += turn_penalty;
    }

    return travel_time;
}



// This function computes the fastest total time to reach destination if I travel as fastest possible speed
double find_fastest_possible_time(IntersectionIdx my_location, IntersectionIdx destination, double total_travel_time_so_far){
    double distance = findDistanceBetweenTwoPoints(getIntersectionPosition(my_location), getIntersectionPosition(destination));
    double time = distance / max_speed + total_travel_time_so_far;

    return time;
}

// This function finds the path between source and destination
bool bfs_path(int srcID, int destID, double turn_penalty){

    bool found_path = false;

    std::priority_queue<WaveElem, std::vector<WaveElem>, CompareWaveElem> wavefront;
    double fastest = find_fastest_possible_time(srcID,destID, 0);
    wavefront.push(WaveElem (srcID, NO_EDGE, NO_TIME, fastest) );


    while (wavefront.size()!=0){
        WaveElem curr = wavefront.top();

        if(curr.edgeID!=NO_EDGE){
            popped.push_back(curr.edgeID);
        }

        wavefront.pop();
        int currID = curr.nodeID;
        int edgeID = curr.edgeID;

        // check if this path is the shortest route time
        if (curr.sort_time < Nodes[currID].best_route_time ){
            Nodes[currID].reaching_edge = edgeID;
            Nodes[currID].best_route_time =  curr.sort_time;


            if(currID == destID){
                found_path =  true;
                break;
            }

            // compute the travel time for this path
            for(int i=0; i<Nodes[currID].out_going_edge_to_Node.size(); ++i){
                StreetSegmentIdx out_edge = Nodes[currID].out_going_edge_to_Node[i].first;
                int to_node = Nodes[currID].out_going_edge_to_Node[i].second;
                double total_time =  curr.current_route_travel_time + travel_time(edgeID, out_edge, turn_penalty);
                double fastest_ideal_time = find_fastest_possible_time(to_node, destID, total_time);

                wavefront.push(WaveElem(to_node,out_edge, total_time, fastest_ideal_time ));
            }
        }
    }
    return found_path;
}


// This function makes vector into a list
std::vector<StreetSegmentIdx> make_vector_from_list(std::list<StreetSegmentIdx> list){

    std::vector<StreetSegmentIdx> result; // Create an empty vector to store the elements

    // Reserve space for the vector to avoid reallocations (optional but can be efficient)
    result.reserve(list.size());

    // Iterate over the elements of the list and copy them into the vector
    for (const auto& elem : list) {
        result.push_back(elem);
    }

    return result;
}

// This function traces back the path to check for common nodes
std::vector<StreetSegmentIdx> bfsTraceBack (int destID){
    std::list<StreetSegmentIdx> path;
    int currNodeID = destID;
    StreetSegmentIdx prevEdge = Nodes[currNodeID].reaching_edge;

    while (prevEdge != NO_EDGE) {
        path.push_front (prevEdge);

        IntersectionIdx from = getStreetSegmentInfo(prevEdge).from;
        IntersectionIdx to = getStreetSegmentInfo(prevEdge).to;
        IntersectionIdx node_at_other_end_of_prevEdge;

        // check the node
        if (currNodeID == from){
            node_at_other_end_of_prevEdge = to;
        }else{
            node_at_other_end_of_prevEdge = from;
        }

        currNodeID = node_at_other_end_of_prevEdge;
        prevEdge = Nodes[currNodeID].reaching_edge;

    }
    return (make_vector_from_list(path));
}

// This function returns the shortest path between the start and destination
std::vector<StreetSegmentIdx> findPathBetweenIntersections(
        const double turn_penalty,
        const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids){

    for (int i=0; i<Nodes.size(); ++i){

        Nodes[i].best_route_time = 99999;
    }

    std::vector<StreetSegmentIdx> path;

    IntersectionIdx start = intersect_ids.first;
    IntersectionIdx end = intersect_ids.second;

    bool found = bfs_path(start, end, turn_penalty);

    if (found){
        path = bfsTraceBack (end);
    }

    return path;

}
