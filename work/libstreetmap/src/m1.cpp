/* 
 * Copyright 2024 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "global.h"

// loadMap will be called with the name of the file that stores the "layer-2"
// map data accessed through StreetsDatabaseAPI: the street and intersection 
// data that is higher-level than the raw OSM data). 
// This file name will always end in ".streets.bin" and you 
// can call loadStreetsDatabaseBIN with this filename to initialize the
// layer 2 (StreetsDatabase) API.
// If you need data from the lower level, layer 1, API that provides raw OSM
// data (nodes, ways, etc.) you will also need to initialize the layer 1 
// OSMDatabaseAPI by calling loadOSMDatabaseBIN. That function needs the 
// name of the ".osm.bin" file that matches your map -- just change 
// ".streets" to ".osm" in the map_streets_database_filename to get the proper
// name.

// Initialize value here
std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;
std::vector<std::vector<IntersectionIdx>> street_intersections;
std::vector<std::vector<StreetSegmentIdx>> street_segments;
std::vector<std::vector<StreetIdx>> intersection_street_id;

//Build OSM Database Functions

//buildNodeData vectors
std::vector<const OSMNode*> NodeIndex_NodeId;
std::vector<OSMID> NodeIndex_OSMId;

//buildWayData vectors
std::vector<const OSMWay*> WayIndex_WayId;
std::vector<OSMID> WayIndex_OSMId;

//load_Way_LatLon_of_Nodes and load_Way_WayLength vectors
std::vector<std::vector<LatLon>> Way_LatLon_of_Nodes;
std::vector<std::vector<double>> Way_WayLength;

//m2.cpp
std::vector<FeatureIdx> parks;
std::vector<FeatureIdx> lakes;
std::vector<FeatureIdx> rivers;
std::vector<FeatureIdx> beaches;
std::vector<FeatureIdx> islands;
std::vector<FeatureIdx> buildings;
std::vector<FeatureIdx> greenspaces;
std::vector<FeatureIdx> golfcourses;
std::vector<FeatureIdx> streams;
std::vector<FeatureIdx> glaciers;

std::vector<LatLon> subway_stations;
std::vector<std::pair<LatLon, std::string >> toilets;
std::vector<std::pair<LatLon, std::string >> toilets_wheelchair;
std::vector<std::pair<LatLon, LatLon>> subway_nodes;
std::vector<std::string>maps;
std::vector<Intersection_data> intersections;

//initialize the variables here
ezgl::surface *surface_subway;
ezgl::surface *surface_toilet_wheelchair;
ezgl::surface *surface_toilet;
ezgl::surface *surface_location;
ezgl::renderer* icon_g;

double avg_lat;
double max_lat;
double min_lat;
double max_lon;
double min_lon;

//m3.cpp
std::vector<std::pair<StreetIdx, double>> segment_time;
std::vector<Node> Nodes;
double max_speed;

// Load map
// COMPLETED
bool loadMap(std::string map_streets_database_filename) {
    bool load_successful = false; //Indicates whether the map has loaded
    //successfully
    std::cout << "loadMap: " << map_streets_database_filename << std::endl;

    // Generate the filename for the second file
    std::string map_OSM_database_filename;
    size_t pos = map_streets_database_filename.find("streets.bin");

    // If "streets.bin" is found, replace it with "osm.bin" to get the second filename
    if (pos != std::string::npos) {
        map_OSM_database_filename = map_streets_database_filename;
        map_OSM_database_filename.replace(pos, 11, "osm.bin");

//        map_OSM_database_filename.replace(0, pos," ");
//        map_OSM_database_filename.replace(pos,map_OSM_database_filename.size()," ");
    }


    // Now you can load the second file using the generated filename
    // Add your code to load the second file here


    //
    // Load your map related data structures here.
    //
    if (!loadStreetsDatabaseBIN(map_streets_database_filename)) {
        return load_successful;
    }

    if (!loadOSMDatabaseBIN(map_OSM_database_filename)) {
        return load_successful;
    }

    //call Helper functions
    build_OSM_Database();
    set_map_database();
    load_intersection_street_segments();
    load_street_intersections();
    load_street_segments();
    load_intersection_street_id();
    load_segment_time();



    initial_parks();
    initial_lakes();
    initial_rivers();
    initial_beaches();
    initial_islands();
    initial_buildings();
    initial_greenspaces();
    initial_golfcourses();
    initial_streams();
    initial_glaciers();

    SUBWAY_OSM_Database();
    toilet_OSM_Database();
    toilets_wheelchair_OSM_Database();

    init_subway_route();

    load_subway_png();
    load_toilet_png();
    load_toilet_wheelchair_png();
    load_location_png();

    load_max_min_lat_lon();

    //m3.cpp
    initial_segment_highlighted();
    init_nodes();

    load_successful = true; //Make sure this is updated to reflect whether
    //loading the map succeeded or failed

    return load_successful;
}

//the Helper function for Load Map
void load_intersection_street_segments () {
    //get the number of intersection
    int numIntersections = getNumIntersections();

    //create a vector for street_segment_ids
    std::vector<StreetSegmentIdx> street_segment_ids;

    //resize the vector intersection_street_segments with size: the number of the intersection
    intersection_street_segments.resize(numIntersections);

    //loop through all intersections to get the number of segments of each intersection
    for (int intersection = 0; intersection < numIntersections; ++intersection) {
        int numSegmentsAtIntersection = getNumIntersectionStreetSegment(intersection);

        //loop through all street segments
        //to get each StreetSegmentIdx at one intersection
        for (int i = 0; i < numSegmentsAtIntersection; ++i) {
            StreetSegmentIdx ss_id = getIntersectionStreetSegment(i, intersection);

            //push back the value into the vector
            intersection_street_segments[intersection].push_back(ss_id);
        }
    }
}

void load_street_intersections () {

    //get the number of Street and the number of the number of intersection
    int numStreets = getNumStreets();
    int numIntersection = getNumIntersections();

    //create a vector for intersectionID
    std::vector<IntersectionIdx> intersection_ids;

    //resize the street_intersections vector
    street_intersections.resize(numStreets);

    //loop through all intersections to get all segments for each intersection
    for (int intersection = 0; intersection < numIntersection; ++ intersection) {
        int numSegmentsAtIntersection = getNumIntersectionStreetSegment(intersection);

        //loop through to get the street segment ID
        //then get the streetID
        for (int i = 0; i < numSegmentsAtIntersection; ++i) {
            StreetSegmentIdx ss_id = getIntersectionStreetSegment(i, intersection);
            StreetIdx st_id = getStreetSegmentInfo(ss_id).streetID;

            street_intersections[st_id].push_back(intersection);

            //loop through to find the same value in the vector in order
//            // ex. [3,5,3,6,7,1,] -> [1, 3, 3, 6, 7]
//            auto sameIntersection = std::unique(street_intersections[st_id].begin(),
//                                                street_intersections[st_id].end());
//            //erase the same value in the vector
//            street_intersections[st_id].erase(sameIntersection, street_intersections[st_id].end());
        }
    }


    // Remove duplicates from each street's intersection vector
    for (auto& intersection_vector : street_intersections) {
        auto uniqueEnd = std::unique(intersection_vector.begin(), intersection_vector.end());
        intersection_vector.erase(uniqueEnd, intersection_vector.end());
    }

}

void load_street_segments(){

    //get the number of streets and the number of segments
    int numStreets = getNumStreets();
    int numSegments = getNumStreetSegments();

    //resize the vector
    street_segments.resize(numStreets);

    //loop through all segments to get all streetID
    //push back to the vector

    max_speed = 0;

    for (int i=0; i<numSegments; ++i){
        StreetIdx st_id = getStreetSegmentInfo(i).streetID;


        if (getStreetSegmentInfo(i).speedLimit > max_speed){
            max_speed = getStreetSegmentInfo(i).speedLimit;
        }
        street_segments[st_id].push_back(i);

    }
}

void load_intersection_street_id() {

    //get the number of the Intersection
    int numIntersection = getNumIntersections();

    //resize the vector
    intersection_street_id.resize(numIntersection);

    //loop through all intersection to get the number of segments
    for (int i = 0; i < numIntersection; ++ i) {
        int numSegments=getNumIntersectionStreetSegment(i);

        //loop through all segments to get the streetID
        //push back
        for (int j=0;  j<numSegments; ++j) {
            StreetIdx streetID_at_intersection = getStreetSegmentInfo(
                    getIntersectionStreetSegment (j,i)).streetID;
            intersection_street_id[i].push_back(streetID_at_intersection);
        }
    }
}

//close Map
//COMPLETED
void closeMap() {
    //Clean-up your map related data structures here

    //clear the vectors
    std::vector<std::vector<StreetSegmentIdx>>().swap(intersection_street_segments);
    std::vector<std::vector<IntersectionIdx>>().swap(street_intersections);
    std::vector<std::vector<StreetSegmentIdx>>().swap(street_segments);
    std::vector<std::vector<StreetIdx>>().swap(intersection_street_id);
    //m2.cpp
    std::vector<Intersection_data>().swap(intersections);
    std::vector<FeatureIdx>().swap(parks);
    std::vector<FeatureIdx>().swap(lakes);
    std::vector<FeatureIdx>().swap(rivers);
    std::vector<FeatureIdx>().swap(beaches);
    std::vector<FeatureIdx>().swap(islands);
    std::vector<FeatureIdx>().swap(buildings);
    std::vector<FeatureIdx>().swap(greenspaces);
    std::vector<FeatureIdx>().swap(golfcourses);
    std::vector<FeatureIdx>().swap(streams);
    std::vector<FeatureIdx>().swap(glaciers);
    std::vector<LatLon>().swap(subway_stations);
    std::vector<std::pair<LatLon, std::string >>().swap(toilets);
    std::vector<std::pair<LatLon, std::string >>().swap(toilets_wheelchair);
    std::vector<std::pair<LatLon, LatLon>>().swap(subway_nodes);

    std::vector<const OSMNode*>().swap(NodeIndex_NodeId);
    std::vector<OSMID>().swap(NodeIndex_OSMId);
    std::vector<const OSMWay*>().swap(WayIndex_WayId);
    std::vector<OSMID>().swap(WayIndex_OSMId);
    std::vector<std::vector<LatLon>>().swap(Way_LatLon_of_Nodes);
    std::vector<std::vector<double>>().swap(Way_WayLength);

    icon_g->free_surface(surface_subway);
    icon_g->free_surface(surface_toilet_wheelchair);
    icon_g->free_surface(surface_toilet);
    icon_g->free_surface(surface_location);

    //m3.cpp
    find_path_pressed = false;
    std::vector<std::pair<StreetIdx, double>>().swap(segment_time);
    std::vector<IntersectionIdx>().swap(highlighted);
    std::vector<bool>().swap(segment_highlighted);
    std::vector<Node>().swap(Nodes);

    //Close the database
    closeStreetDatabase();
    closeOSMDatabase();


}

// Returns the distance between two (lattitude,longitude) coordinates in meters.
// Speed Requirement --> moderate
// COMPLETED
double findDistanceBetweenTwoPoints(LatLon point_1, LatLon point_2) { 
    // load latitude and longitude values of 2 points in radians
    double point1_lat = point_1.latitude() * kDegreeToRadian;
    double point1_long = point_1.longitude() * kDegreeToRadian;

    double point2_lat = point_2.latitude() * kDegreeToRadian;
    double point2_long = point_2.longitude() * kDegreeToRadian;

    // compute latitude average
    double lat_avg = (point1_lat + point2_lat)/2.0;

    // convert 2 points to (x,y) coordinates
    double point1_x = kEarthRadiusInMeters*point1_long*cos(lat_avg);
    double point1_y = kEarthRadiusInMeters*point1_lat;

    double point2_x = kEarthRadiusInMeters*point2_long*cos(lat_avg);
    double point2_y = kEarthRadiusInMeters*point2_lat;
    
    // take difference of y and x values
    double diffpoints_x = point2_x - point1_x;
    double diffpoints_y = point2_y - point1_y;

    // return Pythagora's Theorem value
    return sqrt(pow(diffpoints_x,2)+pow(diffpoints_y,2));
}

// Returns the length of the given street segment in meters.
// Speed Requirement --> moderate
// COMPLETED
double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    StreetSegmentInfo segmentLengthStrName = getStreetSegmentInfo(street_segment_id);

    // find the intersections that make this street segment
    IntersectionIdx start_of_StreetSegID = segmentLengthStrName.from;
    IntersectionIdx end_of_StreetSegID = segmentLengthStrName.to;

    // determine the LatLon position of these intersections
    LatLon start_StreetSegId_Coord = getIntersectionPosition(start_of_StreetSegID);
    LatLon end_StreetSegId_Coord = getIntersectionPosition (end_of_StreetSegID);


    // get number of curves in this street segment
    int numCurvesInSeg = getStreetSegmentInfo(street_segment_id).numCurvePoints;

    // store the total street segment length
    double totalStreetSegLength = 0.0;

    // prev and next variables to keep up
    LatLon prevStreetPoint = start_StreetSegId_Coord;

    LatLon nextStreetPoint;

    // create a for loop that calculates the distance of these
    if (numCurvesInSeg != 0) {
        for (int i = 0; i < numCurvesInSeg; i++) {
            nextStreetPoint = getStreetSegmentCurvePoint(i, street_segment_id);

            // find distance between these points
            double distanceAtCurve = findDistanceBetweenTwoPoints (prevStreetPoint, nextStreetPoint);
            
            totalStreetSegLength += distanceAtCurve;
            // update prevStreetPoint
            prevStreetPoint = nextStreetPoint;
        }

        // add the last distance from last curve point to 2nd intersection
        totalStreetSegLength = totalStreetSegLength + findDistanceBetweenTwoPoints (prevStreetPoint, end_StreetSegId_Coord);
    } else {
        // straight path with no curves
        totalStreetSegLength = findDistanceBetweenTwoPoints (start_StreetSegId_Coord, end_StreetSegId_Coord);
    }

    return totalStreetSegLength;
}

// Returns the travel time to drive from one end of a street segment
// to the other, in seconds, when driving at the speed limit.
// Note: (time = distance/speed_limit)
// Speed Requirement --> high
// COMPLETED
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {

    //distance -> findStreetSegmentLength(street_segment_id)
    //speed_limit -> getStreetSegmentInfo(street_segment_id).speedLimit
    //time = distance/speed_limit
    double travelTime = findStreetSegmentLength(street_segment_id)/
            (getStreetSegmentInfo(street_segment_id).speedLimit);

    return travelTime;
}

// Returns the angle (in radians) that would result as you exit
// src_street_segment_id and enter dst_street_segment_id, if they share an
// intersection.
// If a street segment is not completely straight, use the last piece of the
// segment closest to the shared intersection.
// If the two street segments do not share an intersection, return a constant
// NO_ANGLE, which is defined above.
// Speed Requirement --> none
// COMPLETED
double findAngleBetweenStreetSegments(StreetSegmentIdx src_street_segment_id,
                                      StreetSegmentIdx dst_street_segment_id){

    //check if two street segments share an intersection
    int sameIntersection = 0; //check if share the intersection
    double angleBetweenStreetSeg = 0.0;

    //get the given two street segments' beginning intersections and the ending intersections
    IntersectionIdx intersection_src_from = getStreetSegmentInfo(src_street_segment_id).from;
    IntersectionIdx intersection_src_to = getStreetSegmentInfo(src_street_segment_id).to;
    IntersectionIdx intersection_dst_from = getStreetSegmentInfo(dst_street_segment_id).from;
    IntersectionIdx intersection_dst_to = getStreetSegmentInfo(dst_street_segment_id).to;

    //get each intersections' latitude and longitude
    LatLon position_src_from = getIntersectionPosition(getStreetSegmentInfo(src_street_segment_id).from);
    LatLon position_src_to = getIntersectionPosition(getStreetSegmentInfo(src_street_segment_id).to);
    LatLon position_dst_from = getIntersectionPosition(getStreetSegmentInfo(dst_street_segment_id).from);
    LatLon position_dst_to = getIntersectionPosition(getStreetSegmentInfo(dst_street_segment_id).to);


    //list four situations if two street segments have the shared intersection

    // the first situation: the 'from' intersection of src_street_segment
    // is connected to the 'to' intersection of dst_street_segment
    if (intersection_src_from == intersection_dst_to) sameIntersection = 1;

    // the second situation: the 'from' intersection of src_street_segment
    // is connected to the 'from' intersection of dst_street_segment
    if (intersection_src_from == intersection_dst_from) sameIntersection = 2;

    // the second situation: the 'to' intersection of src_street_segment
    // is connected to the 'to' intersection of dst_street_segment
    if (intersection_src_to == intersection_dst_to) sameIntersection = 3;

    // the second situation: the 'to' intersection of src_street_segment
    // is connected to the 'from' intersection of dst_street_segment
    if (intersection_src_to == intersection_dst_from) sameIntersection = 4;

    //if there is no intersection shared, return
    if (sameIntersection == 0) {return 0.0;}

    // access the number of curve point for each street segment
    int numCurves_src = getStreetSegmentInfo( src_street_segment_id).numCurvePoints;
    int numCurves_dst = getStreetSegmentInfo(dst_street_segment_id).numCurvePoints;

    //If both street segment has no curve point
    if ((numCurves_dst == 0) && (numCurves_src == 0)) {

        //the first situation
        if(sameIntersection == 1) {

            //Using th Cosine Law to find the angle

            //get each length of triangle
            double a = findDistanceBetweenTwoPoints(position_src_from, position_src_to);
            double b = findDistanceBetweenTwoPoints(position_dst_to, position_dst_from);
            double c = findDistanceBetweenTwoPoints(position_src_to, position_dst_from);


            //angle between two street segment -> CosineLaw(a, b, c)
            //store the angle when you exit the src_street_segment and into the dst_street_segment
            //-> 2pi - angle
            angleBetweenStreetSeg = M_PI - CosineLaw(a, b, c);

        }

        //second situation
        if (sameIntersection == 2) {

            // get the length of the triangle
            double a = findDistanceBetweenTwoPoints(position_src_from, position_src_to);
            double b = findDistanceBetweenTwoPoints(position_dst_to, position_dst_from);
            double c = findDistanceBetweenTwoPoints(position_src_to, position_dst_to);

            //store angle value
            angleBetweenStreetSeg = M_PI - CosineLaw(a, b, c);
        }

        //third situation
        if (sameIntersection == 3) {

            // get the length of the triangle
            double a = findDistanceBetweenTwoPoints(position_src_from, position_src_to);
            double b = findDistanceBetweenTwoPoints(position_dst_to, position_dst_from);
            double c = findDistanceBetweenTwoPoints(position_src_from, position_dst_from);

            //store angle value
            angleBetweenStreetSeg = M_PI - CosineLaw(a, b, c);

        }

        //fourth situation
        if (sameIntersection == 4) {

            // get the length of the triangle
            double a = findDistanceBetweenTwoPoints( position_src_from,position_src_to);
            double b = findDistanceBetweenTwoPoints(position_dst_from, position_dst_to);
            double c = findDistanceBetweenTwoPoints(position_src_from, position_dst_to);

            //store angle value
            angleBetweenStreetSeg = M_PI - CosineLaw(a, b, c);
        }
    }

    //If at least one of the street segment has curve point

    else {
        int pointNum_src = getStreetSegmentInfo(src_street_segment_id).numCurvePoints;
        int pointNum_dst = getStreetSegmentInfo(dst_street_segment_id).numCurvePoints;

        // src_street_segment has curve point
        if ((numCurves_dst == 0) && (numCurves_src != 0)) {

            // firstly, get the longitude and latitude of the curve point that would be used in this situation
            LatLon curvePoint_src_first = getStreetSegmentCurvePoint(0, src_street_segment_id);
            LatLon curvePoint_src_last = getStreetSegmentCurvePoint(pointNum_src-1, src_street_segment_id);

            //first situation
            if (sameIntersection == 1) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_from, curvePoint_src_first);
                double b = findDistanceBetweenTwoPoints(position_dst_to, position_dst_from);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_first, position_dst_from);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw(a, b, c);
            }

            //second situation
            else if (sameIntersection == 2) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_from, curvePoint_src_first);
                double b = findDistanceBetweenTwoPoints(position_dst_from, position_dst_to);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_first, position_dst_to);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }

            //third situation
            else if (sameIntersection == 3) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_to, curvePoint_src_last);
                double b = findDistanceBetweenTwoPoints(position_dst_to, position_dst_from);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_last, position_dst_from);

                //store the value
                angleBetweenStreetSeg = M_PI- CosineLaw( a,  b,  c);
            }

            //fourth situation
            else if (sameIntersection == 4) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_to, curvePoint_src_last);
                double b = findDistanceBetweenTwoPoints(position_dst_from, position_dst_to);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_last, position_dst_to);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);

            }
        }

        //dst street segment has the curve point
        else if ((numCurves_dst != 0) && (numCurves_src == 0)) {

            // firstly, get the longitude and latitude of the curve point that would be used in this situation
            LatLon curvePoint_dst_first = getStreetSegmentCurvePoint(0, dst_street_segment_id);
            LatLon curvePoint_dst_last = getStreetSegmentCurvePoint(pointNum_dst-1, dst_street_segment_id);

            //first situation
            if (sameIntersection == 1) {

                //get the length of the triangle
                double a = findDistanceBetweenTwoPoints(position_src_from, position_src_to);
                double b = findDistanceBetweenTwoPoints(position_dst_to, curvePoint_dst_last);
                double c = findDistanceBetweenTwoPoints(position_src_to, curvePoint_dst_last);

                //store the angle
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);

            }

            //second situation
            else if (sameIntersection == 2) {

                //get the length of the triangle
                double a = findDistanceBetweenTwoPoints(position_src_from, position_src_to);
                double b = findDistanceBetweenTwoPoints(position_dst_from, curvePoint_dst_first);
                double c = findDistanceBetweenTwoPoints(position_src_to, curvePoint_dst_first);

                //store the angle
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }

            //third situation
            else if (sameIntersection == 3) {

                //get the length of the triangle
                double a = findDistanceBetweenTwoPoints(position_src_to, position_src_from);
                double b = findDistanceBetweenTwoPoints(position_dst_to, curvePoint_dst_last);
                double c = findDistanceBetweenTwoPoints(position_src_from, curvePoint_dst_last);

                //store the angle
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }

            //fourth situation
            else if (sameIntersection == 4) {

                //get the length of the triangle
                double a = findDistanceBetweenTwoPoints(position_src_to, position_src_from);
                double b = findDistanceBetweenTwoPoints(position_dst_from, curvePoint_dst_first);
                double c = findDistanceBetweenTwoPoints(position_src_from, curvePoint_dst_first);

                //store the angle
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b , c);
            }
        }

        //if both street segment has the curve point
        else if ((numCurves_dst != 0) && (numCurves_src != 0)) {

            //get the latitude and longitude of curve point
            LatLon curvePoint_src_first = getStreetSegmentCurvePoint(0, src_street_segment_id);
            LatLon curvePoint_dst_first = getStreetSegmentCurvePoint(0, dst_street_segment_id);
            LatLon curvePoint_src_last = getStreetSegmentCurvePoint(pointNum_src-1, src_street_segment_id);
            LatLon curvePoint_dst_last = getStreetSegmentCurvePoint(pointNum_dst-1, dst_street_segment_id);

            //first situation
            if (sameIntersection == 1) {

                //get the length
                double a = findDistanceBetweenTwoPoints(curvePoint_src_first, position_src_from);
                double b = findDistanceBetweenTwoPoints(position_dst_to, curvePoint_dst_last);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_first, curvePoint_dst_last);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
           }

            //second situation
            else if (sameIntersection == 2) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_from, curvePoint_src_first);
                double b = findDistanceBetweenTwoPoints(position_dst_from, curvePoint_dst_first);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_first, curvePoint_dst_first);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }

            //third situation
            else if (sameIntersection == 3) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_to, curvePoint_src_last);
                double b = findDistanceBetweenTwoPoints(position_dst_to, curvePoint_dst_last);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_last, curvePoint_dst_last);

                //store the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }

            //the last situation
            else if (sameIntersection == 4) {

                //get the length
                double a = findDistanceBetweenTwoPoints(position_src_to, curvePoint_src_last);
                double b = findDistanceBetweenTwoPoints(position_dst_from, curvePoint_dst_first);
                double c = findDistanceBetweenTwoPoints(curvePoint_src_last, curvePoint_dst_first);

                //get the value
                angleBetweenStreetSeg = M_PI - CosineLaw( a,  b,  c);
            }
        }
    }

    //return the angle
    return angleBetweenStreetSeg;
}

//Helper function for findAngleBetweenStreetSegments function
//Apply the Cosine Law to find the angle between two street segment
double CosineLaw(double a, double b, double c){

    double theta = 0.0;

    //Cosine Law:
    double cosine_theta = (a * a + b * b - c * c) / (2 * a * b);

    //Since cosine range from 1 to -1, set the angle value if out of the range
    if(cosine_theta >= 1.0) theta = M_PI;
    else if(cosine_theta <= -1.0) theta = M_PI;
    else theta = acos(cosine_theta);

    //return
    return theta;
}

// Returns true if the two intersections are directly connected, meaning you can
// legally drive from the first intersection to the second using only one
// streetSegment.
// Speed Requirement --> moderate
// COMPLETED
bool intersectionsAreDirectlyConnected(std::pair<IntersectionIdx, IntersectionIdx> intersection_ids){
    // load the number of segments at each intersection
    int numSegmentsAtIntersection1 = getNumIntersectionStreetSegment(intersection_ids.first);
    int numSegmentsAtIntersection2 = getNumIntersectionStreetSegment(intersection_ids.second);

    // loop through all intersections
    for (int i=0; i<numSegmentsAtIntersection1; ++i ){
        for (int j=0; j<numSegmentsAtIntersection2; ++j ){
            // when there is an intersection with the same street segment, return true
            if (getIntersectionStreetSegment(i, intersection_ids.first) ==
                getIntersectionStreetSegment(j, intersection_ids.second)) {
                return true;
            }
        }
    }
    // no intersection segments are the same, return false
    return false;
}


// Returns the geographically nearest intersection (i.e. as the crow flies) to
// the given position.
// Speed Requirement --> none
// COMPLETED
IntersectionIdx findClosestIntersection(LatLon my_position){
    // load the number of intersections
    int numIntersections = getNumIntersections();

    // initialize variables for closest intersection id and distance
    IntersectionIdx closestIdx = 0;
    double closestDistance = 99999999;

    // loop through all intersections
    for (int intersection = 0; intersection < numIntersections; ++intersection) {
        // find the distance from "intersection" to my_position
        double Distance = findDistanceBetweenTwoPoints(my_position, getIntersectionPosition(intersection));
        // if the distance is smaller than comarison value, update closest distance
        if (Distance < closestDistance){
            closestDistance = Distance;
            closestIdx = intersection;
        }
    }
    return closestIdx;
}

// Returns the street segments that connect to the given intersection.
// Speed Requirement --> high
// COMPLETED
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection (IntersectionIdx intersection_id){
    return intersection_street_segments[intersection_id];
}

// Returns all intersections along the given street.
// There should be no duplicate intersections in the returned vector.
// Speed Requirement --> high
// COMPLETED
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    return street_intersections[street_id];
}

// Return all intersection ids at which the two given streets intersect.
// This function will typically return one intersection id for streets that
// intersect and a length 0 vector for streets that do not. For unusual curved
// streets it is possible to have more than one intersection at which two
// streets cross.
// There should be no duplicate intersections in the returned vector.
// Speed Requirement --> high
// COMPLETED
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids) {
    // load street ids
    StreetIdx street1 = street_ids.first;
    StreetIdx street2 = street_ids.second;

    // initialize vectors to store intersections at street1 and result
    std::vector<IntersectionIdx> IntersectionsAtStreet1 = findIntersectionsOfStreet(street1);
    std::vector<IntersectionIdx> Result;

    // loop through all intersections of street 1
    for (int i=0; i<IntersectionsAtStreet1.size(); ++i){
        // load the intersection index along street
        IntersectionIdx indexAlongTheStreet = IntersectionsAtStreet1[i];
        std::vector<StreetIdx> street_ids_associated_with_index = intersection_street_id[indexAlongTheStreet];
        // use find algorithm to find if street2 has the same intersection
        auto it = std::find(street_ids_associated_with_index.begin(), street_ids_associated_with_index.end(),street2);
        if (it==street_ids_associated_with_index.end()){
            //Not found
            continue;
        }
        //push back indexAlongTheStreet
        Result.push_back(indexAlongTheStreet);
    }

    return Result;
}

// Returns all street ids corresponding to street names that start with the
// given prefix.
// The function should be case-insensitive to the street prefix.
// The function should ignore spaces.
//  For example, both "bloor " and "BloOrst" are prefixes to
//  "Bloor Street East".
// If no street names match the given prefix, this routine returns an empty
// (length 0) vector.
// You can choose what to return if the street prefix passed in is an empty
// (length 0) string, but your program must not crash if street_prefix is a
// length 0 string.
// Speed Requirement --> high
//COMPLETED
std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){

    std::vector<StreetIdx>StreetIdsFromPartialStreetName;
    int numStreets = getNumStreets();

    street_prefix.erase(std::remove_if(street_prefix.begin(),street_prefix.end(),
                                       ::isspace), street_prefix.end());
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);

    for (int street = 0; street < numStreets; ++ street) {
        std::string street_name = getStreetName(street);
        street_name.erase(std::remove_if(street_name.begin(),street_name.end(),::isspace), street_name.end());
        std::transform(street_name.begin(), street_name.end(), street_name.begin(), ::tolower);
        if (street_name.substr(0, street_prefix.length()) == street_prefix) {
            StreetIdsFromPartialStreetName.push_back(street);
        }
    }
    if (StreetIdsFromPartialStreetName.size()==0){
        StreetIdsFromPartialStreetName.push_back(-1);
    }
    return StreetIdsFromPartialStreetName;
}

// Returns the length of a given street in meters.
// Speed Requirement --> high
// COMPLETED
double findStreetLength(StreetIdx street_id){
    // get vector size of intersections along street
    int SegmentsVectorSize = street_segments[street_id].size();

    // variable to store total street length
    double totalStreetLength = 0.0;

    // loop along all intersections
    for (int i = 0; i < SegmentsVectorSize; i++) {
        // add individual segment lengths to totalStreetLength
        totalStreetLength += findStreetSegmentLength (street_segments[street_id][i]);
    }
    return totalStreetLength;
}


//******** The Below is OSM Part ********* //

//OSM Helper Functions
void build_OSM_Database() {
    buildNodeData();
    buildWayData();
    load_Way_LatLon_of_Nodes();
    load_Way_WayLength();
}

void buildNodeData() {
    for (int i=0; i<getNumberOfNodes(); ++i) {
          const OSMNode* node = getNodeByIndex(i);
          NodeIndex_NodeId.push_back(node);
          NodeIndex_OSMId.push_back(node->id());
    }
}

void buildWayData() {
    for (int i = 0; i < getNumberOfWays(); ++i) {
        const OSMWay* way = getWayByIndex(i);
        WayIndex_WayId.push_back(way);
        WayIndex_OSMId.push_back(way->id());
    }
}

void load_Way_LatLon_of_Nodes() {
    // Load Portion 1
    int numberOfOSMWays = getNumberOfWays();

    Way_LatLon_of_Nodes.resize(numberOfOSMWays);

    // Create a mapping from OSMID to OSMNode pointer
    std::unordered_map<OSMID, const OSMNode*> nodeMap;
    for (int i = 0; i < getNumberOfNodes(); ++i) {
        const OSMNode* node = getNodeByIndex(i);
        nodeMap[node->id()] = node;
    }

    for (int i = 0; i < numberOfOSMWays; ++i) {
        const OSMWay* OSMWayID = getWayByIndex(i);
        std::vector<OSMID> OSMNodes = getWayMembers(OSMWayID);

        for (int j = 0; j < OSMNodes.size(); ++j) {
            OSMID a = OSMNodes[j];

            //Use the mapping to directly get the OSMNode pointer
            const OSMNode* OSMNodeID = nodeMap[a];

            LatLon NodeCord = getNodeCoords(OSMNodeID);
            Way_LatLon_of_Nodes[i].push_back(NodeCord);
            // [wayIndex][<node cords>]

        }
    }
}

void load_Way_WayLength() {
    //Load Portion 2
    int numberOfOSMWays = getNumberOfWays();
    Way_WayLength.resize(numberOfOSMWays);

    for (int i = 0; i < numberOfOSMWays; ++i) {

        double wayLength = 0.0;
        for (int j = 0; j < Way_LatLon_of_Nodes[i].size()-1; ++j) {

            //Way_LatLon_of_Nodes[i] = vector <nodes>
            LatLon LocationOfNode1 = Way_LatLon_of_Nodes[i][j];
            LatLon LocationOfNode2 = Way_LatLon_of_Nodes[i][j + 1];

            //Find Way Length For All Way in DataBase
            double smallSegmentLength= findDistanceBetweenTwoPoints(LocationOfNode1, LocationOfNode2);

            wayLength += smallSegmentLength;

            //[wayindex][waylength]
        }
        Way_WayLength[i].push_back(wayLength);
    }
}

//Get Node By ID
const OSMNode* getNodeById(const OSMID& nodeId) {
    auto it = std::find(NodeIndex_OSMId.begin(), NodeIndex_OSMId.end(), nodeId);
    return NodeIndex_NodeId[std::distance(NodeIndex_OSMId.begin(), it)];
}

//Get Way By ID
const OSMWay* getWayById(const OSMID& wayId) {
    auto it = std::find(WayIndex_OSMId.begin(), WayIndex_OSMId.end(), wayId);
    return WayIndex_WayId[std::distance(WayIndex_OSMId.begin(), it)];
}




// Returns the nearest point of interest of the given name (e.g. "Starbucks")
// to the given position.
// Speed Requirement --> none 
//COMPLETED
POIIdx findClosestPOI(LatLon my_position, std::string poi_name) {
    int Num_POI = getNumPointsOfInterest();
    std::vector<POIIdx> TargetPOIs;

    /// Loop Through All POIs
    for (POIIdx POI = 0; POI < Num_POI; ++POI) {
        std::string name = getPOIName(POI);

        if (name == poi_name) {
            TargetPOIs.push_back(POI);
        }
    }

    // Initialize with a large distance
    double SmallestDistance = std::numeric_limits<double>::infinity();
    POIIdx ClosestPOI = TargetPOIs[0];

    // Loop through all Target POI Positions to find their distance from MyLocation
    for (int i = 0; i < TargetPOIs.size(); ++i) {
        LatLon Position_Of_POI = getPOIPosition(TargetPOIs[i]);
        double Distance_Between_Me_And_Target = findDistanceBetweenTwoPoints(Position_Of_POI, my_position);

        if (Distance_Between_Me_And_Target < SmallestDistance) {
            SmallestDistance = Distance_Between_Me_And_Target;
            ClosestPOI = TargetPOIs[i];
        }
    }

    return ClosestPOI;
}

// Returns the area of the given closed feature in square meters.
// Assume a non self-intersecting polygon (i.e. no holes).
// Return 0 if this feature is not a closed polygon
// Speed Requirement --> moderate
// HELPER FUNCTIONS
double x_featureCoordinate (LatLon featurePoint, double lat_avg) {
    // used to convert x coordinates
    // covert longitude to radians
    double featurePoint_long = featurePoint.longitude() * kDegreeToRadian;

    // convert feature point to x coordinate
    double featurePoint_x = kEarthRadiusInMeters*featurePoint_long*cos(lat_avg);
    return featurePoint_x;
}

double y_featureCoordinate (LatLon featurePoint) {
    // used to convert y coordinates
    // covert latitude to radians
    double featurePoint_lat = featurePoint.latitude() * kDegreeToRadian;

    // convert feature point to x coordinate
    double featurePoint_y = kEarthRadiusInMeters * featurePoint_lat;
    return featurePoint_y;
}

double computeFeatureArea (std::vector <double> x_coord, std:: vector <double> y_coord, int numCoords) {
    // used to compute the area of a polygon

    // variable used to store returning area and store crossProduct sum
    double featureArea = 0.0;
    double sumCrossP = 0.0;

    // loop through all coordinate pairs
    for (int i = 0; i < numCoords;i++) {
        // when i reaches last element, should crossProduct with the first elements
        if (i == numCoords-1) {
            sumCrossP += (x_coord[i] * y_coord[0]) - (y_coord[i] * x_coord[0]);
        } else {
            // compute cross product at i
            sumCrossP += (x_coord[i] * y_coord[i+1]) - (y_coord[i] * x_coord[i+1]);
        }
    }

    // compute the area
    featureArea = std::abs(sumCrossP)/2.0;
    return featureArea;
}
double findFeatureArea(FeatureIdx feature_id) {
    // get the number of feature points
    int numFeaturePoints = getNumFeaturePoints (feature_id);

    // intialize vectors to store feature points and xy coords
    std:: vector <LatLon> allFeaturePoints(numFeaturePoints);
    std:: vector <double> all_x_featurePoints (numFeaturePoints);
    std:: vector <double> all_y_featurePoints (numFeaturePoints);

    // initialize variable to store final value
    double totalFeatureArea = 0.0;

    // when closed polygon: loop through all feature points and store LatLons
    for (int i = 0; i< numFeaturePoints; i++) {
        allFeaturePoints[i] = getFeaturePoint(i, feature_id);
    }

     // when the starting point is endpoint for closed polygon
     // or num points < 3 is also not a polygon
    if (((allFeaturePoints[0].latitude() != allFeaturePoints[numFeaturePoints-1].latitude()) &&
        (allFeaturePoints[0].longitude() != allFeaturePoints[numFeaturePoints-1].longitude()) )|| numFeaturePoints < 3) {
        return totalFeatureArea;
    }

    // compute the average latitude
    double lat_avg = 0.0;
    double lat_total = 0.0;
    for (int k = 0; k < numFeaturePoints; k++) {
        lat_total += allFeaturePoints[k].latitude() * kDegreeToRadian;
    }

    lat_avg = lat_total / numFeaturePoints;

    // convert LatLons to xy coords
    for (int j = 0; j < numFeaturePoints; j++) {
        all_x_featurePoints[j] = x_featureCoordinate(allFeaturePoints[j], lat_avg);
        all_y_featurePoints[j] = y_featureCoordinate(allFeaturePoints[j]);

    }
    // compute total feature area
    totalFeatureArea = computeFeatureArea (all_x_featurePoints, all_y_featurePoints, numFeaturePoints);

    return totalFeatureArea;
}

// Returns the length of the OSMWay that has the given OSMID, in meters.
// To implement this function you will have to  access the OSMDatabaseAPI.h
// functions.
// Speed Requirement --> high
//COMPLETED
double findWayLength(OSMID way_id) {

    //Get Way Index by way OSMID
    //Fetch From Database Loaded in Loadmap
    const OSMWay* OSMWay_ID =getWayById(way_id);

    //get Way Index using OSMWay*
    //return Way_WayLength[way index];
    for (int i = 0; i < getNumberOfWays(); ++i) {
        const OSMWay* way = getWayByIndex(i);

        if (way==OSMWay_ID){
            return Way_WayLength[i][0];
            break;
        }
    }
    return 0.0;
}

// Return the value associated with this key on the specified OSMNode.
// If this OSMNode does not exist in the current map, or the specified key is
// not set on the specified OSMNode, return an empty string.
// Speed Requirement --> high

// helper function that finds the OSMNode of the id
const OSMNode* matchingNodesForOSMID (OSMID osm_id) {
    int numberOfOSMNodes = getNumberOfNodes();

    for (int i = 0; i < numberOfOSMNodes; ++i) {
        const OSMNode *nodeAtIndex = getNodeByIndex(i);
        // compare to see if the id is the osm_id
        if (nodeAtIndex->id() == osm_id) {
            // the id is found
            return nodeAtIndex;
        }
    }
    // id not found
    return nullptr;
}

std::string getOSMNodeTagValue(OSMID osm_id, std::string key){
    // call helper function for getting node
    const OSMNode* nodeAtID = matchingNodesForOSMID(osm_id);

    // node not found
    if (nodeAtID == nullptr) {
        return " ";
    }

    int numOfTags = getTagCount(nodeAtID);

    // for loop to loop through all nodes to find the Node OSMID corresponding to the osm_id
    for (int i = 0; i < numOfTags; ++ i) {
        std:: pair<std::string, std::string> tagPairAtId = getTagPair(nodeAtID, i);
        // compare to see of key is same
        if (tagPairAtId.first == key) {
            // when key is the same, return the value
            return tagPairAtId.second;
        }
    }

    // if the tag is not found, return empty string
    return " ";
}

//Helper functions in m2.cpp
//initialize all feature vectors here
void initial_parks(){
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == PARK) {
            parks.push_back(i);
        }
    }
}

void initial_lakes() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == LAKE) {
            lakes.push_back(i);
        }
    }
}

void initial_rivers() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == RIVER) {
            rivers.push_back(i);
        }
    }
}

void initial_beaches() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == BEACH) {
            beaches.push_back(i);
        }
    }
}
void initial_islands() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == ISLAND) {
            islands.push_back(i);
        }
    }
}
void initial_buildings() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == BUILDING) {
            buildings.push_back(i);
        }
    }
}
void initial_greenspaces() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == GREENSPACE) {
            greenspaces.push_back(i);
        }
    }
}
void initial_golfcourses() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == GOLFCOURSE) {
            golfcourses.push_back(i);
        }
    }
}
void initial_streams() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == STREAM) {
            streams.push_back(i);
        }
    }
}
void initial_glaciers() {
    for (int i = 0; i < getNumFeatures(); ++i) {
        if (getFeatureType(i) == GLACIER) {
            glaciers.push_back(i);
        }
    }
}

//initialize all vectors related to OSM_Database here
// subways
void SUBWAY_OSM_Database() {
    //initial the size
    for (int i = 0; i < getNumberOfNodes(); ++i){
        const OSMNode* n = getNodeByIndex(i);
        int tag_count = getTagCount(n);

        for (int j = 0; j < tag_count; j++ ) {
            if (!((getTagPair(n, j).first == "station")&&(getTagPair(n, j).second == "subway"))){
                continue;
            }

            subway_stations.push_back( getNodeCoords(n));
            break;
        }
    }
}
// public washrooms
void toilet_OSM_Database() {
    //initial the size
    for (int i = 0; i < getNumberOfNodes(); ++i){
        const OSMNode* n = getNodeByIndex(i);
        int tag_count = getTagCount(n);

        for (int j = 0; j < tag_count; j++ ) {
            if (!((getTagPair(n, j).first == "toilets")&&(getTagPair(n, j).second == "yes"))){
                continue;
            }

            std::pair<LatLon, std::string> toilet_location_name;
            toilet_location_name.first = getNodeCoords(n);

            for (int k=0; k<tag_count; k++){
                if (getTagPair(n,k).first=="name"){

                    toilet_location_name.second = getTagPair(n,k).second;
                }
            }

            toilets.push_back(toilet_location_name);
        }
    }
}
//accessibility washrooms
void toilets_wheelchair_OSM_Database() {
    //initial the size
    for (int i = 0; i < getNumberOfNodes(); ++i){
        const OSMNode* n = getNodeByIndex(i);
        int tag_count = getTagCount(n);

        for (int j = 0; j < tag_count; j++ ) {
            if (!((getTagPair(n, j).first == "toilets:wheelchair")&&(getTagPair(n, j).second == "yes"))){
                continue;
            }

            std::pair<LatLon, std::string> toilet_wheelchair_location_name;
            toilet_wheelchair_location_name.first = getNodeCoords(n);

            for (int k=0; k<tag_count; k++){
                if (getTagPair(n,k).first=="name"){

                    toilet_wheelchair_location_name.second = getTagPair(n,k).second;
                }
            }

            toilets_wheelchair.push_back(toilet_wheelchair_location_name);
        }
    }
}

//initial the subway route
void init_subway_route() {
    int numberOfOSMWays = getNumberOfWays();
    subway_nodes.resize(numberOfOSMWays);

    for (int i = 0; i < numberOfOSMWays; ++i) {

        for (int j = 0; j < getTagCount(getWayByIndex(i)); j++) {
            if ((getTagPair(getWayByIndex(i), j).first == "railway") &&
                (getTagPair(getWayByIndex(i), j).second == "subway")) {
                for (int k = 0; k < Way_LatLon_of_Nodes[i].size() - 1; ++k) {

                    //Way_LatLon_of_Nodes[i] = vector <nodes>
                    LatLon LocationOfNode1 = Way_LatLon_of_Nodes[i][k];
                    LatLon LocationOfNode2 = Way_LatLon_of_Nodes[i][k + 1];

                    std::pair<LatLon, LatLon> subway_segment;
                    subway_segment.first = LocationOfNode1;
                    subway_segment.second = LocationOfNode2;
                    subway_nodes.push_back(subway_segment);

                }
            }


            if ((getTagPair(getWayByIndex(i), j).first == "highway")){

                if(getTagPair(getWayByIndex(i), j).second == "secondary"){
                    for (int k = 0; k < Way_LatLon_of_Nodes[i].size() - 1; ++k) {

                        //Way_LatLon_of_Nodes[i] = vector <nodes>
                        LatLon LocationOfNode1 = Way_LatLon_of_Nodes[i][k];
                        LatLon LocationOfNode2 = Way_LatLon_of_Nodes[i][k + 1];

                        std::pair<LatLon, LatLon> highway_segment;
                        highway_segment.first = LocationOfNode1;
                        highway_segment.second = LocationOfNode2;
                        secondary_highway_nodes.push_back(highway_segment);

                    }

                }


                if(getTagPair(getWayByIndex(i), j).second == "tertiary"){
                    for (int k = 0; k < Way_LatLon_of_Nodes[i].size() - 1; ++k) {

                        //Way_LatLon_of_Nodes[i] = vector <nodes>
                        LatLon LocationOfNode1 = Way_LatLon_of_Nodes[i][k];
                        LatLon LocationOfNode2 = Way_LatLon_of_Nodes[i][k + 1];

                        std::pair<LatLon, LatLon> highway_segment;
                        highway_segment.first = LocationOfNode1;
                        highway_segment.second = LocationOfNode2;
                        tertiary_highway_nodes.push_back(highway_segment);

                    }

                }



            }
        }
    }

//    getTagPair(getWayByIndex(i), j).second == "primary_link" || getTagPair(getWayByIndex(i), j).second == "trunk_link"
//    || getTagPair(getWayByIndex(i), j).second == "secondary"||  getTagPair(getWayByIndex(i), j).second == "secondary_link"
}

// different load map databases
void set_map_database(){

    maps.push_back("toronto_canada");
    maps.push_back("tokyo_japan");
    maps.push_back("tehran_iran");
    maps.push_back("sydney_australia");
    maps.push_back("singapore");
    maps.push_back("saint-helena");
    maps.push_back("rio-de-janeiro_brazil");
    maps.push_back("new-york_usa");
    maps.push_back("new-delhi_india");
    maps.push_back("london_england");
    maps.push_back("kyiv_ukraine");
    maps.push_back("interlaken_switzerland");
    maps.push_back("iceland");
    maps.push_back("hong-kong_china");
    maps.push_back("hamilton_canada");
    maps.push_back("golden-horseshoe_canada");
    maps.push_back("cape-town_south-africa");
    maps.push_back("boston_usa");
    maps.push_back("beijing_china");

}

//Intersections
void initial_intersections() {
    intersections.resize(getNumIntersections());
    for (int id = 0; id < getNumIntersections(); ++id) {
        intersections[id].position = getIntersectionPosition(id);
        intersections[id].name = getIntersectionName(id);
    }
}

//load all icons used in the map
void load_subway_png() {
    surface_subway = icon_g->load_png("libstreetmap/resources/subway_station.png");
}
void load_toilet_png() {
    surface_toilet = icon_g->load_png("libstreetmap/resources/toilet.png");
}
void load_toilet_wheelchair_png() {
    surface_toilet_wheelchair = icon_g->load_png("libstreetmap/resources/toilet_wheelchair.png");
}
void load_location_png() {
    surface_location = icon_g->load_png("libstreetmap/resources/location.png");
}

// calculate maxLon and maxLat of world
void load_max_min_lat_lon(){
    max_lat = getIntersectionPosition(0).latitude();
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).longitude();
    min_lon = max_lon;

    for (int id  = 0; id < getNumIntersections(); ++id) {

        max_lat = std::max(max_lat, getIntersectionPosition(id).latitude());
        min_lat = std::min(min_lat, getIntersectionPosition(id).latitude());
        max_lon = std::max(max_lon, getIntersectionPosition(id).longitude());
        min_lon = std::min(min_lon, getIntersectionPosition(id).longitude());
    }
    avg_lat = (max_lat+min_lat)/2;
}

//initial the vector for compute the path travel time
//<streetID, time> for each street segment
void load_segment_time(){
    for (int i=0; i<getNumStreetSegments(); ++i){
        double time = findStreetSegmentLength(i)/getStreetSegmentInfo(i).speedLimit;

        StreetIdx street = getStreetSegmentInfo(i).streetID;
        segment_time.push_back(std::make_pair(street, time));

    }
}

//Below is the help functions for m3.cpp
void initial_segment_highlighted(){

    for(int i=0; i<getNumStreetSegments(); ++i){
        segment_highlighted.push_back(false);
    }

    for(int i=0; i<getNumStreetSegments(); ++i){
        explore_highlighted.push_back(false);
    }
}


void init_nodes(){

    std::vector<Node> ().swap(Nodes);
    for(int i=0; i<getNumIntersections(); ++i){
        Node node;
        node.visited= false;
        for(int j=0; j<getNumIntersectionStreetSegment(i); ++j){
            StreetSegmentIdx seg = getIntersectionStreetSegment(j,i);
            IntersectionIdx from = getStreetSegmentInfo(seg).from;
            IntersectionIdx to = getStreetSegmentInfo(seg).to;
            int to_node;
            std::pair<StreetSegmentIdx, int> out_edge_to_node;
            if (i==from){
                to_node = to;
            }else{
                //if is One way street, we don't consider this path
                if(getStreetSegmentInfo(seg).oneWay){
                    continue;
                }
                to_node = from;
            }
            out_edge_to_node.first = seg;
            out_edge_to_node.second = to_node;

            node.out_going_edge_to_Node.push_back(out_edge_to_node);
        }
        Nodes.push_back(node);
    }
}